#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <inttypes.h>

namespace mp4Parser
{

int BEBytes2Uint64(uint8_t bytes[8], uint64_t &out);

int BEBytes2Uint32(uint8_t bytes[4], uint32_t &out);

int BEBytes2Uint16(uint8_t bytes[2], uint16_t &out);

int BEBytes2Int64(uint8_t bytes[8], int64_t &out);

int BEBytes2Int32(uint8_t bytes[4], int32_t &out);

int BEBytes2Int16(uint8_t bytes[2], int16_t &out);

int BEBytes2Flags(uint8_t bytes[3], uint32_t &flags);
}//namespace  mp4Parser

#endif // UTILS_H_INCLUDED
