cmake_minimum_required(VERSION 3.28)
project (moderna_type_check)
set (CMAKE_CXX_STANDARD 23)
set (CMAKE_CXX_STANDARD_REQUIRED true)
set (CMAKE_CXX_SCAN_FOR_MODULES true)

option (MODERNA_TYPE_CHECK_BUILD_TESTS "Build tests for Moderna Type Check" OFF)
option (MODERNA_TYPE_CHECK_BUILD_PYTHON_WRAPPER "Build Python Wrapper for Moderna Type Check" OFF)
option (MODERNA_TYPE_CHECK_BUILD_WASM "Build WebAssembly Wrapper for Moderna Type Check" OFF)

if (NOT TARGET reflectcpp)
  if (MODERNA_INSTALL)
    set (REFLECTCPP_INSTALL ON)
  endif()
  add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/libs/reflect-cpp)
endif()


file(GLOB ${PROJECT_NAME}_src ${CMAKE_CURRENT_LIST_DIR}/src/*.cc)
add_library(${PROJECT_NAME})
add_library(moderna::type_check ALIAS ${PROJECT_NAME})
target_sources(${PROJECT_NAME}
  PUBLIC
    FILE_SET CXX_MODULES FILES ${${PROJECT_NAME}_src}
)
target_link_libraries(${PROJECT_NAME} 
  PRIVATE 
    reflectcpp::reflectcpp
)

# Python Build
if (MODERNA_TYPE_CHECK_BUILD_PYTHON_WRAPPER)
  find_package(Python3 3.7 REQUIRED COMPONENTS Interpreter Development)
  add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/libs/pybind11)
  add_library(${PROJECT_NAME}_python SHARED)
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
endif()

if (MODERNA_TYPE_CHECK_BUILD_TESTS)
  include(CTest)
  add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../test-lib test-lib)
  moderna_add_test(
    ${PROJECT_NAME}_type_check_test
    ${CMAKE_CURRENT_LIST_DIR}/tests/type_check.cc
    LIBRARIES ${PROJECT_NAME} moderna_test_lib
    SANITIZERS all
  )
  moderna_add_test(
    ${PROJECT_NAME}_engine_test
    ${CMAKE_CURRENT_LIST_DIR}/tests/engine.cc
    LIBRARIES ${PROJECT_NAME} moderna_test_lib
    SANITIZERS all
  )
  moderna_add_test(
    ${PROJECT_NAME}_parser_tests
    ${CMAKE_CURRENT_LIST_DIR}/tests/parser.cc
    LIBRARIES ${PROJECT_NAME} moderna_test_lib
    SANITIZERS all
  )
else()
  message (${PROJECT_NAME} : "Clone Moderna Library to Test")
endif()

if(MODERNA_TYPE_CHECK_BUILD_WASM)
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

if (MODERNA_INSTALL)
  include(GNUInstallDirs)
  set (MODERNA_COMPONENT_NAME "type_check")
  set_property(
    TARGET ${PROJECT_NAME} 
    PROPERTY EXPORT_NAME ${MODERNA_COMPONENT_NAME}
  )
  install (
    TARGETS ${PROJECT_NAME}
    EXPORT moderna
    FILE_SET 
      CXX_MODULES 
      DESTINATION ${CMAKE_INSTALL_PREFIX}/cxx_modules/moderna/${MODERNA_COMPONENT_NAME}
      COMPONENT ${MODERNA_COMPONENT_NAME}_file_set
    CXX_MODULES_BMI 
      COMPONENT ${MODERNA_COMPONENT_NAME}
      DESTINATION ${CMAKE_INSTALL_PREFIX}/cxx_modules/moderna/${MODERNA_COMPONENT_NAME}
  )
  if (PROJECT_IS_TOP_LEVEL)
    install (
      EXPORT moderna
      DESTINATION ${CMAKE_INSTALL_LIBDIR}/moderna
      FILE moderna-config.cmake
      NAMESPACE moderna::
      CXX_MODULES_DIRECTORY cxx_modules/moderna
    )
  endif()
endif()