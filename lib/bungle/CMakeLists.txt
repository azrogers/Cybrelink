add_library(bungle "")
configure_uplink_component(bungle)

uplink_glob_files(UPLINK_BUNGLE_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)

uplink_glob_files(UPLINK_BUNGLE_HEADERS ${CMAKE_CURRENT_LIST_DIR}/src/*.h)

target_sources(
  bungle
  PRIVATE ${UPLINK_BUNGLE_SOURCES}
  PUBLIC ${UPLINK_BUNGLE_HEADERS})
target_include_directories(bungle SYSTEM PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src)

target_link_libraries(bungle PRIVATE tosser)
