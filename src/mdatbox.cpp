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
#define _DEBUG_LEVEL_ _DEBUG_LEVEL_I_
#endif // _DEBUG_LEVEL_

//#define _DEBUG_BUFFER_ON_
#include "debug.h"

using namespace std;

namespace mp4Parser{

MdatBox::MdatBox(uint32_t boxSize, const BoxType_t & boxType):Box(boxSize, boxType),m_actualBody()
{
    //ctor
    m_bodyMinSize = 1;
}

MdatBox::~MdatBox()
{
    //dtor
}

int MdatBox::getActualBody(BoxBodyPtr_t& body)
{
	if (m_actualBodySize == 0)
	{
		return ERR_FAIL;
	}

	if (m_actualBody)
	{
	    body = m_actualBody;
		return ERR_OK;
	}

	if (m_srcFilePath.empty())
	{
		LOG_E("Has no source file path.\n");
		return ERR_BAD_PARAM;
	}

	uint8_t *p_body = new uint8_t[m_actualBodySize];
	if (p_body == nullptr)
	{
		LOG_E("Not enough memory.\n");
		return ERR_OUT_MEM;
	}

	FILE *p_fd = fopen(m_srcFilePath.c_str(), "rb");
	if (p_fd == nullptr)
	{
		LOG_E("Fail to open file:'%s'.\n", m_srcFilePath.c_str());
		delete [] p_body;

		return ERR_FAIL;
	}

    int headerSize = 8;
	fseek(p_fd, m_pos + headerSize, SEEK_SET);
	size_t ret = fread(p_body, sizeof(uint8_t), m_actualBodySize - headerSize, p_fd);
	if (ret != m_actualBodySize - headerSize)
	{
		LOG_E("Fail to read data.\n");

		delete []p_body;
		fclose(p_fd);

		return ERR_FAIL;
	}

	fclose(p_fd);

	m_actualBody.reset(new BoxBody(p_body, m_actualBodySize));
	if (!m_actualBody)
	{
		LOG_E("Fail to allocate BoxBody.\n");
		delete []p_body;;
		return ERR_FAIL;
	}

	body = m_actualBody;

	return ERR_OK;
}


}//namespace mp4Parser
