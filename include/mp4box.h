/**
 * @brief All the mp4 box definitions.
 * @author DuanBaoshan
 */

#ifndef MP4BOX_H
#define MP4BOX_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdio.h>
#include <inttypes.h>
#include <string>
#include <vector>
#include <memory>
#include <map>

#include "utils.h"

#include "errvalue.h"

/*HSP:HeaderSpace*/
#define HSP insertOutputSpace()

#define _HEX_2_DIGIT_ "0x" << setw(2) << setfill('0') << right << hex << uppercase
#define _HEX_4_DIGIT_ "0x" << setw(4) << setfill('0') << right << hex << uppercase
#define _HEX_8_DIGIT_ "0x" << setw(8) << setfill('0') << right << hex << uppercase
#define _HEX_DIGIT_ "0x" << hex << uppercase


namespace mp4Parser{

class Box;

typedef char BoxType_t[5];
typedef std::vector<uint8_t> UserType_t;
typedef int32_t BoxMatrix_t[9];

typedef std::shared_ptr<Box> BoxPtr_t;


typedef enum
{
    INVALID_HANDLER = -1,
    HANDLER_VIDE = 0x76696465,
    HANDLER_SOUD = 0x736F756E,
    HANDLER_HINT = 0x68696E74,
    HANDLER_SDSM = 0x7364736D,
    HANDLER_ODSM = 0x6F64736D,
}HandlerType_e;

typedef int (*TransferFunc_t)(BoxPtr_t& box, void *p_userData);

class BoxList
{
public:
    BoxList();
    virtual ~BoxList();

    int insertBox(BoxPtr_t& box);

    /**
     * @brief Search in the box list to find the total count of boxes with the
     * same p_boxType.
     * @param p_boxType box type string
     * @return ERR_OK for success, other for failure
     */
    int getBoxCount(const char* p_boxType) const;

    /**
     * @brief Search in the box list which type is equal to parameter p_boxType.
     *
     * It will only return the first found box even though there are more
     * than one box.If there are more than box with p_boxType, and you need
     * get all the boxes, you can call getNextBox again and again, until the
     * returned box is null.
     */
    BoxPtr_t getBox(const char* p_boxType) const;

    /**
     * @brief Get the next box from box list which has the same type as curBox.
     */
    BoxPtr_t getNextBox(const BoxPtr_t& curBox) const;

    /**
     * @brief Get the first box in the box list.
     */
    BoxPtr_t getFirstBox()const ;

    /**
     * @brief Transfer the whole box list.
     * @param transferFunc: user transfer function
     * @param p_userData: user data
     */
    int transfer(TransferFunc_t transferFunc, void *p_userData);

    int show() const;
    int show(int index) const;

    int clear();

    uint64_t length() const;
protected:

private:
    BoxPtr_t getNextBoxFromSibling(const BoxPtr_t& nextSibling, const char* p_boxType) const;
    BoxPtr_t getNextBoxFromNextContainerUntilRoot(const BoxPtr_t& curBox, BoxPtr_t &rootContainer) const;
    int transferBox(BoxPtr_t& box, TransferFunc_t transferFunc, void *p_userData);
private:
    std::vector<BoxPtr_t> m_boxList;
};

struct BoxBody
{
    BoxBody(uint8_t *p_buffer, uint32_t bufSize):p_body(p_buffer), bodySize(bufSize){}
    ~BoxBody()
    {
        if (p_body != nullptr)
        {
            delete [] p_body;

            p_body = nullptr;
            bodySize = 0;
        }
    }

    uint8_t *p_body;
    uint32_t bodySize;
};

typedef std::shared_ptr<BoxBody> BoxBodyPtr_t;

class Box:public std::enable_shared_from_this<Box>
{
public:
    Box(uint32_t boxSize, const BoxType_t & boxType, bool unknown = false);
    virtual ~Box();

    uint32_t size() const {return m_size;}
    const char* type() const {return m_type;}

    uint64_t largeSize() const {return m_largeSize;}
    const UserType_t& userType() const {return  m_userType;}

