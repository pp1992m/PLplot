/* $Id$
   $Log$
   Revision 1.16  1993/07/16 22:14:18  mjl
   Simplified and fixed dpi settings.

 * Revision 1.15  1993/07/01  21:59:43  mjl
 * Changed all plplot source files to include plplotP.h (private) rather than
 * plplot.h.  Rationalized namespace -- all externally-visible plplot functions
 * now start with "pl"; device driver functions start with "plD_".
 *
 * Revision 1.14  1993/04/26  20:01:59  mjl
 * Changed time type from long to time_t.
 *
 * Revision 1.13  1993/03/28  08:44:21  mjl
 * Eliminated name conflict of getdate() function with builtin of same name
 * on some systems.
 *
 * Revision 1.12  1993/03/15  21:39:19  mjl
 * Changed all _clear/_page driver functions to the names _eop/_bop, to be
 * more representative of what's actually going on.
 *
 * Revision 1.11  1993/03/06  04:57:25  mjl
 * Fix to ensure that a new segment is begun after a line width change.
 *
 * Revision 1.10  1993/03/03  19:42:06  mjl
 * Changed PLSHORT -> short everywhere; now all device coordinates are expected
 * to fit into a 16 bit address space (reasonable, and good for performance).
 *
 * Revision 1.9  1993/03/03  16:18:35  mjl
 * Cleaned up prolog block, fixed (?) default line width setting.
 *
 * Revision 1.8  1993/02/27  04:46:40  mjl
 * Fixed errors in ordering of header file inclusion.  "plplotP.h" should
 * always be included first.
 *
 * Revision 1.7  1993/02/22  23:15:12  mjl
 * Eliminated the plP_adv() driver calls, as these were made obsolete by recent
 * changes to plmeta and plrender.  Also eliminated page clear commands from
 * plP_tidy() -- plend now calls plP_clr() and plP_tidy() explicitly.  Eliminated
 * the (atend) specification for BoundingBox, since this isn't recognized by
 * some programs; instead enough space is left for it and a rewind and
 * subsequent write is done from plD_tidy_ps().  Familying is no longer directly
 * supported by the PS driver as a result.  The output done by the @end
 * function was eliminated to reduce aggravation when viewing with ghostview.
 *
 * Revision 1.6  1993/01/23  05:41:51  mjl
 * Changes to support new color model, polylines, and event handler support
 * (interactive devices only).
 *
 * Revision 1.5  1992/11/07  07:48:47  mjl
 * Fixed orientation operation in several files and standardized certain startup
 * operations. Fixed bugs in various drivers.
 *
 * Revision 1.4  1992/10/22  17:04:56  mjl
 * Fixed warnings, errors generated when compling with HP C++.
 *
 * Revision 1.3  1992/09/30  18:24:58  furnish
 * Massive cleanup to irradicate garbage code.  Almost everything is now
 * prototyped correctly.  Builds on HPUX, SUNOS (gcc), AIX, and UNICOS.
 *
 * Revision 1.2  1992/09/29  04:44:48  furnish
 * Massive clean up effort to remove support for garbage compilers (K&R).
 *
 * Revision 1.1  1992/05/20  21:32:42  furnish
 * Initial checkin of the whole PLPLOT project.
 *
*/

/*	ps.c

	PLPLOT PostScript device driver.
*/
#ifdef PS

#include "plplotP.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "drivers.h"

/* Prototypes for functions in this file. */

static char *pl_getdate(void);
static void plD_init_ps(PLStream *);

/* top level declarations */

#define LINELENGTH      70
#define COPIES          1
#define XSIZE           540	/* 7.5" x 10"  (72 points equal 1 inch) */
#define YSIZE           720
#define ENLARGE         5
#define XPSSIZE         ENLARGE*XSIZE
#define YPSSIZE         ENLARGE*YSIZE
#define XOFFSET         36	/* Offsets are .5" each */
#define YOFFSET         36
#define PSX             XPSSIZE-1
#define PSY             YPSSIZE-1

#define OF		pls->OutFile

static char outbuf[128];
static int llx = XPSSIZE, lly = YPSSIZE, urx = 0, ury = 0, ptcnt;
static PLINT orient;

/* (dev) will get passed in eventually, so this looks weird right now */

static PLDev device;
static PLDev *dev = &device;

/*----------------------------------------------------------------------*\
* plD_init_ps()
*
* Initialize device.
\*----------------------------------------------------------------------*/

