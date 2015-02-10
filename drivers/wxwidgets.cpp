// Copyright (C) 2015  Phil Rosenberg
// Copyright (C) 2005  Werner Smekal, Sjaak Verdoold
// Copyright (C) 2005  Germain Carrera Corraleche
// Copyright (C) 1999  Frank Huebner
//
// This file is part of PLplot.
//
// PLplot is free software; you can redistribute it and/or modify
// it under the terms of the GNU Library General Public License as published
// by the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// PLplot is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public License
// along with PLplot; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
//

// TODO:
// - NA
//

// wxwidgets headers
#include <wx/wx.h>
#include <wx/wfstream.h>
#include <wx/except.h>

#include "plDevs.h"

// plplot headers
#include "plplotP.h"
#include "drivers.h"

// C/C++ headers
#include <cstdio>

#include "wxwidgets.h"

#ifdef __WXMAC__
        #include <Carbon/Carbon.h>
extern "C" { void CPSEnableForegroundOperation( ProcessSerialNumber* psn ); }
#endif


//--------------------------------------------------------------------------
//  void Log_Verbose( const char *fmt, ... )
//
//  Print verbose debug message to stderr (printf style).
//--------------------------------------------------------------------------
void Log_Verbose( const char *fmt, ... )
{
#ifdef _DEBUG_VERBOSE
    va_list args;
    va_start( args, fmt );
    fprintf( stderr, "Verbose: " );
    vfprintf( stderr, fmt, args );
    fprintf( stderr, "\n" );
    va_end( args );
    fflush( stderr );
#else
    (void) fmt;        // Cast to void to silence compiler warnings about unused paraemeter
#endif
}


//--------------------------------------------------------------------------
//  void Log_Debug( const char *fmt, ... )
//
//  Print debug message to stderr (printf style).
//--------------------------------------------------------------------------
void Log_Debug( const char *fmt, ... )
{
#ifdef _DEBUG
    va_list args;
    va_start( args, fmt );
    fprintf( stderr, "Debug: " );
    vfprintf( stderr, fmt, args );
    fprintf( stderr, "\n" );
    va_end( args );
    fflush( stderr );
#else
    (void) fmt;        // Cast to void to silence compiler warnings about unused paraemeter
#endif
}


//--------------------------------------------------------------------------
//  Constructor, creates the object but does not actually create or link to
//  any shared memory.
//--------------------------------------------------------------------------
PLMemoryMap::PLMemoryMap()
{
#ifdef WIN32
	m_mapFile=NULL;
#else
	m_mapFile = -1;
	m_name = NULL;
#endif
	m_buffer=NULL;
	m_size = 0;
}

//--------------------------------------------------------------------------
//  Constructor, creates the shared memory area. If onlyIfExists is true
//  then we will try to access an existing shared memory area rather than
//  creating a new one.
//--------------------------------------------------------------------------
PLMemoryMap::PLMemoryMap( const char *name, PLINT size, bool mustExist, bool mustNotExist )
{
#ifdef WIN32
	m_mapFile = NULL;
#else
	m_mapFile = -1;
	m_name = NULL;
#endif
	m_buffer=NULL;
	m_size = 0;
	create( name, size, mustExist, mustNotExist );
}

//--------------------------------------------------------------------------
//  create does most of the work in trying to create the memory map it is
//  called by the constructor or by the user. If the object already has a
//  shared memory area then that is closed before a new area of memory is
//  created or connected to. As per the constructor if onlyIfExists is true
//  then we will try to access an existing shared memory area rather than
//  creating a new one.
//--------------------------------------------------------------------------
void PLMemoryMap::create( const char *name, PLINT size, bool mustExist, bool mustNotExist )
{
	close();
	assert( ! (mustExist && mustNotExist ) );
	if( mustExist && mustNotExist )
		return;
#ifdef WIN32
	if( mustExist )
		m_mapFile = OpenFileMappingA( FILE_MAP_ALL_ACCESS, FALSE, name);
	else if( mustNotExist )
	{
		m_mapFile = CreateFileMappingA( INVALID_HANDLE_VALUE, NULL,
			PAGE_READWRITE, 0, size, name );
		if( GetLastError() == ERROR_ALREADY_EXISTS )
			close();
	}
	else
		m_mapFile = CreateFileMappingA( INVALID_HANDLE_VALUE, NULL,
			PAGE_READWRITE, 0, size, name );

	if( m_mapFile )
		m_buffer = MapViewOfFile( m_mapFile, FILE_MAP_ALL_ACCESS, 0, 0, size );
#else
	if( mustExist )
		m_mapFile = shm_open( name, O_RDWR, 0 );
	else if( mustNotExist )
	{
		m_mapFile = shm_open( name, O_RDWR|O_CREAT|O_EXCL, S_IRWXU ); //S_IRWXU gives user wrx permissions
		if( ftruncate( m_mapFile, size ) == -1 )
			close( );
	}
	else
	{
		m_mapFile = shm_open( name, O_RDWR|O_CREAT, S_IRWXU ); //S_IRWXU gives user wrx permissions
		if( ftruncate( m_mapFile, size ) == -1 )
			close( );
	}
	if( m_mapFile != -1 )
	{
		m_buffer = mmap( NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, m_mapFile, 0 );
		m_name = new char[strlen( name ) + 1];
		strcpy( m_name, name );
	}
#endif
	if( isValid() )
		m_size = size;
}

