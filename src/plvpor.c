/* $Id$
   $Log$
   Revision 1.8  1994/03/23 08:35:28  mjl
   All external API source files: replaced call to plexit() on simple
   (recoverable) errors with simply printing the error message (via
   plabort()) and returning.  Should help avoid loss of computer time in some
   critical circumstances (during a long batch run, for example).

 * Revision 1.7  1993/11/15  08:40:42  mjl
 * Comment fixes.
 *
 * Revision 1.6  1993/07/01  22:13:47  mjl
 * Changed all plplot source files to include plplotP.h (private) rather than
 * plplot.h.  Rationalized namespace -- all externally-visible internal
 * plplot functions now start with "plP_".
*/

/*	plvpor.c

	Functions dealing with viewports.
*/

#include "plplotP.h"

/*----------------------------------------------------------------------*\
* void plenv()
*
* Simple interface for defining viewport and window. If "just"=1,
* X and Y scales will be the same, otherwise they are scaled
* independently. The "axis" parameter is interpreted as follows:
*
*	axis=-2 : draw no box, axis or labels
*	axis=-1 : draw box only
*	axis= 0 : Draw box and label with coordinates
*	axis= 1 : Also draw the coordinate axes
*	axis= 2 : Draw a grid at major tick positions
*	axis=10 : Logarithmic X axis, Linear Y axis, No X=0 axis
*	axis=11 : Logarithmic X axis, Linear Y axis, X=0 axis
*	axis=20 : Linear X axis, Logarithmic Y axis, No Y=0 axis
*	axis=21 : Linear X axis, Logarithmic Y axis, Y=0 axis
*	axis=30 : Logarithmic X and Y axes
\*----------------------------------------------------------------------*/

void
c_plenv(PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax,
	PLINT just, PLINT axis)
{
    PLINT level;
    PLFLT chrdef, chrht;
    PLFLT lb, rb, tb, bb, dx, dy;
    PLFLT xsize, ysize, xscale, yscale, scale;
    PLFLT spxmin, spxmax, spymin, spymax;
    PLFLT vpxmin, vpxmax, vpymin, vpymax;

    plP_glev(&level);
    if (level < 1) {
	plabort("plenv: Please call plinit first");
	return;
    }
    if (xmin == xmax) {
	plabort("plenv: Invalid xmin and xmax arguments");
	return;
    }
    if (ymin == ymax) {
	plabort("plenv: Invalid ymin and ymax arguments");
	return;
    }
    if ((just != 0) && (just != 1)) {
	plabort("plenv: Invalid just option");
	return;
    }

    pladv(0);
    if (just == 0)
	plvsta();
    else {
	plgchr(&chrdef, &chrht);
	lb = 8.0 * chrht;
	rb = 5.0 * chrht;
	tb = 5.0 * chrht;
	bb = 5.0 * chrht;
	dx = ABS(xmax - xmin);
	dy = ABS(ymax - ymin);
	plgspa(&spxmin, &spxmax, &spymin, &spymax);
	xsize = spxmax - spxmin;
	ysize = spymax - spymin;
	xscale = dx / (xsize - lb - rb);
	yscale = dy / (ysize - tb - bb);
	scale = MAX(xscale, yscale);
	vpxmin = MAX(lb, 0.5 * (xsize - dx / scale));
	vpxmax = vpxmin + (dx / scale);
	vpymin = MAX(bb, 0.5 * (ysize - dy / scale));
	vpymax = vpymin + (dy / scale);

	plsvpa(vpxmin, vpxmax, vpymin, vpymax);
    }
    plwind(xmin, xmax, ymin, ymax);
    if (axis == -2);
    else if (axis == -1)
	plbox("bc", (PLFLT) 0.0, 0, "bc", (PLFLT) 0.0, 0);
    else if (axis == 0)
	plbox("bcnst", (PLFLT) 0.0, 0, "bcnstv", (PLFLT) 0.0, 0);
    else if (axis == 1)
	plbox("abcnst", (PLFLT) 0.0, 0, "abcnstv", (PLFLT) 0.0, 0);
    else if (axis == 2)
	plbox("abcgnst", (PLFLT) 0.0, 0, "abcgnstv", (PLFLT) 0.0, 0);
    else if (axis == 10)
	plbox("bclnst", (PLFLT) 0.0, 0, "bcnstv", (PLFLT) 0.0, 0);
    else if (axis == 11)
	plbox("bclnst", (PLFLT) 0.0, 0, "abcnstv", (PLFLT) 0.0, 0);
    else if (axis == 20)
	plbox("bcnst", (PLFLT) 0.0, 0, "bclnstv", (PLFLT) 0.0, 0);
    else if (axis == 21)
	plbox("bcnst", (PLFLT) 0.0, 0, "abclnstv", (PLFLT) 0.0, 0);
    else if (axis == 30)
	plbox("bclnst", (PLFLT) 0.0, 0, "bclnstv", (PLFLT) 0.0, 0);
    else
	plwarn("plenv: Invalid axis argument");
}