    uint8_t version() const {return m_version;}
    uint32_t flags() const {return m_flags;}

    BoxPtr_t container() const {return m_container.lock();}

    BoxPtr_t firstChild();
    BoxPtr_t nextSibling() {return m_nextSibling.lock();}

    bool isUnknownBox() {return m_unknown;}
    bool hasChildren();

    /**
     * @brief Search in the box's children to find the total count of boxes with the
     * same p_boxType.
     */
    int getBoxCountInChildren(const char *p_boxType);

    /**
     * @brief Search in the box's children which type is equal to parameter p_boxType.
     *
     * It will only return the first found box even though there are more
     * than one box.If there are more than box with p_boxType, and you need
     * get all the boxes with the same type, you can call findNextBoxInChildren
     * again and again, until the returned box is null.
     */
    BoxPtr_t findBoxInChildren(const char *p_boxType);
    BoxPtr_t findNextBoxInChildren(const BoxPtr_t& curBox);

    /**
     * @brief Parse a box's body buffer into a box structure.
     *
     * A body means all the box fields except for size and type filed.
     * For example, the body of ftypbox includes major_brand, minor_version and compatible_brands[] fields.
     * Another example is full box, the body of hdlr includes version, flags, pre_defined,handler_type,reserved
     * and name fields. So the bodySize is always equal the size of box - 8.
     */
    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);



    int level(){return m_level;}
    void setLevel(int level){m_level = level;}

    int setFilePos(off64_t pos){m_pos = pos; return ERR_OK;}
    off64_t filePos() const {return m_pos;}

    static int setSrcFilePath(const std::string& srcFilePath){m_srcFilePath = srcFilePath;return ERR_OK;}

    int setActualBodySize(uint32_t actualBodySize);

protected:
    friend class BoxList;

    int show();

    virtual int showBody();

    int  checkBodyValid(uint8_t *p_body, uint32_t bodySize);

    int  parseBodyAndAppendChild(uint8_t *p_body, uint32_t bodySize);
    int  appendChild(BoxPtr_t &box);

    int  parseVerAndFlags(uint8_t *p_body, uint32_t bodySize);
    void insertOutputSpace();
    void showMatrix(BoxMatrix_t &matrix);
    void showVerFlags();
    void printRawBuff(uint8_t *p_buffer, uint32_t buffSize, uint32_t actualSize);

protected:
    uint32_t m_size;
    BoxType_t m_type;

    uint64_t m_largeSize;
    UserType_t m_userType;

    uint8_t m_version;
    uint32_t m_flags;

    std::weak_ptr<Box> m_container;
    std::weak_ptr<Box> m_nextSibling;

    /*Some box (for example mdat box) has a big body,
     *and we needn't store all the raw data of the body, so we only contain a little data in
     *m_body buffer, the m_bodySize is that little body size.But the whole body size is
     *contained in the m_actualBodySize and will be setted by setActualBodySize.*/
    BoxBodyPtr_t m_body;
    uint32_t m_bodySize;

    uint32_t m_actualBodySize;

    /**
     * A min size which a valid body should be.
     */
    uint32_t m_bodyMinSize;

    BoxList m_boxList;

    int m_level;//only used to beautify the output

    /**
     * The position at which the box exists in the mp4 file.But this value is only valid
     * for the box whose container is file, for example moov, mdat, free and so on.
     */
    off64_t m_pos;

    static std::string m_srcFilePath;

private:
    bool m_unknown;


};

class BoxContainer:public Box
{
public:
    BoxContainer(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~BoxContainer();
    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);
};

class FtypBox:public Box
{
public:
    FtypBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~FtypBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);

    const uint8_t* majorBrand() const {return m_majorBrand;}
    const uint8_t* minorBrand() const {return m_minorVersion;}
    const std::vector<uint8_t> & compatibleBrands() const {return m_compatibleBrands;}

protected:
    virtual int showBody();
private:
    uint8_t m_majorBrand[5];
    uint8_t m_minorVersion[5];
    std::vector<uint8_t> m_compatibleBrands;
};
typedef std::shared_ptr<FtypBox> FtypBoxPtr_t;


