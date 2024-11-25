add_executable(sio_test sio_test.cpp)

CPMGetPackage(Catch2)

target_compile_features(sio_test PRIVATE cxx_std_17)

target_include_directories(sio_test
  PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/../include
  ${CMAKE_CURRENT_LIST_DIR}/../src
)

target_link_libraries(sio_test sioclient Catch2::Catch2WithMain)

add_test(sioclient_test sio_test)
