/*
    adm1021.c - Part of lm_sensors, Linux kernel modules for hardware
        monitoring
    Copyright (c) 1998, 1999  Frodo Looijaard <frodol@dds.nl> and
    Philip Edelbrock <phil@netroedge.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/jiffies.h>
#include <linux/i2c.h>
#include <linux/hwmon.h>
#include <linux/hwmon-sysfs.h>
#include <linux/err.h>
#include <linux/mutex.h>


/* Addresses to scan */
static const unsigned short normal_i2c[] = {
    0x18, 0x19, 0x1a, 0x29, 0x2a, 0x2b, 0x4c, 0x4d, 0x4e, I2C_CLIENT_END };

/* Insmod parameters */
I2C_CLIENT_INSMOD_8(adm1021, adm1023, max1617, max1617a, thmc10, lm84, gl523sm,
            mc1066);

/* adm1021 constants specified below */

/* The adm1021 registers */
/* Read-only */
/* For nr in 0-1 */
#define ADM1021_REG_TEMP(nr)        (nr)
#define ADM1021_REG_STATUS        0x02
/* 0x41 = AD, 0x49 = TI, 0x4D = Maxim, 0x23 = Genesys , 0x54 = Onsemi */
#define ADM1021_REG_MAN_ID        0xFE
/* ADM1021 = 0x0X, ADM1023 = 0x3X */
#define ADM1021_REG_DEV_ID        0xFF
/* These use different addresses for reading/writing */
#define ADM1021_REG_CONFIG_R        0x03
#define ADM1021_REG_CONFIG_W        0x09
#define ADM1021_REG_CONV_RATE_R        0x04
#define ADM1021_REG_CONV_RATE_W        0x0A
/* These are for the ADM1023's additional precision on the remote temp sensor */
#define ADM1023_REG_REM_TEMP_PREC    0x10
#define ADM1023_REG_REM_OFFSET        0x11
#define ADM1023_REG_REM_OFFSET_PREC    0x12
#define ADM1023_REG_REM_TOS_PREC    0x13
#define ADM1023_REG_REM_THYST_PREC    0x14
/* limits */
/* For nr in 0-1 */
#define ADM1021_REG_TOS_R(nr)        (0x05 + 2 * (nr))
#define ADM1021_REG_TOS_W(nr)        (0x0B + 2 * (nr))
#define ADM1021_REG_THYST_R(nr)        (0x06 + 2 * (nr))
#define ADM1021_REG_THYST_W(nr)        (0x0C + 2 * (nr))
/* write-only */
#define ADM1021_REG_ONESHOT        0x0F

/* Initial values */

/* Note: Even though I left the low and high limits named os and hyst,
they don't quite work like a thermostat the way the LM75 does.  I.e.,
a lower temp than THYST actually triggers an alarm instead of
clearing it.  Weird, ey?   --Phil  */

/* Each client has this additional data */
struct adm1021_data {
    struct i2c_client client;
    struct device *hwmon_dev;
    enum chips type;

    struct mutex update_lock;
    char valid;        /* !=0 if following fields are valid */
    unsigned long last_updated;    /* In jiffies */

    s8 temp_max[2];        /* Register values */
    s8 temp_min[2];
    s8 temp[2];
    u8 alarms;
    /* Special values for ADM1023 only */
    u8 remote_temp_prec;
    u8 remote_temp_os_prec;
    u8 remote_temp_hyst_prec;
    u8 remote_temp_offset;
    u8 remote_temp_offset_prec;
};

static int adm1021_attach_adapter(struct i2c_adapter *adapter);
static int adm1021_detect(struct i2c_adapter *adapter, int address, int kind);
static void adm1021_init_client(struct i2c_client *client);
static int adm1021_detach_client(struct i2c_client *client);
static struct adm1021_data *adm1021_update_device(struct device *dev);

/* (amalysh) read only mode, otherwise any limit's writing confuse BIOS */
static int read_only;


/* This is the driver that will be inserted */
static struct i2c_driver adm1021_driver = {
    .driver = {
        .name    = "adm1021",
    },
    .attach_adapter    = adm1021_attach_adapter,
    .detach_client    = adm1021_detach_client,
};

static ssize_t show_temp(struct device *dev,
             struct device_attribute *devattr, char *buf)
{
    int index = to_sensor_dev_attr(devattr)->index;
    struct adm1021_data *data = adm1021_update_device(dev);

