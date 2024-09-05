cmake_minimum_required(VERSION 3.27.6)
project(cpp-pre_file VERSION "0.0.1")

enable_testing()

# Warning as errors to ensure file quality
string(TOUPPER "${CMAKE_CXX_COMPILER_ID}" COMPILER_IN_USE)
if ("${COMPILER_IN_USE}" STREQUAL "GNU" OR "${COMPILER_IN_USE}" MATCHES "CLANG")
	add_definitions(
    -Wall
		#-Werror
		-Wno-unused-local-typedefs
		-Wno-unused-variable
  )
endif()

find_package(Threads)

include (FetchContent)
FetchContent_Declare(
  Boost
  GIT_REPOSITORY https://github.com/boostorg/boost.git
  # boost 1.80
  GIT_TAG         32da69a36f84c5255af8a994951918c258bac601
)
FetchContent_MakeAvailable(Boost)
find_package(boost_filesystem CONFIG REQUIRED)
find_package(boost_system CONFIG REQUIRED)
find_package(boost_uuid CONFIG REQUIRED)

# Define library
add_library(file INTERFACE )
add_library(cpp-pre_file::file ALIAS file)

set(include_install_dir "include")

target_include_directories(file BEFORE INTERFACE 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}> 
  $<INSTALL_INTERFACE:${include_install_dir}/>)

target_link_libraries(file INTERFACE 
  Boost::system Boost::filesystem Boost::uuid 
  ${CMAKE_THREAD_LIBS_INIT}
)


# Targets:
install(
    TARGETS 
    EXPORT "${targets_export_name}"
    LIBRARY DESTINATION "lib"
    ARCHIVE DESTINATION "lib"
    RUNTIME DESTINATION "bin"
    INCLUDES DESTINATION "${include_install_dir}"
)

add_subdirectory(tests)


# Installing

# Layout. This works for all platforms:
#   * <prefix>/lib/cmake/<PROJECT-NAME>
#   * <prefix>/lib/
#   * <prefix>/include/
set(config_install_dir "lib/cmake/${PROJECT_NAME}")

set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")

# Configuration
set(version_config "${generated_dir}/${PROJECT_NAME}ConfigVersion.cmake")
set(project_config "${generated_dir}/${PROJECT_NAME}Config.cmake")
set(targets_export_name "${PROJECT_NAME}Targets")
set(namespace "${PROJECT_NAME}::")

# Include module with fuction 'write_basic_package_version_file'
include(CMakePackageConfigHelpers)

# Configure '<PROJECT-NAME>ConfigVersion.cmake'
# Note: PROJECT_VERSION is used as a VERSION
write_basic_package_version_file(
    "${version_config}" COMPATIBILITY SameMajorVersion
)

# Configure '<PROJECT-NAME>Config.cmake'
# Use variables:
#   * targets_export_name
#   * PROJECT_NAME
configure_package_config_file(
    "cmake/modules/Config.cmake.in"
    "${project_config}"
    INSTALL_DESTINATION "${config_install_dir}"
)


# Targets:
install(
    TARGETS file 
    EXPORT "${targets_export_name}"
    LIBRARY DESTINATION "lib"
    ARCHIVE DESTINATION "lib"
    RUNTIME DESTINATION "bin"
    INCLUDES DESTINATION "${include_install_dir}"
)

# Headers:
install(
    DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    DESTINATION "${include_install_dir}"
    FILES_MATCHING PATTERN "*.[ih]*"
    PATTERN build/ EXCLUDE
    PATTERN "/build*" EXCLUDE 
    PATTERN ".git/*" EXCLUDE 
    PATTERN ".tipistore/*" EXCLUDE 
    PATTERN "doc/*" EXCLUDE 
    PATTERN "node_modules/*" EXCLUDE 
)

# Config
#   * <prefix>/lib/cmake/file/fileConfig.cmake
#   * <prefix>/lib/cmake/file/fileConfigVersion.cmake
#   * <prefix>/lib/cmake/file/fileTargets.cmake
install(
    FILES "${project_config}" "${version_config}"
    DESTINATION "${config_install_dir}"
)

## This overcomes the issue with "cpp-pre_file::file depends on targets 
## that are in no EXPORT sets in plain cmake non cmake-tipi-provider contexts
#set(BOOST_TARGETS boost_filesystem boost_system boost_uuid 
#  boost_assert
#  boost_atomic
#  boost_config
#  boost_container_hash
#  boost_core
#  boost_detail
#  boost_io
#  boost_iterator
#  boost_move
#  boost_numeric_conversion
#  boost_predef
#  boost_random
#  boost_scope
#  boost_smart_ptr
#  boost_static_assert
#  boost_throw_exception
#  boost_tti
#  boost_type_traits
#  boost_variant2
#  boost_winapi
#  boost_align
#  boost_array
#  boost_concept_check
#  boost_conversion
#  boost_describe
#  boost_dynamic_bitset
#  boost_function_types
#  boost_fusion
#  boost_integer
#  boost_mp11
#  boost_mpl
#  boost_optional
#  boost_preprocessor
#  boost_range
#  boost_utility
#  boost_tuple
#  boost_typeof
#  boost_functional
#  boost_regex
#  boost_function
#  boost_bind
#)
#install(
#    TARGETS ${BOOST_TARGETS}
#    EXPORT "${targets_export_name}"
#    LIBRARY DESTINATION "lib"
#    ARCHIVE DESTINATION "lib"
#    RUNTIME DESTINATION "bin"
#    INCLUDES DESTINATION "${include_install_dir}"
#)

install(
    EXPORT "${targets_export_name}"
    NAMESPACE "${namespace}"
    DESTINATION "${config_install_dir}"
)