!***********************************************************************
!  plplot_binding.f90
!
!  Copyright (C) 2005-2016  Arjen Markus
!  Copyright (C) 2006-2016 Alan W. Irwin
!
!  This file is part of PLplot.
!
!  PLplot is free software; you can redistribute it and/or modify
!  it under the terms of the GNU Library General Public License as published
!  by the Free Software Foundation; either version 2 of the License, or
!  (at your option) any later version.
!
!  PLplot is distributed in the hope that it will be useful,
!  but WITHOUT ANY WARRANTY; without even the implied warranty of
!  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
!  GNU Library General Public License for more details.
!
!  You should have received a copy of the GNU Library General Public License
!  along with PLplot; if not, write to the Free Software
!  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
!
!
!  This file is a limited version of what should become the new style
!  Fortran bindings. It is geared to example x00f only.
!
!***********************************************************************

module plplot_types
    include 'included_plplot_interface_private_types.f90'
end module plplot_types

module plplot_single
    use iso_c_binding, only: c_ptr, c_null_char, c_null_ptr, c_loc
    use plplot_types
    implicit none

    integer, parameter :: wp = private_single
    private :: wp, private_single, private_double, private_plint, private_plunicode

    include 'included_plplot_real_interfaces.f90'
end module plplot_single

module plplot_double
    use iso_c_binding, only: c_ptr, c_null_char, c_null_ptr, c_loc
    use plplot_types
    implicit none

    integer, parameter :: wp = private_double
    private :: wp, private_single, private_double, private_plint, private_plunicode

    include 'included_plplot_real_interfaces.f90'

end module plplot_double

module plplot
    use plplot_single
    use plplot_double
    use plplot_types, only: plflt => private_plflt, private_plint, private_plunicode
    implicit none
    integer(kind=private_plint), parameter :: maxlen = 320
    include 'included_plplot_parameters.f90'
    private :: private_plint, private_plunicode
    private :: copystring
!
! Interfaces that do not depend on the real kind
!
    interface pl_setcontlabelformat
        module procedure pl_setcontlabelformat_impl
    end interface pl_setcontlabelformat
    private :: pl_setcontlabelformat_impl

    interface plsetopt
        module procedure plsetopt_impl
    end interface plsetopt
    private :: plsetopt_impl

    interface plstyl
        module procedure plstyl_scalar
        module procedure plstyl_n_array
        module procedure plstyl_array
    end interface
    private :: plstyl_scalar, plstyl_n_array, plstyl_array

    interface pltimefmt
        module procedure pltimefmt_impl
    end interface pltimefmt
    private :: pltimefmt_impl

contains

!
! Private utilities
!
subroutine copystring( fstring, cstring )
    character(len=*), intent(out) :: fstring
    character(len=1), dimension(:), intent(in) :: cstring

    integer :: i

    fstring = ' '
    do i = 1,min(len(fstring),size(cstring))
        if ( cstring(i) /= c_null_char ) then
            fstring(i:i) = cstring(i)
        else
            exit
        endif
    enddo

end subroutine copystring

!
! Interface routines
!
subroutine pl_setcontlabelformat_impl( lexp, sigdig )
   integer, intent(in) :: lexp, sigdig

   interface
       subroutine c_pl_setcontlabelformat( lexp, sigdig ) bind(c,name='c_pl_setcontlabelformat')
           implicit none
           include 'included_plplot_interface_private_types.f90'
           integer(kind=private_plint), value :: lexp, sigdig
       end subroutine c_pl_setcontlabelformat
   end interface

   call c_pl_setcontlabelformat( int(lexp,kind=private_plint), int(sigdig,kind=private_plint) )
end subroutine pl_setcontlabelformat_impl

subroutine pladv( sub )
    integer, intent(in) :: sub
    interface
        subroutine c_pladv( sub ) bind( c, name = 'c_pladv' )
            implicit none
            include 'included_plplot_interface_private_types.f90'
            integer(kind=private_plint), value :: sub
        end subroutine c_pladv
    end interface

    call c_pladv( int(sub,kind=private_plint) )