void
plD_init_psm(PLStream *pls)
{
    if (!pls->colorset)
	pls->color = 0;		/* no color by default: user can override */
    plD_init_ps(pls);
}

void
plD_init_psc(PLStream *pls)
{
    pls->color = 1;		/* always color */
    plD_init_ps(pls);
}

static void
plD_init_ps(PLStream *pls)
{
    float r, g, b;
    float pxlx = YPSSIZE/LPAGE_X;
    float pxly = XPSSIZE/LPAGE_Y;

    pls->termin = 0;		/* not an interactive terminal */
    pls->icol0 = 1;
    pls->bytecnt = 0;
    pls->page = 0;
    pls->family = 0;		/* I don't want to support familying here */

    if (pls->widthset) {
	if (pls->width < 1 || pls->width > 10) {
	    fprintf(stderr, "\nInvalid pen width selection.");
	    pls->width = 1;
	}
	pls->widthset = 0;
    }
    else
	pls->width = 1;

/* Prompt for a file name if not already set */

    plOpenFile(pls);

/* Set up device parameters */

    dev->xold = UNDEFINED;
    dev->yold = UNDEFINED;

    plP_setpxl(pxlx, pxly);

/* Because portrait mode addressing is used by postscript, we need to
   rotate by 90 degrees to get the right mapping. */

    dev->xmin = 0;
    dev->ymin = 0;

    orient = pls->orient + 1;
    if (orient%2 == 1) {
	dev->xmax = PSY;
	dev->ymax = PSX;
    }
    else {
	dev->xmax = PSX;
	dev->ymax = PSY;
    }

    dev->xlen = dev->xmax - dev->xmin;
    dev->ylen = dev->ymax - dev->ymin;

    plP_setphy(dev->xmin, dev->xmax, dev->ymin, dev->ymax);

/* Header comments into PostScript file */

    fprintf(OF, "%%!PS-Adobe-2.0 EPSF-2.0\n");
    fprintf(OF, "%%%%BoundingBox:         \n");
    fprintf(OF, "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");

    fprintf(OF, "%%%%Title: PLPLOT Graph\n");
    fprintf(OF, "%%%%Creator: PLPLOT Version 4.0\n");
    fprintf(OF, "%%%%CreationDate: %s\n", pl_getdate());
    fprintf(OF, "%%%%Pages: (atend)\n");
    fprintf(OF, "%%%%EndComments\n\n");

/* Definitions */
/* Save VM state */

    fprintf(OF, "/PSSave save def\n");

/* Define a dictionary and start using it */

    fprintf(OF, "/PSDict 200 dict def\n");
    fprintf(OF, "PSDict begin\n");

    fprintf(OF, "/@restore /restore load def\n");
    fprintf(OF, "/restore\n");
    fprintf(OF, "   {vmstatus pop\n");
    fprintf(OF, "    dup @VMused lt {pop @VMused} if\n");
    fprintf(OF, "    exch pop exch @restore /@VMused exch def\n");
    fprintf(OF, "   } def\n");
    fprintf(OF, "/@pri\n");
    fprintf(OF, "   {\n");
    fprintf(OF, "    ( ) print\n");
    fprintf(OF, "    (                                       ) cvs print\n");
    fprintf(OF, "   } def\n");
    fprintf(OF, "/@copies\n");	/* n @copies - */
    fprintf(OF, "   {\n");
    fprintf(OF, "    /#copies exch def\n");
    fprintf(OF, "   } def\n");

/* - @start -  -- start everything */

    fprintf(OF, "/@start\n");
    fprintf(OF, "   {\n");
    fprintf(OF, "    vmstatus pop /@VMused exch def pop\n");
    fprintf(OF, "   } def\n");

/* - @end -  -- finished */

    fprintf(OF, "/@end\n");
    fprintf(OF, "   {flush\n");
    fprintf(OF, "    end\n");
    fprintf(OF, "    PSSave restore\n");
    fprintf(OF, "   } def\n");

/* bop -  -- begin a new page */

    fprintf(OF, "/bop\n");
    fprintf(OF, "   {\n");
    fprintf(OF, "    /SaveImage save def\n");
    if (pls->color) {
	fprintf(OF, "    Z %d %d M %d %d D %d %d D %d %d D %d %d",
		0, 0, 0, PSY, PSX, PSY, PSX, 0, 0, 0);
	r = ((float) pls->bgcolor.r) / 255.;
	g = ((float) pls->bgcolor.g) / 255.;
	b = ((float) pls->bgcolor.b) / 255.;
	fprintf(OF, "    closepath %f %f %f setrgbcolor fill\n",
		r, g, b);
    }
    fprintf(OF, "   } def\n");

/* - eop -  -- end a page */

    fprintf(OF, "/eop\n");
    fprintf(OF, "   {\n");
    fprintf(OF, "    showpage\n");
    fprintf(OF, "    SaveImage restore\n");
    fprintf(OF, "   } def\n");

/* Set line parameters */

    fprintf(OF, "/@line\n");
    fprintf(OF, "   {0 setlinecap\n");
    fprintf(OF, "    0 setlinejoin\n");
    fprintf(OF, "    1 setmiterlimit\n");
    fprintf(OF, "   } def\n");

/* d @hsize -  horizontal clipping dimension */

    fprintf(OF, "/@hsize   {/hs exch def} def\n");
    fprintf(OF, "/@vsize   {/vs exch def} def\n");

/* d @hoffset - shift for the plots */

    fprintf(OF, "/@hoffset {/ho exch def} def\n");
    fprintf(OF, "/@voffset {/vo exch def} def\n");

/* Default line width */

    fprintf(OF, "/lw %d def\n", pls->width);
    fprintf(OF, "/@lwidth  {lw setlinewidth} def\n");

/* Setup user specified offsets, scales, sizes for clipping */

    fprintf(OF, "/@SetPlot\n");
    fprintf(OF, "   {\n");
    fprintf(OF, "    ho vo translate\n");
    fprintf(OF, "    XScale YScale scale\n");
    fprintf(OF, "    lw setlinewidth\n");
    fprintf(OF, "   } def\n");

/* Setup x & y scales */

    fprintf(OF, "/XScale\n");
    fprintf(OF, "   {hs %d div} def\n", YPSSIZE);
    fprintf(OF, "/YScale\n");
    fprintf(OF, "   {vs %d div} def\n", XPSSIZE);

/* Stroke definitions, to keep output file as small as possible */

    fprintf(OF, "/M {moveto} def\n");
    fprintf(OF, "/D {lineto} def\n");
    fprintf(OF, "/S {stroke} def\n");
    fprintf(OF, "/Z {stroke newpath} def\n");

/* End of dictionary definition */

    fprintf(OF, "end\n\n");

/* Set up the plots */

    fprintf(OF, "PSDict begin\n");
    fprintf(OF, "@start\n");
    fprintf(OF, "%d @copies\n", COPIES);
    fprintf(OF, "@line\n");
    fprintf(OF, "%d @hsize\n", YSIZE);
    fprintf(OF, "%d @vsize\n", XSIZE);
    fprintf(OF, "%d @hoffset\n", YOFFSET);
    fprintf(OF, "%d @voffset\n", XOFFSET);

    fprintf(OF, "@SetPlot\n\n");
}