class StypBox:public Box
{
public:
    StypBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~StypBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);

    const uint8_t* majorBrand() const {return m_majorBrand;}
    const uint8_t* minorBrand() const {return m_minorVersion;}
    const std::vector<uint8_t> & compatibleBrands() const {return m_compatibleBrands;}
protected:
    virtual int showBody();
private:
    uint8_t m_majorBrand[5];
    uint8_t m_minorVersion[5];
    std::vector<uint8_t> m_compatibleBrands;
};
typedef std::shared_ptr<StypBox> StypBoxPtr_t;


/* ===================================================
 *            moov box and its sub boxes
 * ===================================================
 */
class MvhdBox:public Box
{
public:
    MvhdBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~MvhdBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);

    uint64_t creationTime() const {return m_creationTime;}
    uint64_t modificationTime() const {return m_modificationTime;}
    uint32_t timescale() const {return m_timescale;}
    uint64_t duration() const {return m_duration;}
    int32_t rate() const {return m_rate;}
    int16_t volume() const {return m_volume;}
    const BoxMatrix_t &matrix() const {return m_matrix;}
    uint32_t nextTrackId() const {return m_nextTrackId;}
protected:
    virtual int showBody();
private:
    uint64_t m_creationTime;
    uint64_t m_modificationTime;
    uint32_t m_timescale;
    uint64_t m_duration;
    int32_t m_rate;
    int16_t m_volume;
    BoxMatrix_t m_matrix;
    uint32_t m_nextTrackId;
};
typedef std::shared_ptr<MvhdBox> MvhdBoxPtr_t;

class TkhdBox:public Box
{
public:
    TkhdBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~TkhdBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);

    uint64_t creationTime() const {return m_creationTime;}
    uint64_t modificationTime() const {return m_modificationTime;}
    uint32_t trackID() const {return m_trackID;}
    uint64_t duration() const {return m_duration;}

    int16_t layer() const {return m_layer;}
    int16_t alternateGroup() const {return m_alternateGroup;}
    int16_t volume() const {return m_volume;}
    const BoxMatrix_t &matrix() const {return m_matrix;}

    uint32_t width() const {return m_width;}
    uint32_t height() const {return m_height;}

protected:
    virtual int showBody();

private:
    uint64_t m_creationTime;
    uint64_t m_modificationTime;
    uint32_t m_trackID;
    uint64_t m_duration;

    int16_t m_layer;
    int16_t m_alternateGroup;
    int16_t m_volume;
    BoxMatrix_t m_matrix;

    uint32_t m_width;
    uint32_t m_height;
};
typedef std::shared_ptr<TkhdBox>TkhdBoxPtr_t;

class TrefTypeBox:public Box
{
public:
    TrefTypeBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~TrefTypeBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);

    const std::vector<uint32_t>&  trackIDList(){return m_trackIDList;}
protected:
    virtual int showBody();

private:
    std::vector<uint32_t>  m_trackIDList;
};
typedef std::shared_ptr<TrefTypeBox> TrefTypeBoxPtr_t;

class ElstBox:public Box
{
public:
    ElstBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~ElstBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);

    typedef struct
    {
        uint64_t segmentDuration;
        int64_t mediaTime;
        int16_t mediaRateInteger;
        int16_t mediaRateFraction;
    }EditSegment_t;

    uint32_t entryCount()const {return m_entryCount;}
    const std::vector<EditSegment_t> &editSegmentList() const {return m_editSegmentList;}

protected:
    virtual int showBody();

private:
    uint32_t m_entryCount;
    std::vector<EditSegment_t> m_editSegmentList;
};
typedef std::shared_ptr<ElstBox> ElstBoxPtr_t;

class MdhdBox : public Box
{
public:
    MdhdBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~MdhdBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);

    uint64_t creationTime() const {return m_creationTime;}
    uint64_t modificationTime() const {return m_modificationTime;}
    uint32_t timescale() const {return m_timescale;}
    uint64_t duration() const {return m_duration;}

    /*Contains three characters and terminated with '\0'.*/
    const int8_t*  language() const {return m_language;}
