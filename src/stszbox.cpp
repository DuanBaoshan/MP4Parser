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

StszBox::StszBox(uint32_t boxSize, const BoxType_t & boxType):Box(boxSize, boxType),
        m_sampleSize(0),
        m_sampleCount(0),
        m_entrySizeList()
{
    //ctor
    m_bodyMinSize = 12;
}

StszBox::~StszBox()
{
    //dtor
}

int StszBox::parseBody(uint8_t *p_body, uint32_t bodySize)
{
    uint8_t *p_bodyBuff = p_body;

    DBG_PrintU8Buffer("STSZ body", p_body, bodySize);

    if (checkBodyValid(p_body, bodySize) != ERR_OK)
    {
        LOG_E("Invalid body data.\n");
        return ERR_BAD_PARAM;
    }

    parseVerAndFlags(p_bodyBuff, bodySize);
    p_bodyBuff += 4;

    BEBytes2Uint32(p_bodyBuff, m_sampleSize);
    p_bodyBuff += 4;

    BEBytes2Uint32(p_bodyBuff, m_sampleCount);
    p_bodyBuff += 4;

    if (m_sampleSize == 0)
    {
        if (m_sampleCount > 0)
        {
            m_entrySizeList.reserve(m_sampleCount);

            for (uint32_t i = 0; i < m_sampleCount && p_bodyBuff + 4 <= p_body + bodySize; i++)
            {
                uint32_t entrySize = 0;
                BEBytes2Uint32(p_bodyBuff, entrySize);
                p_bodyBuff += 4;

                m_entrySizeList.push_back(entrySize);
            }
        }
    }

    return ERR_OK;
}

int StszBox::showBody()
{
    showVerFlags();
    HSP;cout << "sample_size:" << m_sampleSize << endl;

    if (m_sampleSize == 0)
    {
        HSP;cout << "Has " << m_sampleCount << " entry_size." << endl;
        for (uint32_t i = 0; i < m_sampleCount &&  i < m_entrySizeList.size(); i++)
        {
            HSP;cout << "entry_size[" << i << "]:" << m_entrySizeList[i] << endl;
        }
    }

    return ERR_OK;
}

}//namespace mp4Parser
