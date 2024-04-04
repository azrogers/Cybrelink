add_library(vanbakel "")
configure_uplink_component(vanbakel)

uplink_glob_files(UPLINK_VANBAKEL_SOURCES
	${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)

uplink_glob_files(UPLINK_VANBAKEL_HEADERS
	${CMAKE_CURRENT_LIST_DIR}/src/*.h)

target_sources(vanbakel PRIVATE ${UPLINK_VANBAKEL_SOURCES} PUBLIC ${UPLINK_VANBAKEL_HEADERS})
target_include_directories(vanbakel SYSTEM PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src)

target_link_libraries(vanbakel PRIVATE 
	tosser
	gucci
	eclipse)