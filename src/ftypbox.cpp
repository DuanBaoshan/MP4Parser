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

FtypBox::FtypBox(uint32_t boxSize, const BoxType_t & boxType):Box(boxSize, boxType),m_compatibleBrands()
{
    m_bodyMinSize = 8;

    memset(m_majorBrand, 0, sizeof(m_majorBrand));
    memset(m_minorVersion, 0, sizeof(m_minorVersion));
}

int FtypBox::parseBody(uint8_t *p_body, uint32_t bodySize)
{
    uint32_t startPos = 0;

    if (checkBodyValid(p_body, bodySize) != ERR_OK)
    {
        LOG_E("Invalid body data.\n");
        return ERR_BAD_PARAM;
    }
	
	DBG_PrintU8Buffer("FTYP Body", p_body, bodySize);

    startPos = 0;
    for (int i = 0; i < 4; i++)
    {
        m_majorBrand[i] = p_body[startPos + i];
    }

    startPos += 4;
    for (int i = 0; i < 4; i++)
    {
        m_minorVersion[i] = p_body[startPos + i];
    }

    startPos += 4;
    if (bodySize > startPos)
    {
        m_compatibleBrands.reserve(bodySize - startPos);

        for (uint32_t i = 0; i < bodySize - startPos; i++)
        {
            m_compatibleBrands.push_back(p_body[startPos + i]);
        }
    }

    return ERR_OK;
}

int FtypBox::showBody()
{
    HSP;cout << "major_brand:'" << m_majorBrand << "'" << endl;
    HSP;cout << "minor_version:'" << m_minorVersion << "'" << endl;

    if (m_compatibleBrands.size() > 0)
    {
        HSP;cout << "compatible_brands:'";
        for (size_t i = 0; i < m_compatibleBrands.size(); i++)
        {
            cout << m_compatibleBrands[i];
        }
        cout << "'" << endl;
    }

    return ERR_OK;
}

FtypBox::~FtypBox()
{
    //LOG_V("Free ftyp body.\n");
}

}//namespace mp4Parser
