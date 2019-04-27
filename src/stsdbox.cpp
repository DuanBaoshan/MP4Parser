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

#include "boxfactory.h"

#ifndef _DEBUG_LEVEL_
#define _DEBUG_LEVEL_  _DEBUG_LEVEL_I_
#endif

//#define _DEBUG_BUFFER_ON_
#include "debug.h"

using namespace std;

namespace mp4Parser
{

StsdBox::StsdBox(uint32_t boxSize, const BoxType_t & boxType):Box(boxSize, boxType),
         m_handlerType(INVALID_HANDLER),
         m_entryCount(0)
{
    //ctor
    m_bodyMinSize = 8;
}

StsdBox::~StsdBox()
{
    //dtor
}

int StsdBox::parseBody(uint8_t *p_body, uint32_t bodySize)
{
    BoxPtr_t box;
    uint8_t *p_bodyBuff = p_body;

    if (checkBodyValid(p_body, bodySize) != ERR_OK)
    {
        LOG_E("Invalid body data.\n");
        return ERR_BAD_PARAM;
    }

    DBG_PrintU8Buffer("STSD Body", p_body, bodySize);

    m_handlerType = findHandlerType();

    parseVerAndFlags(p_bodyBuff, bodySize);
    p_bodyBuff += 4;

    BEBytes2Uint32(p_bodyBuff, m_entryCount);
    p_bodyBuff += 4;

    for (uint32_t i = 0; i < m_entryCount; i++)
    {
        uint32_t boxSize = 0;
        BoxType_t boxType;

        BEBytes2Uint32(p_bodyBuff, boxSize);
        p_bodyBuff += 4;

        if (boxSize < 8)
        {
            LOG_E("Invalid box data, size(%u) is too small.\n", boxSize);
            return ERR_FAIL;
        }

        for (int j = 0; j < 4; j++)
        {
            boxType[j] = p_bodyBuff[j];
        }

        boxType[4] = '\0';
        p_bodyBuff += 4;

        switch (m_handlerType)
        {
        case HANDLER_VIDE:
        {
            //LOG_V("Is video handler.\n");

            box.reset(new VisualSampleEntryBox(boxSize, boxType));
            if (!box)
            {
                LOG_E("Fail to create VisualSampleEntryBox.\n");
                return ERR_FAIL;
            }
        }
        break;
        case HANDLER_SOUD:
        {
            //LOG_V("Is audio handler.\n");

            box.reset(new AudioSampleEntryBox(boxSize, boxType));
            if (!box)
            {
                LOG_E("Fail to create AudioSampleEntryBox.\n");
                return ERR_FAIL;
            }
        }
        break;
        case HANDLER_HINT:
        {
            //LOG_V("Is hint handler.\n");

            box.reset(new HintSampleEntryBox(boxSize, boxType));
            if (!box)
            {
                LOG_E("Fail to create HintSampleEntryBox.\n");
                return ERR_FAIL;
            }
        }
        break;
        default:
            //LOG_W("Other handle:'%s'.\n", HdlrBox::HANDLER_TYPE_MAP[m_handlerType].c_str());
            box.reset(new SampleEntryBox(boxSize, boxType));
            break;
        }

        if (box)
        {
            box->setLevel(level() + 1);
            box->parseBody(p_bodyBuff, boxSize - 8);
            appendChild(box);
            p_bodyBuff += boxSize - 8;
        }
    }

    return ERR_OK;
}

int StsdBox::showBody()
{
    showVerFlags();
    HSP;cout << "Handler is:'" << HdlrBox::HANDLER_TYPE_MAP[m_handlerType] << "'" << endl;
    HSP;cout << "entry_count:" << m_entryCount << endl;
    return ERR_OK;
}

HandlerType_e StsdBox::findHandlerType()
{
    BoxPtr_t boxContainer = container();

    while (boxContainer.get() != nullptr)
    {
        BoxPtr_t box = boxContainer->findBoxInChildren(BoxFactory::HDLR_BOX);
        if (box.get() != nullptr)
        {
            uint32_t handlerType = dynamic_pointer_cast<HdlrBox>(box)->handlerType();
            return static_cast<HandlerType_e>(handlerType);
        }

        boxContainer = boxContainer->container();
    }

    return INVALID_HANDLER;
}

}//namespace mp4Parser