/*----------------------------------------------------------------------*\
* void plvsta()
*
* Defines a "standard" viewport with seven character heights for
* the left margin and four character heights everywhere else.
\*----------------------------------------------------------------------*/

void
c_plvsta(void)
{
    PLFLT xmin, xmax, ymin, ymax;
    PLFLT chrdef, chrht, spdxmi, spdxma, spdymi, spdyma;
    PLINT level;

    plP_glev(&level);
    if (level < 1) {
	plabort("plvsta: Please call plinit first");
	return;
    }

    plgchr(&chrdef, &chrht);
    plP_gspd(&spdxmi, &spdxma, &spdymi, &spdyma);

/*  Find out position of subpage boundaries in millimetres, reduce by */
/*  the desired border, and convert back into normalized subpage */
/*  coordinates */

    xmin = plP_dcscx(plP_mmdcx((PLFLT) (plP_dcmmx(spdxmi) + 8 * chrht)));
    xmax = plP_dcscx(plP_mmdcx((PLFLT) (plP_dcmmx(spdxma) - 5 * chrht)));
    ymin = plP_dcscy(plP_mmdcy((PLFLT) (plP_dcmmy(spdymi) + 5 * chrht)));
    ymax = plP_dcscy(plP_mmdcy((PLFLT) (plP_dcmmy(spdyma) - 5 * chrht)));

    plvpor(xmin, xmax, ymin, ymax);
}

/*----------------------------------------------------------------------*\
* void plvpor()
*
* Creates a viewport with the specified normalized subpage coordinates.
\*----------------------------------------------------------------------*/

void
c_plvpor(PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax)
{
    PLFLT spdxmi, spdxma, spdymi, spdyma;
    PLFLT vpdxmi, vpdxma, vpdymi, vpdyma;
    PLINT vppxmi, vppxma, vppymi, vppyma;
    PLINT clpxmi, clpxma, clpymi, clpyma;
    PLINT phyxmi, phyxma, phyymi, phyyma;
    PLINT nx, ny, cs;
    PLINT level;

    plP_glev(&level);
    if (level < 1) {
	plabort("plvpor: Please call plinit first");
	return;
    }
    if ((xmin >= xmax) || (ymin >= ymax)) {
	plabort("plvpor: Invalid limits");
	return;
    }
    plP_gsub(&nx, &ny, &cs);
    if ((cs <= 0) || (cs > (nx * ny))) {
	plabort("plvpor: Please call pladv or plenv to go to a subpage");
	return;
    }

    plP_gspd(&spdxmi, &spdxma, &spdymi, &spdyma);
    vpdxmi = spdxmi + (spdxma - spdxmi) * xmin;
    vpdxma = spdxmi + (spdxma - spdxmi) * xmax;
    vpdymi = spdymi + (spdyma - spdymi) * ymin;
    vpdyma = spdymi + (spdyma - spdymi) * ymax;
    plP_svpd(vpdxmi, vpdxma, vpdymi, vpdyma);

    vppxmi = plP_dcpcx(vpdxmi);
    vppxma = plP_dcpcx(vpdxma);
    vppymi = plP_dcpcy(vpdymi);
    vppyma = plP_dcpcy(vpdyma);
    plP_svpp(vppxmi, vppxma, vppymi, vppyma);

    plP_gphy(&phyxmi, &phyxma, &phyymi, &phyyma);
    clpxmi = MAX(vppxmi, phyxmi);
    clpxma = MIN(vppxma, phyxma);
    clpymi = MAX(vppymi, phyymi);
    clpyma = MIN(vppyma, phyyma);
    plP_sclp(clpxmi, clpxma, clpymi, clpyma);

    plP_slev(2);
}

/*----------------------------------------------------------------------*\
* void plvpas()
*
* Creates the largest viewport of the specified aspect ratio that fits
* within the specified normalized subpage coordinates.
\*----------------------------------------------------------------------*/

