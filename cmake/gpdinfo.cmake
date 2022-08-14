# nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
# licence: Apache, see LICENCE.md
# file: gpdinfo.cmake - gpdinfo test application build config
# author: Karl-Mihkel Ott

set(GPDINFO_TARGET gpdinfo)
set(GPDINFO_SOURCES src/gpdinfo.c)

add_executable(${GPDINFO_TARGET} ${GPDINFO_SOURCES})

if(NEKOWIN_BUILD_SHARED_LIB)
	add_dependencies(${GPDINFO_TARGET} ${LIBNWIN_SHARED_TARGET})
	target_link_libraries(${GPDINFO_TARGET} 
		PRIVATE ${LIBNWIN_SHARED_TARGET})
else()
	add_dependencies(${GPDINFO_TARGET} ${LIBNWIN_STATIC_TARGET})
	target_link_libraries(${GPDINFO_TARGET}
		PRIVATE ${LIBNWIN_STATIC_TARGET})
endif()