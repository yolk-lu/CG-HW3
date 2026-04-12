cmake_minimum_required (VERSION 3.5)
project (cghw-rast)

#find_package(GLUT)
find_package(OpenGL)
find_package(glm)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
pkg_search_module(GLEW REQUIRED glew)

include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLFW_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} )

#if(CMAKE_COMPILER_IS_GNUCXX)
#    add_definitions(-std=gnu++11)
#endif()


# problem about linking .so file problems
if(UNIX AND NOT APPLE)
    if(EXISTS "/usr/lib/x86_64-linux-gnu")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath-link=/usr/lib/x86_64-linux-gnu")
    elseif(EXISTS "/usr/lib64")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath-link=/usr/lib64")
    elseif(EXISTS "/usr/lib")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath-link=/usr/lib")
    endif()
endif()

# cghw-rast
add_executable(rast hw-rast.cpp swgl-rast.cpp swgl.cpp export_utils.cpp)

if(UNIX AND NOT APPLE)
    # Linux (Ubuntu, CentOS, etc.)

    target_link_libraries(rast ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${GLFW_LIBRARIES} X11 pthread dl Xrandr Xi Xxf86vm)
elseif(APPLE)
    # macOS 
    target_link_libraries(rast ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${GLFW_LIBRARIES} dl pthread)
else()
    # Windows 
    target_link_libraries(rast ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${GLFW_LIBRARIES})
endif()