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
#include <iostream>

#include "mp4parser.h"
#include "utils.h"

#ifndef _DEBUG_LEVEL_
#define _DEBUG_LEVEL_  _DEBUG_LEVEL_I_
#endif

//#define _DEBUG_BUFFER_ON_
#include "debug.h"

using namespace std;


namespace mp4Parser{

class MP4ParserImpl
{
public:
    MP4ParserImpl();
    ~MP4ParserImpl();

    int parse(const std::string &srcFileName);

	int outputBoxList();

    int getBoxCount(const char* p_boxType) const;

    mp4Parser::BoxPtr_t getBox(const char* p_boxType) const;

    mp4Parser::BoxPtr_t getNextBox(const mp4Parser::BoxPtr_t& curBox) const;

    mp4Parser::BoxPtr_t getFirstBox() const;

    bool exists(const char* p_boxType) const;

    int traverse(mp4Parser::TraverseFunc_t traverseFunc, void *p_userData);

    int clear();

	std::string srcFile();

	std::string dstFile();
	int setDstFile(const std::string &outputFilePath);

	int redirectOut2File();
    int restore2Stdout();

private:
    int innerParse();
    int parseFtyp();
    int parseOtherBoxs();
    int parseBox(uint32_t boxSize, mp4Parser::BoxType_t &boxType, mp4Parser::BoxList &boxList);

    bool isRawDataBox(const mp4Parser::BoxType_t &boxType);

    int getBoxSize(uint32_t &boxSize);
    int getBoxType(mp4Parser::BoxType_t &boxType);

    void getFileLength();

    int outputTrackInfo();
    int outputMoofInfo();

    std::string m_srcFileName;
    FILE *m_pSrcFd;

    bool m_isRedirecting;
    std::string m_outputFileName;
	streambuf *m_pOrgCoutStreams;
	fstream    m_outFileStream;

    off64_t m_pos;
    off64_t m_fileLength;

