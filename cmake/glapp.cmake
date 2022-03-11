# nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
# licence: Apache, see LICENCE.md
# file: glapp.cmake - glapp test application build config
# author: Karl-Mihkel Ott

set(GLAPP_TARGET glapp)
set(GLAPP_SOURCES src/glapp.cpp)

add_executable(${GLAPP_TARGET} ${GLAPP_SOURCES})
target_include_directories(${GLAPP_TARGET} PRIVATE include)
target_link_libraries(${GLAPP_TARGET} PRIVATE ${LIBNWIN_SHARED_TARGET})
