#ifndef RINGBUFFER_H
#define RINGBUFFER_H

typedef struct {
	uint8_t* Buffer;
	uint16_t headIndex;
	uint16_t tailIndex;
	uint16_t BufferLength;
}CircularBuffer_t;

typedef enum 
{
	RINGBUFFER_OK       	= 0x00,
	RINGBUFFER_ERROR  = 0x01,
	RINGBUFFER_FULL	= 0x02,
	RINGBUFFER_EMPTY	= 0x03
} RingBufferTypeDef;

void RingBufferInit(CircularBuffer_t* cb,uint8_t* pointer,uint16_t _length);
uint8_t RingBufferPush(CircularBuffer_t* cb,uint8_t data);
uint8_t RingBufferPop(CircularBuffer_t* cb,uint8_t* data);
	
#endif