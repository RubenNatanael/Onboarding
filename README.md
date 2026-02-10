# Onboarding

**ℹ️About**

This project is an onboarding for libiio. It contains three example programs designed to show how to use different parts and versions of the library:

list_iio_devices – an example that lists all IIO devices. This example is written to work with libiio v1.

buffer_iio_devices – a simple buffer streaming example for RF that works with libiio v0.26.

controll_iio_devices – a small LED blink example. This example can be compiled for either libiio v1 or libiio v0.26 by passing a compile-time flag.

**For the controll_iio_devices example:**

Compile with libiio v0.26 using the flag: -DUSE_V0DOT26=1

Compile with libiio v1 using the flag: -DUSE_V1=1
If no flag is provided, the example defaults to the system-installed libiio.

**For the buffer_iio_devices example:**

The buffer_iio_devices example.c reads radio signals and store them in to a CSV file. You can generate a graphical plot in order to see the magnitude and strength over time using the generate_plot.py script.

**🔧 Build Options example**

Using v1 of libiio for the controll_iio_devices example
```
mkdir build && cd build
cmake .. -DUSE_V1=ON
make
```

Using v0.26 of libiio for the controll_iio_devices example
```
mkdir build && cd build
cmake .. -DUSE_V1=OFF
make
```

