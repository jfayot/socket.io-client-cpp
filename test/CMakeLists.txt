add_executable(sio_test sio_test.cpp)

CPMAddPackage("gh:catchorg/Catch2@3.7.1")

target_compile_features(sio_test PRIVATE cxx_std_17)

target_link_libraries(sio_test sioclient Catch2::Catch2WithMain)

add_test(sioclient_test sio_test)
