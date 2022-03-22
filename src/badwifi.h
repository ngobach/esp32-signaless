#pragma once

#include <Arduino.h>
#include <vector>

namespace BadWifi
{
    typedef uint8_t BSSID[6];

    struct AP
    {
        String ssid;
        BSSID bssid;
    };

    struct ScannedAP {
        AP ap;
        uint8_t signal_level;
    };

    struct Stat {
        bool is_attacking;
        uint32_t sent_packets;
        uint32_t duration;
        AP target;
    };

    std::vector<AP> targets;

    std::vector<ScannedAP> scan()
    {
        std::vector<ScannedAP> list;

        list.emplace_back(ScannedAP{
            .ap = AP {
                .ssid = "BachNX WiFi",
                .bssid = {0, 0, 0, 0, 0, 1},
            },
            .signal_level = 3,
        });

        list.emplace_back(ScannedAP{
            .ap = AP {
                .ssid = "Weak WiFi",
                .bssid = {0, 0, 0, 0, 0, 2},
            },
            .signal_level = 1,
        });

        list.emplace_back(ScannedAP{
            .ap = AP {
                .ssid = "Normal WiFi",
                .bssid = {0, 0, 0, 0, 0, 3},
            },
            .signal_level = 2,
        });

        list.emplace_back(ScannedAP{
            .ap = AP {
                .ssid = "Strong WiFi",
                .bssid = {0, 0, 0, 0, 0, 3},
            },
            .signal_level = 3,
        });

        return list;
    }

    void begin_attack(AP ap)
    {
        // no-op'
    }

    std::vector<AP> get_targets() {
        if (targets.empty()) {
            for (int i = 0; i < 20; i++) {
                targets.emplace_back(AP {
                    .ssid = String("Wifi No. ") + i,
                    .bssid = {0, 0, 0, 0, 0, (uint8_t)i},
                });
            }
        }
        return targets;
    }

    Stat get_stats() {
        return Stat {            
        };
    }
};
