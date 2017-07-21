#include "MQTT.h"
#include <string.h>

uint8_t* brokerAddress = "192.168.1.55";
uint8_t* brokerPort = "1883";

uint8_t MQTTpacket[MQTT_MAX_PACKET_SIZE] ;
uint16_t PacketIdentifier = 1;
uint8_t MQTTConnectionFlag = 0;


/* Private functions */
static int EthernetConnect(void *context, const char* host, word16 port, int timeout_ms) {
	int returnCode = 0;
	returnCode = WifiTCPConnection(host, port);
	return returnCode;
}

static int EthernetRead(void *context, byte* buf, int buf_len, int timeout_ms) {
	int recvd = 0;
	/* While data and buffer available */
	while (UARTWifi_RxAvailable() > 0 && recvd < buf_len) {
		buf[recvd] = UARTWifi_ReceiveByte();
		recvd++;
	}

	return recvd;
}

static int EthernetWrite(void *context, const byte* buf, int buf_len, int timeout_ms) {
	int returnCode = 0;
	returnCode = WifiSendData(buf, buf_len);
	return returnCode;
}

static int EthernetDisconnect(void *context) {
 	 WifiTCPDisConnection();
	return 0;
}




void mqtt_init_ctx(MQTTCtx* mqttCtx)
{
    XMEMSET(mqttCtx, 0, sizeof(MQTTCtx));
    mqttCtx->host = DEFAULT_MQTT_HOST;
    mqttCtx->qos = DEFAULT_MQTT_QOS;
    mqttCtx->clean_session = 1;
    mqttCtx->keep_alive_sec = DEFAULT_KEEP_ALIVE_SEC;
    mqttCtx->client_id = DEFAULT_CLIENT_ID;
    mqttCtx->topic_name = DEFAULT_TOPIC_NAME;
    mqttCtx->cmd_timeout_ms = DEFAULT_CMD_TIMEOUT_MS;
}


/* Public Functions */
int MqttClientNet_Init(MqttNet* net){
    if (net) {
        XMEMSET(net, 0, sizeof(MqttNet));
        net->connect = NetConnect;
        net->read = NetRead;
        net->write = NetWrite;
        net->disconnect = NetDisconnect;
        net->context = WOLFMQTT_MALLOC(sizeof(UartContext));
    }
    return 0;
}

MQTTCtx mqttCtx;
void MQTTInit(){
	
	//WifiTCPConnection(brokerAddress,brokerPort);
	mqtt_init_ctx(&mqttCtx);
       mqttCtx.app_name = "mqttclient";
}

uint8_t MQTTConnectPacket(const char *ClientID, const char *ClientUser, const char *ClientPass, const char* willTopic, uint8_t willQos, uint8_t willRetain, const char* willMessage){	
	uint8_t PacketIndex = 0;
	uint16_t length_temp = 0;
	uint8_t index;
	uint8_t connack[4] = {0x20,0x02,0x00,0x00};
	// fixed header, connection messsage no flags
	MQTTpacket[PacketIndex] = MQTTCONNECT;
	PacketIndex+=2;
#if MQTT_VERSION == MQTT_VERSION_3_1
	#define MQTT_HEADER_VERSION_LENGTH 9
	uint8_t MQTTHeader[MQTT_HEADER_VERSION_LENGTH] = {0x00,0x06,'M','Q','I','s','d','p', MQTT_VERSION_3_1};
#elif MQTT_VERSION == MQTT_VERSION_3_1_1
	#define MQTT_HEADER_VERSION_LENGTH 7
	uint8_t MQTTHeader[MQTT_HEADER_VERSION_LENGTH] = {0x00,0x04,'M','Q','T','T',MQTT_VERSION_3_1_1};	
#else
	printf("Version Error\r\n");
#endif

	for (index = 0;index < MQTT_HEADER_VERSION_LENGTH;index++){
		 MQTTpacket[PacketIndex++] = MQTTHeader[index];
	}
	// Connect Flag bit
	MQTTpacket[PacketIndex] |= MQTT_CONN_CLEANSESSION;	
	// set the will flags if needed
	if (strlen(willTopic) > 0) {
		MQTTpacket[PacketIndex] |= MQTT_CONN_WILLFLAG;
		MQTTpacket[PacketIndex] |= (willQos<<3)|(willRetain<<5);
	}
	if(strlen(ClientUser) > 0) {
		MQTTpacket[PacketIndex] |= MQTT_CONN_USERNAMEFLAG;
		if(strlen(ClientPass) > 0) {
			 MQTTpacket[PacketIndex] |= MQTT_CONN_PASSWORDFLAG;
		}
	}
	
	PacketIndex++;	
	MQTTpacket[PacketIndex++] = MQTT_KEEPALIVE >> 8;
	MQTTpacket[PacketIndex++]= MQTT_KEEPALIVE & 0xFF;
	/********* PAYLOAD ***********/
	// CLIENT ID
	length_temp = strlen(ClientID);
	MQTTpacket[PacketIndex++] = length_temp >> 8;
	MQTTpacket[PacketIndex++]= length_temp & 0xFF;
	length_temp = strlen(ClientID);
	for (index = 0;index<length_temp;index++){
		MQTTpacket[PacketIndex++] = ClientID[index];
	}
	// WILL TOPIC
	if (strlen(willTopic) > 0){
		length_temp = strlen(willTopic);
		MQTTpacket[PacketIndex++] = length_temp >> 8;
		MQTTpacket[PacketIndex++]= length_temp & 0xFF;
		for (index = 0;index<length_temp;index++){
			MQTTpacket[PacketIndex++] = willTopic[index];
		}
		length_temp = strlen(willMessage);
		MQTTpacket[PacketIndex++] = length_temp >> 8;
		MQTTpacket[PacketIndex++]= length_temp & 0xFF;
		for (index = 0;index<length_temp;index++){
			MQTTpacket[PacketIndex++] = willMessage[index];
		}
	}
	
	if(strlen(ClientUser) > 0) {
		length_temp = strlen(ClientUser);
		MQTTpacket[PacketIndex++] = length_temp >> 8;
		MQTTpacket[PacketIndex++]= length_temp & 0xFF;
		for (index = 0;index<length_temp;index++){
			MQTTpacket[PacketIndex++] = ClientUser[index];
		}		
		if(strlen(ClientPass) > 0) {
			length_temp = strlen(ClientPass);
			MQTTpacket[PacketIndex++] = length_temp >> 8;
			MQTTpacket[PacketIndex++]= length_temp & 0xFF;
			for (index = 0;index<length_temp;index++){
				MQTTpacket[PacketIndex++] = ClientPass[index];
			}
		}
	}
	
	MQTTpacket[1] = PacketIndex-2;
	/*printf("pk =");
	for (uint8_t i = 0;i<PacketIndex;i++){
		printf("%c",MQTTpacket[i]);
	}*/
	/*if (WifiSendData(MQTTpacket,PacketIndex) == 0){
		WifiTCPConnection(brokerAddress,brokerPort);
	}	*/
	if (WifiSendData(MQTTpacket,PacketIndex) == 1){
		// Wait CONNACK
		printf("Wait CONNACK\r\n");	
		if (UARTWifi_WaitString(3000,connack)){
			printf("MQTT connected\r\n");
			MQTTConnectionFlag = 1;
			return 1;
		}
	}
	return 0;
} 

