add_library(gucci "")
configure_uplink_component(gucci)

target_compile_definitions(gucci PRIVATE USE_SDL2 USE_FTGL)

uplink_glob_files(UPLINK_GUCCI_SOURCES
	${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)

uplink_glob_files(UPLINK_GUCCI_HEADERS
	${CMAKE_CURRENT_LIST_DIR}/src/*.h
	${CMAKE_CURRENT_LIST_DIR}/data/*.txt)

target_sources(gucci PRIVATE ${UPLINK_GUCCI_SOURCES} PUBLIC ${UPLINK_GUCCI_HEADERS})
target_include_directories(gucci 
	SYSTEM PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src)

find_package(xxHash CONFIG REQUIRED)
find_package(soil2 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(TIFF REQUIRED)

target_link_libraries(gucci PRIVATE 
	tosser
	ftgl
	TIFF::TIFF
	xxHash::xxhash
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
	soil2
	glm::glm
	GLEW::GLEW)