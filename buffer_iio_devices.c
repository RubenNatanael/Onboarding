#include "/home/ruben/libiio-0.26/iio.h" // libiio v0.26
#include <stdio.h>
#include <signal.h>
#include <math.h>

struct iio_context *ctx = NULL;
struct iio_device *ad9361 = NULL;

const struct iio_device *tx = NULL;
const struct iio_device *rx = NULL;

struct iio_channel *rx0_i = NULL;
struct iio_channel *rx0_q = NULL;
struct iio_channel *tx0_i = NULL;
struct iio_channel *tx0_q = NULL;
struct iio_buffer *rx_buf = NULL;
struct iio_buffer *tx_buf = NULL;

FILE *fp = NULL;

static void close(void) {
    if (rx_buf) iio_buffer_destroy(rx_buf);
    if (tx_buf) iio_buffer_destroy(tx_buf);
    if (rx0_i) iio_channel_disable(rx0_i);
    if (rx0_q) iio_channel_disable(rx0_q);
    if (tx0_i) iio_channel_disable(tx0_i);
    if (tx0_q) iio_channel_disable(tx0_q);
    if (ctx) iio_context_destroy(ctx);
    if (fp) fclose(fp);
    exit(0);

}

#define DEBUG

#ifdef DEBUG
    #define LOG_D(fmt, ...) printf("[DEBUG] " fmt "\n", ##__VA_ARGS__)
#else
    #define LOG_D(fmt, ...) do {} while (0)
#endif

#define LOG_I(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)

#define MHZ(x) ((long long)(x*1000000.0 + .5))
#define GHZ(x) ((long long)(x*1000000000.0 + .5))

#define MAX_MAGNITUDE_12b 2895
#define MAX_MAGNITUDE_16b 46340

static bool stop;
static void handle_sig(int sig)
{
	printf("Waiting for process to finish... Got signal %d\n", sig);
	stop = true;
}

int main(int argc, char **argv) {
    printf("Read/Write using buffer\n");
    signal(SIGINT, handle_sig);

    char *uri = "ip:10.48.69.106";
    if (argc == 2) {
        uri = argv[argc - 1];
    }

    ctx = iio_create_context_from_uri(uri);

    if ( iio_context_get_devices_count(ctx) <= 0) {
        LOG_D("Not found any device");
        close();
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
    LOG_D("* Configuring AD9361 for streaming");
    LOG_D("INPUT");
    struct iio_channel *ch = NULL;
    ch = iio_device_find_channel(ad9361, "voltage0", false); // INPUT
    iio_channel_attr_write(ch, "rf_port_select", "A_BALANCED");
    iio_channel_attr_write_longlong(ch, "rf_bandwidth", MHZ(2));
    iio_channel_attr_write_longlong(ch, "sampling_frequency", MHZ(2.5));
    ch = iio_device_find_channel(ad9361, "altvoltage0", true);
    iio_channel_attr_write_longlong(ch, "frequency", GHZ(2.5));

    LOG_D("OUTPUT");
    ch = iio_device_find_channel(ad9361, "voltage0", true); // OUTPUT
    iio_channel_attr_write(ch, "rf_port_select", "A");
    iio_channel_attr_write_longlong(ch, "rf_bandwidth", MHZ(2.5));
    iio_channel_attr_write_longlong(ch, "sampling_frequency", MHZ(2.5));
    ch = iio_device_find_channel(ad9361, "altvoltage1", true);
    iio_channel_attr_write_longlong(ch, "frequency", GHZ(2.5));

    // Getting and configurating channel for ad9361 IIO
    LOG_D("* Initializing AD9361 IIO streaming channels\n");
    rx0_i = iio_device_find_channel(rx, "voltage0", false);
    rx0_q = iio_device_find_channel(rx, "voltage1", false);
    tx0_i = iio_device_find_channel(tx, "voltage0", true);
    tx0_q = iio_device_find_channel(tx, "voltage1", true);

    LOG_D("* Enabling IIO streaming channels\n");
	iio_channel_enable(rx0_i);
	iio_channel_enable(rx0_q);
	iio_channel_enable(tx0_i);
	iio_channel_enable(tx0_q);


    // Create buffer for input and output data
    LOG_D("Creating buffers");
    rx_buf = iio_device_create_buffer(rx, 1024*1024, false);
    tx_buf = iio_device_create_buffer(tx, 1024*1024, false);
    if (!tx_buf || ! rx_buf) close();


    fp = fopen("build/iq_data.csv", "w");
    if (fp == NULL) {
        LOG_D("Unable to open file");
        close();
    }
    fprintf(fp, "I,Q,Magnitude,Strength\n");
    while (!stop) {
        size_t nbytes_rx, nbytes_tx;
        char *p_start, *p_end;
        ptrdiff_t p_steps;

        // Read data
        nbytes_rx = iio_buffer_refill(rx_buf);
        if (nbytes_rx < 0) {
            LOG_D("Error refilling buffer rx");
            close();
        }
        p_start = (char*)iio_buffer_first(rx_buf, rx0_i);
        p_steps = iio_buffer_step(rx_buf);
        p_end = iio_buffer_end(rx_buf);
        double magnitude;
        double strength;
        for (char *p_dat = p_start; p_dat < p_end; p_dat += p_steps) {
            const int16_t i = ((int16_t*)p_dat)[0];
			const int16_t q = ((int16_t*)p_dat)[1];
            magnitude = sqrt((double)i*i + (double)q*q);
            strength = (magnitude / MAX_MAGNITUDE_12b) * 100;
            fprintf(fp, "%d,%d,%.2f,%.2f\n", i, q, magnitude, strength);
            LOG_I("Magnitude: %f, signal: %.2f %%", magnitude, strength);
        }

        //Writing data
        p_start = (char*)iio_buffer_first(tx_buf, tx0_i);
        p_steps = iio_buffer_step(tx_buf);
        p_end = iio_buffer_end(tx_buf);
        for (char *p_dat = p_start; p_dat < p_end; p_dat += p_steps) {
            ((int16_t*)p_dat)[0] = 1;
            ((int16_t*)p_dat)[1] = 2;
        }
        nbytes_tx = iio_buffer_push(tx_buf);
        if (nbytes_tx < 0) {
            LOG_D("Error pushing buffer tx");
            close();
        }
    }

    LOG_D("Finish.");
    close();

    return 0;
    
}