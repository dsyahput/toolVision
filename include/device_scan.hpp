#ifndef DEVICE_SCAN_HPP
#define DEVICE_SCAN_HPP

#include <libudev.h>
#include <iostream>
#include <cstring>
#include <sstream>
#include <vector>
namespace device_scan
{
    std::vector<std::string> devlinks_list(const char *vendor_name, const char *subsystem)
    {
        std::vector<std::string> vec_to_return;

        struct udev *udev = udev_new();
        if (!udev)
        {
            std::cerr << "Cannot create udev context." << std::endl;
            return vec_to_return;
        }

        struct udev_enumerate *enumerate = udev_enumerate_new(udev);
        udev_enumerate_add_match_subsystem(enumerate, subsystem);
        udev_enumerate_scan_devices(enumerate);

        struct udev_list_entry *devices = udev_enumerate_get_list_entry(enumerate);
        struct udev_list_entry *dev_list_entry;

        udev_list_entry_foreach(dev_list_entry, devices)
        {
            const char *path = udev_list_entry_get_name(dev_list_entry);
            struct udev_device *dev = udev_device_new_from_syspath(udev, path);

            const char *vendor = udev_device_get_property_value(dev, "ID_VENDOR");
            if (vendor && strcmp(vendor, vendor_name) == 0)
            {
                const char *devlinks = udev_device_get_property_value(dev, "DEVLINKS");
                if (devlinks)
                {
                    std::istringstream devlinks_stream(devlinks);
                    std::string first_devlink;
                    devlinks_stream >> first_devlink;
                    std::cout << "Device found: " << path << std::endl;
                    std::cout << "DEVLINKS: " << first_devlink << std::endl;
                    vec_to_return.push_back(first_devlink);
                }
            }
            udev_device_unref(dev);
        }

        udev_enumerate_unref(enumerate);
        udev_unref(udev);

        return vec_to_return;
    }

}

#endif