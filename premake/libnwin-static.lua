--- nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
--- licence: Apache, see LICENCE.md
--- file: libnwin-static.lua - libnwin static library build configuration
--- author: Karl-Mihkel Ott

project "nwin"
	basedir("..")
	kind "StaticLib"
	language "C"
	cdialect "C99"

	includedirs { "../include" }
	files {
		"../include/*.h",
		"../src/*.c"
	}

	defines { "LIBNWIN_EXPORT_LIBRARY", "LIBNWIN_STATIC" }
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