end subroutine pladv

subroutine plbop()
    interface
        subroutine interface_plbop() bind(c,name='c_plbop')
        end subroutine interface_plbop
     end interface
     call interface_plbop()
end subroutine plbop

subroutine plclear()
    interface
        subroutine interface_plclear() bind(c,name='c_plclear')
        end subroutine interface_plclear
     end interface
     call interface_plclear()
end subroutine plclear

subroutine plcol0( icol )
    integer, intent(in)  :: icol
    interface
        subroutine c_plcol0( icol ) bind(c, name = 'c_plcol0' )
            implicit none
            include 'included_plplot_interface_private_types.f90'
            integer(kind=private_plint), value :: icol
        end subroutine c_plcol0
    end interface

    call c_plcol0( int(icol,kind=private_plint) )
end subroutine plcol0

subroutine plend()
    interface
        subroutine interface_plend() bind( c, name = 'c_plend' )
        end subroutine interface_plend
     end interface
     call interface_plend()
end subroutine plend

subroutine plend1()
    interface
        subroutine interface_plend1() bind( c, name = 'c_plend1' )
        end subroutine interface_plend1
     end interface
     call interface_plend1()
end subroutine plend1

subroutine pleop()
    interface
        subroutine interface_pleop() bind( c, name = 'c_pleop' )
        end subroutine interface_pleop
     end interface
     call interface_pleop()
end subroutine pleop

subroutine plfamadv()
    interface
        subroutine interface_plfamadv() bind( c, name = 'c_plfamadv' )
        end subroutine interface_plfamadv
     end interface
     call interface_plfamadv()
end subroutine plfamadv

subroutine plflush()
    interface
        subroutine interface_plflush() bind( c, name = 'c_plflush' )
        end subroutine interface_plflush
     end interface
     call interface_plflush()
end subroutine plflush

subroutine plfont( font )
    integer, intent(in)  :: font
    interface
        subroutine c_plfont( font ) bind( c, name = 'c_plfont' )
            implicit none
            include 'included_plplot_interface_private_types.f90'
            integer(kind=private_plint), value :: font
        end subroutine c_plfont
    end interface

    call c_plfont( int(font,kind=private_plint) )
end subroutine plfont

subroutine plfontld( charset )
    integer, intent(in)  :: charset
    interface
        subroutine c_plfontld( charset ) bind( c, name = 'c_plfontld' )
            implicit none
            include 'included_plplot_interface_private_types.f90'
            integer(kind=private_plint), value :: charset
        end subroutine c_plfontld
    end interface

    call c_plfontld( int(charset,kind=private_plint) )
end subroutine plfontld

subroutine plgcol0( icol, r, g, b )
    integer, intent(in) :: icol
    integer, intent(out) :: r, g, b

    integer(kind=private_plint) :: r_out, g_out, b_out

    interface
        subroutine c_plgcol0( icol, r, g, b ) bind( c, name = 'c_plgcol0' )
            implicit none
            include 'included_plplot_interface_private_types.f90'
            integer(kind=private_plint), value :: icol
            integer(kind=private_plint), intent(out) :: r, g, b
        end subroutine c_plgcol0
    end interface

    call c_plgcol0( int(icol,kind=private_plint), r_out, g_out, b_out )
    r = int(r_out)
    g = int(g_out)
    b = int(b_out)
end subroutine plgcol0

subroutine plgcolbg( r, g, b )
    integer, intent(out) :: r, g, b

    integer(kind=private_plint) :: r_out, g_out, b_out

    interface
        subroutine c_plgcolbg( r, g, b ) bind( c, name = 'c_plgcolbg' )
            implicit none
            include 'included_plplot_interface_private_types.f90'
            integer(kind=private_plint), intent(out) :: r, g, b
        end subroutine c_plgcolbg
    end interface

    call c_plgcolbg( r_out, g_out, b_out )
    r = int(r_out)
    g = int(g_out)
    b = int(b_out)
