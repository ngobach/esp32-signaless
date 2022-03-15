#include <Arduino.h>
#include <esp_wifi.h>

// wifi_init_config_t wifiInitializationConfig = WIFI_INIT_CONFIG_DEFAULT();

const char* SSID = "esp32-beaconspam";
const char* PASSWORD = "dummypassword";

void setup2()
{
	// nvs_flash_init();
	tcpip_adapter_init();

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

	// ESP_ERROR_CHECK(esp_event_loop_init(c, NULL));
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

	// Init dummy AP to specify a channel and get WiFi hardware into
	// a mode where we can send the actual fake beacon frames.
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
	wifi_config_t ap_config = {
		.ap = {
			{ .ssid = "esp32-beaconspam" },
			{ .password = "dummypassword" },
			.ssid_len = 0,
			.channel = 1,
			.authmode = WIFI_AUTH_WPA2_PSK,
			.ssid_hidden = 1,
			.max_connection = 4,
			.beacon_interval = 60000
		}
	};

	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
	ESP_ERROR_CHECK(esp_wifi_start());
	ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));
}

uint8_t beacon_raw[] = {
    0x80, 0x00,                                                 // 0-1: Frame Control
    0x00, 0x00,                                                 // 2-3: Duration
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff,                         // 4-9: Destination address (broadcast)
    0xba, 0xde, 0xaf, 0xfe, 0x00, 0x06,                         // 10-15: Source address
    0xba, 0xde, 0xaf, 0xfe, 0x00, 0x06,                         // 16-21: BSSID
    0x00, 0x00,                                                 // 22-23: Sequence / fragment number
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,             // 24-31: Timestamp (GETS OVERWRITTEN TO 0 BY HARDWARE)
    0x64, 0x00,                                                 // 32-33: Beacon interval
    0x31, 0x04,                                                 // 34-35: Capability info
    0x00, 0x00, /* FILL CONTENT HERE */                         // 36-38: SSID parameter set, 0x00:length:content
    0x01, 0x08, 0x82, 0x84, 0x8b, 0x96, 0x0c, 0x12, 0x18, 0x24, // 39-48: Supported rates
    0x03, 0x01, 0x01,                                           // 49-51: DS Parameter set, current channel 1 (= 0x01),
    0x05, 0x04, 0x01, 0x02, 0x00, 0x00,                         // 52-57: Traffic Indication Map
};

char *rick_ssids[] = { "bao", "bao 5G" };

#define BEACON_SSID_OFFSET 38
#define SRCADDR_OFFSET 10
#define BSSID_OFFSET 16
#define SEQNUM_OFFSET 22
#define TOTAL_LINES (sizeof(rick_ssids) / sizeof(char *))

void loop2()
{

  uint8_t line = 0;

  // Keep track of beacon sequence numbers on a per-songline-basis
  uint16_t seqnum[TOTAL_LINES] = {0};

  for (;;)
  {
    delay(10);

    // Insert line of Rick Astley's "Never Gonna Give You Up" into beacon packet
    printf("%i %i %s\r\n", strlen(rick_ssids[line]), TOTAL_LINES, rick_ssids[line]);

    uint8_t beacon_rick[200];
    memcpy(beacon_rick, beacon_raw, BEACON_SSID_OFFSET - 1);
    beacon_rick[BEACON_SSID_OFFSET - 1] = strlen(rick_ssids[line]);
    memcpy(&beacon_rick[BEACON_SSID_OFFSET], rick_ssids[line], strlen(rick_ssids[line]));
    memcpy(&beacon_rick[BEACON_SSID_OFFSET + strlen(rick_ssids[line])], &beacon_raw[BEACON_SSID_OFFSET], sizeof(beacon_raw) - BEACON_SSID_OFFSET);

    // Last byte of source address / BSSID will be line number - emulate multiple APs broadcasting one song line each
    beacon_rick[SRCADDR_OFFSET + 5] = line;
    beacon_rick[BSSID_OFFSET + 5] = line;

    // Update sequence number
    beacon_rick[SEQNUM_OFFSET] = (seqnum[line] & 0x0f) << 4;
    beacon_rick[SEQNUM_OFFSET + 1] = (seqnum[line] & 0xff0) >> 4;
    seqnum[line]++;
    if (seqnum[line] > 0xfff)
      seqnum[line] = 0;

    esp_wifi_80211_tx(WIFI_IF_AP, beacon_rick, sizeof(beacon_raw) + strlen(rick_ssids[line]), false);

    if (++line >= TOTAL_LINES)
      line = 0;
  }

  delay(100);
}