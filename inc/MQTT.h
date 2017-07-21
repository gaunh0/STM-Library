#ifndef MQTT_H
#define MQTT_H

#include "stm32f1xx_hal.h"

#define MQTT_VERSION_3_1      3
#define MQTT_VERSION_3_1_1    4

// MQTT_VERSION : Pick the version
//#define MQTT_VERSION MQTT_VERSION_3_1
#ifndef MQTT_VERSION
#define MQTT_VERSION MQTT_VERSION_3_1_1
#endif

// MQTT_MAX_PACKET_SIZE : Maximum packet size
#ifndef MQTT_MAX_PACKET_SIZE
#define MQTT_MAX_PACKET_SIZE 128
#endif

// MQTT_KEEPALIVE : keepAlive interval in Seconds
#ifndef MQTT_KEEPALIVE
#define MQTT_KEEPALIVE 		60
#endif

// MQTT_SOCKET_TIMEOUT: socket timeout interval in Seconds
#ifndef MQTT_SOCKET_TIMEOUT
#define MQTT_SOCKET_TIMEOUT 15
#endif

// MQTT_MAX_TRANSFER_SIZE : limit how much data is passed to the network client
//  in each write call. Needed for the Arduino Wifi Shield. Leave undefined to
//  pass the entire MQTT packet in each write call.
//#define MQTT_MAX_TRANSFER_SIZE 80

// Possible values for client.state()
#define MQTT_CONNECTION_TIMEOUT     -4
#define MQTT_CONNECTION_LOST        -3
#define MQTT_CONNECT_FAILED         -2
#define MQTT_DISCONNECTED           -1
#define MQTT_CONNECTED               0
#define MQTT_CONNECT_BAD_PROTOCOL    1
#define MQTT_CONNECT_BAD_CLIENT_ID   2
#define MQTT_CONNECT_UNAVAILABLE     3
#define MQTT_CONNECT_BAD_CREDENTIALS 4
#define MQTT_CONNECT_UNAUTHORIZED    5

#define MQTTCONNECT     			0x10  // Client request to connect to Server
#define MQTTCONNACK    				0x20  // Connect Acknowledgment
#define MQTTPUBLISH     				0x30  // Publish message
#define MQTTPUBACK      				0x40  // Publish Acknowledgment
#define MQTTPUBREC      				0x50  // Publish Received (assured delivery part 1)
#define MQTTPUBREL      				0x60  // Publish Release (assured delivery part 2)
#define MQTTPUBCOMP     			0x70  // Publish Complete (assured delivery part 3)
#define MQTTSUBSCRIBE   			0x80  // Client Subscribe request
#define MQTTSUBACK      				0x90  // Subscribe Acknowledgment
#define MQTTUNSUBSCRIBE 			0xA0 // Client Unsubscribe request
#define MQTTUNSUBACK    			0xB0 // Unsubscribe Acknowledgment
#define MQTTPINGREQ     				0xC0 // PING Request
#define MQTTPINGRESP    			0xD0 // PING Response
#define MQTTDISCONNECT  			0xE0 // Client is Disconnecting
#define MQTTReserved    				0xF0 // Reserved

#define MQTTQOS0        			(0 << 1)
#define MQTTQOS1        			(1 << 1)
#define MQTTQOS2        			(2 << 1)

#define MQTT_CONN_USERNAMEFLAG    	0x80
#define MQTT_CONN_PASSWORDFLAG    	0x40
#define MQTT_CONN_WILLRETAIN      		0x20
#define MQTT_CONN_WILLQOS_1       		0x08
#define MQTT_CONN_WILLQOS_2       		0x18
#define MQTT_CONN_WILLFLAG        		0x04
#define MQTT_CONN_CLEANSESSION    		0x02

/* Default Configurations */
#define DEFAULT_CMD_TIMEOUT_MS  30000
#define DEFAULT_CON_TIMEOUT_MS  5000
#define DEFAULT_MQTT_QOS        MQTT_QOS_0
#define DEFAULT_KEEP_ALIVE_SEC  60
#define DEFAULT_CLIENT_ID       "WolfMQTTClient"
#define WOLFMQTT_TOPIC_NAME     "wolfMQTT/example/"
#define DEFAULT_TOPIC_NAME      WOLFMQTT_TOPIC_NAME"testTopic"

#define PRINT_BUFFER_SIZE       80
#define MAX_PACKET_ID           ((1 << 16) - 1)


/* MQTT Client state */
typedef enum MQTTCtxState {
    WMQ_BEGIN = 0,
    WMQ_NET_INIT,
    WMQ_INIT,
    WMQ_TCP_CONN,
    WMQ_MQTT_CONN,
    WMQ_SUB,
    WMQ_PUB,
    WMQ_WAIT_MSG,
    WMQ_UNSUB,
    WMQ_DISCONNECT,
    WMQ_NET_DISCONNECT,
    WMQ_DONE,
} MQTTCtxState;

/* MQTT Client context */
typedef struct MQTTCtx {
    MQTTCtxState stat;
    int return_code;

    /* configuration */
    const char* app_name;
    word16 port;
    const char* host;
    int use_tls;
    MqttQoS qos;
    byte clean_session;
    word16 keep_alive_sec;
    const char* client_id;
    int enable_lwt;
    const char* username;
    const char* password;
    byte *tx_buf, *rx_buf;
    const char* topic_name;
    word32 cmd_timeout_ms;
    byte test_mode;
    const char* pub_file;
    int retain;
#ifdef ENABLE_AZUREIOTHUB_EXAMPLE
    char sasToken[400];
#endif

    /* client and net containers */    
    MqttClient client;
    MqttNet net;

    /* temp mqtt containers */
    MqttConnect connect;
    MqttMessage lwt_msg;
    MqttSubscribe subscribe;
    MqttUnsubscribe unsubscribe;
    MqttTopic topics[1], *topic;
    MqttPublish publish;
} MQTTCtx;

void MQTTInit(void);
uint8_t MQTTConnectPacket(const char *ClientID, const char *ClientUser, const char *ClientPass, const char* willTopic, uint8_t willQos, uint8_t willRetain, const char* willMessage);
uint8_t MQTTSubscribePacket(const char* topic, uint8_t qos);
uint8_t MQTTUnSubscribePacket(const char* topic, uint8_t qos);
uint8_t MQTTPingRequest(void);

uint16_t GetPacketIdentifer(void);
void IncreasePacketIdentifier(void);
void ResetPacketIdentifier(void);

void mqtt_init_ctx(MQTTCtx* mqttCtx);
int mqtt_tls_cb(MqttClient* client);
word16 mqtt_get_packetid(void);

#endif


