cmake_minimum_required(VERSION 3.15)
project(ONBOARDINIG_IIO)

option(USE_V1 "Use libiio v0.26 headers from a custom path" OFF)

set(OLD_IIO_PATH "/home/ruben/libiio-0.26")
set(OLD_IIO_LIB  "${OLD_IIO_PATH}/build")

# Buffer example(Use always v0.26)
add_executable(buffer_iio_devices buffer_iio_devices.c)

target_compile_options(buffer_iio_devices PRIVATE -g)
target_include_directories(buffer_iio_devices PRIVATE ${OLD_IIO_PATH})
target_link_directories(buffer_iio_devices PRIVATE ${OLD_IIO_LIB})
target_link_libraries(buffer_iio_devices PRIVATE iio m)

set_target_properties(buffer_iio_devices PROPERTIES
BUILD_WITH_INSTALL_RPATH TRUE
INSTALL_RPATH "${OLD_IIO_LIB}")

if(USE_V1)
#Controll ex(Use v1)
    add_executable(controll_iio_devices controll_iio_devices.c)
    target_link_libraries(controll_iio_devices PRIVATE iio)

else()
#Controll ex(Use v0.26)
    add_executable(controll_iio_devices controll_iio_devices.c)

    target_compile_options(controll_iio_devices PRIVATE -g)
    target_include_directories(controll_iio_devices PRIVATE ${OLD_IIO_PATH})
    target_link_directories(controll_iio_devices PRIVATE ${OLD_IIO_LIB})
    target_link_libraries(controll_iio_devices PRIVATE iio m)

    set_target_properties(controll_iio_devices PROPERTIES
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH "${OLD_IIO_LIB}")
endif()

#List ex(Use always v1)
add_executable(list_iio_devices list_iio_devices.c)
target_link_libraries(list_iio_devices PRIVATE iio)
