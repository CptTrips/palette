workspace "palette"
    configurations { "Debug", "Release" }
    location "build"

    architecture "x64"

project "palette"
    kind "StaticLib"
    language "C++"
    targetdir "lib/%{cfg.buildcfg}"

    files {"palette.h", "palette.cc"}

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"


project "DemoApp"
    kind "ConsoleApp"
    language "C++"
    targetdir "bin/%{cfg.buildcfg}"

    files {"main.cc"}

    links {"palette"}

    libdirs {"lib/%{cfg.buildcfg}"}

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"