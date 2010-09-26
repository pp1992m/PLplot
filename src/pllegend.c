/* pllegend()
 *
 * Copyright (C) 2010  Hezekiah M. Carty
 * Copyright (C) 2010  Alan W. Irwin
 *
 * This file is part of PLplot.
 *
 * PLplot is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Library Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * PLplot is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with PLplot; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "plplotP.h"

static void get_world_per_mm( PLFLT *x_world_per_mm, PLFLT *y_world_per_mm )
{
    // Normalized viewport limits
    PLFLT vxmin, vxmax, vymin, vymax;
    // Size of subpage in mm
    PLFLT mxmin, mxmax, mymin, mymax;
    // Viewport limits in world coordinates
    PLFLT wxmin, wxmax, wymin, wymax;
    plgvpd( &vxmin, &vxmax, &vymin, &vymax );
    plgspa( &mxmin, &mxmax, &mymin, &mymax );
    plgvpw( &wxmin, &wxmax, &wymin, &wymax );
    *x_world_per_mm = ( wxmax - wxmin ) / ( ( vxmax - vxmin ) * ( mxmax - mxmin ) );
    *y_world_per_mm = ( wymax - wymin ) / ( ( vymax - vymin ) * ( mymax - mymin ) );
}

static PLFLT get_character_or_symbol_height( PLINT ifcharacter )
{
    // Character height in mm
    PLFLT default_mm, char_height_mm;
    PLFLT x_world_per_mm, y_world_per_mm;

    if ( ifcharacter )
    {
        plgchr( &default_mm, &char_height_mm );
    }
    else
    {
        default_mm     = plsc->symdef;
        char_height_mm = plsc->symht;
    }
    get_world_per_mm( &x_world_per_mm, &y_world_per_mm );
    return ( char_height_mm * y_world_per_mm );
}

#define normalized_to_world_x( nx )    ( ( xmin ) + ( nx ) * ( ( xmax ) - ( xmin ) ) )
#define normalized_to_world_y( ny )    ( ( ymin ) + ( ny ) * ( ( ymax ) - ( ymin ) ) )

// pllegend - Draw a legend using lines, symbols, cmap0 colours, or cmap1
//   colours.
// plot_width: width of plotted areas (lines, symbols, or coloured
//   area) in legend. (Uses normalized viewport units).
// text_offset: offset of text area from plot area in units of character width.
// N.B. the total width of the legend is made up of plplot_width +
//   text_offset (converted to normalized viewport coordinates) + width
//   of the longest string.  The latter quantity is calculated internally
//   using plstrl and converted to normalized viewport coordinates.
//
// x, y: Normalized position of the upper-left corner of the
//   legend in the viewport.
// nlegend: Number of legend entries
// text_colors: Color map 0 indices of the colors to use for label text
// text: text string for each legend entry
// cmap0_colors: cmap0 color index for each legend entry
// nsymbols: number of points/symbols to be drawn for each plot_width
// symbols: Symbol to draw for each legend entry.

void
c_pllegend( PLINT opt, PLFLT plot_width,
            PLFLT x, PLFLT y, PLINT bg_color,
            PLINT *opt_array, PLINT nlegend,
            PLFLT text_offset, PLFLT text_scale, PLFLT text_spacing,
            PLINT text_justification, PLINT *text_colors, char **text,
            PLINT *line_colors, PLINT *line_styles, PLINT *line_widths,
            PLINT *nsymbols, PLINT *symbol_colors,
            PLFLT *symbol_scales, PLINT *symbols,
            PLFLT cmap0_height, PLINT *cmap0_colours, PLINT *cmap0_patterns )

{
    // Viewport world-coordinate limits
    PLFLT xmin, xmax, ymin, ymax;
    // Legend position
    PLFLT plot_x, plot_x_end, plot_x_world, plot_x_end_world;
    PLFLT plot_y, plot_y_world;
    PLFLT text_x, text_y, text_x_world, text_y_world;
    // Character height (world coordinates)
    PLFLT character_height, character_width, symbol_width;
    // y-position of the current legend entry
    PLFLT ty;
    // Positions of the legend entries
    PLFLT dxs, *xs, *ys, xl[2], yl[2];
    PLINT i, j;
    // Active attributes to be saved and restored afterward.
    PLINT col0_save         = plsc->icol0, line_style_save = plsc->line_style,
          line_width_save   = plsc->width;
    PLFLT text_scale_save   = plsc->chrht / plsc->chrdef,
          symbol_scale_save = plsc->symht / plsc->symdef;
    PLFLT x_world_per_mm, y_world_per_mm, text_width = 0.;
    PLFLT total_width_border, total_width, total_height;
    PLINT some_lines   = 0, some_symbols = 0, some_cmap0s;
    PLINT max_nsymbols = 0;

    plschr( 0., text_scale );

    for ( i = 0; i < nlegend; i++ )
    {
        if ( opt_array[i] & PL_LEGEND_LINE )
            some_lines = 1;
        if ( opt_array[i] & PL_LEGEND_SYMBOL )
        {
            max_nsymbols = MAX( max_nsymbols, nsymbols[i] );
            some_symbols = 1;
        }
        if ( opt_array[i] & PL_LEGEND_CMAP0 )
            some_cmap0s = 1;
    }

    //sanity check
    if ( ( opt & PL_LEGEND_CMAP1 ) && ( some_lines || some_symbols || some_cmap0s ) )
    {
        plabort( "pllegend: invalid attempt to combine cmap1 legend with any other style of legend" );
        return;
    }

    plgvpw( &xmin, &xmax, &ymin, &ymax );

    // World coordinates for legend plots
    plot_x           = x;
    plot_y           = y;
    plot_x_end       = plot_x + plot_width;
    plot_x_world     = normalized_to_world_x( plot_x );
    plot_y_world     = normalized_to_world_y( plot_y );
    plot_x_end_world = normalized_to_world_x( plot_x_end );

    // Get character height and width in world coordinates
    character_height = get_character_or_symbol_height( 1 );
    character_width  = character_height * fabs( ( xmax - xmin ) / ( ymax - ymin ) );
    // Get world-coordinate positions of the start of the legend text
    text_x       = plot_x_end;
    text_y       = plot_y;
    text_x_world = normalized_to_world_x( text_x ) +
                   text_offset * character_width;
    text_y_world = normalized_to_world_y( text_y );

    // Calculate maximum width of text area (first in mm, then converted
    // to x world coordinates) including text_offset area.
    for ( i = 0; i < nlegend; i++ )
    {
        text_width = MAX( text_width, plstrl( text[i] ) );
    }
    get_world_per_mm( &x_world_per_mm, &y_world_per_mm );
    text_width = x_world_per_mm * text_width + text_offset * character_width;
    // make small border area where only the background is plotted
    // for left and right of legend.  0.4 seems to be a reasonable factor
    // that gives a good-looking result.
    total_width_border = 0.4 * character_width;
    total_width        = 2. * total_width_border + text_width + ( xmax - xmin ) * plot_width;
    total_height       = nlegend * text_spacing * character_height;

    if ( opt & PL_LEGEND_BACKGROUND )
    {
        PLINT pattern_save = plsc->patt;
        PLFLT xbg[4]       = {
            plot_x_world,
            plot_x_world,
            plot_x_world + total_width,
            plot_x_world + total_width,
        };
        PLFLT ybg[4] = {
            plot_y_world,
            plot_y_world - total_height,
            plot_y_world - total_height,
            plot_y_world,
        };
        plpsty( 0 );
        plcol0( bg_color );
        plfill( 4, xbg, ybg );
        plpsty( pattern_save );
        plcol0( col0_save );
    }

    if ( opt & PL_LEGEND_TEXT_LEFT )
    {
        // text area on left, plot area on right.
        text_x_world      = plot_x_world;
        plot_x_world     += text_width;
        plot_x_end_world += text_width;
    }
    // adjust border after background is drawn.
    plot_x_world     += total_width_border;
    plot_x_end_world += total_width_border;
    text_x_world     += total_width_border;

    if ( some_lines )
    {
        xl[0] = plot_x_world;
        xl[1] = plot_x_end_world;
    }

    if ( some_symbols )
    {
        max_nsymbols = MAX( 2, max_nsymbols );
        if ( ( ( xs = (PLFLT *) malloc( max_nsymbols * sizeof ( PLFLT ) ) ) == NULL ) ||
             ( ( ys = (PLFLT *) malloc( max_nsymbols * sizeof ( PLFLT ) ) ) == NULL ) )
        {
            plexit( "pllegend: Insufficient memory" );
        }

        // Get symbol width in world coordinates if symbols are plotted to
        // adjust ends of line of symbols.
        // AWI, no idea why must use 0.5 factor to get ends of symbol lines
        // to line up approximately correctly with plotted legend lines.
        // Factor should be unity.
        symbol_width = 0.5 * get_character_or_symbol_height( 0 ) *
                       fabs( ( xmax - xmin ) / ( ymax - ymin ) );
    }

    ty = text_y_world + 0.5 * text_spacing * character_height;
    // Draw each legend entry
    for ( i = 0; i < nlegend; i++ )
    {
        // y position of text, lines, symbols, and/or centre of cmap0 box.
        ty = ty - ( text_spacing * character_height );
        // Label/name for the legend
        plcol0( text_colors[i] );
        plptex( text_x_world, ty, 0.0, 0.0, 0.0, text[i] );

        if ( opt_array[i] & PL_LEGEND_LINE )
        {
            plcol0( line_colors[i] );
            pllsty( line_styles[i] );
            plwid( line_widths[i] );
            yl[0] = ty;
            yl[1] = ty;
            plline( 2, xl, yl );
            pllsty( line_style_save );
            plwid( line_width_save );
        }
        if ( opt_array[i] & PL_LEGEND_SYMBOL )
        {
            plcol0( symbol_colors[i] );
            plssym( 0., symbol_scales[i] );
            dxs = ( plot_x_end_world - plot_x_world - symbol_width ) / (double) ( MAX( nsymbols[i], 2 ) - 1 );
            for ( j = 0; j < nsymbols[i]; j++ )
            {
                xs[j] = plot_x_world + 0.5 * symbol_width + dxs * (double) j;
                ys[j] = ty;
            }
            plpoin( nsymbols[i], xs, ys, symbols[i] );
        }
    }
    if ( some_symbols )
    {
        free( xs );
        free( ys );
    }

    // Restore
    plcol0( col0_save );
    plschr( 0., text_scale_save );
    plssym( 0., symbol_scale_save );

    return;
}
