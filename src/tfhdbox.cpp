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

TfhdBox::TfhdBox(uint32_t boxSize, const BoxType_t & boxType):Box(boxSize, boxType),
    m_trackID(0),
    m_baseDataOffset(0),
    m_sampleDescriptionIndex(0),
    m_defaultSampleDuration(0),
    m_defaultSampleSize(0),
    m_defaultSampleFlags(0)
{
    //ctor
    m_bodyMinSize = 8;
}

TfhdBox::~TfhdBox()
{
    //dtor
}

int TfhdBox::parseBody(uint8_t *p_body, uint32_t bodySize)
{
    uint8_t *p_bodyBuff = p_body;

    DBG_PrintU8Buffer("TFHD body", p_body, bodySize);

    if (checkBodyValid(p_body, bodySize) != ERR_OK)
    {
        LOG_E("Invalid body data.\n");
        return ERR_BAD_PARAM;
    }

    parseVerAndFlags(p_bodyBuff, bodySize);
    p_bodyBuff += 4;

    BEBytes2Uint32(p_bodyBuff, m_trackID);
    p_bodyBuff += 4;

    if (baseDataOffsetPresent() && (p_bodyBuff + 8) <= p_body + bodySize)
    {
        BEBytes2Uint64(p_bodyBuff, m_baseDataOffset);
        p_bodyBuff += 8;
    }


    if (sampleDescIndexPresent() && (p_bodyBuff + 4) <= p_body + bodySize)
    {
        BEBytes2Uint32(p_bodyBuff, m_sampleDescriptionIndex);
        p_bodyBuff += 4;
    }

    if (defaultSampleDurationRresent() && (p_bodyBuff + 4) <= p_body + bodySize)
    {
        BEBytes2Uint32(p_bodyBuff, m_defaultSampleDuration);
        p_bodyBuff += 4;
    }

    if (defaultSampleSizePresent() && (p_bodyBuff + 4) <= p_body + bodySize)
    {
        BEBytes2Uint32(p_bodyBuff, m_defaultSampleSize);
        p_bodyBuff += 4;
    }

    if (defaultSampleFlagsPresent() && (p_bodyBuff + 4) <= p_body + bodySize)
    {
        BEBytes2Uint32(p_bodyBuff, m_defaultSampleFlags);
        p_bodyBuff += 4;
    }

    if (defaultSampleFlagsPresent() && (p_bodyBuff + 4) <= p_body + bodySize)
    {
        BEBytes2Uint32(p_bodyBuff, m_defaultSampleFlags);
        p_bodyBuff += 4;
    }

    return ERR_OK;
}

int TfhdBox::showBody()
{
    showVerFlags();
    HSP;cout << "track_ID:" << m_trackID << endl;

    if(baseDataOffsetPresent())
    {
        HSP;cout << "base_data_offset:" << m_baseDataOffset << endl;
    }

    if(sampleDescIndexPresent())
    {
        HSP;cout << "sample_description_index:" << m_sampleDescriptionIndex << endl;
    }

    if(defaultSampleDurationRresent())
    {
        HSP;cout << "default_sample_duration:" << m_defaultSampleDuration << endl;
    }

    if(defaultSampleSizePresent())
    {
        HSP;cout << "default_sample_size:" << m_defaultSampleSize << endl;
    }

    if(defaultSampleFlagsPresent())
    {
        HSP;cout << "default_sample_flags:" << m_defaultSampleFlags << endl;
    }

    return ERR_OK;
}

}//namespace mp4Parser
