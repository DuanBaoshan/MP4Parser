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

#include <iostream>

#include "mp4box.h"

#ifndef _DEBUG_LEVEL_
#define _DEBUG_LEVEL_  _DEBUG_LEVEL_I_
#endif

//#define _DEBUG_BUFFER_ON_
#include "debug.h"

using namespace std;

namespace mp4Parser{

StscBox::StscBox(uint32_t boxSize, const BoxType_t & boxType):Box(boxSize, boxType),m_entryCount(0),m_chunks()
{
    //ctor
    m_bodyMinSize = 8;
}

StscBox::~StscBox()
{
    //dtor
}

int StscBox::parseBody(uint8_t *p_body, uint32_t bodySize)
{
    uint8_t *p_bodyBuff = p_body;

    DBG_PrintU8Buffer("STSC body", p_body, bodySize);

    if (checkBodyValid(p_body, bodySize) != ERR_OK)
    {
        LOG_E("Invalid body data.\n");
        return ERR_BAD_PARAM;
    }

    parseVerAndFlags(p_bodyBuff, bodySize);
    p_bodyBuff += 4;

    BEBytes2Uint32(p_bodyBuff, m_entryCount);
    p_bodyBuff += 4;

    if (m_entryCount > 0)
    {
        m_chunks.reserve(m_entryCount);
        for (uint32_t i = 0; i < m_entryCount && p_bodyBuff + 12 <= p_body + bodySize; i++)
        {
            ChunkInfo_t chunk;

            BEBytes2Uint32(p_bodyBuff, chunk.firstChunk);
            p_bodyBuff += 4;

            BEBytes2Uint32(p_bodyBuff, chunk.samplesPerChunk);
            p_bodyBuff += 4;

            BEBytes2Uint32(p_bodyBuff, chunk.sampleDescriptionIndex);
            p_bodyBuff += 4;

            m_chunks.push_back(chunk);
        }
    }
    return ERR_OK;
}

int StscBox::showBody()
{
    showVerFlags();

    HSP;cout << "Has " << m_entryCount << " chunks." << endl;
    for (uint32_t i = 0; i < m_entryCount; i++)
    {
        HSP;cout << "Chunk " << i + 1 << ":";
        cout << "(first_chunk:" << m_chunks[i].firstChunk;
        cout << ", samples_per_chunk:" << m_chunks[i].samplesPerChunk;
        cout << ", sample_description_index:" <<  m_chunks[i].sampleDescriptionIndex << ")" << endl;
    }

    return ERR_OK;
}

}//namespace mp4Parser
