add_library(ftgl "")
configure_uplink_component(ftgl)

target_compile_definitions(ftgl PRIVATE FTGL_LIBRARY_STATIC)

uplink_glob_files(UPLINK_FTGL_SOURCES
	${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)

uplink_glob_files(UPLINK_FTGL_HEADERS
	${CMAKE_CURRENT_LIST_DIR}/include/FTGL/*.h)

target_sources(ftgl PRIVATE ${UPLINK_FTGL_SOURCES} PUBLIC ${UPLINK_FTGL_HEADERS})
target_include_directories(ftgl SYSTEM PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include/FTGL)

find_package(Freetype REQUIRED)
target_link_libraries(ftgl PRIVATE Freetype::Freetype)