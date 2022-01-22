--- nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
--- licence: Apache, see LICENCE.md
--- file: premake5 - main premake configuration file
--- author: Karl-Mihkel Ott

workspace "nwin"
    cleancommands { "make clean %{cfg.buildcfg}" }
    configurations { "Debug", "Release" }
    platforms { "Win32", "Linux" }
    includedirs { "include", "include/third_party" }
    architecture "x86_64"
    pic "On"

    -- Debug and 
    filter "configurations:Debug"
        symbols "On"
        optimize "Debug"
        targetdir "build/debug"

    filter "configurations:Release"
        symbols "Off"
        optimize "Speed"
        targetdir "build/release"

    -- Include Vulkan headers from the SDK path if using Windows
    filter "platforms:Win32" 
        includedirs { "C:\\VulkanSDK\\**\\Include" }
        libdirs { "C:\\VulkanSDK\\**\\Lib" }
    filter {}

newoption {
    trigger = "no-test-apps",
    description = "Do not build test applications with nekowin"
}

newoption {
    trigger = "shared",
    description = "Build shared libraries instead of static ones"
}

if _OPTIONS["shared"] then
	require "premake/libnwin-shared"
else
	require "premake/libnwin-static"
end

if not _OPTIONS["no-test-app"] then
    require "premake/glapp"
end
