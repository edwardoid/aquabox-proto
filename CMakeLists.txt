cmake_minimum_required(VERSION 3.5)
project(aquabox-proto VERSION 1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 11)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp")
file(GLOB INCLUDES "${CMAKE_CURRENT_LIST_DIR}/include/*.h")

if (LIBOWT_SHARED)
   add_library(${PROJECT_NAME} SHARED ${SOURCES})
else()
   add_library(${PROJECT_NAME} STATIC ${SOURCES})
endif()

target_include_directories(${PROJECT_NAME} PRIVATE include)

set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE 1)
set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION 1)

install(TARGETS ${PROJECT_NAME} DESTINATION lib)

if (BUILD_TEST)
   file(GLOB TEST_SOURCES "${CMAKE_CURRENT_LIST_DIR}/tests/*.cpp")
   add_executable(${PROJECT_NAME}_test ${PROJECT_NAME} ${TEST_SOURCES})
   include_directories(${PROJECT_NAME}_test "include")

   target_link_libraries (${PROJECT_NAME}_test ${PROJECT_NAME})
endif()

set(BUILD_INSPECTOR ON)

if (BUILD_INSPECTOR)
   file(GLOB INSPECTOR_SOURCES "${CMAKE_CURRENT_LIST_DIR}/inspector/*.cpp")
   add_executable(${PROJECT_NAME}_inspector ${INSPECTOR_SOURCES})
   include_directories(${PROJECT_NAME}_inspector "include")

   target_link_libraries (${PROJECT_NAME}_inspector ${PROJECT_NAME})
endif()


