cmake_minimum_required(VERSION 3.18)

set(version 3.2.0)

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

option(BUILD_TESTS "Build tests" ${IS_ROOT_PROJECT})
option(BUILD_SHARED_LIBS "Build the shared library" OFF)

set(ALL_SRC
  ${CMAKE_CURRENT_LIST_DIR}/src/sio_client.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/sio_socket.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/sio_message.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/internal/sio_client_impl.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/internal/sio_packet.cpp
)
add_library(sioclient ${ALL_SRC})
add_library(sioclient::sioclient ALIAS sioclient)

# download CPM.cmake
file(
  DOWNLOAD
  https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.40.2/CPM.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
  EXPECTED_HASH SHA256=c8cdc32c03816538ce22781ed72964dc864b2a34a310d3b7104812a5ca2d835d
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

find_package(Threads REQUIRED)

CPMAddPackage(
  NAME asio
  VERSION 1.32.0
  URL https://github.com/chriskohlhoff/asio/archive/refs/tags/asio-1-32-0.zip
)

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

CPMAddPackage(
  NAME websocketpp
  VERSION 0.8.2
  URL https://github.com/zaphoyd/websocketpp/archive/refs/tags/0.8.2.zip
  OPTIONS
  "BUILD_TESTS OFF"
  "BUILD_EXAMPLES OFF"
)

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

CPMAddPackage(
  NAME json
  VERSION 3.11.3
  URL https://github.com/nlohmann/json/archive/refs/tags/v3.11.3.zip
  OPTIONS
  "JSON_Install ON"
)

target_include_directories(sioclient
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(sioclient
  PRIVATE
  $<BUILD_INTERFACE:websocketpp>
  PUBLIC
  nlohmann_json::nlohmann_json
)

target_compile_features(sioclient
  PRIVATE
  cxx_std_17
)

if(BUILD_SHARED_LIBS)
  set_target_properties(sioclient PROPERTIES
    SOVERSION ${PROJECT_VERSION_MAJOR}
    VERSION ${PROJECT_VERSION}
  )
endif()

find_package(OpenSSL)

if(OPENSSL_FOUND)
  add_library(sioclient_tls ${ALL_SRC})
  add_library(sioclient::sioclient_tls ALIAS sioclient_tls)
  target_include_directories(sioclient_tls
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )

  target_link_libraries(sioclient_tls
    PRIVATE
    $<BUILD_INTERFACE:websocketpp>
    OpenSSL::SSL
    OpenSSL::Crypto
    PUBLIC
    nlohmann_json::nlohmann_json
  )

  target_compile_definitions(sioclient_tls PRIVATE -DSIO_TLS)

  target_compile_features(sioclient_tls
    PRIVATE
    cxx_std_17
  )

  if(BUILD_SHARED_LIBS)
    set_target_properties(sioclient_tls PROPERTIES
      SOVERSION ${PROJECT_VERSION_MAJOR}
      VERSION ${PROJECT_VERSION}
    )
  endif()
endif()

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()

include(GNUInstallDirs)

install(TARGETS sioclient sioclient_tls
  EXPORT sioclientTargets
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/sioclient
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/sioclient
)

install(
  FILES
  ${CMAKE_CURRENT_LIST_DIR}/src/sio_client.h
  ${CMAKE_CURRENT_LIST_DIR}/src/sio_message.h
  ${CMAKE_CURRENT_LIST_DIR}/src/sio_socket.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/sioclient
)

install(
  EXPORT sioclientTargets
  FILE sioclientTargets.cmake
  NAMESPACE sioclient::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sioclient
)

include(CMakePackageConfigHelpers)

# generate package's config file
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/sioclientConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sioclient
)

# generate the version file for the config file
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/sioclientConfigVersion.cmake
  VERSION ${version}
  COMPATIBILITY AnyNewerVersion
)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/sioclientConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/sioclientConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sioclient
)
