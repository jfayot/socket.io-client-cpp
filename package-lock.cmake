  # "OPENSSL_CONFIGURE_OPTIONS no-shared\\\\;no-tests"
CPMDeclarePackage(OpenSSL
  NAME openssl-cmake
  URL https://github.com/jimmy-park/openssl-cmake/archive/refs/tags/3.4.0.zip
  OPTIONS
  "BUILD_SHARED_LIBS ON"
  "OPENSSL_TEST OFF"
  "OPENSSL_TARGET_VERSION 3.4.0"
  "OPENSSL_INSTALL ON"
  "OPENSSL_CONFIGURE_OPTIONS --prefix=${CMAKE_INSTALL_PREFIX}"
  EXCLUDE_FROM_ALL YES
)

CPMDeclarePackage(asio
  NAME asio
  VERSION 1.32.0
  URL https://github.com/chriskohlhoff/asio/archive/refs/tags/asio-1-32-0.zip
  EXCLUDE_FROM_ALL YES
)

CPMDeclarePackage(websocketpp
  NAME websocketpp
  VERSION 0.8.2
  URL https://github.com/zaphoyd/websocketpp/archive/refs/tags/0.8.2.zip
  OPTIONS
  "ENABLE_CPP11 ON"
  "BUILD_TESTS OFF"
  "BUILD_EXAMPLES OFF"
  EXCLUDE_FROM_ALL YES
)

CPMDeclarePackage(json
  NAME json
  VERSION 3.11.3
  URL https://github.com/nlohmann/json/archive/refs/tags/v3.11.3.zip
  OPTIONS
  "JSON_Install ON"
  EXCLUDE_FROM_ALL YES
)

CPMDeclarePackage(Catch2
  NAME Catch2
  VERSION 3.7.1
  GITHUB_REPOSITORY catchorg/Catch2
  EXCLUDE_FROM_ALL YES
)

CPMDeclarePackage(CPMPackageProject
  NAME PackageProject.cmake
  GITHUB_REPOSITORY TheLartians/PackageProject.cmake
  VERSION 1.12.0
)
