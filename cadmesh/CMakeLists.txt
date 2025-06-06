cmake_minimum_required(VERSION 3.6 FATAL_ERROR)

project(cadmesh)

set(CADMESH_MAJOR_VERSION 1)
set(CADMESH_MINOR_VERSION 1)
set(CADMESH_PATCH_VERSION 0)
set(CADMESH_VERSION
    ${CADMESH_MAJOR_VERSION}.${CADMESH_MINOR_VERSION}.${CADMESH_PATCH_VERSION})

include(FindPkgConfig)
include(ExternalProject)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/Modules ${CMAKE_MODULE_PATH})

# Make the include and lib directories in the build area.
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/include)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# GEANT4
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
    find_package(Geant4 REQUIRED ui_all vis_all)
else()
    find_package(Geant4 REQUIRED)
endif()
include(${Geant4_USE_FILE})

# X11 if on a Mac
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(OPENGL_INCLUDE_DIR_MAC /var/empty/X11/include )
    include_directories(${OPENGL_INCLUDE_DIR_MAC})

    set(OPENGL_INCLUDE_DIR /var/empty/include )
    include_directories(${OPENGL_INCLUDE_DIR})
endif()

# Tetgen
option(WITH_SYS_TETGEN "WITH_SYS_TETGEN" OFF)
if(${WITH_SYS_TETGEN} MATCHES "ON")
    find_package(tetgen)
else()
    ExternalProject_Add(
        tetgen_external
        SOURCE_DIR ${PROJECT_SOURCE_DIR}/external/tetgen
        GIT_REPOSITORY https://github.com/christopherpoole/tetgen.git
        CMAKE_ARGS -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR}
    )
    include_directories(${PROJECT_BINARY_DIR}/include)
endif()

# Open Asset Importer Library
option(WITH_SYS_ASSIMP "WITH_SYS_ASSIMP" OFF)
if(${WITH_SYS_ASSIMP} MATCHES "ON")
    find_package(assimp)
else()
    ExternalProject_Add(
        assimp_external
        SOURCE_DIR ${PROJECT_SOURCE_DIR}/external/assimp
        GIT_REPOSITORY https://github.com/assimp/assimp.git
        GIT_TAG v5.4.3
        CMAKE_ARGS -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR}
    )
    include_directories(${PROJECT_BINARY_DIR}/include/assimp/include)
endif()

# copy cadmeshBuild/lib64

# CADMesh
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${CMAKE_INSTALL_PREFIX}/include)

# Set up the target.
add_library(cadmesh "SHARED" ${sources} ${headers}) 

# Link against either the system libraries, or the external project libaries.
if(${WITH_SYS_TETGEN} MATCHES "OFF")
    add_dependencies(cadmesh tetgen_external)
    target_link_libraries(cadmesh -L${CMAKE_BINARY_DIR}/lib tet)
else()
    target_link_libraries(cadmesh tet)
endif()

if(${WITH_SYS_ASSIMP} MATCHES "OFF")
    add_dependencies(cadmesh assimp_external)
    target_link_libraries(cadmesh -L${CMAKE_BINARY_DIR}/lib assimp)
    target_link_libraries(cadmesh -L${CMAKE_BINARY_DIR}/lib64 assimp)
else()
    target_link_libraries(cadmesh assimp)
endif()

target_link_libraries(cadmesh ${Geant4_LIBRARIES})

# Configure the installation.
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cadmesh-config.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/cadmesh-config.cmake" @ONLY IMMEDIATE)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/cadmesh-config.cmake" DESTINATION "lib/cmake/cadmesh-${CADMESH_MAJOR_VERSION}.${CADMESH_MINOR_VERSION}.${CADMESH_PATCH_VERSION}")

# Install the CADMesh headers library.
install(FILES ${PROJECT_SOURCE_DIR}/include/CADMesh.hh DESTINATION include)
install(TARGETS cadmesh DESTINATION lib)

# Install the external project headers.
install(DIRECTORY ${CMAKE_BINARY_DIR}/include/ DESTINATION include)

# Install the external project libraries.
install(DIRECTORY ${CMAKE_BINARY_DIR}/lib/ DESTINATION lib)

# Build the examples as well.
add_subdirectory(${PROJECT_SOURCE_DIR}/examples/basic)

# Set CPack to build an archive.
set(CPACK_GENERATOR "TGZ")
set(CPACK_PACKAGE_NAME "cadmesh")
set(CPACK_PACKAGE_VERSION ${CADMESH_VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR ${CADMESH_MAJOR_VERSION})
set(CPACK_PACKAGE_VERSION_MINOR ${CADMESH_MINOR_VERSION})
set(CPACK_PACKAGE_VERSION_PATCH ${CADMESH_PATCH_VERSION})
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY 0)
include(CPack)

