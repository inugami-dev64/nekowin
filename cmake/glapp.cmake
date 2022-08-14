# nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
# licence: Apache, see LICENCE.md
# file: glapp.cmake - glapp test application build config
# author: Karl-Mihkel Ott

set(GLAPP_TARGET glapp)
set(GLAPP_SOURCES src/glapp.cpp)

add_executable(${GLAPP_TARGET} ${GLAPP_SOURCES})

if(NEKOWIN_BUILD_SHARED_LIB)
	add_dependencies(${GLAPP_TARGET} ${LIBNWIN_SHARED_TARGET})
	target_link_libraries(${GLAPP_TARGET} PRIVATE ${LIBNWIN_SHARED_TARGET})
else()
	add_dependencies(${GLAPP_TARGET} ${LIBNWIN_STATIC_TARGET})
	target_link_libraries(${GLAPP_TARGET} PRIVATE ${LIBNWIN_STATIC_TARGET})
endif()