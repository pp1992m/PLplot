/* Demonstration of contour plotting */
/* $Id$
   $Log$
   Revision 1.5  1993/02/22 23:16:17  mjl
   Changed over to new style of initialization using plinit(), and added
   function to parse plplot command line flags.

 * Revision 1.4  1993/01/23  06:10:30  mjl
 * Instituted exit codes for all example codes.  Also deleted color functions
 * no longer supported (plancol).  Enhanced x09c to exploit new contour
 * capabilities.
 *
 * Revision 1.3  1992/09/30  18:25:22  furnish
 * Massive cleanup to irradicate garbage code.  Almost everything is now
 * prototyped correctly.  Builds on HPUX, SUNOS (gcc), AIX, and UNICOS.
 *
 * Revision 1.2  1992/09/29  04:45:17  furnish
 * Massive clean up effort to remove support for garbage compilers (K&R).
 *
 * Revision 1.1  1992/05/20  21:32:57  furnish
 * Initial checkin of the whole PLPLOT project.
 *
*/

/* Note the compiler should automatically convert all non-pointer arguments
   to satisfy the prototype, but some have problems with constants. */

#define PL_NEED_MALLOC
#include "plplot.h"
#include <stdlib.h>
#include <math.h>

#define XPTS    35
#define YPTS    46
#define XSPA    2./(XPTS-1)
#define YSPA    2./(YPTS-1)
#define PI	3.1415926535897932384

PLFLT tr[6] =
{XSPA, 0.0, -1.0, 0.0, YSPA, -1.0};

void
mypltr(PLFLT x, PLFLT y, PLFLT *tx, PLFLT *ty, void *pltr_data)
{
    *tx = tr[0] * x + tr[1] * y + tr[2];
    *ty = tr[3] * x + tr[4] * y + tr[5];
}

static PLFLT clevel[11] =
{-1., -.8, -.6, -.4, -.2, 0, .2, .4, .6, .8, 1.};

int
main(int argc, char *argv[])
{
    int i, j;
    PLFLT xx, yy, argx, argy, distort;
    static PLINT mark = 1500, space = 1500;

    PLFLT **z, **w;
    PLFLT xg1[XPTS], yg1[YPTS];
    PLcGrid  cgrid1;
    PLcGrid2 cgrid2;

/* Parse and process command line arguments */

    (void) plParseInternalOpts(&argc, argv, PL_PARSE_FULL);

/* Initialize plplot */

    plinit();

/* Set up function arrays */

    Alloc2dGrid(&z, XPTS, YPTS);
    Alloc2dGrid(&w, XPTS, YPTS);

    for (i = 0; i < XPTS; i++) {
	xx = (double) (i - (XPTS / 2)) / (double) (XPTS / 2);
	for (j = 0; j < YPTS; j++) {
	    yy = (double) (j - (YPTS / 2)) / (double) (YPTS / 2) - 1.0;
	    z[i][j] = xx * xx - yy * yy;
	    w[i][j] = 2 * xx * yy;
	}
    }

/* Set up grids */

    cgrid1.xg = xg1;
    cgrid1.yg = yg1;
    cgrid1.nx = XPTS;
    cgrid1.ny = YPTS;

    Alloc2dGrid(&cgrid2.xg, XPTS, YPTS);
    Alloc2dGrid(&cgrid2.yg, XPTS, YPTS);
    cgrid2.nx = XPTS;
    cgrid2.ny = YPTS;

    for (i = 0; i < XPTS; i++) {
	for (j = 0; j < YPTS; j++) {
	    mypltr((PLFLT) i, (PLFLT) j, &xx, &yy, NULL);

	    argx = xx * PI/2;
	    argy = yy * PI/2;
	    distort = 0.4;

	    cgrid1.xg[i] = xx + distort * cos(argx);
	    cgrid1.yg[j] = yy - distort * cos(argy);

	    cgrid2.xg[i][j] = xx + distort * cos(argx) * cos(argy);
	    cgrid2.yg[i][j] = yy - distort * cos(argx) * cos(argy);
	}
    }

/* Plot using identity transform */

    plenv((PLFLT) -1.0, (PLFLT) 1.0, (PLFLT) -1.0, (PLFLT) 1.0, 0, 0);
    plcol(2);
    plcont(z, XPTS, YPTS, 1, XPTS, 1, YPTS, clevel, 11, mypltr, NULL);
    plstyl(1, &mark, &space);
    plcol(3);
    plcont(w, XPTS, YPTS, 1, XPTS, 1, YPTS, clevel, 11, mypltr, NULL);
    plstyl(0, &mark, &space);
    plcol(1);
    pllab("X Coordinate", "Y Coordinate", "Streamlines of flow");

/* Plot using 1d coordinate transform */

    plenv((PLFLT) -1.0, (PLFLT) 1.0, (PLFLT) -1.0, (PLFLT) 1.0, 0, 0);
    plcol(2);
    plcont(z, XPTS, YPTS, 1, XPTS, 1, YPTS, clevel, 11,
	   pltr1, (void *) &cgrid1);

    plstyl(1, &mark, &space);
    plcol(3);
    plcont(w, XPTS, YPTS, 1, XPTS, 1, YPTS, clevel, 11,
	   pltr1, (void *) &cgrid1);
    plstyl(0, &mark, &space);
    plcol(1);
    pllab("X Coordinate", "Y Coordinate", "Streamlines of flow");

/* Plot using 2d coordinate transform */

    plenv((PLFLT) -1.0, (PLFLT) 1.0, (PLFLT) -1.0, (PLFLT) 1.0, 0, 0);
    plcol(2);
    plcont(z, XPTS, YPTS, 1, XPTS, 1, YPTS, clevel, 11,
	   pltr2, (void *) &cgrid2);

    plstyl(1, &mark, &space);
    plcol(3);
    plcont(w, XPTS, YPTS, 1, XPTS, 1, YPTS, clevel, 11,
	   pltr2, (void *) &cgrid2);
    plstyl(0, &mark, &space);
    plcol(1);
    pllab("X Coordinate", "Y Coordinate", "Streamlines of flow");

    plend();
    free((VOID *) w);
    free((VOID *) z);
    exit(0);
}
