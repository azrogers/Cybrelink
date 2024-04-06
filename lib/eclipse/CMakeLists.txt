add_library(eclipse "")
configure_uplink_component(eclipse)

uplink_glob_files(UPLINK_ECLIPSE_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)

uplink_glob_files(UPLINK_ECLIPSE_HEADERS ${CMAKE_CURRENT_LIST_DIR}/src/*.h)

target_sources(
  eclipse
  PRIVATE ${UPLINK_ECLIPSE_SOURCES}
  PUBLIC ${UPLINK_ECLIPSE_HEADERS})
target_include_directories(eclipse SYSTEM PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src)

find_package(spdlog CONFIG REQUIRED)

target_link_libraries(eclipse PRIVATE tosser gucci spdlog::spdlog)
