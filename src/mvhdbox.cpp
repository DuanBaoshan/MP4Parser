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

MvhdBox::MvhdBox(uint32_t boxSize, const BoxType_t & boxType):Box(boxSize, boxType),
    m_creationTime(0),
    m_modificationTime(0),
    m_timescale(0),
    m_duration(0),
    m_rate(0),
    m_volume(0),
    m_nextTrackId(0)
{
    //ctor
    m_bodyMinSize = 100;

    memset(m_matrix, 0, sizeof(BoxMatrix_t));
}

MvhdBox::~MvhdBox()
{
    //dtor
}

int MvhdBox::parseBody(uint8_t *p_body, uint32_t bodySize)
{
    uint8_t *p_bodyBuff = p_body;

    if (checkBodyValid(p_body, bodySize) != ERR_OK)
    {
        LOG_E("Invalid body data.\n");
        return ERR_BAD_PARAM;
    }

    DBG_PrintU8Buffer("MVHD body", p_body, bodySize);

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

    BEBytes2Int32(p_bodyBuff, m_rate);
    p_bodyBuff += 4;

    BEBytes2Int16(p_bodyBuff, m_volume);
    p_bodyBuff += 2;

    p_bodyBuff += 2;//skip const bit(16) reserved = 0
    p_bodyBuff += 8;//skip const unsigned int(32)[2] reserved = 0

    for (int i = 0; i < 9; i++)
    {
        BEBytes2Int32(p_bodyBuff, m_matrix[i]);
        p_bodyBuff += 4;
    }

    p_bodyBuff += 24;//skip bit(32)[6] pre_defined = 0

    BEBytes2Uint32(p_bodyBuff, m_nextTrackId);

    return ERR_OK;
}

int MvhdBox::showBody()
{
    showVerFlags();

    HSP;cout << "creation_time:" << _HEX_DIGIT_ << m_creationTime << endl;
    HSP;cout << "modification_time:" <<  _HEX_DIGIT_ << m_modificationTime << endl;
    HSP;cout << dec << "timescale:" << m_timescale << endl;
    HSP;cout << "duration:" << m_duration << endl;
    HSP;cout << "rate:" << _HEX_8_DIGIT_ << m_rate << endl;
    HSP;cout << "volume:" << _HEX_4_DIGIT_ << m_volume << endl;

    showMatrix(m_matrix);

    HSP;cout << dec << "next_track_id:" << m_nextTrackId << endl;

    return ERR_OK;
}

}//namespace mp4Parser
