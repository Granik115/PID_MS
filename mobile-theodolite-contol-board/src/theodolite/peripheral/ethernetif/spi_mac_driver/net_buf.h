/*
 * net_buf.h
 *
 *  Created on: Feb 27, 2021
 *      Author: maxim
 */

#ifndef INC_NET_BUF_H_
#define INC_NET_BUF_H_

/**
 * @brief Structure describing a chunk of data
 **/

typedef struct
{
	void *address;
	uint16_t length;
	uint16_t size;
} ChunkDesc;

/**
 * @brief Structure describing a buffer that spans multiple chunks
 **/

typedef struct
{
	uint16_t chunkCount;
	uint16_t maxChunkCount;
	ChunkDesc chunk[];
} NetBuffer;

#endif /* INC_NET_BUF_H_ */