//--------------------------------------------------------------------------
//  Close an area of mapped memory. When all processes have closed their
//  connections the area will be removed by the OS.
//--------------------------------------------------------------------------
void PLMemoryMap::close()
{
#ifdef WIN32
	if( m_buffer )
		UnmapViewOfFile( m_buffer );
	if( m_mapFile )
		CloseHandle( m_mapFile );
	m_mapFile = NULL;
#else
	if( m_buffer )
		munmap( m_buffer, m_size );
	if( m_mapFile != -1 )
		shm_unlink( m_name );
	if( m_name )
		delete[] m_name;
	m_mapFile = -1;
	m_name = NULL;
#endif
	m_buffer = NULL;
	m_size = 0;
}

//--------------------------------------------------------------------------
//  Destructor, closes the connection to the mapped memory.
//--------------------------------------------------------------------------
PLMemoryMap::~PLMemoryMap()
{
	close();
}


#ifdef HAVE_NAMED_MUTEX
PLNamedMutex::PLNamedMutex()
{
#ifdef WIN32
	m_mutex = NULL;
#else
#endif
}

PLNamedMutex::PLNamedMutex( const char *name, bool aquireOnCreate )
{
#ifdef WIN32
	m_mutex = NULL;
#else
#endif
	create( name, aquireOnCreate );
}

void PLNamedMutex::create( const char *name, bool aquireOnCreate )
{
#ifdef WIN32
	m_mutex = CreateMutexA( NULL, aquireOnCreate ? TRUE : FALSE, name );
#else
#endif
}

void PLNamedMutex::aquire()
{
#ifdef WIN32
	DWORD result = WaitForSingleObject( m_mutex, INFINITE );
	if( result != WAIT_OBJECT_0 && result != WAIT_ABANDONED )
		throw( result );
#else
#endif
}

bool PLNamedMutex::aquire( unsigned long millisecs)
{
#ifdef WIN32
	DWORD result = WaitForSingleObject( m_mutex, millisecs );
	return result == WAIT_OBJECT_0 || result == WAIT_ABANDONED;
#else
#endif
}

bool PLNamedMutex::aquireNoWait()
{
#ifdef WIN32
	return WAIT_OBJECT_0 == WaitForSingleObject( m_mutex, 0 );
#else
#endif
}

void PLNamedMutex::release()
{
	if( m_mutex )
		ReleaseMutex( m_mutex );
}

void PLNamedMutex::clear()
{
	release();
	CloseHandle( m_mutex );
}

PLNamedMutex::~PLNamedMutex()
{
	clear();
}

PLNamedMutexLocker::PLNamedMutexLocker( PLNamedMutex *mutex )
{
	m_mutex = mutex;
	m_mutex->aquire();
}

bool PLNamedMutex::isValid()
{
	return m_mutex != NULL;
}

PLNamedMutexLocker::~PLNamedMutexLocker( )
{
	m_mutex ->release();
	m_mutex = NULL;
}

#endif

//--------------------------------------------------------------------------
//  In the following you'll find the driver functions which are
//  needed by the plplot core.
//--------------------------------------------------------------------------

// Device info
#ifdef __cplusplus
extern "C" {
#endif

PLDLLIMPEXP_DRIVER const char* plD_DEVICE_INFO_wxwidgets =
#ifdef PLD_wxwidgets
    "wxwidgets:wxWidgets Driver:1:wxwidgets:51:wxwidgets\n"
#endif
#ifdef PLD_wxpng
    "wxpng:wxWidgets PNG Driver:0:wxwidgets:52:wxpng\n"
#endif
;

#ifdef __cplusplus
}
#endif


