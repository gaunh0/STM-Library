#ifndef ESP8266WIFITYPE_H_
#define ESP8266WIFITYPE_H_

typedef enum WiFiMode 
{
    WIFI_OFF = 0, WIFI_STA = 1, WIFI_AP = 2, WIFI_AP_STA = 3
} WiFiMode_t;

typedef enum WiFiPhyMode
{
    WIFI_PHY_MODE_11B = 1, WIFI_PHY_MODE_11G = 2, WIFI_PHY_MODE_11N = 3
} WiFiPhyMode_t;

typedef enum WiFiSleepType
{
    WIFI_NONE_SLEEP = 0, WIFI_LIGHT_SLEEP = 1, WIFI_MODEM_SLEEP = 2
} WiFiSleepType_t;


#endif /* ESP8266WIFITYPE_H_ */
