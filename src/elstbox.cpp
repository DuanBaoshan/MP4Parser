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

ElstBox::ElstBox(uint32_t boxSize, const BoxType_t & boxType):Box(boxSize, boxType),
        m_entryCount(0),
        m_editSegmentList()
{
    //ctor
    m_bodyMinSize = 16;
}

ElstBox::~ElstBox()
{
    //dtor
}

int ElstBox::parseBody(uint8_t *p_body, uint32_t bodySize)
{
    uint8_t *p_bodyBuff = p_body;

    LOG_V("Parse elst body.\n");

    if (checkBodyValid(p_body, bodySize) != ERR_OK)
    {
        LOG_E("Invalid body data.\n");
        return ERR_BAD_PARAM;
    }

    DBG_PrintU8Buffer("ELST body", p_body, bodySize);

    parseVerAndFlags(p_bodyBuff, bodySize);
    p_bodyBuff += 4;

    BEBytes2Uint32(p_bodyBuff, m_entryCount);
    p_bodyBuff += 4;

    for (uint32_t i = 0; i < m_entryCount; i++)
    {
        EditSegment_t editSeg;

        if (m_version == 1)
        {
            BEBytes2Uint64(p_bodyBuff, editSeg.segmentDuration);
            p_bodyBuff += 8;

            BEBytes2Int64(p_bodyBuff, editSeg.mediaTime);
            p_bodyBuff += 8;
        }
        else
        {
            uint32_t segmentDuration = 0;
            BEBytes2Uint32(p_bodyBuff, segmentDuration);
            editSeg.segmentDuration = segmentDuration;
            p_bodyBuff += 4;

            int32_t mediaTime = 0;
            BEBytes2Int32(p_bodyBuff, mediaTime);
            editSeg.mediaTime = mediaTime;
            p_bodyBuff += 4;
        }

        BEBytes2Int16(p_bodyBuff, editSeg.mediaRateInteger);
        p_bodyBuff += 2;

        BEBytes2Int16(p_bodyBuff, editSeg.mediaRateFraction);
        p_bodyBuff += 2;

        m_editSegmentList.push_back(editSeg);
    }

    return ERR_OK;
}

int ElstBox::showBody()
{
    showVerFlags();
    HSP;cout << "Has " << m_entryCount << " edit segments." << endl;
    for (uint32_t i = 0; i < m_entryCount; i++)
    {
        HSP;cout << "edit_segment[" << i << "]:(";
        cout << "segment_duration:" << m_editSegmentList[i].segmentDuration;
        cout << ", media_time:" << m_editSegmentList[i].mediaTime;
        cout << ", media_rate_integer:" << m_editSegmentList[i].mediaRateInteger;
        cout << ", media_rate_fraction:" << m_editSegmentList[i].mediaRateFraction << ")" << endl;
    }

    return ERR_OK;
}

}//namespace mp4Parser