void
c_plvpas(PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax, PLFLT aspect)
{
    PLFLT vpxmi, vpxma, vpymi, vpyma;
    PLINT level;
    PLFLT vpxmid, vpymid, vpxlen, vpylen, w_aspect, ratio;

    plP_glev(&level);
    if (level < 1) {
	plabort("plvpas: Please call plinit first");
	return;
    }
    if ((xmin >= xmax) || (ymin >= ymax)) {
	plabort("plvpas: Invalid limits");
	return;
    }

    if (aspect <= 0.0) {
	c_plvpor(xmin, xmax, ymin, ymax);
	return;
    }

    vpxmi = plP_dcmmx(xmin);
    vpxma = plP_dcmmx(xmax);
    vpymi = plP_dcmmy(ymin);
    vpyma = plP_dcmmy(ymax);

    vpxmid = (vpxmi + vpxma) / 2.;
    vpymid = (vpymi + vpyma) / 2.;

    vpxlen = vpxma - vpxmi;
    vpylen = vpyma - vpymi;

    w_aspect = vpylen / vpxlen;
    ratio = aspect / w_aspect;

/*
* If ratio < 1, you are requesting an aspect ratio (y/x) less than the natural
* aspect ratio of the specified window, and you will need to reduce the length
* in y correspondingly.  Similarly, for ratio > 1, x length must be reduced.
*/

    if (ratio <= 0.) {
	plabort("plvpas: Error in aspect ratio setting");
	return;
    }
    else if (ratio < 1.)
	vpylen = vpylen * ratio;
    else
	vpxlen = vpxlen / ratio;

    vpxmi = vpxmid - vpxlen / 2.;
    vpxma = vpxmid + vpxlen / 2.;
    vpymi = vpymid - vpylen / 2.;
    vpyma = vpymid + vpylen / 2.;

    plsvpa(vpxmi, vpxma, vpymi, vpyma);
}

/*----------------------------------------------------------------------*\
* void plvasp()
*
* Sets the edges of the viewport with the given aspect ratio, leaving
* room for labels.
\*----------------------------------------------------------------------*/

void
c_plvasp(PLFLT aspect)
{
    PLINT level;
    PLFLT chrdef, chrht, spxmin, spxmax, spymin, spymax;
    PLFLT vpxmin, vpxmax, vpymin, vpymax;
    PLFLT xsize, ysize, nxsize, nysize;
    PLFLT lb, rb, tb, bb;

    plP_glev(&level);
    if (level < 1) {
	plabort("plvasp: Please call plinit first");
	return;
    }

    plgchr(&chrdef, &chrht);
    lb = 8.0 * chrht;
    rb = 5.0 * chrht;
    tb = 5.0 * chrht;
    bb = 5.0 * chrht;
    plgspa(&spxmin, &spxmax, &spymin, &spymax);
    xsize = spxmax - spxmin;
    ysize = spymax - spymin;
    xsize -= lb + rb;		/* adjust for labels */
    ysize -= bb + tb;
    if (aspect * xsize > ysize) {
	nxsize = ysize / aspect;
	nysize = ysize;
    }
    else {
	nxsize = xsize;
	nysize = xsize * aspect;
    }

/* center plot within page */

    vpxmin = .5 * (xsize - nxsize) + lb;
    vpxmax = vpxmin + nxsize;
    vpymin = .5 * (ysize - nysize) + bb;
    vpymax = vpymin + nysize;
    plsvpa(vpxmin, vpxmax, vpymin, vpymax);
}

/*----------------------------------------------------------------------*\
* void plsvpa()
*
* Sets the edges of the viewport to the specified absolute
* coordinates (mm), measured with respect to the current subpage
* boundaries.
\*----------------------------------------------------------------------*/

void
c_plsvpa(PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax)
{
    PLINT nx, ny, cs;
    PLFLT sxmin, symin;
    PLFLT spdxmi, spdxma, spdymi, spdyma;
    PLFLT vpdxmi, vpdxma, vpdymi, vpdyma;
    PLINT level;

    plP_glev(&level);
    if (level < 1) {
	plabort("plsvpa: Please call plinit first");
	return;
    }
    if ((xmin >= xmax) || (ymin >= ymax)) {
	plabort("plsvpa: Invalid limits");
	return;
    }
    plP_gsub(&nx, &ny, &cs);
    if ((cs <= 0) || (cs > (nx * ny))) {
	plabort("plsvpa: Please call pladv or plenv to go to a subpage");
	return;
    }

    plP_gspd(&spdxmi, &spdxma, &spdymi, &spdyma);
    sxmin = plP_dcmmx(spdxmi);
    symin = plP_dcmmy(spdymi);

    vpdxmi = plP_mmdcx((PLFLT) (sxmin + xmin));
    vpdxma = plP_mmdcx((PLFLT) (sxmin + xmax));
    vpdymi = plP_mmdcy((PLFLT) (symin + ymin));
    vpdyma = plP_mmdcy((PLFLT) (symin + ymax));

    plP_svpd(vpdxmi, vpdxma, vpdymi, vpdyma);
    plP_svpp(plP_dcpcx(vpdxmi), plP_dcpcx(vpdxma), plP_dcpcy(vpdymi), plP_dcpcy(vpdyma));
    plP_sclp(plP_dcpcx(vpdxmi), plP_dcpcx(vpdxma), plP_dcpcy(vpdymi), plP_dcpcy(vpdyma));
    plP_slev(2);
}