protected:
    virtual int showBody();

private:
    uint64_t m_creationTime;
    uint64_t m_modificationTime;
    uint32_t m_timescale;
    uint64_t m_duration;
    int8_t   m_language[4];
};
typedef std::shared_ptr<MdhdBox> MdhdBoxPtr_t;

class HdlrBox : public Box
{
public:
    HdlrBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~HdlrBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);

    uint32_t handlerType() const {return m_handlerType;}
    const char* name() const {return m_name.c_str();}

    static std::map<HandlerType_e, std::string> HANDLER_TYPE_MAP;
protected:
    virtual int showBody();
private:
    uint32_t m_handlerType;
    std::string m_name;
};
typedef std::shared_ptr<HdlrBox> HdlrBoxPtr_t;


class VmhdBox : public Box
{
public:
    VmhdBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~VmhdBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);

    uint16_t graphicsmode() const {return m_graphicsmode;}
    const uint16_t* opcolor() const {return  m_opcolor;}
protected:
    virtual int showBody();
private:
    uint16_t m_graphicsmode;
    uint16_t m_opcolor[3];
};
typedef std::shared_ptr<VmhdBox> VmhdBoxPtr_t;

class SmhdBox : public Box
{
public:
    SmhdBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~SmhdBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);

    int16_t balance() const { return m_balance;}
protected:
    virtual int showBody();
private:
    int16_t m_balance;
};
typedef std::shared_ptr<SmhdBox> SmhdBoxPtr_t;

class HmhdBox : public Box
{
public:
    HmhdBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~HmhdBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);

    uint16_t maxPDUsize() const {return m_maxPDUsize;}
    uint16_t avgPDUsize() const {return m_avgPDUsize;}
    uint32_t maxBitrate() const {return m_maxBitrate;}
    uint32_t avgBitrate() const {return m_avgBitrate;}

protected:
    virtual int showBody();
private:
    uint16_t m_maxPDUsize;
    uint16_t m_avgPDUsize;
    uint32_t m_maxBitrate;
    uint32_t m_avgBitrate;
    uint32_t m_reserved;
};
typedef std::shared_ptr<HmhdBox> HmhdBoxPtr_t;

class NmhdBox : public Box
{
public:
    NmhdBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~NmhdBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);
protected:
    virtual int showBody();
private:

};
typedef std::shared_ptr<NmhdBox> NmhdBoxPtr_t;

class DrefBox : public Box
{
public:
    DrefBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~DrefBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);

    uint32_t entryCount() const {return m_entryCount;}
protected:
    virtual int showBody();
private:
    uint32_t m_entryCount;
};
typedef std::shared_ptr<DrefBox> DrefBoxPtr_t;

class UrlBox : public Box
{
public:
    UrlBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~UrlBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);

    const std::string & location() const {return m_location;}
protected:
    virtual int showBody();
private:
    std::string m_location;
};
typedef std::shared_ptr<UrlBox> UrlBoxPtr_t;

class StsdBox : public Box
{
public:
    StsdBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~StsdBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);

    uint32_t entryCount() const {return m_entryCount;}
protected:
    virtual int showBody();
private:
    HandlerType_e findHandlerType();
private:
    HandlerType_e m_handlerType;
    uint32_t m_entryCount;

};
typedef std::shared_ptr<StsdBox> StsdBoxPtr_t;

class SampleEntryBox:public BoxContainer
{
public:
    SampleEntryBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~SampleEntryBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);
    uint16_t dataReferenceIndex() const {return m_dataReferenceIndex;}
protected:
    virtual int showBody();
    uint16_t m_dataReferenceIndex;
};
typedef std::shared_ptr<SampleEntryBox> SampleEntryBoxPtr_t;

class HintSampleEntryBox:public SampleEntryBox
{
public:
    HintSampleEntryBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~HintSampleEntryBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);

    const std::vector<uint8_t>& data() const {return m_data;}
protected:
    virtual int showBody();
