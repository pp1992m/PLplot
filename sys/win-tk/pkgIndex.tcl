if {[info exists tcl_platform(debug)]} {
    set file [file join $dir plplot511d[info sharedlibextension]]
} else {
    set file [file join $dir plplot511[info sharedlibextension]]
}

# This little helper is needed to deal seamlessly with the
# possibility that either or both $dir and $file may contain
# spaces, or characters like []{}
proc loadPlplot {dir file} {
    global pllibrary
    set pllibrary $dir
    load $file Plplotter
    rename loadPlplot {}
# put core tcl scripts in path
    lappend auto_path $dir/tcl
}

package ifneeded Plplotter 5.1.1 [list loadPlplot $dir $file]
unset file