#ifdef PLD_wxwidgets

//--------------------------------------------------------------------------
//  void plD_dispatch_init_wxwidgets( PLDispatchTable *pdt )
//
//  Make wxwidgets driver functions known to plplot.
//--------------------------------------------------------------------------
void plD_dispatch_init_wxwidgets( PLDispatchTable *pdt )
{
#ifndef ENABLE_DYNDRIVERS
    pdt->pl_MenuStr = "wxWidgets DC";
    pdt->pl_DevName = "wxwidgets";
#endif
    pdt->pl_type     = plDevType_Interactive;
    pdt->pl_seq      = 51;
    pdt->pl_init     = (plD_init_fp) plD_init_wxwidgets;
    pdt->pl_line     = (plD_line_fp) plD_line_wxwidgets;
    pdt->pl_polyline = (plD_polyline_fp) plD_polyline_wxwidgets;
    pdt->pl_eop      = (plD_eop_fp) plD_eop_wxwidgets;
    pdt->pl_bop      = (plD_bop_fp) plD_bop_wxwidgets;
    pdt->pl_tidy     = (plD_tidy_fp) plD_tidy_wxwidgets;
    pdt->pl_state    = (plD_state_fp) plD_state_wxwidgets;
    pdt->pl_esc      = (plD_esc_fp) plD_esc_wxwidgets;
}

//--------------------------------------------------------------------------
//  plD_init_wxwidgets( PLStream* pls )
//
//  Initialize wxWidgets device.
//--------------------------------------------------------------------------
void plD_init_wxwidgets( PLStream* pls )
{
    // Log_Verbose( "plD_init_wxwidgets()" );

    // default options
    static PLINT text        = 1;
    static PLINT hrshsym     = 0;
	static char  *mfo        = NULL;
	static PLINT mfisize     = 0;

    DrvOpt wx_options[] = {
        { "hrshsym",  DRV_INT, &hrshsym,     "Use Hershey symbol set (hrshsym=0|1)"                      },
        { "text",     DRV_INT, &text,        "Use own text routines (text=0|1)"                          },
		{ "mfo",      DRV_STR, &mfo,         "output metafile"                                           },
		{ "mfisize",  DRV_INT, &mfisize,     "input metafile size"                                       },
        { NULL,       DRV_INT, NULL,         NULL                                                        }
    };

    // Check for and set up driver options
    plParseDrvOpts( wx_options );

    // by default the own text routines are used for wxDC
    if ( text == -1 )
        text = 0;

    // create the new device
	wxPLDevice* dev = new wxPLDevice ( pls, mfo, mfisize, text, hrshsym );
    if ( dev == NULL )
    {
        plexit( "Insufficient memory" );
    }

    // If portrait mode, apply a rotation and set freeaspect
    if ( pls->portrait )
    {
        plsdiori( (PLFLT) ( 4 - ORIENTATION ) );
        pls->freeaspect = 1;
    }

}

#endif  // PLD_wxwidgets


#ifdef PLD_wxpng

//--------------------------------------------------------------------------
//  void plD_dispatch_init_wxpng( PLDispatchTable *pdt )
//
//  Make wxpng driver functions known to plplot.
//--------------------------------------------------------------------------
void plD_dispatch_init_wxpng( PLDispatchTable *pdt )
{
#ifndef ENABLE_DYNDRIVERS
    pdt->pl_MenuStr = "wxWidgets PNG driver";
    pdt->pl_DevName = "wxpng";
#endif
    pdt->pl_type     = plDevType_FileOriented;
    pdt->pl_seq      = 52;
    pdt->pl_init     = (plD_init_fp) plD_init_wxpng;
    pdt->pl_line     = (plD_line_fp) plD_line_wxwidgets;
    pdt->pl_polyline = (plD_polyline_fp) plD_polyline_wxwidgets;
    pdt->pl_eop      = (plD_eop_fp) plD_eop_wxwidgets;
    pdt->pl_bop      = (plD_bop_fp) plD_bop_wxwidgets;
    pdt->pl_tidy     = (plD_tidy_fp) plD_tidy_wxwidgets;
    pdt->pl_state    = (plD_state_fp) plD_state_wxwidgets;
    pdt->pl_esc      = (plD_esc_fp) plD_esc_wxwidgets;
}