private:
    std::vector<uint8_t> m_data;
};
typedef std::shared_ptr<HintSampleEntryBox> HintSampleEntryBoxPtr_t;

class VisualSampleEntryBox:public SampleEntryBox
{
public:
    VisualSampleEntryBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~VisualSampleEntryBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);

    uint16_t dataReferenceIndex() const {return m_dataReferenceIndex;}
    uint16_t width() const {return m_width;}
    uint16_t height() const {return m_height;}
    uint32_t horizResolution() const {return m_horizResolution;}
    uint32_t vertResolution() const {return m_vertResolution;}
    uint16_t frameCount() const {return m_frameCount;}
    const std::string& compressorName() const {return m_compressorName;}
    uint16_t depth() const {return m_depth;}

protected:
    virtual int showBody();
private:
    uint16_t m_dataReferenceIndex;
    uint16_t m_width;
    uint16_t m_height;
    uint32_t m_horizResolution;
    uint32_t m_vertResolution;
    uint16_t m_frameCount;
    std::string m_compressorName;
    uint16_t m_depth;
};
typedef std::shared_ptr<VisualSampleEntryBox> VisualSampleEntryBoxPtr_t;


class AudioSampleEntryBox:public SampleEntryBox
{
public:
    AudioSampleEntryBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~AudioSampleEntryBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);

    uint16_t dataReferenceIndex() const {return m_dataReferenceIndex;}
    uint16_t channelCount() const {return m_channelCount;}
    uint16_t sampleSize() const {return m_sampleSize;}
    uint32_t sampleRate() const {return m_sampleRate;}
protected:
    virtual int showBody();
private:
    uint16_t m_dataReferenceIndex;
    uint16_t m_channelCount;
    uint16_t m_sampleSize;
    uint32_t m_sampleRate;
};
typedef std::shared_ptr<AudioSampleEntryBox> AudioSampleEntryBoxPtr_t;

class SttsBox:public Box
{
public:
    SttsBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~SttsBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);

    typedef struct
    {
        uint32_t sampleCount;
        uint32_t sampleDelta;
    }SampleInfo_t;

    const std::vector<SampleInfo_t>& samples() const {return  m_samples;}
protected:
    virtual int showBody();

private:
    uint32_t m_entryCount;
    std::vector<SampleInfo_t> m_samples;
};
typedef std::shared_ptr<SttsBox> SttsBoxPtr_t;

class CttsBox:public Box
{
public:
    CttsBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~CttsBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);

    typedef struct
    {
        uint32_t sampleCount;
        uint32_t sampleOffset;
    }SampleInfo_t;

    const std::vector<SampleInfo_t>& samples() const {return  m_samples;}
protected:
    virtual int showBody();

private:
    uint32_t m_entryCount;
    std::vector<SampleInfo_t> m_samples;
};
typedef std::shared_ptr<CttsBox> CttsBoxPtr_t;

class StscBox:public Box
{
public:
    StscBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~StscBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);

    typedef struct
    {
        uint32_t firstChunk;
        uint32_t samplesPerChunk;
        uint32_t sampleDescriptionIndex;
    }ChunkInfo_t;

    const std::vector<ChunkInfo_t>& chunks() const {return m_chunks;}
protected:
    virtual int showBody();

private:
    uint32_t m_entryCount;
    std::vector<ChunkInfo_t> m_chunks;
};
typedef std::shared_ptr<StscBox> StscBoxPtr_t;

class StszBox:public Box
{
public:
    StszBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~StszBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);

    uint32_t sampleSize() const {return m_sampleSize;}
    uint32_t sampleCount() const {return m_sampleCount;}
    const std::vector<uint32_t>& entrySizeList() const {return  m_entrySizeList;}
protected:
    virtual int showBody();

private:
    uint32_t  m_sampleSize;
    uint32_t  m_sampleCount;
    std::vector<uint32_t> m_entrySizeList;
};
typedef std::shared_ptr<StszBox> StszBoxPtr_t;

