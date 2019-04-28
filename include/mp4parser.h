/**
 * @brief  Header file for mp4Parser.cpp
 * @author DuanBaoshan
 */

#ifndef MP4PARSER_H
#define MP4PARSER_H

#include <stdio.h>
#include <string>
#include <inttypes.h>

#include "errvalue.h"
#include "mp4box.h"
#include "boxfactory.h"

namespace mp4Parser{
class MP4ParserImpl;

class MP4Parser
{
public:
    MP4Parser();
    virtual ~MP4Parser();

    /**
     * @brief Parse a mp4 file and construct a box list containing all the mp4 boxes.
     * @param srcFileName mp4 file name
     */
    int parse(const std::string &srcFileName);


    /**
     * @brief Output all the box information to a file or stdout.
     *
     * If never set output file by setDstFile(outputFilePath) or has cleared output file by setDstFile(""),
     * it will output all the box information to stdout, or else it will output to a file.
     */
    int outputBoxList();

    /**
     * @brief Search in the box list to find the total count of boxes with the
     *  same p_boxType.
     */
    int getBoxCount(const char* p_boxType) const;

    /**
     * @brief Find the box which type is equal to p_boxType from the box list.
     *
     * It will only return the first found box even though there are more
     * than one box.If there are more than one box with p_boxType, and you need
     * get all the boxes, you can call getNextBox again and again, until the
     * returned box is null.
     */
    BoxPtr_t getBox(const char* p_boxType) const;

    /**
     * @brief Get the next box from box list which has the same type as curBox.
     */
    BoxPtr_t getNextBox(const BoxPtr_t& curBox) const;

    /**
     * @brief Get the first box from box list.
     */
    BoxPtr_t getFirstBox() const;

    bool exists(const char* p_boxType) const;

    int traverse(TraverseFunc_t traverseFunc, void *p_userData);

    /**
     * @brief Clear the source file name, the parsed result(all the box information).
     */
    int clear();

    std::string srcFile();

    std::string dstFile();

    /**
     * @brief Set the output file path.
     *
     * When set a valid output file path by this function, outputBoxList() will
     * output to that file, and redirectOut2File() also will redirect cout to that
     * file. When clear the output file by dstFile(""), the outputBoxList() and
     * redirectOut2File() will output to stdout again.
     *
     */
    int setDstFile(const std::string &outputFilePath);

    /**
     * @brief Redirect the output of cout to a file.
     *
     * When cout some information, the default output is stdout. But you can
     * cout to a file with this function. The output file must be setted by
     * setDstFile(outputFilePath) before you call this function.If you haven't setted the
     * output file, or set output file by setDstFile(""), this function will do nothing.
     */
    int redirectOut2File();

    /**
     * @brief Restore cout to stdout.
     *
     *  If you have called redirectOut2File, and at the end of cout, you must call
     *  restore2Stdout to  restore cout to stdout.How many times you call redirectOut2File,
     *  how many times you must call restore2Stdout.
     */
    int restore2Stdout();

private:
    MP4ParserImpl *m_pImpl;
};
}//namespace  mp4Parser

#endif // MP4PARSER_H