//--------------------------------------------------------------------------
//  void plD_init_wxpng( PLStream *pls )
//
//  Initialize wxpng device.
//--------------------------------------------------------------------------
void plD_init_wxpng( PLStream *pls )
{
    // Log_Verbose( "plD_init_wxwidgets()" );

    wxPLDevBase* dev;
    dev = common_init( pls );

    // Initialize family file info
    plFamInit( pls );

    // Prompt for a file name if not already set.
    plOpenFile( pls );

    pls->plbuf_write = 1;             // use the plot buffer!
    pls->dev_flush   = 0;             // No need for flushes
    pls->termin      = 0;             // file oriented device
    pls->graphx      = GRAPHICS_MODE; //  No text mode for this driver (at least for now, might add a console window if I ever figure it out and have the inclination)
    pls->page        = 0;

    dev->showGUI    = false;
    dev->bitmapType = wxBITMAP_TYPE_PNG;
}

#endif  // PLD_wxpng


//--------------------------------------------------------------------------
//  void plD_line_wxwidgets( PLStream *pls, short x1a, short y1a,
//													 short x2a, short y2a )
//
//  Draws a line from (x1a, y1a) to (x2a, y2a).
//--------------------------------------------------------------------------
void plD_line_wxwidgets( PLStream *pls, short x1a, short y1a, short x2a, short y2a )
{
    // Log_Verbose( "plD_line_wxwidgets(x1a=%d, y1a=%d, x2a=%d, y2a=%d)", x1a, y1a, x2a, y2a );

    wxPLDevice* dev = (wxPLDevice *) pls->dev;
    dev->DrawLine( x1a, y1a, x2a, y2a );
}


//--------------------------------------------------------------------------
//  void plD_polyline_wxwidgets( PLStream *pls, short *xa, short *ya,
//															 PLINT npts )
//
//  Draw a poly line - points are in xa and ya arrays.
//--------------------------------------------------------------------------
void plD_polyline_wxwidgets( PLStream *pls, short *xa, short *ya, PLINT npts )
{
    // Log_Verbose( "plD_polyline_wxwidgets()" );

    wxPLDevice* dev = (wxPLDevice *) pls->dev;
    dev->DrawPolyline( xa, ya, npts );
}


//--------------------------------------------------------------------------
//  void plD_eop_wxwidgets( PLStream *pls )
//
//  End of Page. This function is called if a "end of page" is send by the
//  user. This command is ignored if we have the plot embedded in a
//  wxWidgets application, otherwise the application created by the device
//  takes over.
//--------------------------------------------------------------------------
void plD_eop_wxwidgets( PLStream *pls )
{
    // Log_Verbose( "plD_eop_wxwidgets()" );

    wxPLDevice* dev = (wxPLDevice *) pls->dev;
	dev->EndPage( pls );
}


//--------------------------------------------------------------------------
//  void plD_bop_wxwidgets( PLStream *pls )
//
//  Begin of page. Before any plot command, this function is called, If we
//  have already a dc the background is cleared in background color and some
//  state calls are resent - this is because at the first call of this
//  function, a dc does most likely not exist, but state calls are recorded
//  and when a new dc is created this function is called again.
//--------------------------------------------------------------------------
void plD_bop_wxwidgets( PLStream *pls )
{
    // Log_Verbose( "plD_bop_wxwidgets()" );

    wxPLDevice* dev = (wxPLDevice *) pls->dev;
	dev->BeginPage( pls );
}


//--------------------------------------------------------------------------
//  void plD_tidy_wxwidgets( PLStream *pls )
//
//  This function is called, if all plots are done.
//--------------------------------------------------------------------------
void plD_tidy_wxwidgets( PLStream *pls )
{
    // Log_Verbose( "plD_tidy_wxwidgets()" );

    wxPLDevice* dev = (wxPLDevice *) pls->dev;

    delete dev;
    pls->dev = NULL; // since in plcore.c pls->dev is free_mem'd
}


//--------------------------------------------------------------------------
//  void plD_state_wxwidgets( PLStream *pls, PLINT op )
//
//  Handler for several state codes. Here we take care of setting the width
//  and color of the pen.
//--------------------------------------------------------------------------
void plD_state_wxwidgets( PLStream *pls, PLINT op )
{
    // Log_Verbose( "plD_state_wxwidgets(op=%d)", op );

    wxPLDevice* dev = (wxPLDevice *) pls->dev;

    switch ( op )
    {
    case PLSTATE_WIDTH: // 1
        dev->SetWidth( pls );
        break;

    case PLSTATE_COLOR0: // 2
        dev->SetColor( pls );
        break;

    case PLSTATE_COLOR1: // 3
        dev->SetColor( pls );
        break;
    }
}


