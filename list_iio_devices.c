#include <iio/iio.h>

struct iio_context *ctx = NULL;
char* tmpstr[64];

#define DEBUGi

#ifdef DEBUG
    #define LOG_D(fmt, ...) printf("[DEBUG] " fmt "\n", ##__VA_ARGS__)
#else
    #define LOG_D(fmt, ...) do {} while (0)
#endif

#define LOG_I(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)

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

    struct iio_device *dev;
    const char *name, *label;
    struct iio_event_stream *stream;
    struct iio_channels_mask *mask;

    struct iio_channel *ch;

    for (int i = 0; i < num_of_devices; i++) {
        dev = iio_context_get_device(ctx, i);
        name = iio_device_get_name(dev);
        label = iio_device_get_label(dev);
        stream = iio_device_create_event_stream(dev);
        if (label) LOG_I("Device: %s  label: %s: ", name, label);
        if (!label) LOG_I("Device: %s: ", name);

        int num_channels = iio_device_get_channels_count(dev);
        LOG_D("Channels found: %i", num_channels);

        mask = num_channels ? iio_create_channels_mask(num_channels) : NULL;
        for (int j = 0; j < num_channels; j++) {
            ch = iio_device_get_channel(dev, j);

            if (mask)
                iio_channel_enable(ch, mask);
            const char *id_ch = iio_channel_get_id(ch);
            LOG_I("\t%s: %s",id_ch, iio_channel_is_output(ch) ? "OUTPUT" : "INPUT");
            int num_attr = iio_channel_get_attrs_count(ch);

            LOG_D("Found %d attributes for channel %s", num_attr, id_ch);
            
            const struct iio_attr *att;
            for (int z = 0; z < num_attr; z++) {
                att = iio_channel_get_attr(ch, z);
                const char *attr_name, *fnn;
                attr_name = iio_attr_get_name(att);
                fnn = iio_attr_get_filename(att);
                char value[256];
                iio_attr_read_raw(att, value, sizeof(value));
                

                LOG_I("\t\tattr %d: %s %s value: %s", z, attr_name, fnn, value);
            }
        }
    }
    iio_context_destroy(ctx);
}