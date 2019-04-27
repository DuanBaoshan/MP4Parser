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

AudioSampleEntryBox::AudioSampleEntryBox(uint32_t boxSize, const BoxType_t & boxType):SampleEntryBox(boxSize, boxType),
                    m_channelCount(0),
                    m_sampleSize(0),
                    m_sampleRate(0)
{
    //ctor
    m_bodyMinSize = 20;
}

AudioSampleEntryBox::~AudioSampleEntryBox()
{
    //dtor
}

int AudioSampleEntryBox::parseBody(uint8_t *p_body, uint32_t bodySize)
{
    uint8_t *p_bodyBuff = p_body;

    if (checkBodyValid(p_body, bodySize) != ERR_OK)
    {
        LOG_E("Invalid body data.\n");
        return ERR_BAD_PARAM;
    }

    DBG_PrintU8Buffer("ASE Body", p_body, bodySize);

    p_bodyBuff += 6;//skip const unsigned int(8)[6] reserved = 0;

    BEBytes2Uint16(p_bodyBuff, m_dataReferenceIndex);
    p_bodyBuff += 2;

    p_bodyBuff += 8;//skip const unsigned int(32)[2] reserved = 0;


    BEBytes2Uint16(p_bodyBuff, m_channelCount);
    p_bodyBuff += 2;

    BEBytes2Uint16(p_bodyBuff, m_sampleSize);
    p_bodyBuff += 2;

    p_bodyBuff += 2;//skip unsigned int(16) pre_defined = 0;
    p_bodyBuff += 2;//skip const unsigned int(16) reserved = 0

    BEBytes2Uint32(p_bodyBuff, m_sampleRate);
    p_bodyBuff += 4;

    //Has sub box
    if (p_bodyBuff < p_body + bodySize)
    {
        BoxContainer::parseBody(p_bodyBuff, p_body + bodySize - p_bodyBuff);
    }

    return ERR_OK;
}

int AudioSampleEntryBox::showBody()
{
    HSP;cout <<  "data_reference_index:" << m_dataReferenceIndex << endl;
    HSP;cout <<  "channel_count:"<< m_channelCount << endl;
    HSP;cout <<  "sample_size:" << m_sampleSize << endl;
    HSP;cout <<  "sample_rate:" << m_sampleRate << endl;
    return ERR_OK;
}

}//namespace mp4Parser
