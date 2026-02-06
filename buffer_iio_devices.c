#include "/home/ruben/libiio-0.26/iio.h" // libiio v0.26
#include <stdio.h>
#include <signal.h>

struct iio_context *ctx = NULL;
struct iio_device *ad9361 = NULL;

const struct iio_device *tx = NULL;
const struct iio_device *rx = NULL;

#define DEBUG

#ifdef DEBUG
    #define LOG_D(fmt, ...) printf("[DEBUG] " fmt "\n", ##__VA_ARGS__)
#else
    #define LOG_D(fmt, ...) do {} while (0)
#endif

#define LOG_I(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)

#define MHZ(x) ((long long)(x*1000000.0 + .5))
#define GHZ(x) ((long long)(x*1000000000.0 + .5))

static bool stop;
static void handle_sig(int sig)
{
	printf("Waiting for process to finish... Got signal %d\n", sig);
	stop = true;
}

void set_attr_in_channel(struct iio_channel *ch, char* attr, char *value ) {
    iio_channel_attr_write(ch, attr, value);
}
void set_attr_in_channel(struct iio_channel *ch, char* attr, long long value ) {
    iio_channel_attr_write(ch, attr, value);
}

int main() {
    printf("Read/Write using buffer\n");
    signal(SIGINT, handle_sig);

    ctx = iio_create_default_context();

    if ( iio_context_get_devices_count(ctx)) {
        LOG_D("Not found any device");
        return -1;
    }

    // Getting ADC device
    ad9361 = iio_context_find_device(ctx, "ad9361-phy");

    // Getting stream device receiver, trsnamitter
    tx = iio_context_find_device(ctx, "cf-ad9361-dds-core-lpc");
    rx = iio_context_find_device(ctx, "cf-ad9361-lpc");

    // Getting and configurating channel for ad9361-phy
    /*
    Here I am making the configuration for the ADC for the input
    and the output channel.
    */
    LOG_D("* Configuring AD9361 for streaming\n");
    struct iio_channel *ch = NULL;
    ch = iio_device_find_channel(ad9361, "voltage0", false); // INPUT
    set_attr_in_channel(ch, "rf_port_select", "A_BALANCED");
    set_attr_in_channel(ch, "rf_bandwidth", MHZ(2));
    set_attr_in_channel(ch, "sampling_frequency", MHZ(2.5));
    ch = iio_device_find_channel(ad9361, "altvoltage0", false);
    iio_channel_attr_write(ch, "frequency", GHZ(2.5));

    ch = iio_device_find_channel(ad9361, "voltage0", true); // OUTPUT
    set_attr_in_channel(ch, "rf_port_select", "A");
    set_attr_in_channel(ch, "rf_bandwidth", MHZ(1.5));
    set_attr_in_channel(ch, "sampling_frequency", MHZ(2.5));
    ch = iio_device_find_channel(ad9361, "altvoltage1", true);
    iio_channel_attr_write(ch, "frequency", GHZ(2.5));

    // Getting and configurating channel for ad9361 IIO
    LOG_D("* Initializing AD9361 IIO streaming channels\n");
    struct iio_channel *rx0_i = iio_device_find_channel(rx, "voltage0", false);
    struct iio_channel *rx0_q = iio_device_find_channel(rx, "voltage1", false);
    struct iio_channel *tx0_i = iio_device_find_channel(tx, "voltage0", true);
    struct iio_channel *tx0_q = iio_device_find_channel(tx, "voltage1", true);

    LOG_D("* Enabling IIO streaming channels\n");
	iio_channel_enable(rx0_i);
	iio_channel_enable(rx0_q);
	iio_channel_enable(tx0_i);
	iio_channel_enable(tx0_q);


    // Create buffer for input and output data
    LOG_D("Creating buffers");
    struct iio_buffer *rx_buf = iio_device_create_buffer(rx, 1024*1024, false);
    struct iio_buffer *tx_buf = iio_device_create_buffer(tx, 1024*1024, false);
    if (!tx_buf || ! rx_buf) return -1;  // TODO Here must delete ctx and disale channels

    while (!stop) {

    }
    
}