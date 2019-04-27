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

TrunBox::TrunBox(uint32_t boxSize, const BoxType_t & boxType):Box(boxSize, boxType),
        m_sampleCount(0),
        m_dataOffset(0),
        m_firstSampleFlags(0),
        m_sampleInfoArray()
{
    //ctor
    m_bodyMinSize = 8;
}

TrunBox::~TrunBox()
{
    //dtor
}

int TrunBox::parseBody(uint8_t *p_body, uint32_t bodySize)
{
    uint8_t *p_bodyBuff = p_body;

    DBG_PrintU8Buffer("TRUN body", p_body, bodySize);

    if (checkBodyValid(p_body, bodySize) != ERR_OK)
    {
        LOG_E("Invalid body data.\n");
        return ERR_BAD_PARAM;
    }

    parseVerAndFlags(p_bodyBuff, bodySize);
    p_bodyBuff += 4;

    BEBytes2Uint32(p_bodyBuff, m_sampleCount);
    p_bodyBuff += 4;


    if (dataOffsetPresent() && (p_bodyBuff + 4) <= p_body + bodySize)
    {
        BEBytes2Uint32(p_bodyBuff, m_dataOffset);
        p_bodyBuff += 4;
    }

    if (firstSampleFlagsPresent() && (p_bodyBuff + 4) <= p_body + bodySize)
    {
        BEBytes2Uint32(p_bodyBuff, m_firstSampleFlags);
        p_bodyBuff += 4;
    }

    if ((p_bodyBuff + 16) <= p_body + bodySize)
    {
        m_sampleInfoArray.reserve(m_sampleCount);

        while ((p_bodyBuff + 16) <= p_body + bodySize && m_sampleInfoArray.size() < m_sampleCount)
        {
            SampleInfo_t sampleInfo;

            BEBytes2Uint32(p_bodyBuff, sampleInfo.sampleDuration);
            p_bodyBuff += 4;

            BEBytes2Uint32(p_bodyBuff, sampleInfo.sampleSize);
            p_bodyBuff += 4;

            BEBytes2Uint32(p_bodyBuff, sampleInfo.sampleFlags);
            p_bodyBuff += 4;

            BEBytes2Uint32(p_bodyBuff, sampleInfo.sampleCompositionTimeOffset);
            p_bodyBuff += 4;

            m_sampleInfoArray.push_back(sampleInfo);
        }
    }

    return ERR_OK;
}

int TrunBox::showBody()
{
    showVerFlags();
    HSP;cout << "sample_count:" << m_sampleCount << endl;
    if(dataOffsetPresent())
    {
        HSP;cout << "data_offset:" << m_dataOffset << endl;
    }

    if(firstSampleFlagsPresent())
    {
        HSP;cout << "first_sample_flags:" << m_firstSampleFlags << endl;
    }

    for (uint32_t i = 0; i < m_sampleCount; i++)
    {
        HSP;cout << "sample[" << i << "]:(";
        cout << "sample_duration:" << m_sampleInfoArray[i].sampleDuration;
        cout << ", sample_size:" << m_sampleInfoArray[i].sampleSize;
        cout << ", sample_flags:" << _HEX_DIGIT_ << m_sampleInfoArray[i].sampleFlags << dec;
        cout << ", sample_composition_time_offset:" << m_sampleInfoArray[i].sampleCompositionTimeOffset << ")" << endl;
    }

    return ERR_OK;
}

}//namespace mp4Parser
