#----------------------------------------------------------------------------
# $Id$
#----------------------------------------------------------------------------

# plshade demo, using color fill.

proc x16 {{w loopback}} {

    set ns 20
    set nx 35
    set ny 46

    set pi 3.14159265358979323846

    set sh_cmap 1
    set min_color 1; set min_width 0; set max_color 0; set max_width 0

    matrix clevel f $ns
    matrix xg1 f $nx
    matrix yg1 f $ny
    matrix xg2 f $nx $ny
    matrix yg2 f $nx $ny
    matrix zz f $nx $ny
    matrix ww f $nx $ny

# Set up data array

    for {set i 0} {$i < $nx} {incr i} {
	set x [expr ($i - ($nx/2.)) / ($nx/2.)]
	for {set j 0} {$j < $ny} {incr j} {
	    set y [expr ($j - .5 * $ny) / (.5 * $ny) - 1.]

	    zz $i $j = [expr -sin(7.*$x) * cos(7.*$y) + $x*$x - $y*$y ]
	    ww $i $j = [expr -cos(7.*$x) * sin(7.*$y) + 2 * $x * $y ]
	}
    }

    set zmin [zz 0 0]
    set zmax $zmin
    for {set i 0} {$i < $nx} {incr i} {
	for {set j 0} {$j < $ny} {incr j} {
	    if {[zz $i $j] < $zmin} { set zmin [zz $i $j] }
	    if {[zz $i $j] > $zmax} { set zmax [zz $i $j] }
	}
    }

    for {set i 0} {$i < $ns} {incr i} {
	clevel $i = [expr $zmin + ($zmax - $zmin) * ($i + .5) / $ns.]
    }

# Build the 1-d coord arrays.

    set distort .4

    for {set i 0} {$i < $nx} {incr i} {
	set xx [expr -1. + $i * ( 2. / ($nx-1.) )]
	xg1 $i = [expr $xx + $distort * cos( .5 * $pi * $xx ) ]
    }

    for {set j 0} {$j < $ny} {incr j} {
	set yy [expr -1. + $j * ( 2. / ($ny-1.) )]
	yg1 $j = [expr $yy - $distort * cos( .5 * $pi * $yy ) ]
    }

# Build the 2-d coord arrays.

    for {set i 0} {$i < $nx} {incr i} {
	set xx [expr -1. + $i * ( 2. / ($nx-1.) )]
	for {set j 0} {$j < $ny} {incr j} {
	    set yy [expr -1. + $j * ( 2. / ($ny-1.) )]

	    set argx [expr .5 * $pi * $xx]
	    set argy [expr .5 * $pi * $yy]

	    xg2 $i $j = [expr $xx + $distort * cos($argx) * cos($argy) ]
	    yg2 $i $j = [expr $yy - $distort * cos($argx) * cos($argy) ]
	}
    }

# Plot using identity transform

    $w cmd pladv 0
    $w cmd plvpor 0.1 0.9 0.1 0.9
    $w cmd plwind -1.0 1.0 -1.0 1.0

    for {set i 0} {$i < $ns} {incr i} {
	set shade_min [expr $zmin + ($zmax - $zmin) * $i / $ns.]
	set shade_max [expr $zmin + ($zmax - $zmin) * ($i +1.) / $ns.]
	set sh_color [expr $i. / ($ns-1.)]
	set sh_width 2
	$w cmd plpsty 0

#	plshade(z, nx, ny, NULL, -1., 1., -1., 1., 
#		shade_min, shade_max, 
#		sh_cmap, sh_color, sh_width,
#		min_color, min_width, max_color, max_width,
#		plfill, 1, NULL, NULL);
	$w cmd plshade zz -1. 1. -1. 1. \
	    $shade_min $shade_max $sh_cmap $sh_color $sh_width \
	    $min_color $min_width $max_color $max_width \
	    1
    }

    $w cmd plcol0 1
    $w cmd plbox "bcnst" 0.0 0 "bcnstv" 0.0 0
    $w cmd plcol0 2

#    plcont(w, nx, ny, 1, nx, 1, ny, clevel, ns, mypltr, NULL);

    $w cmd pllab "distance" "altitude" "Bogon density"

# Plot using 1d coordinate transform
    
    $w cmd pladv 0
    $w cmd plvpor 0.1 0.9 0.1 0.9
    $w cmd plwind -1.0 1.0 -1.0 1.0

    for {set i 0} {$i < $ns} {incr i} {
	set shade_min [expr $zmin + ($zmax - $zmin) * $i / $ns.]
	set shade_max [expr $zmin + ($zmax - $zmin) * ($i +1.) / $ns.]
	set sh_color [expr $i. / ($ns-1.)]
	set sh_width 2
	$w cmd plpsty 0

#	plshade(z, nx, ny, NULL, -1., 1., -1., 1., 
#		shade_min, shade_max, 
#		sh_cmap, sh_color, sh_width,
#		min_color, min_width, max_color, max_width,
#		plfill, 1, pltr1, (void *) &cgrid1);

	$w cmd plshade zz -1. 1. -1. 1. \
	    $shade_min $shade_max $sh_cmap $sh_color $sh_width \
	    $min_color $min_width $max_color $max_width \
	    1 pltr1 xg1 yg1
    }

    $w cmd plcol0 1
    $w cmd plbox "bcnst" 0.0 0 "bcnstv" 0.0 0
    $w cmd plcol0 2

#    plcont(w, nx, ny, 1, nx, 1, ny, clevel, ns, pltr1, (void *) &cgrid1);

    $w cmd pllab "distance" "altitude" "Bogon density"

# Plot using 2d coordinate transform

    $w cmd pladv 0
    $w cmd plvpor 0.1 0.9 0.1 0.9
    $w cmd plwind -1.0 1.0 -1.0 1.0

    for {set i 0} {$i < $ns} {incr i} {
	set shade_min [expr $zmin + ($zmax - $zmin) * $i / $ns.]
	set shade_max [expr $zmin + ($zmax - $zmin) * ($i +1.) / $ns.]
	set sh_color [expr $i. / ($ns-1.)]
	set sh_width 2
	$w cmd plpsty 0

	$w cmd plshade zz -1. 1. -1. 1. \
	    $shade_min $shade_max $sh_cmap $sh_color $sh_width \
	    $min_color $min_width $max_color $max_width \
	    0 pltr2 xg2 yg2
    }

    $w cmd plcol0 1
    $w cmd plbox "bcnst" 0.0 0 "bcnstv" 0.0 0
    $w cmd plcol0 2
#    plcont(w, nx, ny, 1, nx, 1, ny, clevel, ns, pltr2, (void *) &cgrid2);
    $w cmd plcont ww clevel pltr2 xg2 yg2

    $w cmd pllab "distance" "altitude" "Bogon density, with streamlines"

# Do it again, but show wrapping support.

    $w cmd pladv 0
    $w cmd plvpor 0.1 0.9 0.1 0.9
    $w cmd plwind -1.0 1.0 -1.0 1.0

# Hold perimeter
    matrix px f 100; matrix py f 100

    for {set i 0} {$i < 100} {incr i} {
	set t [expr 2. * $pi * $i / 99.]
	px $i = [expr cos($t)]
	py $i = [expr sin($t)]
    }
# We won't draw it now b/c it should be drawn /after/ the plshade stuff.

# Now build the new coordinate matricies.

    matrix xg f $nx $ny
    matrix yg f $nx $ny
    matrix z  f $nx $ny

    for {set i 0} {$i < $nx} {incr i} {
	set r [expr $i / ($nx - 1.)]
	for {set j 0} {$j < $ny} {incr j} {
	    set t [expr 2. * $pi * $j / ($ny - 1.)]

	    xg $i $j = [expr $r * cos($t)]
	    yg $i $j = [expr $r * sin($t)]

	    z $i $j = [expr exp(-$r*$r) * cos(5.*$t) * cos(5.*$pi*$r) ]
	}
    }

# Need a new clevel to go allong with the new data set.

    set zmin [z 0 0]
    set zmax $zmin
    for {set i 0} {$i < $nx} {incr i} {
	for {set j 0} {$j < $ny} {incr j} {
	    if {[z $i $j] < $zmin} { set zmin [z $i $j] }
	    if {[z $i $j] > $zmax} { set zmax [z $i $j] }
	}
    }

    for {set i 0} {$i < $ns} {incr i} {
	clevel $i = [expr $zmin + ($zmax - $zmin) * ($i + .5) / $ns.]
    }

# Now we can shade the interior region.

    for {set i 0} {$i < $ns} {incr i} {
	set shade_min [expr $zmin + ($zmax - $zmin) * $i / $ns.]
	set shade_max [expr $zmin + ($zmax - $zmin) * ($i +1.) / $ns.]
	set sh_color [expr $i. / ($ns-1.)]
	set sh_width 2
	$w cmd plpsty 0

	$w cmd plshade z -1. 1. -1. 1. \
	    $shade_min $shade_max $sh_cmap $sh_color $sh_width \
	    $min_color $min_width $max_color $max_width \
	    0 pltr2 xg yg 2
    }

# Now we can draw the perimeter.
    $w cmd plcol0 1
    $w cmd plline 100 px py

# And label the plot.
    $w cmd plcol0 2
    $w cmd pllab "" "" "Tokamak Bogon Instability"
}
