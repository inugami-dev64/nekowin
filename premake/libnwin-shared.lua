--- nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
--- licence: Apache, see LICENCE.md
--- file: libnwin-shared.lua - libnwin shared library build configuration
--- author: Karl-Mihkel Ott

project "nwin"
	basedir("..")
	kind "SharedLib"
	language "C"
	cdialect "C99"

	includedirs { "../include" }
	files {
		"../include/*.h",
		"../src/*.c"
	}
	
	defines { "LIBNWIN_EXPORT_LIBRARY", "GLAD_GLAPI_EXPORT", "GLAD_GLAPI_EXPORT_BUILD" }

	removefiles{ "../src/vk.c", "../include/vk.h" }

	filter "platforms:Win32"
		removefiles { "../src/x11_window.c" }
		defines { "_CRT_SECURE_NO_WARNINGS" }
		links {
			"vulkan-1",
			"gdi32",
			"kernel32"
		}

	filter "platforms:Linux"
		removefiles { "../src/win32_window.c" }
		links {
			"X11",
			"Xcursor",
			"Xrandr",
			"GL",
			"dl",
			"vulkan"
		}

	filter {}