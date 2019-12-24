# MessagesLib shared library

cmake_minimum_required (VERSION 3.8)

set(HELPER_LIB_NAME "MessagesLib")

file(GLOB_RECURSE LIB_SOURCES "src/*.cpp")
file(GLOB_RECURSE LIB_HEADERS "src/*.h" "include/*.h")

include_directories(include ${ZeroMQ_INCLUDE_DIR})
include_directories(include ${OpenCV_INCLUDE_DIRS})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Add source to this project's executable.
add_library(${HELPER_LIB_NAME} STATIC ${LIB_SOURCES} ${LIB_HEADERS})

if(NOT WIN32)
	target_link_libraries(${HELPER_LIB_NAME} stdc++fs)
endif()
target_link_libraries(${HELPER_LIB_NAME} libzmq-static)
target_link_libraries(${HELPER_LIB_NAME} JsonCppLib)
target_include_directories(${HELPER_LIB_NAME} PUBLIC src PUBLIC include PUBLIC include/rapidjson)

install(TARGETS ${HELPER_LIB_NAME} DESTINATION lib)
install(FILES ${LIB_HEADERS} DESTINATION include)