/*----------------------------------------------------------------------*\
* plD_line_ps()
*
* Draw a line in the current color from (x1,y1) to (x2,y2).
\*----------------------------------------------------------------------*/

void
plD_line_ps(PLStream *pls, short x1a, short y1a, short x2a, short y2a)
{
    int x1 = x1a, y1 = y1a, x2 = x2a, y2 = y2a;

    if (pls->linepos + 21 > LINELENGTH) {
	putc('\n', OF);
	pls->linepos = 0;
    }
    else
	putc(' ', OF);

    pls->bytecnt++;

/* Because portrait mode addressing is used here, we need to complement
   the orientation flag to get the right mapping. */

    orient = pls->orient + 1;
    plRotPhy(orient, dev, &x1, &y1, &x2, &y2);

    if (x1 == dev->xold && y1 == dev->yold && ptcnt < 40) {
	sprintf(outbuf, "%d %d D", x2, y2);
	ptcnt++;
    }
    else {
	sprintf(outbuf, "Z %d %d M %d %d D", x1, y1, x2, y2);
	llx = MIN(llx, x1);
	lly = MIN(lly, y1);
	urx = MAX(urx, x1);
	ury = MAX(ury, y1);
	ptcnt = 1;
    }
    llx = MIN(llx, x2);
    lly = MIN(lly, y2);
    urx = MAX(urx, x2);
    ury = MAX(ury, y2);

    fprintf(OF, "%s", outbuf);
    pls->bytecnt += strlen(outbuf);
    dev->xold = x2;
    dev->yold = y2;
    pls->linepos += 21;
}