//--------------------------------------------------------------------------
//  void plD_esc_wxwidgets( PLStream *pls, PLINT op, void *ptr )
//
//  Handler for several escape codes. Here we take care of filled polygons,
//  XOR or copy mode, initialize device (install dc from outside), and if
//  there is freetype support, rerendering of text.
//--------------------------------------------------------------------------
void plD_esc_wxwidgets( PLStream *pls, PLINT op, void *ptr )
{
    // Log_Verbose( "plD_esc_wxwidgets(op=%d, ptr=%x)", op, ptr );

    wxPLDevice* dev = (wxPLDevice *) pls->dev;

    switch ( op )
    {
    case PLESC_FILL:
		dev->FillPolygon( pls );
        break;

    case PLESC_XORMOD:
        // switch between wxXOR and wxCOPY
        // if( dev->ready ) {
        //                  if( dev->m_dc->GetLogicalFunction() == wxCOPY )
        //                          dev->m_dc->SetLogicalFunction( wxXOR );
        //                  else if( dev->m_dc->GetLogicalFunction() == wxXOR )
        //                          dev->m_dc->SetLogicalFunction( wxCOPY );
        //          }
        break;

    case PLESC_DEVINIT:
        dev->SetExternalBuffer( pls, ptr );
        break;

    case PLESC_HAS_TEXT:
        dev->ProcessString( pls, (EscText *) ptr );
        break;

    case PLESC_RESIZE:
    {
        wxSize* size = (wxSize *) ptr;
        dev->SetSize( pls, size->GetWidth(), size->GetHeight() );
    }
    break;

    case PLESC_CLEAR:
        dev->ClearBackground( pls, pls->sppxmi, pls->sppymi,
			pls->sppxma, pls->sppyma );
        break;

    case PLESC_FLUSH:        // forced update of the window
		//currently does nothing
        break;

    case PLESC_GETC:
        //if ( dev->ownGUI )
            //GetCursorCmd( pls, (PLGraphicsIn *) ptr );
        break;

    case PLESC_FIXASPECT:
		dev->FixAspectRatio( *((bool *)ptr) );
        //if ( dev->ownGUI )
            //GetCursorCmd( pls, (PLGraphicsIn *) ptr );
        break;

    default:
        break;
    }
}


//--------------------------------------------------------------------------
// GetCursorCmd()
//
// Waits for a graphics input event and returns coordinates.
//--------------------------------------------------------------------------
/*static void GetCursorCmd( PLStream* pls, PLGraphicsIn* ptr )
{
    // Log_Verbose( "GetCursorCmd" );

    wxPLDevBase  *dev = (wxPLDevBase *) pls->dev;
    PLGraphicsIn *gin = &( dev->gin );

    // Initialize
    plGinInit( gin );
    dev->locate_mode = LOCATE_INVOKED_VIA_API;
    dev->draw_xhair  = true;

    // Run event loop until a point is selected
    wxRunApp( pls, false );

    *ptr = *gin;
    if ( dev->locate_mode )
    {
        dev->locate_mode = 0;
        dev->draw_xhair  = false;
    }
}*/



//--------------------------------------------------------------------------
//  void wxRunApp( PLStream *pls, bool runonce )
//
//  This is a hacked wxEntry-function, so that wxUninitialize is not
//  called twice. Here we actually start the wxApplication.
//--------------------------------------------------------------------------
/*static void wxRunApp( PLStream *pls, bool runonce )
{
    // Log_Verbose( "wxRunApp" );

    wxPLDevBase* dev = (wxPLDevBase *) pls->dev;

    dev->waiting = true;
    wxTRY
    {
        class CallOnExit
        {
public:
            // only call OnExit if exit is true (i.e. due an exception)
            ~CallOnExit() { if ( exit ) wxPLGetApp().OnExit();}
            bool exit;
        } callOnExit;

        callOnExit.exit = true;
        wxPLGetApp().SetAdvanceFlag( runonce );
        wxPLGetApp().SetRefreshFlag();

        // add an idle event is necessary for Linux (wxGTK2)
        // but not for Windows, but it doesn't harm
        wxIdleEvent event;
        wxPLGetApp().AddPendingEvent( event );
        wxPLGetApp().OnRun();           // start wxWidgets application
        callOnExit.exit = false;
    }
    wxCATCH_ALL( wxPLGetApp().OnUnhandledException(); plexit( "Problem running wxWidgets!" ); )

    if ( dev->exit )
    {
        wxPLGetApp().OnExit();
        plexit( "" );
    }

    dev->waiting = false;
}*/
