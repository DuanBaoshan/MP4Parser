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

VisualSampleEntryBox::VisualSampleEntryBox(uint32_t boxSize, const BoxType_t & boxType):SampleEntryBox(boxSize, boxType),
                       m_width(0),
                       m_height(0),
                       m_horizResolution(0),
                       m_vertResolution(0),
                       m_frameCount(0),
                       m_compressorName(""),
                       m_depth(0)
{
    //ctor
    m_bodyMinSize = 80;
}

VisualSampleEntryBox::~VisualSampleEntryBox()
{
    //dtor
}

int VisualSampleEntryBox::parseBody(uint8_t *p_body, uint32_t bodySize)
{
    uint8_t *p_bodyBuff = p_body;

    if (checkBodyValid(p_body, bodySize) != ERR_OK)
    {
        LOG_E("Invalid body data.\n");
        return ERR_BAD_PARAM;
    }

    DBG_PrintU8Buffer("VSE Body", p_body, bodySize);

    p_bodyBuff += 6;//skip const unsigned int(8)[6] reserved = 0;

    BEBytes2Uint16(p_bodyBuff, m_dataReferenceIndex);
    p_bodyBuff += 2;

    p_bodyBuff += 2;//skip unsigned int(16) pre_defined = 0;
    p_bodyBuff += 2;//skip const unsigned int(16) reserved = 0;
    p_bodyBuff += 12;//skip unsigned int(32)[3] pre_defined = 0;

    BEBytes2Uint16(p_bodyBuff, m_width);
    p_bodyBuff += 2;

    BEBytes2Uint16(p_bodyBuff, m_height);
    p_bodyBuff += 2;

    BEBytes2Uint32(p_bodyBuff, m_horizResolution);
    p_bodyBuff += 4;

    BEBytes2Uint32(p_bodyBuff, m_vertResolution);
    p_bodyBuff += 4;

    p_bodyBuff += 4;//skip const unsigned int(32) reserved = 0;

    BEBytes2Uint16(p_bodyBuff, m_frameCount);
    p_bodyBuff += 2;

    for (int i = 0; i < 32; i++)
    {
        m_compressorName += *p_bodyBuff;
        p_bodyBuff++;
    }

    BEBytes2Uint16(p_bodyBuff, m_depth);
    p_bodyBuff += 2;

    p_bodyBuff += 2;//skip int(16) pre_defined = -1;

    //Has sub box
    if (p_bodyBuff < p_body + bodySize)
    {
        BoxContainer::parseBody(p_bodyBuff, p_body + bodySize - p_bodyBuff);
    }

    return ERR_OK;
}

int VisualSampleEntryBox::showBody()
{
    HSP;cout << "data_reference_index:" << m_dataReferenceIndex << endl;
    HSP;cout << "width:" << m_width << endl;
    HSP;cout << "height:" << m_height << endl;
    HSP;cout << "horiz_resolution:" << _HEX_8_DIGIT_ << m_horizResolution << endl;
    HSP;cout << "vert_resolution:" << _HEX_8_DIGIT_ << m_vertResolution << endl << dec;
    HSP;cout << "frame_count:" << m_frameCount << endl;
    HSP;cout << "compressor_name:'" << m_compressorName.c_str() << "'" << endl;
    HSP;cout << "depth:" << m_depth << endl;

    return ERR_OK;
}

}//namespace mp4Parser