end subroutine plgcolbg

subroutine plgcompression( compression )
    integer, intent(out) :: compression

    integer(kind=private_plint) :: compression_out

    interface
        subroutine c_plgcompression( compression ) bind( c, name = 'c_plgcompression' )
            implicit none
            include 'included_plplot_interface_private_types.f90'
            integer(kind=private_plint), intent(out) :: compression
        end subroutine c_plgcompression
    end interface

    call c_plgcompression( compression_out )
    compression = int(compression_out)
end subroutine plgcompression

subroutine plgdev(dev)
    character*(*), intent(out) :: dev

    character(len=1), dimension(100) :: dev_out

    interface
        subroutine c_plgdev( dev ) bind(c,name='c_plgdev')
            implicit none
            character(len=1), dimension(*), intent(out) :: dev
        end subroutine c_plgdev
    end interface

    call c_plgdev( dev_out )
    call copystring( dev, dev_out )
end subroutine plgdev

subroutine plgfam( fam, num, bmax )
    integer, intent(out) :: fam, num, bmax

    integer(kind=private_plint) :: fam_out, num_out, bmax_out

    interface
        subroutine c_plgfam( fam, num, bmax ) bind( c, name = 'c_plgfam' )
            implicit none
            include 'included_plplot_interface_private_types.f90'
            integer(kind=private_plint), intent(out) :: fam, num, bmax
        end subroutine c_plgfam
    end interface

    call c_plgfam( fam_out, num_out, bmax_out )
    fam  = int(fam_out)
    num  = int(num_out)
    bmax = int(bmax_out)
end subroutine plgfam

subroutine plgfont( family, style, weight )
    integer, intent(out) :: family, style, weight

    integer(kind=private_plint) :: family_out, style_out, weight_out

    interface
        subroutine c_plgfont( family, style, weight ) bind( c, name = 'c_plgfont' )
            implicit none
            include 'included_plplot_interface_private_types.f90'
            integer(kind=private_plint), intent(out) :: family, style, weight
        end subroutine c_plgfont
    end interface

    call c_plgfont( family_out, style_out, weight_out )
    family = int(family_out)
    style  = int(style_out)
    weight = int(weight_out)
end subroutine plgfont

subroutine plglevel( level )
    integer, intent(out) :: level

    integer(kind=private_plint) :: level_out

    interface
        subroutine c_plglevel( level ) bind( c, name = 'c_plglevel' )
            implicit none
            include 'included_plplot_interface_private_types.f90'
            integer(kind=private_plint), intent(out) :: level
        end subroutine c_plglevel
    end interface

    call c_plglevel( level_out )
    level = int(level_out)
end subroutine plglevel

subroutine plgra()
    interface
        subroutine interface_plgra() bind( c, name = 'c_plgra' )
        end subroutine interface_plgra
     end interface
     call interface_plgra()
end subroutine plgra

subroutine plgstrm( strm )
    integer, intent(out) :: strm

    integer(kind=private_plint) :: strm_out

    interface
        subroutine c_plgstrm( strm ) bind( c, name = 'c_plgstrm' )
            implicit none
            include 'included_plplot_interface_private_types.f90'
            integer(kind=private_plint), intent(out) :: strm
        end subroutine c_plgstrm
    end interface

    call c_plgstrm( strm_out )
    strm = int(strm_out)
end subroutine plgstrm

subroutine plgver(ver)
    character*(*), intent(out) :: ver

    character(len=1), dimension(100) :: ver_out

    interface
        subroutine c_plgver( ver ) bind(c,name='c_plgver')
            implicit none
            character(len=1), dimension(*), intent(out) :: ver
        end subroutine c_plgver
    end interface

    call c_plgver( ver_out )
    call copystring( ver, ver_out )