class StcoBox:public Box
{
public:
    StcoBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~StcoBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);

    uint32_t  entryCount() const {return m_entryCount;}
    const std::vector<uint32_t>& chunkOffset() const {return m_chunkOffset;}
protected:
    virtual int showBody();

private:
    uint32_t  m_entryCount;
    std::vector<uint32_t> m_chunkOffset;
};
typedef std::shared_ptr<StcoBox> StcoBoxPtr_t;

class StssBox:public Box
{
public:
    StssBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~StssBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);

    uint32_t  entryCount() const {return m_entryCount;}
    const std::vector<uint32_t>& sampleNumberList() const {return m_sampleNumberList;}
protected:
    virtual int showBody();

private:
    uint32_t  m_entryCount;
    std::vector<uint32_t> m_sampleNumberList;
};
typedef std::shared_ptr<StssBox> StssBoxPtr_t;

class MehdBox:public Box
{
public:
    MehdBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~MehdBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);

    uint64_t  fragmentDuration() const {return m_fragmentDuration;}
protected:
    virtual int showBody();

private:
    uint64_t  m_fragmentDuration;
};
typedef std::shared_ptr<MehdBox> MehdBoxPtr_t;

/* ===================================================
 *          moof box and its sub boxes
 * ===================================================
 */
class MfhdBox : public Box
{
public:
    MfhdBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~MfhdBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);

    uint32_t sequenceNumber() const {return m_sequenceNumber;}
protected:
    virtual int showBody();

private:
    uint32_t m_sequenceNumber;
};
typedef std::shared_ptr<MfhdBox> MfhdBoxPtr_t;

class TfhdBox:public Box
{
public:
    TfhdBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~TfhdBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);

    uint32_t  trackID()const {return m_trackID;}
    uint64_t  baseDataOffset()const {return m_baseDataOffset;}
    uint32_t  sampleDescriptionIndex()const {return m_sampleDescriptionIndex;}
    uint32_t  defaultSampleDuration()const {return m_defaultSampleDuration;}
    uint32_t  defaultSampleSize()const {return m_defaultSampleSize;}
    uint32_t  defaultSampleFlags()const {return m_defaultSampleFlags;}

    bool baseDataOffsetPresent() const { return (m_flags & 0x000001) ? true : false;}
    bool sampleDescIndexPresent() const { return (m_flags & 0x000002) ? true : false;}
    bool defaultSampleDurationRresent() const { return (m_flags & 0x000008) ? true : false;}
    bool defaultSampleSizePresent() const { return (m_flags & 0x000010) ? true : false;}
    bool defaultSampleFlagsPresent() const { return (m_flags & 0x000020) ? true : false;}
protected:
    virtual int showBody();

private:
    uint32_t  m_trackID;

    uint64_t  m_baseDataOffset;
    uint32_t  m_sampleDescriptionIndex;
    uint32_t  m_defaultSampleDuration;
    uint32_t  m_defaultSampleSize;
    uint32_t  m_defaultSampleFlags;
};
typedef std::shared_ptr<TfhdBox> TfhdBoxPtr_t;


class TrunBox:public Box
{
public:
    TrunBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~TrunBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);

    typedef struct
    {
        uint32_t sampleDuration;
        uint32_t sampleSize;
        uint32_t sampleFlags;
        uint32_t sampleCompositionTimeOffset;
    }SampleInfo_t;

    uint32_t  sampleCount() const {return m_sampleCount;}
    uint32_t  dataOffset() const {return m_dataOffset;}
    uint32_t  firstSampleFlags() const {return m_firstSampleFlags;}
    const std::vector<SampleInfo_t>&  sampleInfoArray() const {return m_sampleInfoArray;}

    bool dataOffsetPresent() const {return (m_flags & 0x000001) ? true : false;}
    bool firstSampleFlagsPresent() const {return (m_flags & 0x000004) ? true : false;}
    bool sampleDurationPresent() const {return (m_flags & 0x000100) ? true : false;}
    bool sampleSizePresent() const {return (m_flags & 0x000200) ? true : false;}
    bool sampleFlagsPresent() const {return (m_flags & 0x000400) ? true : false;}
    bool sampleCompositionTimeOffsetsPresent() const {return (m_flags & 0x000800) ? true : false;}