    return sprintf(buf, "%d\n", 1000 * data->temp[index]);
}

static ssize_t show_temp_max(struct device *dev,
                 struct device_attribute *devattr, char *buf)
{
    int index = to_sensor_dev_attr(devattr)->index;
    struct adm1021_data *data = adm1021_update_device(dev);

    return sprintf(buf, "%d\n", 1000 * data->temp_max[index]);
}

static ssize_t show_temp_min(struct device *dev,
                 struct device_attribute *devattr, char *buf)
{
    int index = to_sensor_dev_attr(devattr)->index;
    struct adm1021_data *data = adm1021_update_device(dev);

    return sprintf(buf, "%d\n", 1000 * data->temp_min[index]);
}

static ssize_t show_alarm(struct device *dev, struct device_attribute *attr,
              char *buf)
{
    int index = to_sensor_dev_attr(attr)->index;
    struct adm1021_data *data = adm1021_update_device(dev);
    return sprintf(buf, "%u\n", (data->alarms >> index) & 1);
}

static ssize_t show_alarms(struct device *dev,
               struct device_attribute *attr,
               char *buf)
{
    struct adm1021_data *data = adm1021_update_device(dev);
    return sprintf(buf, "%u\n", data->alarms);
}

static ssize_t set_temp_max(struct device *dev,
                struct device_attribute *devattr,
                const char *buf, size_t count)
{
    int index = to_sensor_dev_attr(devattr)->index;
    struct i2c_client *client = to_i2c_client(dev);
    struct adm1021_data *data = i2c_get_clientdata(client);
    long temp = simple_strtol(buf, NULL, 10) / 1000;

    mutex_lock(&data->update_lock);
    data->temp_max[index] = SENSORS_LIMIT(temp, -128, 127);
    if (!read_only)
        i2c_smbus_write_byte_data(client, ADM1021_REG_TOS_W(index),
                      data->temp_max[index]);
    mutex_unlock(&data->update_lock);

    return count;
}

static ssize_t set_temp_min(struct device *dev,
                struct device_attribute *devattr,
                const char *buf, size_t count)
{
    int index = to_sensor_dev_attr(devattr)->index;
    struct i2c_client *client = to_i2c_client(dev);
    struct adm1021_data *data = i2c_get_clientdata(client);
    long temp = simple_strtol(buf, NULL, 10) / 1000;

    mutex_lock(&data->update_lock);
    data->temp_min[index] = SENSORS_LIMIT(temp, -128, 127);
    if (!read_only)
        i2c_smbus_write_byte_data(client, ADM1021_REG_THYST_W(index),
                      data->temp_min[index]);
    mutex_unlock(&data->update_lock);

    return count;
}

static SENSOR_DEVICE_ATTR(temp1_input, S_IRUGO, show_temp, NULL, 0);
static SENSOR_DEVICE_ATTR(temp1_max, S_IWUSR | S_IRUGO, show_temp_max,
              set_temp_max, 0);
static SENSOR_DEVICE_ATTR(temp1_min, S_IWUSR | S_IRUGO, show_temp_min,
              set_temp_min, 0);
static SENSOR_DEVICE_ATTR(temp2_input, S_IRUGO, show_temp, NULL, 1);
static SENSOR_DEVICE_ATTR(temp2_max, S_IWUSR | S_IRUGO, show_temp_max,
              set_temp_max, 1);
static SENSOR_DEVICE_ATTR(temp2_min, S_IWUSR | S_IRUGO, show_temp_min,
              set_temp_min, 1);
static SENSOR_DEVICE_ATTR(temp1_max_alarm, S_IRUGO, show_alarm, NULL, 6);
static SENSOR_DEVICE_ATTR(temp1_min_alarm, S_IRUGO, show_alarm, NULL, 5);
static SENSOR_DEVICE_ATTR(temp2_max_alarm, S_IRUGO, show_alarm, NULL, 4);
static SENSOR_DEVICE_ATTR(temp2_min_alarm, S_IRUGO, show_alarm, NULL, 3);
static SENSOR_DEVICE_ATTR(temp2_fault, S_IRUGO, show_alarm, NULL, 2);

static DEVICE_ATTR(alarms, S_IRUGO, show_alarms, NULL);

static int adm1021_attach_adapter(struct i2c_adapter *adapter)
{
    if (!(adapter->class & I2C_CLASS_HWMON))
        return 0;
    return i2c_probe(adapter, &addr_data, adm1021_detect);
}

