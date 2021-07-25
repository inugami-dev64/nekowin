--- nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
--- licence: Apache, see LICENCE.md
--- file: libnwin.lua - libnwin build configuration
--- author: Karl-Mihkel Ott

local libnwin = {}

function libnwin.build() 
    project "nwin"
        if _OPTIONS["shared"] then
            kind "SharedLib"
        else 
            kind "StaticLib"
        end
        language "C"
        cdialect "C99"

        files {
            "include/*.h",
            "src/*.c",
        }

        removefiles{ "src/vk.c", "include/vk.h" }

        filter "platforms:Win32"
            removefiles { "src/x11_surface.c" }
            links {
                "vulkan-1",
                "comctl32",
                "gdi32",
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
                "dl",
                "vulkan"
            }

        filter {}
end


return libnwin
