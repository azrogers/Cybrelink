add_library(irclib "")
configure_uplink_component(irclib)

uplink_glob_files(UPLINK_IRCLIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp
                  $<UNIX:${CMAKE_CURRENT_LIST_DIR}/src/linux/*.cpp>)

uplink_glob_files(UPLINK_IRCLIB_HEADERS ${CMAKE_CURRENT_LIST_DIR}/src/*.h
                  $<UNIX:${CMAKE_CURRENT_LIST_DIR}/src/linux/*.h>)

target_sources(
  irclib
  PRIVATE ${UPLINK_IRCLIB_SOURCES}
  PUBLIC ${UPLINK_IRCLIB_HEADERS})
target_include_directories(irclib SYSTEM PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src)