end subroutine plgver

subroutine plgxax( digmax, digits )
    integer, intent(out) :: digmax, digits

    integer(kind=private_plint) :: digmax_out, digits_out

    interface
        subroutine c_plgxax( digmax, digits ) bind( c, name = 'c_plgxax' )
            implicit none
            include 'included_plplot_interface_private_types.f90'
            integer(kind=private_plint), intent(out) :: digmax, digits
        end subroutine c_plgxax
    end interface

    call c_plgxax( digmax_out, digits_out )
    digmax = int(digmax_out)
    digits = int(digits_out)
end subroutine plgxax

subroutine plgyax( digmax, digits )
    integer, intent(out) :: digmax, digits

    integer(kind=private_plint) :: digmax_out, digits_out

    interface
        subroutine c_plgyax( digmax, digits ) bind( c, name = 'c_plgyax' )
            implicit none
            include 'included_plplot_interface_private_types.f90'
            integer(kind=private_plint), intent(out) :: digmax, digits
        end subroutine c_plgyax
    end interface

    call c_plgyax( digmax_out, digits_out )
    digmax = int(digmax_out)
    digits = int(digits_out)
end subroutine plgyax

subroutine plgzax( digmax, digits )
    integer, intent(out) :: digmax, digits

    integer(kind=private_plint) :: digmax_out, digits_out

    interface
        subroutine c_plgzax( digmax, digits ) bind( c, name = 'c_plgzax' )
            implicit none
            include 'included_plplot_interface_private_types.f90'
            integer(kind=private_plint), intent(out) :: digmax, digits
        end subroutine c_plgzax
    end interface

    call c_plgzax( digmax_out, digits_out )
    digmax = int(digmax_out)
    digits = int(digits_out)
end subroutine plgzax

subroutine plinit()
    interface
        subroutine interface_plinit() bind( c, name = 'c_plinit' )
        end subroutine interface_plinit
     end interface
     call interface_plinit()
end subroutine plinit

