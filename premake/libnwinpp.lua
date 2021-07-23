--- nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
--- licence: Apache, see LICENCE.md
--- file: libnwinpp.lua - libnwinpp build configuration
--- author: Karl-Mihkel Ott

local libnwinpp = {}

function libnwinpp.build() 
    project "nwinpp"
        if _OPTIONS["shared"] then
            kind "SharedLib"
        else
            kind "StaticLib"
        end
        language "C++"
        cppdialect "C++14"

        files {
            "include/*.h",
            "src/*.c",
            "src/nwin.cpp"
        }

        removefiles { "src/vk.c", "include/vk.h" }

        filter "platforms:Win32"
            removefiles { "src/x11_surface.c" }
            links {
                "vulkan-1",
                "comctl32",
                "iphlpapi"
            }
        filter "platforms:Linux"
            removefiles { "src/win32_surface.c" }
            links {
                "X11",
                "Xcursor",
                "Xrandr",
                "GL",
                "GLU",
                "dl"
            }
        filter {}
end

return libnwinpp