protected:
    virtual int showBody();

private:

    uint32_t  m_sampleCount;

    uint32_t  m_dataOffset;
    uint32_t  m_firstSampleFlags;
    std::vector<SampleInfo_t>  m_sampleInfoArray;
};
typedef std::shared_ptr<TrunBox> TrunBoxPtr_t;

/* ===================================================
 *          udat box and its sub boxes
 * ===================================================
 */
class NameBox : public Box
{
public:
    NameBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~NameBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);

    /*The name text is terminated with '\0'.*/
    const std::vector<char>& nameText() const{return  m_nameText;}
protected:
    virtual int showBody();
private:
    std::vector<char> m_nameText;
};
typedef std::shared_ptr<NameBox> NameBoxPtr_t;

class TrpyBox : public Box
{
public:
    TrpyBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~TrpyBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);

    uint64_t bytesSent()const {return m_bytesSent;}
protected:
    virtual int showBody();
private:
    uint64_t m_bytesSent;
};
typedef std::shared_ptr<TrpyBox> TrpyBoxPtr_t;

class NumpBox : public Box
{
public:
    NumpBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~NumpBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);

    uint64_t packetsSent()const {return m_packetsSent;}
protected:
    virtual int showBody();
private:
    uint64_t m_packetsSent;
};
typedef std::shared_ptr<NumpBox> NumpBoxPtr_t;

class TpylBox : public Box
{
public:
    TpylBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~TpylBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);

    uint64_t bytesSent() const {return m_bytesSent;}
protected:
    virtual int showBody();
private:
    uint64_t m_bytesSent;
};
typedef std::shared_ptr<TpylBox> TpylBoxPtr_t;

class TotlBox : public Box
{
public:
    TotlBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~TotlBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);

    uint32_t bytesSent() const {return m_bytesSent;}
protected:
    virtual int showBody();
private:
    uint32_t m_bytesSent;
};
typedef std::shared_ptr<TotlBox> TotlBoxPtr_t;

class NpckBox : public Box
{
public:
    NpckBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~NpckBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);

    uint32_t packetsSent()const {return m_packetsSent;}
protected:
    virtual int showBody();
private:
    uint32_t m_packetsSent;
};
typedef std::shared_ptr<NpckBox> NpckBoxPtr_t;

class TpayBox : public Box
{
public:
    TpayBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~TpayBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);

    uint32_t bytesSent() const {return m_bytesSent;}
protected:
    virtual int showBody();
private:
    uint32_t m_bytesSent;
};
typedef std::shared_ptr<TpayBox> TpayBoxPtr_t;

class MaxrBox : public Box
{
public:
    MaxrBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~MaxrBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);

    uint32_t period()const {return m_period;}
    uint32_t bytes() const {return m_bytes;}
protected:
    virtual int showBody();
private:
    uint32_t m_period;
    uint32_t m_bytes;
};
typedef std::shared_ptr<MaxrBox> MaxrBoxPtr_t;

class DmedBox : public Box
{
public:
    DmedBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~DmedBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);

    uint64_t bytesSent() const {return m_bytesSent;}
protected:
    virtual int showBody();
private:
    uint64_t m_bytesSent;
};
typedef std::shared_ptr<DmedBox> DmedBoxPtr_t;

class DimmBox : public Box
{
public:
    DimmBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~DimmBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);

    uint64_t bytesSent() const {return m_bytesSent;}
protected:
    virtual int showBody();
private:
    uint64_t m_bytesSent;
};
typedef std::shared_ptr<DimmBox> DimmBoxPtr_t;

class DrepBox : public Box
{
public:
    DrepBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~DrepBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);

    uint64_t bytesSent() const {return m_bytesSent;}
protected:
    virtual int showBody();
private:
    uint64_t m_bytesSent;
};
typedef std::shared_ptr<DrepBox> DrepBoxPtr_t;

class TminBox : public Box
{
public:
    TminBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~TminBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);

    int32_t time() const {return m_time;}
