add_library(redshirt "")
configure_uplink_component(redshirt)

uplink_glob_files(UPLINK_REDSHIRT_SOURCES
	${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)

uplink_glob_files(UPLINK_REDSHIRT_HEADERS
	${CMAKE_CURRENT_LIST_DIR}/src/*.h)

target_sources(redshirt PRIVATE ${UPLINK_REDSHIRT_SOURCES} PUBLIC ${UPLINK_REDSHIRT_HEADERS})
target_include_directories(redshirt SYSTEM PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src)

target_link_libraries(redshirt PRIVATE 
	tosser
	bungle
)