cmake_minimum_required(VERSION 3.2)
project(controllerpatcher)
include("${WUT_ROOT}/share/wut.cmake" REQUIRED)

file(GLOB_RECURSE SOURCE_FILES *.c *.cpp)
file(GLOB_RECURSE HEADER_FILES *.h)

add_library(controllerpatcher STATIC ${SOURCE_FILES} ${HEADER_FILES})

target_link_libraries(controllerpatcher
   utils)

target_include_directories(controllerpatcher PUBLIC "include")
target_include_directories(controllerpatcher PRIVATE "src")


include_directories("${WUT_ROOT}/include/libutils" REQUIRED)

wut_enable_stdcpp(controllerpatcher)
wut_default_malloc(controllerpatcher)

target_include_directories(controllerpatcher PUBLIC "include")
target_compile_options(controllerpatcher PUBLIC "-D__LOGGING__")


install(TARGETS controllerpatcher
        ARCHIVE DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
        DESTINATION "${CMAKE_INSTALL_PREFIX}/include"
        FILES_MATCHING PATTERN "*.h*")