uint8_t MQTTSubscribePacket(const char* topic, uint8_t qos) {
	uint8_t PacketIndex = 0;	
	uint8_t topicLength;
	uint16_t PacketID = GetPacketIdentifer();
	IncreasePacketIdentifier();
	PacketIndex = 0;
	// fixed header, connection messsage no flags
	MQTTpacket[PacketIndex] = 0x82;
	PacketIndex+=2;
	MQTTpacket[PacketIndex++] = PacketID>>8;
	MQTTpacket[PacketIndex++] = PacketID&0xFF;
	topicLength = strlen(topic);
	MQTTpacket[PacketIndex++] = topicLength>>8;
	MQTTpacket[PacketIndex++] = topicLength&0xFF;
	for (uint8_t i=0;i<topicLength;i++){
		MQTTpacket[PacketIndex++] = topic[i];
	}
	MQTTpacket[PacketIndex++] = qos;
	MQTTpacket[1] = PacketIndex - 2;
	WifiSendData(MQTTpacket,PacketIndex);
}

uint8_t MQTTUnSubscribePacket(const char* topic, uint8_t qos) {
	uint8_t PacketIndex = 0;
	uint16_t PacketID = 0;
	uint8_t topicLength;
	
	PacketIndex = 0;
	PacketID = GetPacketIdentifer();
	IncreasePacketIdentifier();
	// fixed header, connection messsage no flags
	MQTTpacket[PacketIndex] = 0xA2;
	PacketIndex+=2;
	MQTTpacket[PacketIndex++] = PacketID>>8;
	MQTTpacket[PacketIndex++] = PacketID&0xFF;
	topicLength = strlen(topic);
	MQTTpacket[PacketIndex++] = topicLength>>8;
	MQTTpacket[PacketIndex++] = topicLength&0xFF;
	for (uint8_t i=0;i<topicLength;i++){
		MQTTpacket[PacketIndex++] = topic[i];
	}
	MQTTpacket[PacketIndex] = qos;
	MQTTpacket[1] = PacketIndex - 2;
	
	WifiSendData(MQTTpacket,PacketIndex);
}

uint8_t MQTTPingRequest(){
	uint8_t pingResp[2] = {0xD0,0x00};
	MQTTpacket[0] = 0xC0;
	MQTTpacket[1] = 0;
	WifiSendData(MQTTpacket,2);
	// Check PINGRESP
	UARTWifi_ClearRxBuffer();
	if (UARTWifi_WaitString(1000,pingResp)){
		return 1;
	}
	return 0;
	
	/*if (WifiSendData(MQTTpacket,2) == 0){
		WifiTCPConnection(brokerAddress,brokerPort);
	}*/
}

uint8_t MQTTPingACK(){
	
}

void ReconnecBroker(){
	WifiTCPConnection(brokerAddress,brokerPort);
}

uint16_t GetPacketIdentifer(){
	return PacketIdentifier;
}
void IncreasePacketIdentifier(){
	PacketIdentifier++;
}
void ResetPacketIdentifier(){
	PacketIdentifier = 0;	
}
