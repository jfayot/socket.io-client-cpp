cmake_minimum_required(VERSION 3.18)

set(version 3.2.2)

PROJECT(sioclient
  VERSION ${version}
  LANGUAGES CXX
)

# check if project is standalone or embedded
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  set(IS_ROOT_PROJECT ON)
else()
  set(IS_ROOT_PROJECT OFF)
endif()

option(BUILD_SIO_TESTS "Build tests" ${IS_ROOT_PROJECT})
option(BUILD_SHARED_LIBS "Build the shared library" OFF)

set(ALL_SRC
  ${CMAKE_CURRENT_LIST_DIR}/src/sio_client_impl.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/sio_client.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/sio_message.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/sio_packet.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/sio_socket.cpp
)
add_library(sioclient ${ALL_SRC})
add_library(sioclient::sioclient ALIAS sioclient)

add_library(sioclient_tls ${ALL_SRC})
add_library(sioclient::sioclient_tls ALIAS sioclient_tls)

# download CPM.cmake
file(
  DOWNLOAD
  https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.40.2/CPM.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
  EXPECTED_HASH SHA256=c8cdc32c03816538ce22781ed72964dc864b2a34a310d3b7104812a5ca2d835d
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)
CPMUsePackageLock(package-lock.cmake)

CPMGetPackage(OpenSSL)
CPMGetPackage(asio)
CPMGetPackage(websocketpp)
CPMGetPackage(json)
CPMGetPackage(CPMPackageProject)

find_package(Threads REQUIRED)

if(asio_ADDED)
  add_library(asio INTERFACE)

  target_include_directories(asio INTERFACE
    $<BUILD_INTERFACE:${asio_SOURCE_DIR}/asio/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )

  target_compile_definitions(asio INTERFACE
    ASIO_STANDALONE
  )

  target_link_libraries(asio INTERFACE
    Threads::Threads
  )

  target_compile_features(asio INTERFACE
    cxx_std_11
  )
endif()

if(websocketpp_ADDED)
  add_library(websocketpp INTERFACE)

  target_include_directories(websocketpp INTERFACE
    $<BUILD_INTERFACE:${websocketpp_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )

  target_link_libraries(websocketpp INTERFACE
    asio
  )
endif()

target_include_directories(sioclient
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_include_directories(sioclient_tls
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_link_libraries(sioclient
  PRIVATE
  $<BUILD_INTERFACE:websocketpp>
  PUBLIC
  nlohmann_json::nlohmann_json
)

target_link_libraries(sioclient_tls
  PRIVATE
  $<BUILD_INTERFACE:websocketpp>
  OpenSSL::SSL
  OpenSSL::Crypto
  PUBLIC
  nlohmann_json::nlohmann_json
)

target_compile_features(sioclient
  PRIVATE
  cxx_std_17
)

target_compile_features(sioclient_tls
  PRIVATE
  cxx_std_17
)

target_compile_definitions(sioclient PRIVATE
  -D_WEBSOCKETPP_CPP11_STL_
  -D_WEBSOCKETPP_CPP11_FUNCTIONAL_
  -D_WEBSOCKETPP_CPP11_TYPE_TRAITS_
  -D_WEBSOCKETPP_CPP11_CHRONO_
)

target_compile_definitions(sioclient_tls PRIVATE
  -DSIO_TLS
  -D_WEBSOCKETPP_CPP11_STL_
  -D_WEBSOCKETPP_CPP11_FUNCTIONAL_
  -D_WEBSOCKETPP_CPP11_TYPE_TRAITS_
  -D_WEBSOCKETPP_CPP11_CHRONO_
)

if(BUILD_SHARED_LIBS)
  set_target_properties(sioclient PROPERTIES
    SOVERSION ${PROJECT_VERSION_MAJOR}
    VERSION ${PROJECT_VERSION}
  )

  set_target_properties(sioclient_tls PROPERTIES
    SOVERSION ${PROJECT_VERSION_MAJOR}
    VERSION ${PROJECT_VERSION}
  )
endif()

if(BUILD_SIO_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()

include(GNUInstallDirs)

packageProject(
  NAME sioclient
  VERSION ${PROJECT_VERSION}
  NAMESPACE sioclient
  BINARY_DIR ${PROJECT_BINARY_DIR}
  INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include
  INCLUDE_DESTINATION include
  VERSION_HEADER sioclient/version.h
  DISABLE_VERSION_SUFFIX YES
  COMPATIBILITY SameMajorVersion
  DEPENDENCIES "OpenSSL 3.4.0; nlohmann_json 3.11.3"
)

packageProject(
  NAME sioclient_tls
  VERSION ${PROJECT_VERSION}
  NAMESPACE sioclient
  BINARY_DIR ${PROJECT_BINARY_DIR}
  INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include
  INCLUDE_DESTINATION include
  DISABLE_VERSION_SUFFIX YES
  COMPATIBILITY SameMajorVersion
  DEPENDENCIES "OpenSSL 3.4.0; nlohmann_json 3.11.3"
)

# install(TARGETS sioclient sioclient_tls
#   EXPORT sioclientTargets
#   INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/sioclient
#   ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/sioclient
# )

# install(
#   FILES
#   ${CMAKE_CURRENT_LIST_DIR}/src/sio_client.h
#   ${CMAKE_CURRENT_LIST_DIR}/src/sio_message.h
#   ${CMAKE_CURRENT_LIST_DIR}/src/sio_socket.h
#   DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/sioclient
# )

# install(
#   EXPORT sioclientTargets
#   FILE sioclientTargets.cmake
#   NAMESPACE sioclient::
#   DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sioclient
# )

# include(CMakePackageConfigHelpers)

# # generate package's config file
# configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in
#   ${CMAKE_CURRENT_BINARY_DIR}/sioclientConfig.cmake
#   INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sioclient
# )

# # generate the version file for the config file
# write_basic_package_version_file(
#   ${CMAKE_CURRENT_BINARY_DIR}/sioclientConfigVersion.cmake
#   VERSION ${version}
#   COMPATIBILITY AnyNewerVersion
# )

# install(FILES
#   ${CMAKE_CURRENT_BINARY_DIR}/sioclientConfig.cmake
#   ${CMAKE_CURRENT_BINARY_DIR}/sioclientConfigVersion.cmake
#   DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sioclient
# )
