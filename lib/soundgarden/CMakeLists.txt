add_library(soundgarden "")
configure_uplink_component(soundgarden)

target_compile_definitions(soundgarden PRIVATE USE_SDL2)

uplink_glob_files(UPLINK_SOUNDGARDEN_SOURCES
                  ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)

uplink_glob_files(UPLINK_SOUNDGARDEN_HEADERS ${CMAKE_CURRENT_LIST_DIR}/src/*.h)

target_sources(
  soundgarden
  PRIVATE ${UPLINK_SOUNDGARDEN_SOURCES}
  PUBLIC ${UPLINK_SOUNDGARDEN_HEADERS})
target_include_directories(soundgarden SYSTEM
                           PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src)

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)

target_link_libraries(
  soundgarden
  PRIVATE
    tosser
    redshirt
    eclipse
    gucci
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>
)
