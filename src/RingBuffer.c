#include "RingBuffer.h"

void RingBufferInit(CircularBuffer_t* cb,uint8_t* pointer,uint16_t _length){
	cb->Buffer = pointer;
	cb->headIndex = 0;
	cb->tailIndex = 0;
	cb->BufferLength = _length;
}

uint8_t RingBufferPush(CircularBuffer_t* cb,uint8_t data){
	uint16_t next;
	next = cb->headIndex+1;
	if (next >= cb.BufferLength){
		next = 0;
	}
	// Cicular buffer is full
	if (next == cb->tailIndex)
	return RINGBUFFER_FULL;  // quit with an error

	cb->Buffer[cb->headIndex] = data;
	cb->headIndex = next;
	return RINGBUFFER_OK;
}

uint8_t RingBufferPop(CircularBuffer_t* cb,uint8_t* data){
	uint16_t next;
	// if the head isn't ahead of the tail, we don't have any characters
	if (cb->headIndex == cb->tailIndex)
	return RINGBUFFER_EMPTY;  	// quit with an error
	*data = cb->Buffer[cb->tailIndex];
	cb->Buffer[cb->tailIndex] = 0;  // clear the data (optional)

	next = cb->tailIndex + 1;
	if(next >= cb->BufferLength){
		next = 0;
	}
	cb->tailIndex = next;
	return RINGBUFFER_OK;
}

