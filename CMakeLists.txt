cmake_minimum_required(VERSION 3.2)
project(controllerpatcherwut)
include("${WUT_ROOT}/share/wut.cmake" REQUIRED)

file(GLOB_RECURSE SOURCE_FILES *.c *.cpp)
file(GLOB_RECURSE HEADER_FILES *.h)

add_library(controllerpatcherwut STATIC ${SOURCE_FILES} ${HEADER_FILES})

target_link_libraries(controllerpatcherwut
   utilswut)

target_include_directories(controllerpatcherwut PUBLIC "include")
target_include_directories(controllerpatcherwut PRIVATE "src")

include_directories("${WUT_ROOT}/include/libutils" REQUIRED)

wut_enable_stdcpp(controllerpatcherwut)
wut_default_malloc(controllerpatcherwut)

target_include_directories(controllerpatcherwut PUBLIC "include")
target_compile_options(controllerpatcherwut PUBLIC "-D__LOGGING__")


install(TARGETS controllerpatcherwut
        ARCHIVE DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
        DESTINATION "${CMAKE_INSTALL_PREFIX}/include"
        FILES_MATCHING PATTERN "*.h*")