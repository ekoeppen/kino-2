#!/usr/bin/X11/mofe --f

load /usr/lib/X11/wafe/tcllib/bin/m-kino2Gen.so KINO2

AHDKino k topLevel
realize

proc callFunc {name} {
    upvar current current

    AHDCallFunc $current $name
}

proc setVar {name value} {
    upvar current current

    AHDSetVar $current $name $value
}

proc getVar {name} {
    upvar current current

    return [AHDGetVar $current $name]
}

proc scriptCallback {k source current script} {
    set kino $k

    eval $script
}

proc push {data} {
    upvar kino kino

    AHDPush $kino.ahd $data
}

proc pop {} {
    upvar kino kino

    return [AHDPop $kino.ahd]
}

sV k scriptCallback "scriptCallback %W {%e} {%C} {%s}"

set file [open "test.html"]
set text [read $file]
sV k text $text