static struct attribute *adm1021_attributes[] = {
    &sensor_dev_attr_temp1_max.dev_attr.attr,
    &sensor_dev_attr_temp1_min.dev_attr.attr,
    &sensor_dev_attr_temp1_input.dev_attr.attr,
    &sensor_dev_attr_temp2_max.dev_attr.attr,
    &sensor_dev_attr_temp2_min.dev_attr.attr,
    &sensor_dev_attr_temp2_input.dev_attr.attr,
    &sensor_dev_attr_temp1_max_alarm.dev_attr.attr,
    &sensor_dev_attr_temp1_min_alarm.dev_attr.attr,
    &sensor_dev_attr_temp2_max_alarm.dev_attr.attr,
    &sensor_dev_attr_temp2_min_alarm.dev_attr.attr,
    &sensor_dev_attr_temp2_fault.dev_attr.attr,
    &dev_attr_alarms.attr,
    NULL
};

static const struct attribute_group adm1021_group = {
    .attrs = adm1021_attributes,
};

static int adm1021_detect(struct i2c_adapter *adapter, int address, int kind)
{
    int i;
    struct i2c_client *client;
    struct adm1021_data *data;
    int err = 0;
    const char *type_name = "";
    int conv_rate, status, config;

    if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_BYTE_DATA)) {
        pr_debug("adm1021: detect failed, "
             "smbus byte data not supported!\n");
        goto error0;
    }

    /* OK. For now, we presume we have a valid client. We now create the
       client structure, even though we cannot fill it completely yet.
       But it allows us to access adm1021 register values. */

    if (!(data = kzalloc(sizeof(struct adm1021_data), GFP_KERNEL))) {
        pr_debug("adm1021: detect failed, kzalloc failed!\n");
        err = -ENOMEM;
        goto error0;
    }

    client = &data->client;
    i2c_set_clientdata(client, data);
    client->addr = address;
    client->adapter = adapter;
    client->driver = &adm1021_driver;
    status = i2c_smbus_read_byte_data(client, ADM1021_REG_STATUS);
    conv_rate = i2c_smbus_read_byte_data(client,
                         ADM1021_REG_CONV_RATE_R);
    config = i2c_smbus_read_byte_data(client, ADM1021_REG_CONFIG_R);

    /* Now, we do the remaining detection. */
    if (kind < 0) {
        if ((status & 0x03) != 0x00 || (config & 0x3F) != 0x00
            || (conv_rate & 0xF8) != 0x00) {
            pr_debug("adm1021: detect failed, "
                 "chip not detected!\n");
            err = -ENODEV;
            goto error1;
        }
    }

    /* Determine the chip type. */
    if (kind <= 0) {
        i = i2c_smbus_read_byte_data(client, ADM1021_REG_MAN_ID);
        if (i == 0x41)
            if ((i2c_smbus_read_byte_data(client,
                    ADM1021_REG_DEV_ID) & 0xF0) == 0x30)
                kind = adm1023;
            else
                kind = adm1021;
        else if (i == 0x49)
            kind = thmc10;
        else if (i == 0x23)
            kind = gl523sm;
        else if ((i == 0x4d) &&
             (i2c_smbus_read_byte_data(client,
                           ADM1021_REG_DEV_ID) == 0x01))
            kind = max1617a;
        else if (i == 0x54)
            kind = mc1066;
        /* LM84 Mfr ID in a different place, and it has more unused bits */
        else if (conv_rate == 0x00
             && (kind == 0 /* skip extra detection */
                 || ((config & 0x7F) == 0x00
                 && (status & 0xAB) == 0x00)))
            kind = lm84;
        else
            kind = max1617;
    }

    if (kind == max1617) {
        type_name = "max1617";
    } else if (kind == max1617a) {
        type_name = "max1617a";
    } else if (kind == adm1021) {
        type_name = "adm1021";
    } else if (kind == adm1023) {
        type_name = "adm1023";
    } else if (kind == thmc10) {
        type_name = "thmc10";
    } else if (kind == lm84) {
        type_name = "lm84";
    } else if (kind == gl523sm) {
        type_name = "gl523sm";
    } else if (kind == mc1066) {
        type_name = "mc1066";
    }
    pr_debug("adm1021: Detected chip %s at adapter %d, address 0x%02x.\n",
         type_name, i2c_adapter_id(adapter), address);

    /* Fill in the remaining client fields */
    strlcpy(client->name, type_name, I2C_NAME_SIZE);
    data->type = kind;
    mutex_init(&data->update_lock);

    /* Tell the I2C layer a new client has arrived */
    if ((err = i2c_attach_client(client)))
        goto error1;

    /* Initialize the ADM1021 chip */
    if (kind != lm84 && !read_only)
        adm1021_init_client(client);

    /* Register sysfs hooks */
    if ((err = sysfs_create_group(&client->dev.kobj, &adm1021_group)))
        goto error2;

    data->hwmon_dev = hwmon_device_register(&client->dev);
    if (IS_ERR(data->hwmon_dev)) {
        err = PTR_ERR(data->hwmon_dev);
        goto error3;
    }

    return 0;

