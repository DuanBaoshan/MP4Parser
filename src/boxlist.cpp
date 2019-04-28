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

#include "mp4box.h"

#ifndef _DEBUG_LEVEL_
#define _DEBUG_LEVEL_  _DEBUG_LEVEL_I_
#endif
#include "debug.h"

using namespace std;

namespace mp4Parser{

BoxList::BoxList():m_boxList()
{
    //ctor
}

BoxList::~BoxList()
{
    //dtor
}

int BoxList::insertBox(BoxPtr_t& box)
{
    if (m_boxList.size() > 0)
    {
        auto it = m_boxList.end() - 1;
        (*it)->m_nextSibling = box;
    }

    m_boxList.push_back(box);

    return ERR_OK;
}

BoxPtr_t BoxList::getBox(const char* p_boxType) const
{
    for (auto it = m_boxList.begin(); it != m_boxList.end(); it++)
    {
        BoxPtr_t box = *it;

        if (strcmp(box->type(), p_boxType) == 0)
        {
            return *it;
        }

        if (box->hasChildren())
        {
            BoxPtr_t retBox = box->findBoxInChildren(p_boxType);
            if (retBox)
            {
                return retBox;
            }
        }
    }

    return BoxPtr_t();
}

BoxPtr_t BoxList::getNextBox(const BoxPtr_t& curBox) const
{
    BoxPtr_t box;
    BoxPtr_t rootContainer;

    box = getNextBoxFromSibling(curBox->nextSibling(), curBox->type());
    if (box)
    {
        return box;
    }

    box = getNextBoxFromNextContainerUntilRoot(curBox, rootContainer);
    if (box)
    {
        return box;
    }

    box = getNextBoxFromSibling(rootContainer->nextSibling(), curBox->type());
    if (box)
    {
        return box;
    }

    return BoxPtr_t();
}

BoxPtr_t BoxList::getFirstBox()const
{
    if (m_boxList.size() == 0)
    {
        return BoxPtr_t();
    }

    return *(m_boxList.begin());
}

int BoxList::traverse(TraverseFunc_t traverseFunc, void *p_userData)
{
    BoxPtr_t box = getFirstBox();

    while (box)
    {
        traverseBox(box, traverseFunc, p_userData);
        box = box->nextSibling();
    }

    return ERR_OK;
}

int BoxList::traverseBox(BoxPtr_t& box, TraverseFunc_t traverseFunc, void *p_userData)
{
    traverseFunc(box, p_userData);

    if (!box->hasChildren())
    {
        return ERR_OK;
    }

    BoxPtr_t child = box->firstChild();
    while (child)
    {
        traverseBox(child, traverseFunc, p_userData);
        child = child->nextSibling();
    }

    return ERR_OK;
}

int BoxList::show() const
{
	cout << "MP4" << endl;
    for (auto it = m_boxList.begin(); it != m_boxList.end(); it++)
    {
        (*it)->show();
    }

    return ERR_OK;
}

int BoxList::show(int index) const
{
    m_boxList[index]->show();
    return ERR_OK;
}

int BoxList::getBoxCount(const char* p_boxType) const
{
    int boxCount = 0;

    for (auto it = m_boxList.begin(); it != m_boxList.end(); it++)
    {
        BoxPtr_t box = *it;

        if (strcmp(box->type(), p_boxType) == 0)
        {
            boxCount++;
        }

        if (box->hasChildren())
        {
            boxCount += box->getBoxCountInChildren(p_boxType);
        }
    }

    return boxCount;
}

uint64_t BoxList::length() const
{
    return m_boxList.size();
}

int BoxList::clear()
{
    m_boxList.clear();

    return ERR_OK;
}


BoxPtr_t BoxList::getNextBoxFromSibling(const BoxPtr_t& nextSibling, const char* p_boxType) const
{
    BoxPtr_t box;
    BoxPtr_t nextSiblingBox = nextSibling;

    while (nextSiblingBox)
    {
        if (strcmp(nextSiblingBox->type(), p_boxType) == 0)
        {
            return nextSiblingBox;
        }

        box = nextSiblingBox->findBoxInChildren(p_boxType);
        if (box)
        {
            return box;
        }

        nextSiblingBox = nextSiblingBox->nextSibling();
    }

    return BoxPtr_t();
}

BoxPtr_t BoxList::getNextBoxFromNextContainerUntilRoot(const BoxPtr_t& curBox, BoxPtr_t &rootContainer) const
{
    BoxPtr_t box;
    BoxPtr_t containerBox;
    BoxPtr_t nextSiblingContainerBox;

    containerBox = curBox->container();
    if (!containerBox)
    {
        rootContainer = curBox;
        return BoxPtr_t();
    }

    while (containerBox->container())
    {
        nextSiblingContainerBox = containerBox->nextSibling();
        while (nextSiblingContainerBox)
        {
            if (strcmp(nextSiblingContainerBox->type(), curBox->type()) == 0)
            {
                return nextSiblingContainerBox;
            }

            box = nextSiblingContainerBox->findBoxInChildren(curBox->type());
            if (box)
            {
                return box;
            }

            nextSiblingContainerBox = nextSiblingContainerBox->nextSibling();
        }

        containerBox = containerBox->container();
    }

    rootContainer = containerBox;

    return BoxPtr_t();
}

}//namespace mp4Parser
