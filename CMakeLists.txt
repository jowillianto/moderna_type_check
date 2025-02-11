cmake_minimum_required(VERSION 3.28)
project (moderna_type_check)
set (CMAKE_CXX_STANDARD 23)
set (CMAKE_CXX_STANDARD_REQUIRED true)
set (CMAKE_CXX_SCAN_FOR_MODULES true)

if (NOT TARGET ${PROJECT_NAME})
  file(GLOB ${PROJECT_NAME}_src ${CMAKE_CURRENT_LIST_DIR}/src/*.cc)
  add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/libs/reflect-cpp)
  add_library(${PROJECT_NAME})
  target_sources(${PROJECT_NAME}
    PUBLIC
      FILE_SET CXX_MODULES FILES ${${PROJECT_NAME}_src}
  )
  target_link_libraries(${PROJECT_NAME} PUBLIC reflectcpp)
endif()
# Python Build
if (EXISTS ${CMAKE_CURRENT_LIST_DIR}/libs/pybind11/CMakeLists.txt AND NOT BUILD_EMSCRIPTEN)
  find_package(Python3 3.7 REQUIRED COMPONENTS Interpreter Development)
  add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/libs/pybind11 EXCLUDE_FROM_ALL)
  add_library(${PROJECT_NAME}_python SHARED EXCLUDE_FROM_ALL)
  target_sources(${PROJECT_NAME}_python
    PUBLIC
      ${CMAKE_CURRENT_LIST_DIR}/wrappers/python.cpp
    PUBLIC FILE_SET CXX_MODULES FILES ${${PROJECT_NAME}_src} 
  )
  target_include_directories(${PROJECT_NAME}_python
    PUBLIC 
      ${pybind11_INCLUDE_DIRS}
      ${Python_INCLUDE_DIRS}
  )
  target_link_libraries(${PROJECT_NAME}_python
      ${pybind11_LIBRARIES}
      pybind11::module
      reflectcpp
  )
  set_target_properties(${PROJECT_NAME}_python PROPERTIES PREFIX "")
  set_target_properties(${PROJECT_NAME}_python PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
  set_target_properties(${PROJECT_NAME}_python PROPERTIES SUFFIX ".so")
  target_compile_options(${PROJECT_NAME}_python PRIVATE "-fPIC")
endif()

if (
  EXISTS ${CMAKE_CURRENT_LIST_DIR}/../test-lib
  AND PROJECT_IS_TOP_LEVEL
  AND NOT BUILD_EMSCRIPTEN
)
  include(CTest)
  add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../test-lib test-lib)
  add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../io io)
  moderna_add_test(
    ${PROJECT_NAME}_tests
    ${CMAKE_CURRENT_LIST_DIR}/tests/tests.cpp
    LIBRARIES ${PROJECT_NAME} moderna_test_lib moderna_io
    COMPILE_DEFINITIONS 
      TYPE_A_FILE="${CMAKE_CURRENT_LIST_DIR}/assets/type_a.json" 
      TYPE_B_FILE="${CMAKE_CURRENT_LIST_DIR}/assets/type_b.json"
    SANITIZERS all
  )
  moderna_add_test(
    ${PROJECT_NAME}_parser_tests
    ${CMAKE_CURRENT_LIST_DIR}/tests/parser.cpp
    LIBRARIES ${PROJECT_NAME} moderna_test_lib moderna_io
    SANITIZERS all
  )
else()
  message (${PROJECT_NAME} : "Clone Moderna Library to Test")
endif()

if(BUILD_EMSCRIPTEN)
  add_executable(${PROJECT_NAME}_js)
  target_sources(${PROJECT_NAME}_js PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/wrappers/wasm.cpp
  )
  add_compile_definitions("__wasi__")
  target_compile_options(reflectcpp PUBLIC "-Wno-missing-template-arg-list-after-template-kw")
  target_compile_options(${PROJECT_NAME}_js PUBLIC -I${CMAKE_CURRENT_LIST_DIR}/libs/emsdk/upstream/emscripten/cache/sysroot/include)
  target_link_options(${PROJECT_NAME}_js PUBLIC "-lembind" --emit-tsd ${PROJECT_NAME}_js.d.ts)
  target_link_libraries(${PROJECT_NAME}_js
    PUBLIC
      reflectcpp
      ${PROJECT_NAME}
  )
endif()