error3:
    sysfs_remove_group(&client->dev.kobj, &adm1021_group);
error2:
    i2c_detach_client(client);
error1:
    kfree(data);
error0:
    return err;
}

static void adm1021_init_client(struct i2c_client *client)
{
    /* Enable ADC and disable suspend mode */
    i2c_smbus_write_byte_data(client, ADM1021_REG_CONFIG_W,
        i2c_smbus_read_byte_data(client, ADM1021_REG_CONFIG_R) & 0xBF);
    /* Set Conversion rate to 1/sec (this can be tinkered with) */
    i2c_smbus_write_byte_data(client, ADM1021_REG_CONV_RATE_W, 0x04);
}

static int adm1021_detach_client(struct i2c_client *client)
{
    struct adm1021_data *data = i2c_get_clientdata(client);
    int err;

    hwmon_device_unregister(data->hwmon_dev);
    sysfs_remove_group(&client->dev.kobj, &adm1021_group);

    if ((err = i2c_detach_client(client)))
        return err;

    kfree(data);
    return 0;
}

static struct adm1021_data *adm1021_update_device(struct device *dev)
{
    struct i2c_client *client = to_i2c_client(dev);
    struct adm1021_data *data = i2c_get_clientdata(client);

    mutex_lock(&data->update_lock);

    if (time_after(jiffies, data->last_updated + HZ + HZ / 2)
        || !data->valid) {
        int i;

        dev_dbg(&client->dev, "Starting adm1021 update\n");

        for (i = 0; i < 2; i++) {
            data->temp[i] = i2c_smbus_read_byte_data(client,
                        ADM1021_REG_TEMP(i));
            data->temp_max[i] = i2c_smbus_read_byte_data(client,
                        ADM1021_REG_TOS_R(i));
            data->temp_min[i] = i2c_smbus_read_byte_data(client,
                        ADM1021_REG_THYST_R(i));
        }
        data->alarms = i2c_smbus_read_byte_data(client,
                        ADM1021_REG_STATUS) & 0x7c;
        if (data->type == adm1023) {
            data->remote_temp_prec =
                i2c_smbus_read_byte_data(client,
                        ADM1023_REG_REM_TEMP_PREC);
            data->remote_temp_os_prec =
                i2c_smbus_read_byte_data(client,
                        ADM1023_REG_REM_TOS_PREC);
            data->remote_temp_hyst_prec =
                i2c_smbus_read_byte_data(client,
                        ADM1023_REG_REM_THYST_PREC);
            data->remote_temp_offset =
                i2c_smbus_read_byte_data(client,
                        ADM1023_REG_REM_OFFSET);
            data->remote_temp_offset_prec =
                i2c_smbus_read_byte_data(client,
                        ADM1023_REG_REM_OFFSET_PREC);
        }
        data->last_updated = jiffies;
        data->valid = 1;
    }

    mutex_unlock(&data->update_lock);

    return data;
}

static int __init sensors_adm1021_init(void)
{
    return i2c_add_driver(&adm1021_driver);
}

static void __exit sensors_adm1021_exit(void)
{
    i2c_del_driver(&adm1021_driver);
}

MODULE_AUTHOR ("Frodo Looijaard <frodol@dds.nl> and "
        "Philip Edelbrock <phil@netroedge.com>");
MODULE_DESCRIPTION("adm1021 driver");
MODULE_LICENSE("GPL");

module_param(read_only, bool, 0);
MODULE_PARM_DESC(read_only, "Don't set any values, read only mode");

module_init(sensors_adm1021_init)
module_exit(sensors_adm1021_exit)
