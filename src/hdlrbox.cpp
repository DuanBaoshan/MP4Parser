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
std::map<HandlerType_e, std::string> HdlrBox::HANDLER_TYPE_MAP = {
    {mp4Parser::INVALID_HANDLER, "INVALID"},
    {mp4Parser::HANDLER_VIDE, "VIDE"},
    {mp4Parser::HANDLER_SOUD, "SOUND"},
    {mp4Parser::HANDLER_HINT, "HINT"},
    {mp4Parser::HANDLER_SDSM, "SDSM"},
    {mp4Parser::HANDLER_ODSM, "ODSM"},
    };

HdlrBox::HdlrBox(uint32_t boxSize, const BoxType_t & boxType):Box(boxSize, boxType),
         m_handlerType(mp4Parser::INVALID_HANDLER),
         m_name()
{
    //ctor
    m_bodyMinSize = 24;
}

HdlrBox::~HdlrBox()
{
    //dtor
}

int HdlrBox::parseBody(uint8_t *p_body, uint32_t bodySize)
{
    uint8_t *p_bodyBuff = p_body;

    if (checkBodyValid(p_body, bodySize) != ERR_OK)
    {
        LOG_E("Invalid body data.\n");
        return ERR_BAD_PARAM;
    }

    DBG_PrintU8Buffer("HDLR Body", p_body, bodySize);

    parseVerAndFlags(p_bodyBuff, bodySize);
    p_bodyBuff += 4;

    p_bodyBuff += 4;//skip unsigned int(32) pre_defined = 0

    BEBytes2Uint32(p_bodyBuff, m_handlerType);
    p_bodyBuff += 4;

    p_bodyBuff += 12;//skip const unsigned int(32)[3] reserved = 0

    while (p_bodyBuff < (p_body + bodySize) && *p_bodyBuff != 0)
    {
        m_name += *p_bodyBuff;
        p_bodyBuff++;
    }

    return ERR_OK;
}

int HdlrBox::showBody()
{
    showVerFlags();
    HSP;cout << "handler_type:'" << HANDLER_TYPE_MAP[(HandlerType_e)m_handlerType] << "', " _HEX_DIGIT_ << m_handlerType << dec << endl;
    HSP;cout << "name:'" << m_name.c_str() << "'" << endl;

    return ERR_OK;
}

}//namespace mp4Parser