subroutine pllab( xlab, ylab, title )
   character(len=*), intent(in) :: xlab, ylab, title

   interface
       subroutine c_pllab( xlab, ylab, title ) bind(c,name='c_pllab')
           implicit none
           character(len=1), dimension(*), intent(in) :: xlab, ylab, title
       end subroutine c_pllab
   end interface

   call c_pllab( trim(xlab) // c_null_char, trim(ylab) // c_null_char, trim(title) // c_null_char )

end subroutine pllab

subroutine pllsty( lin )
    integer, intent(in) :: lin
    interface
        subroutine c_pllsty( lin ) bind( c, name = 'c_pllsty' )
            implicit none
            include 'included_plplot_interface_private_types.f90'
            integer(kind=private_plint), value :: lin
        end subroutine c_pllsty
    end interface

    call c_pllsty( int(lin,kind=private_plint) )
end subroutine pllsty

subroutine plmkstrm( strm )
    integer, intent(in) :: strm
    interface
        subroutine c_plmkstrm( strm ) bind( c, name = 'c_plmkstrm' )
            implicit none
            include 'included_plplot_interface_private_types.f90'
            integer(kind=private_plint), value :: strm
        end subroutine c_plmkstrm
    end interface

    call c_plmkstrm( int(strm,kind=private_plint) )
end subroutine plmkstrm

subroutine plparseopts(mode)
  integer                :: mode
  integer                :: iargs, numargs
  integer, parameter     :: maxargs = 100
  character (len=maxlen), dimension(0:maxargs) :: arg
  
  interface
     subroutine interface_plparseopts( length, nargs, arg, mode ) bind(c,name='fc_plparseopts')
       use iso_c_binding, only: c_char
       implicit none
       include 'included_plplot_interface_private_types.f90'
       integer(kind=private_plint), value :: length, nargs, mode
       ! This Fortran argument requires special processing done
       ! in fc_plparseopts at the C level to interoperate properly
       ! with the C version of plparseopts.
       character(c_char) arg(length, nargs)
     end subroutine interface_plparseopts
  end interface
  
  numargs = command_argument_count()
  if (numargs < 0) then
     !       This actually happened historically on a badly linked Cygwin platform.
     write(0,'(a)') 'f95 plparseopts ERROR: negative number of arguments'
     return
  endif
  if(numargs > maxargs) then
     write(0,'(a)') 'f95 plparseopts ERROR: too many arguments'
     return
  endif
  do iargs = 0, numargs
     call get_command_argument(iargs, arg(iargs))
  enddo
  call interface_plparseopts(len(arg(0), kind=private_plint), int(numargs+1, kind=private_plint), arg, &
       int(mode, kind=private_plint))
end subroutine plparseopts

subroutine plpat( nlin, inc, del )
    integer, intent(in) :: nlin, inc, del
    interface
        subroutine c_plpat( nlin, inc, del ) bind( c, name = 'c_plpat' )
            implicit none
            include 'included_plplot_interface_private_types.f90'
            integer(kind=private_plint), value :: nlin, inc, del
        end subroutine c_plpat
    end interface

    call c_plpat( int(nlin,kind=private_plint), int(inc,kind=private_plint), int(del,kind=private_plint) )

end subroutine plpat

subroutine plprec( setp, prec )
    integer, intent(in) :: setp, prec
    interface
        subroutine c_plprec( setp, prec ) bind( c, name = 'c_plprec' )
            implicit none
            include 'included_plplot_interface_private_types.f90'
            integer(kind=private_plint), value :: setp, prec
        end subroutine c_plprec
    end interface

    call c_plprec( int(setp,kind=private_plint), int(prec,kind=private_plint) )
end subroutine plprec

subroutine plpsty( patt )
    integer, intent(in) :: patt
    interface
        subroutine c_plpsty( patt ) bind( c, name = 'c_plpsty' )
            implicit none
            include 'included_plplot_interface_private_types.f90'
            integer(kind=private_plint), value :: patt
        end subroutine c_plpsty
    end interface

    call c_plpsty( int(patt,kind=private_plint) )
end subroutine plpsty

! Should be defined only once - return type not part of disambiguation
real (kind=private_plflt) function plrandd()
    interface
        function c_plrandd() bind(c,name='c_plrandd')
            implicit none
            include 'included_plplot_interface_private_types.f90'
            real(kind=private_plflt) :: c_plrandd
        end function c_plrandd
    end interface

    plrandd = c_plrandd()
end function plrandd

subroutine plreplot()
    interface
        subroutine interface_plreplot() bind(c,name='c_plreplot')
        end subroutine interface_plreplot
     end interface
     call interface_plreplot()
end subroutine plreplot

subroutine plscmap0( r, g, b )
    integer, dimension(:), intent(in) :: r, g, b

    interface
        subroutine c_plscmap0( r, g, b, n ) bind(c,name='c_plscmap0')
            implicit none
            include 'included_plplot_interface_private_types.f90'
            integer(kind=private_plint), dimension(*), intent(in) :: r, g, b
            integer(kind=private_plint), value :: n
        end subroutine c_plscmap0
    end interface

    call c_plscmap0( int(r,kind=private_plint), int(g,kind=private_plint), int(b,kind=private_plint), &
                     size(r,kind=private_plint) )
end subroutine plscmap0

subroutine plscmap0n( n )
    integer, intent(in) :: n
    interface
        subroutine c_plscmap0n( n ) bind( c, name = 'c_plscmap0n' )
            implicit none
            include 'included_plplot_interface_private_types.f90'
            integer(kind=private_plint), value :: n
        end subroutine c_plscmap0n
    end interface

    call c_plscmap0n( int(n,kind=private_plint) )
end subroutine plscmap0n

subroutine plscmap1n( n )
    integer, intent(in) :: n
    interface
        subroutine c_plscmap1n( n ) bind( c, name = 'c_plscmap1n' )
            implicit none
            include 'included_plplot_interface_private_types.f90'
            integer(kind=private_plint), value :: n
        end subroutine c_plscmap1n
    end interface

    call c_plscmap1n( int(n,kind=private_plint) )
end subroutine plscmap1n

subroutine plscol0( icol, r, g, b )
    integer, intent(in) :: icol, r, g, b
    interface
        subroutine c_plscol0( icol, r, g, b ) bind( c, name = 'c_plscol0' )
            implicit none
            include 'included_plplot_interface_private_types.f90'
            integer(kind=private_plint), value :: icol, r, g, b
        end subroutine c_plscol0
    end interface

    call c_plscol0( int(icol,kind=private_plint), &
                    int(r,kind=private_plint), int(g,kind=private_plint), int(b,kind=private_plint) )
end subroutine plscol0

subroutine plscolbg( r, g, b )
    integer, intent(in) :: r, g, b
    interface
        subroutine c_plscolbg( r, g, b ) bind( c, name = 'c_plscolbg' )
            implicit none
            include 'included_plplot_interface_private_types.f90'
            integer(kind=private_plint), value :: r, g, b
        end subroutine c_plscolbg
    end interface

    call c_plscolbg( int(r,kind=private_plint), int(g,kind=private_plint), int(b,kind=private_plint) )
end subroutine plscolbg


subroutine plscolor( color )
    integer, intent(in) :: color
    interface
        subroutine c_plscolor( color ) bind( c, name = 'c_plscolor' )
            implicit none
            include 'included_plplot_interface_private_types.f90'
            integer(kind=private_plint), value :: color
        end subroutine c_plscolor
    end interface

    call c_plscolor( int(color,kind=private_plint) )
end subroutine plscolor

subroutine plscompression( compression )
    integer, intent(in) :: compression
    interface
        subroutine c_plscompression( compression ) bind( c, name = 'c_plscompression' )
            implicit none
            include 'included_plplot_interface_private_types.f90'
            integer(kind=private_plint), value :: compression
        end subroutine c_plscompression
    end interface

    call c_plscompression( int(compression,kind=private_plint) )
end subroutine plscompression

subroutine plsdev( devname )
   character(len=*), intent(in) :: devname

   interface
       subroutine c_plsdev( devname ) bind(c,name='c_plsdev')
           implicit none
           character(len=1), dimension(*), intent(in) :: devname
       end subroutine c_plsdev
   end interface

   call c_plsdev( trim(devname) // c_null_char )

end subroutine plsdev

subroutine plseed( s )
    integer, intent(in) :: s
    interface
        subroutine c_plseed( s ) bind( c, name = 'c_plseed' )
            implicit none
            include 'included_plplot_interface_private_types.f90'
            integer(kind=private_plint), value :: s
        end subroutine c_plseed
    end interface

    call c_plseed( int(s,kind=private_plint) )
end subroutine plseed

! TODO: character-version
subroutine plsesc( esc )
    integer, intent(in) :: esc
    interface
        subroutine c_plsesc( esc ) bind( c, name = 'c_plsesc' )
            implicit none
            include 'included_plplot_interface_private_types.f90'
            integer(kind=private_plint), value :: esc
        end subroutine c_plsesc
    end interface

    call c_plsesc( int(esc,kind=private_plint) )
end subroutine plsesc

subroutine plsetopt_impl( opt, optarg )
   character(len=*), intent(in) :: opt, optarg

   interface
       subroutine c_plsetopt( opt, optarg ) bind(c,name='c_plsetopt')
           implicit none
           character(len=1), dimension(*), intent(in) :: opt, optarg
       end subroutine c_plsetopt
   end interface

   call c_plsetopt( trim(opt) // c_null_char, trim(optarg) // c_null_char )

end subroutine plsetopt_impl

subroutine plsfam( fam, num, bmax )
    integer, intent(in) :: fam, num, bmax
    interface
        subroutine c_plsfam( fam, num, bmax ) bind( c, name = 'c_plsfam' )
            implicit none
            include 'included_plplot_interface_private_types.f90'
            integer(kind=private_plint), value :: fam, num, bmax
        end subroutine c_plsfam
    end interface

    call c_plsfam( int(fam,kind=private_plint), int(num,kind=private_plint), int(bmax,kind=private_plint) )
end subroutine plsfam

subroutine plsfont( family, style, weight )
    integer, intent(in) :: family, style, weight
    interface
        subroutine c_plsfont( family, style, weight ) bind( c, name = 'c_plsfont' )
            implicit none
            include 'included_plplot_interface_private_types.f90'
            integer(kind=private_plint), value :: family, style, weight
        end subroutine c_plsfont
    end interface

    call c_plsfont( int(family,kind=private_plint), int(style,kind=private_plint), int(weight,kind=private_plint) )
end subroutine plsfont

subroutine plsori( rot )
    integer, intent(in) :: rot
    interface
        subroutine c_plsori( rot ) bind( c, name = 'c_plsori' )
            implicit none
            include 'included_plplot_interface_private_types.f90'
            integer(kind=private_plint), value :: rot
        end subroutine c_plsori
    end interface

    call c_plsori( int(rot,kind=private_plint) )
end subroutine plsori

subroutine plspal0( filename )
  character(len=*), intent(in) :: filename

   interface
       subroutine c_plspal0( filename ) bind(c,name='c_plspal0')
           implicit none
           include 'included_plplot_interface_private_types.f90'
           character(len=1), dimension(*), intent(in) :: filename
       end subroutine c_plspal0
   end interface

   call c_plspal0( trim(filename) // c_null_char )

end subroutine plspal0

subroutine plspal1( filename, interpolate )
  character(len=*), intent(in) :: filename
  logical, intent(in) :: interpolate

   interface
       subroutine c_plspal1( filename, interpolate ) bind(c,name='c_plspal1')
           implicit none
           include 'included_plplot_interface_private_types.f90'
           integer(kind=private_plint), value :: interpolate
           character(len=1), dimension(*), intent(in) :: filename
       end subroutine c_plspal1
   end interface

   call c_plspal1( trim(filename) // c_null_char, int( merge(1,0,interpolate),kind=private_plint) )

end subroutine plspal1

subroutine plspause( lpause )
    logical, intent(in) :: lpause

    interface
        subroutine c_plspause( ipause ) bind(c,name='c_plspause')
            implicit none
            include 'included_plplot_interface_private_types.f90'
            integer(kind=private_plint), value :: ipause
        end subroutine c_plspause
    end interface

   call c_plspause( int( merge(1,0,lpause),kind=private_plint) )
end subroutine plspause

subroutine plsstrm( strm )
    integer, intent(in) :: strm
    interface
        subroutine c_plsstrm( strm ) bind( c, name = 'c_plsstrm' )
            implicit none
            include 'included_plplot_interface_private_types.f90'
            integer(kind=private_plint), value :: strm
        end subroutine c_plsstrm
    end interface

    call c_plsstrm( int(strm,kind=private_plint) )
end subroutine plsstrm

subroutine plssub( nx, ny )
    integer, intent(in) :: nx, ny
    interface
        subroutine c_plssub( nx, ny ) bind( c, name = 'c_plssub' )
            implicit none
            include 'included_plplot_interface_private_types.f90'
            integer(kind=private_plint), value :: nx, ny
        end subroutine c_plssub
    end interface

    call c_plssub( int(nx,kind=private_plint), int(ny,kind=private_plint) )
end subroutine plssub

subroutine plstar( nx, ny )
    integer, intent(in) :: nx, ny
    interface
        subroutine c_plstar( nx, ny ) bind( c, name = 'c_plstar' )
            implicit none
            include 'included_plplot_interface_private_types.f90'
            integer(kind=private_plint), value :: nx, ny
        end subroutine c_plstar
    end interface

    call c_plstar( int(nx,kind=private_plint), int(ny,kind=private_plint) )
end subroutine plstar

subroutine plstripd( id )
    integer, intent(in) :: id
    interface
        subroutine c_plstripd( id ) bind( c, name = 'c_plstripd' )
            implicit none
            include 'included_plplot_interface_private_types.f90'
            integer(kind=private_plint), value :: id
        end subroutine c_plstripd
    end interface

    call c_plstripd( int(id,kind=private_plint) )
end subroutine plstripd

subroutine plstyl_scalar( n, mark, space )
    integer, intent(in) :: n, mark, space

    call plstyl_n_array( n, (/ mark /), (/ space /) )
end subroutine plstyl_scalar

subroutine plstyl_array( mark, space )
    integer, dimension(:), intent(in) :: mark, space

    call plstyl_n_array( size(mark), mark, space )
end subroutine plstyl_array

subroutine plstyl_n_array( n, mark, space )
    integer, intent(in) :: n
    integer, dimension(:), intent(in) :: mark, space
    interface
        subroutine c_plstyl( n, mark, space ) bind( c, name = 'c_plstyl' )
            implicit none
            include 'included_plplot_interface_private_types.f90'
            integer(kind=private_plint), value :: n
            integer(kind=private_plint), dimension(*) :: mark, space
        end subroutine c_plstyl
    end interface

    call c_plstyl( int(n,kind=private_plint), int(mark,kind=private_plint), int(space,kind=private_plint) )
end subroutine plstyl_n_array

subroutine plsxax( digmax, digits )
    integer, intent(in) :: digmax, digits
    interface
        subroutine c_plsxax( digmax, digits ) bind( c, name = 'c_plsxax' )
            implicit none
            include 'included_plplot_interface_private_types.f90'
            integer(kind=private_plint), value :: digmax, digits
        end subroutine c_plsxax
    end interface

    call c_plsxax( int(digmax,kind=private_plint), int(digits,kind=private_plint) )
end subroutine plsxax

subroutine plsyax( digmax, digits )
    integer, intent(in) :: digmax, digits
    interface
        subroutine c_plsyax( digmax, digits ) bind( c, name = 'c_plsyax' )
            implicit none
            include 'included_plplot_interface_private_types.f90'
            integer(kind=private_plint), value :: digmax, digits
        end subroutine c_plsyax
    end interface

    call c_plsyax( int(digmax,kind=private_plint), int(digits,kind=private_plint) )
end subroutine plsyax

subroutine plszax( digmax, digits )
    integer, intent(in) :: digmax, digits
    interface
        subroutine c_plszax( digmax, digits ) bind( c, name = 'c_plszax' )
            implicit none
            include 'included_plplot_interface_private_types.f90'
            integer(kind=private_plint), value :: digmax, digits
        end subroutine c_plszax
    end interface

    call c_plszax( int(digmax,kind=private_plint), int(digits,kind=private_plint) )
end subroutine plszax

subroutine pltext()
    interface
        subroutine interface_pltext() bind(c,name='c_pltext')
        end subroutine interface_pltext
     end interface
     call interface_pltext()
end subroutine pltext

subroutine pltimefmt_impl( fmt )
   character(len=*), intent(in) :: fmt

   interface
       subroutine c_pltimefmt( fmt ) bind(c,name='c_pltimefmt')
           implicit none
           character(len=1), dimension(*), intent(in) :: fmt
       end subroutine c_pltimefmt
   end interface

   call c_pltimefmt( trim(fmt) // c_null_char )

end subroutine pltimefmt_impl

subroutine plvsta()
    interface
        subroutine interface_plvsta() bind(c,name='c_plvsta')
        end subroutine interface_plvsta
     end interface
     call interface_plvsta()
end subroutine plvsta

end module plplot
