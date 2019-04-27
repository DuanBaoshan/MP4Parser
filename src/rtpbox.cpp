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
#include <stdio.h>

#include "mp4box.h"

#ifndef _DEBUG_LEVEL_
#define _DEBUG_LEVEL_  _DEBUG_LEVEL_I_
#endif

//#define _DEBUG_BUFFER_ON_
#include "debug.h"

using namespace std;

namespace mp4Parser
{

RtpBox::RtpBox(uint32_t boxSize, const BoxType_t & boxType):Box(boxSize, boxType), m_sdpText()
{
    //ctor
    m_bodyMinSize = 4;

    memset(m_descriptionFormat, 0, sizeof(m_descriptionFormat));
}

RtpBox::~RtpBox()
{
    //dtor
}

int RtpBox::parseBody(uint8_t *p_body, uint32_t bodySize)
{
    uint8_t *p_bodyBuff = p_body;

    if (checkBodyValid(p_body, bodySize) != ERR_OK)
    {
        LOG_E("Invalid body data.\n");
        return ERR_BAD_PARAM;
    }

    DBG_PrintU8Buffer("RTP Body", p_body, bodySize);

    for (int i = 0; i < 4; i++)
    {
        m_descriptionFormat[i] = p_bodyBuff[i];
    }

    p_bodyBuff += 4;
    if (p_bodyBuff < p_body + bodySize)
    {
        int length = p_body + bodySize - p_bodyBuff;

        m_sdpText.reserve(length);
        for (int i = 0; i < length; i++)
        {
            m_sdpText.push_back(p_bodyBuff[i]);
        }
    }

    return ERR_OK;
}

int RtpBox::showBody()
{
    HSP;cout << "description format:'" << m_descriptionFormat << "'" << endl;
    HSP;cout << "sdp text:'";
    for (uint32_t i = 0; i < m_sdpText.size() && m_sdpText[i] != 0x0D; i++)
    {
        cout << m_sdpText[i];
    }
    cout << endl;

    return ERR_OK;
}

}//namespace mp4Parser
