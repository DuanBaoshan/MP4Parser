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

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <iomanip>
#include <fstream>

#include "mp4box.h"
#include "boxfactory.h"

#include "errvalue.h"

#ifndef _DEBUG_LEVEL_
#define _DEBUG_LEVEL_  _DEBUG_LEVEL_I_
#endif

#include "debug.h"

using namespace std;

namespace mp4Parser{

std::string Box::m_srcFilePath = "";

//=====================================================
//                   Box
//=====================================================

Box::Box(uint32_t boxSize, const BoxType_t & boxType, bool unknown):
         m_size(boxSize),
         m_largeSize(0),
         m_userType(),
         m_version(0),
         m_flags(0),
         m_container(),
         m_nextSibling(),
         m_body(),
         m_bodySize(0),
         m_actualBodySize(0),
         m_bodyMinSize(0),
         m_boxList(),
         m_level(0),
         m_pos(0),
         m_unknown(unknown)
{
    //ctor
    memset(m_type, 0, sizeof(m_type));
    memcpy(m_type, boxType, sizeof(BoxType_t));
}

Box::~Box()
{
    //LOG_V("Destroy box:'%s'.\n", m_type);

    //dtor
}

int  Box::checkBodyValid(uint8_t *p_body, uint32_t bodySize)
{
    if (p_body == nullptr)
    {
        LOG_E("body is null.\n");
        return ERR_BAD_PARAM;
    }

    if (bodySize < m_bodyMinSize)
    {
        LOG_E("bodySize(%u) is too small, the min size is:%u, .\n", bodySize, m_bodyMinSize);
        return ERR_BAD_PARAM;
    }

    return ERR_OK;
}

int Box::parseBody(uint8_t *p_body, uint32_t bodySize)
{
    //LOG_V("Parse body of box:'%s', only copy data.\n", m_type);

    if (p_body == nullptr)
    {
        LOG_E("Body is NULL.\n");
        return ERR_BAD_PARAM;
    }

	uint8_t *p_newBuffer = new uint8_t[bodySize];
	if (p_newBuffer == nullptr)
	{
		LOG_E("Fail to allocate buffer.\n");
		return ERR_OUT_MEM;
	}

	memcpy(p_newBuffer, p_body, bodySize);

	m_body.reset(new BoxBody(p_newBuffer, bodySize));
	if (!m_body)
	{
		LOG_E("Fail to create box body.\n");

		delete [] p_newBuffer;
		return ERR_FAIL;
	}

    m_bodySize = bodySize;
    m_bodyMinSize = m_bodySize;

    return ERR_OK;
}

int Box::setActualBodySize(uint32_t actualBodySize)
{
    m_actualBodySize = actualBodySize;

    return ERR_OK;
}

int Box::show()
{
    HSP;cout << endl;
    if (m_unknown)
    {
        HSP;cout << "+++++++++++" << m_type << "(unknown) begin++++++++++++++" << endl;
    }
    else
    {
        HSP;cout << "+++++++++++" << m_type << " begin++++++++++++++" << endl;
    }

    HSP;cout << "size:" << m_size << endl;
    HSP;cout << "type:'" << m_type << "'" << endl;

    if (strcmp(m_type, "ftyp") == 0 || m_pos > 0)
    {
        HSP;cout << "file position:" << m_pos << endl;
    }

    if (container().get() != nullptr)
    {
        HSP;cout << "container:" << container()->m_type << endl;
    }
    else
    {
        HSP;cout <<  "container:file" << endl;
    }

    if (nextSibling())
    {
        HSP;cout <<  "next_sibling:" << nextSibling()->m_type << endl;
    }
    else
    {
        HSP;cout <<  "next_sibling:null" << endl;
    }

    HSP;cout <<  "body:" << endl;
    showBody();

    if (m_boxList.length() > 0)
    {
        HSP;cout <<  "Has " << m_boxList.length() << " sub box." << endl;

        for (uint64_t i = 0; i < m_boxList.length(); i++)
        {
            HSP;cout <<  "sub box " << i + 1 << ":" << endl;
            m_boxList.show(i);
        }

    }
    HSP;cout <<  "-----------" << m_type << " end--------------" << endl;

    return ERR_OK;
}

int Box::showBody()
{
    if (m_body)
    {
        printRawBuff(m_body->p_body, m_bodySize, m_actualBodySize);
    }
    return ERR_OK;
}

int  Box::parseBodyAndAppendChild(uint8_t *p_body, uint32_t bodySize)
{
    int ret = 0;
    uint32_t totalLen = 0;

    uint32_t boxSize = 0;
    BoxType_t boxType;

    uint8_t *p_bodyBuff = p_body;
    if (p_body == nullptr)
    {
        LOG_E("Body is null.\n");
        return ERR_BAD_PARAM;
    }

    while (totalLen < bodySize)
    {
        boxSize = (p_bodyBuff[0] << 24) | (p_bodyBuff[1] << 16) | (p_bodyBuff[2] << 8) | p_bodyBuff[3];
        if (boxSize < 8)
        {
            LOG_E("Invalid boxSize:%u.\n", boxSize);
            return ERR_BAD_PARAM;
        }

        memset(boxType, 0, sizeof(boxType));
        memcpy(boxType, p_bodyBuff + 4, 4);

        Box *p_box = BoxFactory::createBox(boxSize, boxType);
        if (p_box == nullptr)
        {
            LOG_E("Fail to create box:%s.\n", boxType);
            return ERR_OUT_MEM;
        }

        p_box->m_container = shared_from_this();
        p_box->m_level = m_level + 1;

        BoxPtr_t box(p_box);
        if (boxSize > 8)
        {
            ret = box->parseBody(p_bodyBuff + 8, boxSize - 8);
            if (ret != ERR_OK)
            {
                LOG_E("Fail to parse body of:%s.\n", boxType);
                return ERR_FAIL;
            }
        }

        m_boxList.insertBox(box);

        totalLen += boxSize;
        p_bodyBuff += boxSize;
    }

    return ERR_OK;
}

int  Box::appendChild(BoxPtr_t &box)
{
    box->m_container = shared_from_this();
    box->m_level = m_level + 1;

    m_boxList.insertBox(box);

    return ERR_OK;
}

bool Box::hasChildren()
{
    return (m_boxList.length() > 0) ? true : false;
}

BoxPtr_t Box::firstChild()
{
    if (!hasChildren())
    {
        return BoxPtr_t();
    }

    return m_boxList.getFirstBox();
}

BoxPtr_t Box::findBoxInChildren(const char* p_boxType)
{
    if (!hasChildren())
    {
        return BoxPtr_t();
    }

    return m_boxList.getBox(p_boxType);
}

BoxPtr_t Box::findNextBoxInChildren(const BoxPtr_t& curBox)
{
    if (!hasChildren())
    {
        return BoxPtr_t();
    }

    return m_boxList.getNextBox(curBox);
}

int Box::getBoxCountInChildren(const char *p_boxType)
{
    if (!hasChildren())
    {
        return 0;
    }

    return m_boxList.getBoxCount(p_boxType);
}

int  Box::parseVerAndFlags(uint8_t *p_body, uint32_t bodySize)
{
    uint8_t *p_bodyBuff = p_body;

    if (p_body == nullptr)
    {
        LOG_E("Invalid body.\n");
        return ERR_BAD_PARAM;
    }

    if (bodySize < 4)
    {
        LOG_E("Invalid body size.\n");
        return ERR_BAD_PARAM;
    }

    m_version = p_bodyBuff[0];
    p_bodyBuff += 1;

    BEBytes2Flags(p_bodyBuff, m_flags);

    return ERR_OK;
}

void Box::insertOutputSpace()
{
    for (int i = 0; i < m_level; i++)
    {
        cout << "    ";
    }

    cout << "|";

	return;
}

void Box::printRawBuff(uint8_t *p_buffer, uint32_t buffSize, uint32_t actualSize)
{
    int value = 0;

    HSP;cout <<  "total size:" << ((actualSize > 0) ? actualSize : buffSize) << endl;
    for (uint32_t i = 0; i < buffSize; i++)
    {
        value = p_buffer[i];
        if (i == 0)
        {
            HSP;
        }
        else if ((i % 8) == 0)
        {
            cout << endl;
            HSP;
        }

        cout << _HEX_2_DIGIT_ << value << " ";
    }

    cout << dec << endl;

    if (buffSize < actualSize)
    {
        HSP;cout <<  "....  ....  ....  ...." << endl;
    }

    return ;
}

void Box::showMatrix(BoxMatrix_t &matrix)
{
    HSP;cout <<  "matrix:" << endl;
    for (int i = 0; i < 9; i += 3)
    {
        if (i != 0 && (i % 3) == 0)
        {
            cout << endl;
        }

        HSP;cout << _HEX_8_DIGIT_ << matrix[i] << "    " << _HEX_8_DIGIT_ << matrix[i + 1] << "    " << _HEX_8_DIGIT_ << matrix[i + 2];
    }

    cout << dec << endl;

    return;
}
void Box::showVerFlags()
{
    int version = static_cast<int>(m_version);
    HSP;cout <<  "version:" << version << endl;
    HSP;cout <<  "flags:" << _HEX_DIGIT_ << m_flags << dec << endl;

    return;
}
//=====================================================
//                 BoxContainer
//=====================================================
BoxContainer::BoxContainer(uint32_t boxSize, const BoxType_t & boxType):Box(boxSize, boxType)
{
}

BoxContainer::~BoxContainer()
{

}

int BoxContainer::parseBody(uint8_t *p_body, uint32_t bodySize)
{
    return parseBodyAndAppendChild(p_body, bodySize);
}


}//namespace mp4Parser
