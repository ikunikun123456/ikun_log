cmake_minimum_required(VERSION 3.20.0)

project(ikun_log VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type")
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    Debug Release RelWithDebInfo MinSizeRel)
endif()

option(IKUN_LOG_USE_STDERR "Enable USE_STDERR" ON)
option(IKUN_LOG_BUILD_TESTS "Enable Build Tests" OFF)
option(IKUN_LOG_NOFORMAT   "Disable formatting in ikun_log" OFF)


include(GNUInstallDirs)
include(CMakePackageConfigHelpers)


add_library(ikun_log INTERFACE)
add_library(ikun_log::ikun_log ALIAS ikun_log)

if(IKUN_LOG_USE_STDERR)
    target_compile_definitions(ikun_log INTERFACE IKUN_LOG_USE_STDERR=1)
endif()

if(IKUN_LOG_NOFORMAT)
    target_compile_definitions(ikun_log INTERFACE IKUN_LOG_NOFORMAT=1)
endif()

target_include_directories(ikun_log INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_options(ikun_log INTERFACE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Werror>
)

target_compile_features(ikun_log INTERFACE cxx_std_20)

install(TARGETS ikun_log EXPORT ikun_logTargets)
install(EXPORT ikun_logTargets
    NAMESPACE ikun_log::
    DESTINATION lib/cmake/ikun_log
)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION include)

if(IKUN_LOG_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# 生成 ikun_logConfig.cmake
configure_package_config_file(
    cmake/ikun_logConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/ikun_logConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ikun_log
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/ikun_logConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/ikun_logConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/ikun_logConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ikun_log
)

