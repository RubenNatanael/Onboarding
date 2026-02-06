#include <iio/iio.h>
#include <unistd.h>

struct iio_context *ctx = NULL;
char* tmpstr[64];

#define DEBUG

#ifdef DEBUG
    #define LOG_D(fmt, ...) printf("[DEBUG] " fmt "\n", ##__VA_ARGS__)
#else
    #define LOG_D(fmt, ...) do {} while (0)
#endif

#define LOG_I(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)


void blink_channel (struct iio_channel *ch) {
    const char *id_ch = iio_channel_get_id(ch);
    int num_attr = iio_channel_get_attrs_count(ch);
    if (num_attr == 0) return;

    const struct iio_attr *att_raw = iio_channel_find_attr(ch, "raw");
    if (!att_raw) {
        LOG_D("Error, raw attrinut not found for channel %s", id_ch);
        return;
    }

    char *data = "4000";
    LOG_D("Set value for channel: %s", id_ch);

    for (int j = 0; j < 3; j++) {
        char buff[256];
        iio_attr_write_string(att_raw, data);
        iio_attr_read_raw(att_raw, buff, sizeof(buff));
        LOG_I("Reading data: %s", buff);
        sleep(1);
        iio_attr_write_string(att_raw, "0");
        iio_attr_read_raw(att_raw, buff, sizeof(buff));
        LOG_I("Reading data: %s", buff);
        sleep(1);
    }

    // Turn off LED always
    iio_attr_write_string(att_raw, "0");
}

int main() {
    char *uri = "ip:10.87.54.80";
    int err;
    ctx = iio_create_context(NULL, uri);
    err = iio_err(ctx);
    if (err) {
        LOG_D("Error while creating context %d\n", err);
        return -1;
    }

    int num_of_devices = iio_context_get_devices_count(ctx);
    if ( num_of_devices <= 0) {
        LOG_D("Can't find any device\n");
    }
    LOG_D("Found %d devices\n", num_of_devices);

    struct iio_device *ad5592r;
    const char *name;
    struct iio_channels_mask *mask;

    struct iio_channel *ch;
    ad5592r = iio_context_find_device(ctx, "ad5592r");

    int num_channels = iio_device_get_channels_count(ad5592r);
    LOG_D("Channels found: %i", num_channels);

    mask = num_channels ? iio_create_channels_mask(num_channels) : NULL;

    struct iio_channel *voltage2 = iio_device_find_channel(ad5592r, "voltage2", true);
    struct iio_channel *voltage3 = iio_device_find_channel(ad5592r, "voltage3", true);


    if (mask) {
        iio_channel_enable(voltage2, mask);
        iio_channel_enable(voltage3, mask);
    }

    blink_channel(voltage2);
    blink_channel(voltage3);

        
    iio_context_destroy(ctx);
}