    mp4Parser::BoxList m_boxList;
};

/*****************************************************************
 *                MP4ParserImpl Implemetion
 *****************************************************************/
MP4ParserImpl::MP4ParserImpl():
    m_srcFileName(""),
    m_pSrcFd(nullptr),
    m_isRedirecting(false),
    m_outputFileName(""),
    m_pOrgCoutStreams(nullptr),
    m_outFileStream(),
    m_pos(0),
    m_fileLength(0),
    m_boxList()
{
    //ctor
}

MP4ParserImpl::~MP4ParserImpl()
{
    //dtor
    clear();
}

int MP4ParserImpl::parse(const std::string &srcFileName)
{
    int ret = ERR_OK;

    m_pSrcFd = fopen(srcFileName.c_str(), "rb");
    if (m_pSrcFd == nullptr)
    {
        LOG_E("Fail to open file:'%s'.\n", srcFileName.c_str());
        return ERR_FAIL;
    }

    m_srcFileName = srcFileName;

    ret = innerParse();
    if (ret != ERR_OK)
    {
        LOG_E("Fail to parse file:%s.\n", srcFileName.c_str());
    }

    fclose(m_pSrcFd);
    m_pSrcFd = nullptr;
    return ret;
}



int MP4ParserImpl::outputBoxList()
{
	redirectOut2File();
	m_boxList.show();
	restore2Stdout();

    return ERR_OK;
}


int MP4ParserImpl::getBoxCount(const char* p_boxType) const
{
    if (p_boxType == nullptr)
    {
        LOG_E("p_boxType is NULL!\n");
        return ERR_BAD_PARAM;
    }

    return m_boxList.getBoxCount(p_boxType);
}

BoxPtr_t MP4ParserImpl::getBox(const char* p_boxType) const
{
    if (p_boxType == nullptr)
    {
        LOG_E("p_boxType is NULL!\n");
        return BoxPtr_t();
    }

    return m_boxList.getBox(p_boxType);
}

BoxPtr_t MP4ParserImpl::getNextBox(const BoxPtr_t& curBox) const
{
    return m_boxList.getNextBox(curBox);
}

BoxPtr_t MP4ParserImpl::getFirstBox() const
{
    return m_boxList.getFirstBox();
}

bool MP4ParserImpl::exists(const char* p_boxType) const
{
    if (p_boxType == nullptr)
    {
        LOG_E("p_boxType is NULL!\n");
        return false;
    }

    BoxPtr_t box = getBox(p_boxType);

    if (box)
    {
        return true;
    }

    return false;
}

int MP4ParserImpl::traverse(TraverseFunc_t traverseFunc, void *p_userData)
{
    return m_boxList.traverse(traverseFunc, p_userData);
}

int MP4ParserImpl::clear()
{
    m_srcFileName = "";
    m_boxList.clear();
	Box::setSrcFilePath("");

    return ERR_OK;
}

std::string MP4ParserImpl::srcFile()
{
	return m_srcFileName;
}

std::string MP4ParserImpl::dstFile()
{
	return m_outputFileName;
}

int MP4ParserImpl::setDstFile(const std::string &outputFilePath)
{
	m_outputFileName = outputFilePath;
	return ERR_OK;
}

int MP4ParserImpl::redirectOut2File()
{
	if (m_outputFileName.empty())
	{
		return ERR_OK;
	}

	if (m_isRedirecting)
	{
		return ERR_OK;
	}

	m_outFileStream.open(m_outputFileName, ios::out | ios::app);
	m_pOrgCoutStreams = cout.rdbuf();
	cout.rdbuf(m_outFileStream.rdbuf());
	m_isRedirecting = true;

	return ERR_OK;
}

int MP4ParserImpl::restore2Stdout()
{
	if (!m_isRedirecting)
	{
		return ERR_OK;
	}

	m_outFileStream.flush();
	m_outFileStream.close();
	cout.rdbuf(m_pOrgCoutStreams);

	m_isRedirecting = false;
	m_pOrgCoutStreams = nullptr;

	return ERR_OK;
}


int MP4ParserImpl::innerParse()
{
    int ret = 0;

    m_pos = 0;
    getFileLength();
    cout << "File length:" << m_fileLength << endl;

    ret = parseFtyp();
    if (ret != ERR_OK)
    {
        LOG_E("Fail to get ftyp.\n");
        return ret;
    }

    ret = parseOtherBoxs();
    if (ret != ERR_OK)
    {
        LOG_E("Fail to get other boxs.\n");
        return ret;
    }

    return ERR_OK;
}

int MP4ParserImpl::parseFtyp()
{
    uint32_t boxSize = 0;
    BoxType_t boxType;

    int ret = getBoxSize(boxSize);
    if (ret != ERR_OK)
    {
        LOG_E("Fail to read box size");
        return ERR_FAIL;
    }

    //Read box type
    ret = getBoxType(boxType);
    if (ret != ERR_OK)
    {
        LOG_E("Fail to read box type.\n");
        return ERR_FAIL;
    }

    if (strcmp(boxType, BoxFactory::FTYP_BOX) != 0)
    {
        LOG_E("Invalid ftyp flag.\n");
        return ERR_FAIL;
    }

    ret = parseBox(boxSize, boxType, m_boxList);
    if (ret != ERR_OK)
    {
        LOG_E("Fail to parse ftyp box.\n");
    }

    m_pos += boxSize;
    fseek(m_pSrcFd, m_pos, SEEK_SET);
    return ret;
}

int MP4ParserImpl::parseOtherBoxs()
{
    int ret = ERR_OK;
    uint32_t boxSize = 0;
    BoxType_t boxType;

    while (m_pos < m_fileLength)
    {
        ret = getBoxSize(boxSize);
        if (ret == ERR_EOF)
        {
            return ERR_OK;
        }

        if (ret != ERR_OK)
        {
            LOG_E("Fail to get box size.\n");
            return ERR_FAIL;
        }

        ret = getBoxType(boxType);
        if (ret != ERR_OK)
        {
            LOG_E("Fail to get box type.\n");
            return ERR_FAIL;
        }

        ret = parseBox(boxSize, boxType, m_boxList);
        if (ret != ERR_OK)
        {
            LOG_E("Fail to parse box:%s.\n", boxType);
            return ret;
        }

        m_pos += boxSize;
        fseek(m_pSrcFd, m_pos, SEEK_SET);
        //LOG_V("m_pos:%llu, m_fileLength:%llu.\n", m_pos, m_fileLength);
    }

    return ERR_OK;
}

int MP4ParserImpl::parseBox(uint32_t boxSize, BoxType_t &boxType, BoxList &boxList)
{
    int ret = 0;
    size_t readSize = 0;
    uint8_t *p_body = nullptr;
    uint32_t bodySize = 0;
    uint32_t actualBodySize = 0;
    bool isRaw = false;

    Box *p_box = BoxFactory::createBox(boxSize, boxType);
    if (p_box == nullptr)
    {
        LOG_E("Fail to create a box:%s.\n", boxType);
        return ERR_FAIL;
    }

    BoxPtr_t box(p_box);

	Box::setSrcFilePath(m_srcFileName);
    box->setFilePos(m_pos);

    if (boxSize < 8)
    {
        LOG_E("Invalid box data, boxSize(%u) is too small.\n", boxSize);
        return ERR_FAIL;
    }

    if (boxSize == 8)
    {
        boxList.insertBox(box);
        return ERR_OK;
    }

    //Read body buffer
    bodySize = boxSize - 8;
	actualBodySize = bodySize;
	box->setActualBodySize(actualBodySize);

    isRaw = isRawDataBox(boxType);
    if (isRaw)//If is raw data box, we need not store all the data.
    {
        bodySize = (bodySize > 64) ? 64 : bodySize;
    }

    p_body = new uint8_t[bodySize];
    if (p_body == nullptr)
    {
        LOG_E("Fail to allocate body buffer.\n");
        ret = ERR_FAIL;
        goto EXIT;
    }

    readSize = fread(p_body, sizeof(uint8_t), bodySize, m_pSrcFd);
    if (readSize != bodySize)
    {
        LOG_E("Fail to get body.\n");
        ret = ERR_FAIL;
        goto EXIT;
    }

    if (isRaw && actualBodySize > bodySize)
    {
        fseek(m_pSrcFd, actualBodySize - bodySize, SEEK_CUR);
    }

    //parse body
    ret = box->parseBody(p_body, bodySize);
    if (ret != ERR_OK)
    {
		LOG_E("Fail to parse box body.\n");
        ret = ERR_FAIL;
        goto EXIT;
	}

    boxList.insertBox(box);

    EXIT:
    delete [] p_body;
    return ret;
}

bool MP4ParserImpl::isRawDataBox(const BoxType_t &boxType)
{
    return strcmp(boxType, "mdat") == 0;
}

int MP4ParserImpl::getBoxSize(uint32_t &boxSize)
{
    uint8_t sizeBytes[4];
    int ret = fread(sizeBytes, sizeof(uint8_t), 4, m_pSrcFd);
    if (ret != 4)
    {
        LOG_E("Fail to read box size");
        return ERR_FAIL;
    }

    BEBytes2Uint32(sizeBytes, boxSize);

    if (boxSize == 0)
    {
        LOG_A("Reach file end.\n");
        return ERR_EOF;
    }

    if (boxSize < 8)
    {
        LOG_E("Invalid box size:%u.\n", boxSize);
        return ERR_FAIL;
    }

    return ERR_OK;
}

int MP4ParserImpl::getBoxType(BoxType_t &boxType)
{
    int ret = ERR_OK;

    memset(boxType, 0, sizeof(boxType));
    ret = fread(boxType, sizeof(uint8_t), 4, m_pSrcFd);
    if (ret != 4)
    {
        LOG_E("Fail to read box type.\n");
        return ERR_FAIL;
    }

    return ERR_OK;
}

void MP4ParserImpl::getFileLength()
{
    fseek(m_pSrcFd, 0L, SEEK_END);
    m_fileLength = ftell(m_pSrcFd);
    fseek(m_pSrcFd, 0L, SEEK_SET);

    return;
}

/*****************************************************************
 *                MP4Parser Implemetion
 *****************************************************************/
MP4Parser::MP4Parser()
{
    m_pImpl = new MP4ParserImpl();
}

MP4Parser::~MP4Parser()
{
    if (m_pImpl != nullptr)
    {
        delete m_pImpl;
    }
}

int MP4Parser::parse(const std::string &srcFileName)
{
    return m_pImpl->parse(srcFileName);
}

int MP4Parser::outputBoxList()
{
    return m_pImpl->outputBoxList();
}

int MP4Parser::getBoxCount(const char* p_boxType) const
{
    return m_pImpl->getBoxCount(p_boxType);
}

BoxPtr_t MP4Parser::getBox(const char* p_boxType) const
{
    return m_pImpl->getBox(p_boxType);
}

BoxPtr_t MP4Parser::getNextBox(const BoxPtr_t& curBox) const
{
    return m_pImpl->getNextBox(curBox);
}

BoxPtr_t MP4Parser::getFirstBox() const
{
    return m_pImpl->getFirstBox();
}

bool MP4Parser::exists(const char* p_boxType) const
{
    return m_pImpl->exists(p_boxType);
}

int MP4Parser::traverse(TraverseFunc_t traverseFunc, void *p_userData)
{
    return m_pImpl->traverse(traverseFunc, p_userData);
}

int MP4Parser::clear()
{
    return m_pImpl->clear();
}

std::string MP4Parser::srcFile()
{
	return m_pImpl->srcFile();
}

std::string MP4Parser::dstFile()
{
	return m_pImpl->dstFile();
}

int MP4Parser::setDstFile(const std::string &outputFilePath)
{
	return m_pImpl->setDstFile(outputFilePath);
}

int MP4Parser::redirectOut2File()
{
	return m_pImpl->redirectOut2File();
}

int MP4Parser::restore2Stdout()
{
	return m_pImpl->restore2Stdout();
}

}//namespace  mp4Parser
