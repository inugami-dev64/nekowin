--- nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
--- licence: MIT, see LICENCE.md
--- file: premake5.lua - main premake configuration file
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

local libnwin = require("libnwin")
libnwin.build()
