cmake_minimum_required(VERSION 3.10)
project(jsonObject)
add_library(jsonObject STATIC src/JsonObject.cpp)
target_include_directories(jsonObject PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_executable(test src/test.cpp)
target_link_libraries(test PRIVATE jsonObject)