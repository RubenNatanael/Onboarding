OLD_IIO_PATH = /home/ruben/libiio-0.26
OLD_IIO_LIB  = $(OLD_IIO_PATH)/build

BUILD = ./build

buffer_iio_devices: buffer_iio_devices.c
	gcc -g buffer_iio_devices.c -o $(BUILD)/buffer_iio_devices \
	-I$(OLD_IIO_PATH) \
	-L$(OLD_IIO_LIB) \
	-Wl,-rpath,$(OLD_IIO_LIB) \
	-liio -lm

# 2. The tools using the system-installed v1
controll_iio_devices: controll_iio_devices.c
	gcc controll_iio_devices.c -o $(BUILD)/controll_iio_devices -liio

list_iio_devices: list_iio_devices.c
	gcc list_iio_devices.c -o $(BUILD)/list_iio_devices -liio

clean:
	rm -f $(BUILD)/buffer_iio_devices $(BUILD)/controll_iio_devices $(BUILD)/list_iio_devices