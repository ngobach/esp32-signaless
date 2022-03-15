#pragma once

#include <Arduino.h>
#include <vector>

struct BadWifiAP
{
    String ssid;
    uint8_t bssid[6];
    uint8_t signal_level;
};

class BadWifi
{
private:
public:
    static std::vector<BadWifiAP> ScanAPs()
    {
        std::vector<BadWifiAP> list;
        list.emplace_back(BadWifiAP {
            .ssid = "BachNX WiFi",
            .bssid = {0, 0, 0, 0, 0, 1},
            .signal_level = 3,
        });

        list.emplace_back(BadWifiAP {
            .ssid = "Weak WiFi",
            .bssid = {0, 0, 0, 0, 0, 2},
            .signal_level = 1,
        });

        list.emplace_back(BadWifiAP {
            .ssid = "Normal WiFi",
            .bssid = {0, 0, 0, 0, 0, 3},
            .signal_level = 2,
        });

        list.emplace_back(BadWifiAP {
            .ssid = "Strong WiFi",
            .bssid = {0, 0, 0, 0, 0, 4},
            .signal_level = 3,
        });

        return list;
    }
};