/*----------------------------------------------------------------------*\
* plD_polyline_ps()
*
* Draw a polyline in the current color.
\*----------------------------------------------------------------------*/

void
plD_polyline_ps(PLStream *pls, short *xa, short *ya, PLINT npts)
{
    PLINT i;

    for (i = 0; i < npts - 1; i++)
	plD_line_ps(pls, xa[i], ya[i], xa[i + 1], ya[i + 1]);
}

/*----------------------------------------------------------------------*\
* plD_eop_ps()
*
* End of page.
\*----------------------------------------------------------------------*/

void
plD_eop_ps(PLStream *pls)
{
    fprintf(OF, " S\neop\n");
}

/*----------------------------------------------------------------------*\
* plD_bop_ps()
*
* Set up for the next page.
\*----------------------------------------------------------------------*/

void
plD_bop_ps(PLStream *pls)
{
    dev->xold = UNDEFINED;
    dev->yold = UNDEFINED;

    pls->page++;
    fprintf(OF, " bop\n");
    fprintf(OF, "%%%%Page: %d %d\n", pls->page, pls->page);
    pls->linepos = 0;
}

/*----------------------------------------------------------------------*\
* plD_tidy_ps()
*
* Close graphics file or otherwise clean up.
\*----------------------------------------------------------------------*/

void
plD_tidy_ps(PLStream *pls)
{
    fprintf(OF, "@end\n\n");
    fprintf(OF, "%%%%Trailer\n");

    llx /= ENLARGE;
    lly /= ENLARGE;
    urx /= ENLARGE;
    ury /= ENLARGE;
    llx += XOFFSET;
    lly += YOFFSET;
    urx += XOFFSET;
    ury += YOFFSET;
    fprintf(OF, "%%%%Pages: %d\n", pls->page);

/* Backtrack to write the BoundingBox at the beginning */
/* Some applications don't like it atend */

    rewind(OF);
    fprintf(OF, "%%!PS-Adobe-2.0 EPSF-2.0\n");
    fprintf(OF, "%%%%BoundingBox: %d %d %d %d\n", llx, lly, urx, ury);
    fclose(OF);
    pls->fileset = 0;
    pls->page = 0;
    pls->linepos = 0;
    OF = NULL;
}

/*----------------------------------------------------------------------*\
* plD_color_ps()
*
* Set pen color.
\*----------------------------------------------------------------------*/

void
plD_color_ps(PLStream *pls)
{
    float r, g, b;

    if (pls->color) {
	r = ((float) pls->curcolor.r) / 255.0;
	g = ((float) pls->curcolor.g) / 255.0;
	b = ((float) pls->curcolor.b) / 255.0;

	fprintf(OF, " S %f %f %f setrgbcolor\n", r, g, b);
    }
}

/*----------------------------------------------------------------------*\
* plD_text_ps()
*
* Switch to text mode.
\*----------------------------------------------------------------------*/

void
plD_text_ps(PLStream *pls)
{
}

/*----------------------------------------------------------------------*\
* plD_graph_ps()
*
* Switch to graphics mode.
\*----------------------------------------------------------------------*/

void
plD_graph_ps(PLStream *pls)
{
}

/*----------------------------------------------------------------------*\
* plD_width_ps()
*
* Set pen width.
\*----------------------------------------------------------------------*/

void
plD_width_ps(PLStream *pls)
{
    if (pls->width < 1 || pls->width > 10)
	fprintf(stderr, "\nInvalid pen width selection.");
    else {
	fprintf(OF, " S\n/lw %d def\n@lwidth\n", pls->width);
    }
    dev->xold = UNDEFINED;
    dev->yold = UNDEFINED;
}

/*----------------------------------------------------------------------*\
* plD_esc_ps()
*
* Escape function.
\*----------------------------------------------------------------------*/

void
plD_esc_ps(PLStream *pls, PLINT op, void *ptr)
{
}

/*----------------------------------------------------------------------*\
* pl_getdate()
*
* Get the date and time
\*----------------------------------------------------------------------*/

static char *
pl_getdate(void)
{
    int len;
    time_t t;
    char *p;

    t = time((time_t *) 0);
    p = ctime(&t);
    len = strlen(p);
    *(p + len - 1) = '\0';	/* zap the newline character */
    return p;
}

#else
int 
pldummy_ps()
{
    return 0;
}

#endif				/* PS */
