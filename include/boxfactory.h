#ifndef BOXFACTORY_H
#define BOXFACTORY_H

#include "mp4box.h"


namespace mp4Parser{

class BoxFactory
{
public:
    BoxFactory();
    virtual ~BoxFactory();
    static Box* createBox(uint32_t boxSize, const BoxType_t & boxType);

    static const char* FTYP_BOX;
    static const char* STYP_BOX;

    //moov box
    static const char* MOOV_BOX;
    static const char* MVHD_BOX;
    static const char* TRAK_BOX;
    static const char* TKHD_BOX;
    static const char* TREF_BOX;
    static const char* EDTS_BOX;
    static const char* ELST_BOX;
    static const char* MDIA_BOX;
    static const char* MDHD_BOX;
    static const char* HDLR_BOX;
    static const char* MINF_BOX;
    static const char* VMHD_BOX;
    static const char* SMHD_BOX;
    static const char* HMHD_BOX;
    static const char* NMHD_BOX;
    static const char* DINF_BOX;
    static const char* DREF_BOX;
    static const char* URL_BOX;
    static const char* URN_BOX;
    static const char* STBL_BOX;
    static const char* STSD_BOX;
    static const char* STTS_BOX;
    static const char* CTTS_BOX;
    static const char* STSC_BOX;
    static const char* STSZ_BOX;
    static const char* STCO_BOX;
    static const char* STSS_BOX;
    static const char* MVEX_BOX;
    static const char* MEHD_BOX;
    static const char* TREX_BOX;
    static const char* HINT_BOX;
    static const char* CDSC_BOX;
    static const char* SYNC_BOX;
    static const char* TFDT_BOX;
    static const char* MFRA_BOX;

    //moof box
    static const char* MOOF_BOX;
    static const char* MFHD_BOX;
    static const char* TRAF_BOX;
    static const char* TFHD_BOX;
    static const char* TRUN_BOX;

    //mdat box
    static const char* MDAT_BOX;

    //free box
    static const char* FREE_BOX;

    //skip box
    static const char* SKIP_BOX;
    static const char* UDTA_BOX;
    static const char* HNTI_BOX;
    static const char* HINF_BOX;
    static const char* TRPY_BOX;
    static const char* NUMP_BOX;
    static const char* TPYL_BOX;
    static const char* TOTL_BOX;
    static const char* NPCK_BOX;
    static const char* TPAY_BOX;
    static const char* MAXR_BOX;
    static const char* DMED_BOX;
    static const char* DIMM_BOX;
    static const char* DREP_BOX;
    static const char* TMIN_BOX;
    static const char* TMAX_BOX;
    static const char* PMAX_BOX;
    static const char* DMAX_BOX;
    static const char* PAYT_BOX;
    static const char* RTP_BOX;
    static const char* SDP_BOX;
    static const char* NAME_BOX;

protected:

private:
    static bool isContainer(const BoxType_t & boxType);
    static bool isBasicBox(const BoxType_t & boxType);
    
};

}//namespace  mp4Parser

#endif // BOXFACTORY_H
