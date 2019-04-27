/*
MIT License

Copyright (c) 2018 DuanBaoshan

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Author:DuanBaoshan
E-Mail:duanbaoshan79@163.com
Date:2018.6.29
*/

#include <stdio.h>

#include "utils.h"

#include "errvalue.h"
#include "debug.h"

namespace mp4Parser{

int BEBytes2Uint64(uint8_t bytes[8], uint64_t &out)
{
    out =   ((uint64_t)bytes[0] << 56)
            | ((uint64_t)bytes[1] << 48)
            | ((uint64_t)bytes[2] << 40)
            | ((uint64_t)bytes[3] << 32)
            | ((uint64_t)bytes[4] << 24)
            | ((uint64_t)bytes[5] << 16)
            | ((uint64_t)bytes[6] << 8)
            |  bytes[7];

    return ERR_OK;
}

int BEBytes2Uint32(uint8_t bytes[4], uint32_t &out)
{
    out = (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
    return ERR_OK;
}

int BEBytes2Uint16(uint8_t bytes[2], uint16_t &out)
{
    out = (bytes[0] << 8) | bytes[1] ;
    return ERR_OK;
}

int BEBytes2Int64(uint8_t bytes[8], int64_t &out)
{
    out =   ((int64_t)bytes[0] << 56)
            | ((int64_t)bytes[1] << 48)
            | ((int64_t)bytes[2] << 40)
            | ((int64_t)bytes[3] << 32)
            | ((int64_t)bytes[4] << 24)
            | ((int64_t)bytes[5] << 16)
            | ((int64_t)bytes[6] << 8)
            |  bytes[7];

    return ERR_OK;
}

int BEBytes2Int32(uint8_t bytes[4], int32_t &out)
{
    out = (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
    return ERR_OK;
}

int BEBytes2Int16(uint8_t bytes[2], int16_t &out)
{
    out = (bytes[0] << 8) | bytes[1] ;
    return ERR_OK;
}

int BEBytes2Flags(uint8_t bytes[3], uint32_t &flags)
{
    flags = (bytes[0] << 16) | (bytes[1] << 8) | bytes[2];
    return ERR_OK;
}
}//namespace  mp4Parser
