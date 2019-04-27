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
#define _DEBUG_LEVEL_ _DEBUG_LEVEL_I_
#endif // _DEBUG_LEVEL_

//#define _DEBUG_BUFFER_ON_
#include "debug.h"

using namespace std;

namespace mp4Parser{

MdhdBox::MdhdBox(uint32_t boxSize, const BoxType_t & boxType):Box(boxSize, boxType),
    m_creationTime(0),
    m_modificationTime(0),
    m_timescale(0),
    m_duration(0)
{
    //ctor
    m_bodyMinSize = 24;
    memset(m_language, 0, sizeof(m_language));
}

MdhdBox::~MdhdBox()
{
    //dtor
}

int MdhdBox::parseBody(uint8_t *p_body, uint32_t bodySize)
{
    uint8_t *p_bodyBuff = p_body;

    DBG_PrintU8Buffer("MDHD body", p_body, bodySize);

    if (checkBodyValid(p_body, bodySize) != ERR_OK)
    {
        LOG_E("Invalid body data.\n");
        return ERR_BAD_PARAM;
    }

    parseVerAndFlags(p_bodyBuff, bodySize);
    p_bodyBuff += 4;

    if (m_version == 1)
    {
        BEBytes2Uint64(p_bodyBuff, m_creationTime);
        p_bodyBuff += 8;

        BEBytes2Uint64(p_bodyBuff, m_modificationTime);
        p_bodyBuff += 8;

        BEBytes2Uint32(p_bodyBuff, m_timescale);
        p_bodyBuff += 4;

        BEBytes2Uint64(p_bodyBuff, m_duration);
        p_bodyBuff += 8;
    }
    else if (m_version == 0)
    {
        uint32_t creation_time = 0;
        uint32_t modification_time = 0;
        uint32_t duration = 0;

        BEBytes2Uint32(p_bodyBuff, creation_time);
        m_creationTime = creation_time;
        p_bodyBuff += 4;

        BEBytes2Uint32(p_bodyBuff, modification_time);
        m_modificationTime = modification_time;
        p_bodyBuff += 4;

        BEBytes2Uint32(p_bodyBuff, m_timescale);
        p_bodyBuff += 4;

        BEBytes2Uint32(p_bodyBuff, duration);
        m_duration = duration;
        p_bodyBuff += 4;
    }

    m_language[0] = (p_bodyBuff[0] & 0x7C) >> 2;
    m_language[0] += 0x60;

    m_language[1] = ((p_bodyBuff[0] & 0x03) << 3) | ((p_bodyBuff[1] & 0xE0 ) >> 5);
    m_language[1] += 0x60;

    m_language[2] = p_bodyBuff[1] & 0x1F;
    m_language[2] += 0x60;

    return ERR_OK;
}

int MdhdBox::showBody()
{
    showVerFlags();
    HSP;cout << "creation_time:" << m_creationTime << endl;
    HSP;cout << "modification_time:" << m_modificationTime << endl;
    HSP;cout << "timescale:" << m_timescale << endl;
    HSP;cout << "duration:" << m_duration << endl;
    HSP;cout << "language:'" << m_language << "'" << endl;

    return ERR_OK;
}

}//namespace mp4Parser
