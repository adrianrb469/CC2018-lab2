cmake_minimum_required(VERSION 3.21)

project(conway VERSION 1.0)

# Enable C++20 features
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# Find SDL2
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

# Add the source files to the project
set(SOURCES
    src/main.cpp
    src/Framebuffer.cpp
    src/Color.cpp
    src/Vertex2.cpp
)

# Add the header files to the project
set(HEADERS
    src/Framebuffer.h
    src/Color.h
    src/Vertex2.h   
)


add_executable(${PROJECT_NAME}
    src/main.cpp
    ${SOURCES}
)

target_include_directories(${PROJECT_NAME}
    PRIVATE ${PROJECT_SOURCE_DIR}/include
    PUBLIC ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME}
    ${SDL2_LIBRARIES}

)