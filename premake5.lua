--- nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
--- licence: Apache, see LICENCE.md
--- file: premake5 - main premake configuration file
--- author: Karl-Mihkel Ott

workspace "nwin"
    configurations { "Debug", "Release" }
    platforms { "Win32", "Linux" }
    includedirs { "include" }
    architecture "x86_64"
    targetdir "build"
    pic "On"

    -- Debug and 
    filter "configurations:Debug"
        symbols "On"
        optimize "Debug"
    filter "configurations:Release"
        symbols "Off"
        optimize "Speed"
    filter {}

newoption {
    trigger = "no-test-apps",
    description = "Do not build test applications with nekowin"
}

newoption {
    trigger = "shared",
    description = "Build shared libraries instead of static ones"
}

local libnwin = require("premake/libnwin")
libnwin.build()

local libnwinpp = require("premake/libnwinpp")
libnwinpp.build()

if not _OPTIONS["no-test-app"] then
    local glapp = require("premake/glapp")
    glapp.build()
end
