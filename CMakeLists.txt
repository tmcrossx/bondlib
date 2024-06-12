cmake_minimum_required(VERSION 3.16)
project(bondlib.t)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(bondlib.t bondlib.t.cpp)

target_compile_definitions(bondlib.t PUBLIC _DEBUG)
target_compile_options(bondlib.t PUBLIC
	-Wall
	-Wno-unknown-pragmas
	-Wextra
	-Wpedantic
	-fsanitize=address
)
target_link_options(bondlib.t PUBLIC -fsanitize=address)

if(FALSE)
target_include_directories(bondlib.t PRIVATE 
${PROJECT_SOURCE_DIR}/curve
${PROJECT_SOURCE_DIR}/date
${PROJECT_SOURCE_DIR}/fms_iterable
${PROJECT_SOURCE_DIR}/instrument
${PROJECT_SOURCE_DIR}/math
${PROJECT_SOURCE_DIR}/valuation
${PROJECT_SOURCE_DIR}/variate)
endif()


