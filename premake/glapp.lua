--- nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
--- licence: Apache, see LICENCE.md
--- file: premake5.lua - main premake configuration file
--- author: Karl-Mihkel Ott


project "glapp"
	basedir("..")
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++14"

	includedirs { "../include" }
	files { 
		"../src/glapp.cpp",
		"../include/*.h"
	}
	links { "nwin" }
	
	if not _OPTIONS["shared"] then
		defines { "LIBNWIN_STATIC" }
	end

	filter "platforms:Win32"
		includedirs { "C:\\VulkanSDK\\**\\Include" }

	filter {}
