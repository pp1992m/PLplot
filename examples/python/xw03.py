#!/usr/bin/env python
#	Polar plot demo.

import sys
import os

module_dir = "@MODULE_DIR@"

if module_dir[0] == '@':
	module_dir = os.getcwd ()

sys.path.insert (0, module_dir)

from Numeric import *
from pl import *

# main
#
# Generates polar plot, with 1-1 scaling.

def main():

    dtr = pi / 180.0
    x0 = cos(dtr*arrayrange(361))
    y0 = sin(dtr*arrayrange(361))

    # Parse and process command line arguments

    plParseOpts(sys.argv, PARSE_FULL)

    # Initialize plplot

    plinit()
    plcol0(1)

    # Set up viewport and window, but do not draw box

    plenv(-1.3, 1.3, -1.3, 1.3, 1, -2)

    i = 0.1*arrayrange(11)
    #ravel(outerproduct(i,x0)) and ravel(outerproduct(i,y0)) is what we are 
    #mocking up here since old numpy version does not have outerproduct.
    x=ravel(transpose(resize(i,(len(x0),len(i))))*x0)
    y=ravel(transpose(resize(i,(len(y0),len(i))))*y0)
    
    # Draw circles for polar grid

    plline(x, y)

    plcol0(2)
    for i in range(12):
	theta = 30.0 * i
	dx = cos(dtr * theta)
	dy = sin(dtr * theta)

	# Draw radial spokes for polar grid

	pljoin(0.0, 0.0, dx, dy)

	# Write labels for angle

	text = `int(theta)`
	if dx >= 0:
	    plptex(dx, dy, dx, dy, -0.15, text)
	else:
	    plptex(dx, dy, -dx, -dy, 1.15, text)

    # Draw the graph

    r = sin((dtr*5.)*arrayrange(361))
    x = x0*r
    y = y0*r

    plcol0(3)
    plline(x, y)

    plcol0(4)
    plmtex("t", 2.0, 0.5, 0.5, "#frPLplot Example 3 - r(#gh)=sin 5#gh")

    # Close the plot at end

    pleop()
    plend()

main()
