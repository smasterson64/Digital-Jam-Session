cmake_minimum_required(VERSION 3.18)
project(as6 CXX)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(../raylib-cpp raylib)

include(../assets/includeable.cmake)

add_executable(as6 as6.cpp skybox.cpp raygui.cpp)
target_link_libraries(as6 PUBLIC raylib raylib_cpp raygui raylib::buffered)

make_includeable(../assets/shaders/skybox.vs generated/skybox.vs)
make_includeable(../assets/shaders/skybox.fs generated/skybox.fs)
make_includeable(../assets/shaders/cubemap.vs generated/cubemap.vs)
make_includeable(../assets/shaders/cubemap.fs generated/cubemap.fs)


configure_file(../assets/models/flute.glb models/flute.glb COPYONLY)
configure_file(../assets/models/piano.glb models/piano.glb COPYONLY)
configure_file(../assets/models/trumpet.glb models/trumpet.glb COPYONLY)
configure_file(../assets/models/violin.glb models/violin.glb COPYONLY)
configure_file(../assets/audio/ins_flute.wav audio/ins_flute.wav COPYONLY)
configure_file(../assets/audio/testsong.wav audio/testsong.wav COPYONLY)
configure_file(../assets/audio/ins_piano.wav audio/ins_piano.wav COPYONLY)
configure_file(../assets/audio/ins_trumpet.wav audio/ins_trumpet.wav COPYONLY)
configure_file(../assets/audio/ins_violin.wav audio/ins_violin.wav COPYONLY)

configure_file(../assets/textures/skybox.png textures/skybox.png COPYONLY)
configure_file(../assets/textures/snow.jpg textures/snow.jpg COPYONLY)