protected:
    virtual int showBody();
private:
    int32_t m_time;
};
typedef std::shared_ptr<TminBox> TminBoxPtr_t;

class TmaxBox : public Box
{
public:
    TmaxBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~TmaxBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);

    int32_t time() const {return m_time;}
protected:
    virtual int showBody();
private:
    int32_t m_time;
};
typedef std::shared_ptr<TmaxBox> TmaxBoxPtr_t;

class PmaxBox : public Box
{
public:
    PmaxBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~PmaxBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);

    uint32_t bytes() const{return m_bytes;}
protected:
    virtual int showBody();
private:
    uint32_t m_bytes;
};
typedef std::shared_ptr<PmaxBox> PmaxBoxPtr_t;

class DmaxBox : public Box
{
public:
    DmaxBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~DmaxBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);
    uint32_t time() const {return m_time;}
protected:
    virtual int showBody();
private:
    uint32_t m_time;
};
typedef std::shared_ptr<DmaxBox> DmaxBoxPtr_t;

class PaytBox : public Box
{
public:
    PaytBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~PaytBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);

    uint32_t payloadID()const {return m_payloadID;}
    uint8_t  count() const {return m_count;}
    const std::vector<char>& rtpmapString()const {return m_rtpmapString;}

protected:
    virtual int showBody();
private:
    uint32_t m_payloadID;
    uint8_t  m_count;
    std::vector<char> m_rtpmapString;
};
typedef std::shared_ptr<PaytBox> PaytBoxPtr_t;


class RtpBox : public Box
{
public:
    RtpBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~RtpBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);

    const uint8_t* descriptionFormat() const {return m_descriptionFormat;}
    const std::vector<char>& sdpText() const {return m_sdpText;}
protected:
    virtual int showBody();
private:
    uint8_t m_descriptionFormat[5];
    std::vector<char> m_sdpText;

};
typedef std::shared_ptr<RtpBox> RtpBoxPtr_t;

class SdpBox : public Box
{
public:
    SdpBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~SdpBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);

   const std::vector<char>&sdpText() const {return m_sdpText;}
protected:
    virtual int showBody();
private:
    std::vector<char> m_sdpText;
};
typedef std::shared_ptr<SdpBox> SdpBoxPtr_t;

class TfdtBox : public Box
{
public:
    TfdtBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~TfdtBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);

    int32_t baseMediaDecodeTime() const {return m_baseMediaDecodeTime;}
protected:
    virtual int showBody();
private:
    int32_t m_baseMediaDecodeTime;
};
typedef std::shared_ptr<TfdtBox> TfdtBoxPtr_t;

class TrexBox:public Box
{
public:
    TrexBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~TrexBox();

    virtual int parseBody(uint8_t *p_body, uint32_t bodySize);

    uint32_t trackID() const {return m_trackID;}
    uint32_t defaultSampleDescriptionIndex() const {return  m_defaultSampleDescriptionIndex;}
    uint32_t defaultSampleDuration() const {return  m_defaultSampleDuration;}
    uint32_t defaultSampleSize() const {return  m_defaultSampleSize;}
    uint32_t defaultSampleFlags() const {return  m_defaultSampleFlags;}

protected:
    virtual int showBody();

private:
    uint32_t m_trackID;
    uint32_t m_defaultSampleDescriptionIndex;
    uint32_t m_defaultSampleDuration;
    uint32_t m_defaultSampleSize;
    uint32_t m_defaultSampleFlags;
};
typedef std::shared_ptr<TrexBox> TrexBoxPtr_t;

/* ===================================================
 *                mdat box
 * ===================================================
 */
class MdatBox:public Box
{
public:
    MdatBox(uint32_t boxSize, const BoxType_t & boxType);
    virtual ~MdatBox();

    int getActualBody(BoxBodyPtr_t &body);

protected:

private:    
    BoxBodyPtr_t m_actualBody;

};
typedef std::shared_ptr<MdatBox> MdatBoxPtr_t;

}//namespace mp4Parser

#endif // MP4BOX_H
