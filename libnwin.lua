--- nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
--- licence: MIT, see LICENCE.md
--- file: libnwin.lua - libnwin build configuration
--- author: Karl-Mihkel Ott

local libnwin = {}

function libnwin.build() 
    project "nwin"
        kind "SharedLib"
        language "C++"
        cppdialect "C++14"

        files {
            "include/*.h",
            "src/*.c",
            "src/*.cpp"
        }

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
                "GL",
                "GLU",
                "dl"
            }

        filter {}
end


return libnwin
