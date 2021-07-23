--- nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
--- licence: Apache, see LICENCE.md
--- file: premake5.lua - main premake configuration file
--- author: Karl-Mihkel Ott

local glapp = {}

function glapp.build()
    project "glapp"
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++14"

        files { 
            "src/glapp.cpp",
            "include/*.h"
        }
        links { 
            "nwin",
            "vulkan"
        }
end

return glapp
