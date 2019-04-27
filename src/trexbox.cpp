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

TrexBox::TrexBox(uint32_t boxSize, const BoxType_t & boxType):Box(boxSize, boxType),
        m_trackID(0),
        m_defaultSampleDescriptionIndex(0),
        m_defaultSampleDuration(0),
        m_defaultSampleSize(0),
        m_defaultSampleFlags(0)
{
    //ctor
    m_bodyMinSize = 24;
}

TrexBox::~TrexBox()
{
    //dtor
}

int TrexBox::parseBody(uint8_t *p_body, uint32_t bodySize)
{
    uint8_t *p_bodyBuff = p_body;

    if (checkBodyValid(p_body, bodySize) != ERR_OK)
    {
        LOG_E("Invalid body data.\n");
        return ERR_BAD_PARAM;
    }

    DBG_PrintU8Buffer("TREX body", p_body, bodySize);

    parseVerAndFlags(p_bodyBuff, bodySize);
    p_bodyBuff += 4;

    BEBytes2Uint32(p_bodyBuff, m_trackID);
    p_bodyBuff += 4;

    BEBytes2Uint32(p_bodyBuff, m_defaultSampleDescriptionIndex);
    p_bodyBuff += 4;

    BEBytes2Uint32(p_bodyBuff, m_defaultSampleDuration);
    p_bodyBuff += 4;

    BEBytes2Uint32(p_bodyBuff, m_defaultSampleSize);
    p_bodyBuff += 4;

    BEBytes2Uint32(p_bodyBuff, m_defaultSampleFlags);

    return ERR_OK;
}

int TrexBox::showBody()
{
    showVerFlags();
    HSP;cout << "track_ID:" << m_trackID << endl;
    HSP;cout << "default_sample_description_index:" << m_defaultSampleDescriptionIndex << endl;
    HSP;cout << "default_sample_duration:" <<  m_defaultSampleDuration << endl;
    HSP;cout << "default_sample_size:" <<  m_defaultSampleSize << endl;
    HSP;cout << "default_sample_flags:" <<  m_defaultSampleFlags << endl;

    return ERR_OK;
}

}//namespace mp4Parser
