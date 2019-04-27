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

#include "boxfactory.h"

#ifndef _DEBUG_LEVEL_
#define _DEBUG_LEVEL_  _DEBUG_LEVEL_I_
#endif
#include "debug.h"

namespace mp4Parser{

const char* BoxFactory::FTYP_BOX = "ftyp";
const char* BoxFactory::STYP_BOX = "styp";
const char* BoxFactory::MOOV_BOX = "moov";
const char* BoxFactory::MVHD_BOX = "mvhd";
const char* BoxFactory::TRAK_BOX = "trak";
const char* BoxFactory::TKHD_BOX = "tkhd";
const char* BoxFactory::TREF_BOX = "tref";
const char* BoxFactory::HINT_BOX = "hint";
const char* BoxFactory::CDSC_BOX = "cdsc";
const char* BoxFactory::SYNC_BOX = "sync";
const char* BoxFactory::MDIA_BOX = "mdia";
const char* BoxFactory::MDHD_BOX = "mdhd";
const char* BoxFactory::HDLR_BOX = "hdlr";
const char* BoxFactory::MOOF_BOX = "moof";
const char* BoxFactory::MFHD_BOX = "mfhd";
const char* BoxFactory::EDTS_BOX = "edts";
const char* BoxFactory::ELST_BOX = "elst";
const char* BoxFactory::MINF_BOX = "minf";
const char* BoxFactory::VMHD_BOX = "vmhd";
const char* BoxFactory::SMHD_BOX = "smhd";
const char* BoxFactory::HMHD_BOX = "hmhd";
const char* BoxFactory::NMHD_BOX = "nmhd";
const char* BoxFactory::DINF_BOX = "dinf";
const char* BoxFactory::DREF_BOX = "dref";
const char* BoxFactory::URL_BOX  = "url ";
const char* BoxFactory::URN_BOX  = "urn ";
const char* BoxFactory::STBL_BOX = "stbl";
const char* BoxFactory::STSD_BOX = "stsd";
const char* BoxFactory::STTS_BOX = "stts";
const char* BoxFactory::CTTS_BOX = "ctts";
const char* BoxFactory::STSC_BOX = "stsc";
const char* BoxFactory::STSS_BOX = "stss";
const char* BoxFactory::STSZ_BOX = "stsz";
const char* BoxFactory::STCO_BOX = "stco";
const char* BoxFactory::MVEX_BOX = "mvex";
const char* BoxFactory::MEHD_BOX = "mehd";
const char* BoxFactory::TREX_BOX = "trex";
const char* BoxFactory::TRAF_BOX = "traf";
const char* BoxFactory::TFHD_BOX = "tfhd";
const char* BoxFactory::TFDT_BOX = "tfdt";
const char* BoxFactory::TRUN_BOX = "trun";
const char* BoxFactory::MFRA_BOX = "mfra";
const char* BoxFactory::SKIP_BOX = "skip";
const char* BoxFactory::UDTA_BOX = "udta";
const char* BoxFactory::HNTI_BOX = "hnti";
const char* BoxFactory::HINF_BOX = "hinf";
const char* BoxFactory::TRPY_BOX = "trpy";
const char* BoxFactory::NUMP_BOX = "nump";
const char* BoxFactory::TPYL_BOX = "tpyl";
const char* BoxFactory::TOTL_BOX = "totl";
const char* BoxFactory::NPCK_BOX = "npck";
const char* BoxFactory::TPAY_BOX = "tpay";
const char* BoxFactory::MAXR_BOX = "maxr";
const char* BoxFactory::DMED_BOX = "dmed";
const char* BoxFactory::DIMM_BOX = "dimm";
const char* BoxFactory::DREP_BOX = "drep";
const char* BoxFactory::TMIN_BOX = "tmin";
const char* BoxFactory::TMAX_BOX = "tmax";
const char* BoxFactory::PMAX_BOX = "pmax";
const char* BoxFactory::DMAX_BOX = "dmax";
const char* BoxFactory::PAYT_BOX = "payt";
const char* BoxFactory::RTP_BOX  = "rtp ";
const char* BoxFactory::SDP_BOX  = "sdp ";
const char* BoxFactory::NAME_BOX = "name";
const char* BoxFactory::MDAT_BOX = "mdat";
const char* BoxFactory::FREE_BOX = "free";

BoxFactory::BoxFactory()
{
    //ctor
}

BoxFactory::~BoxFactory()
{
    //dtor
}

Box* BoxFactory::createBox(uint32_t boxSize, const BoxType_t & boxType)
{
    //LOG_V("Find box:'%s'.\n", boxType);

    if (isContainer(boxType))
    {
        //LOG_V("Box '%s' is a container.\n", boxType);
        return new BoxContainer(boxSize, boxType);
    }

	if (isBasicBox(boxType))
	{
		return new Box(boxSize, boxType);
	}

    if (strcmp(boxType, BoxFactory::FTYP_BOX) == 0)
    {
        return new FtypBox(boxSize, boxType);
    }
    else if (strcmp(boxType, BoxFactory::STYP_BOX) == 0)
    {
        return new StypBox(boxSize, boxType);
    }
    else if (strcmp(boxType, BoxFactory::MVHD_BOX) == 0)
    {
        return new MvhdBox(boxSize, boxType);
    }
    else if (strcmp(boxType, BoxFactory::MFHD_BOX) == 0)
    {
        return new MfhdBox(boxSize, boxType);
    }
    else if (strcmp(boxType, BoxFactory::TKHD_BOX) == 0)
    {
        return new TkhdBox(boxSize, boxType);
    }
    else if (strcmp(boxType, BoxFactory::HINT_BOX) == 0)
    {
        return new TrefTypeBox(boxSize, boxType);
    }
    else if (strcmp(boxType, BoxFactory::CDSC_BOX) == 0)
    {
        return new TrefTypeBox(boxSize, boxType);
    }
    else if (strcmp(boxType, BoxFactory::SYNC_BOX) == 0)
    {
        return new TrefTypeBox(boxSize, boxType);
    }
    else if (strcmp(boxType, BoxFactory::MDHD_BOX) == 0)
    {
        return new MdhdBox(boxSize, boxType);
    }
    else if (strcmp(boxType, BoxFactory::DREF_BOX) == 0)
    {
        return new DrefBox(boxSize, boxType);
    }
    else if (strcmp(boxType, BoxFactory::HDLR_BOX) == 0)
    {
        return new HdlrBox(boxSize, boxType);
    }
    else if (strcmp(boxType, BoxFactory::VMHD_BOX) == 0)
    {
        return new VmhdBox(boxSize, boxType);
    }
    else if (strcmp(boxType, BoxFactory::SMHD_BOX) == 0)
    {
        return new SmhdBox(boxSize, boxType);
    }
    else if (strcmp(boxType, BoxFactory::HMHD_BOX) == 0)
    {
        return new HmhdBox(boxSize, boxType);
    }
    else if (strcmp(boxType, BoxFactory::NMHD_BOX) == 0)
    {
        return new NmhdBox(boxSize, boxType);
    }
    else if (strcmp(boxType, BoxFactory::URL_BOX) == 0)
    {
        return new UrlBox(boxSize, boxType);
    }
    else if (strcmp(boxType, BoxFactory::STTS_BOX) == 0)
    {
        return new SttsBox(boxSize, boxType);
    }
    else if (strcmp(boxType, BoxFactory::CTTS_BOX) == 0)
    {
        return new CttsBox(boxSize, boxType);
    }
    else if (strcmp(boxType, BoxFactory::STSC_BOX) == 0)
    {
        return new StscBox(boxSize, boxType);
    }
    else if (strcmp(boxType, BoxFactory::STSS_BOX) == 0)
    {
        return new StssBox(boxSize, boxType);
    }
    else if (strcmp(boxType, BoxFactory::STSD_BOX) == 0)
    {
        return new StsdBox(boxSize, boxType);
    }
    else if (strcmp(boxType, BoxFactory::STSZ_BOX) == 0)
    {
        return new StszBox(boxSize, boxType);
    }
    else if (strcmp(boxType, BoxFactory::STCO_BOX) == 0)
    {
        return new StcoBox(boxSize, boxType);
    }
    else if (strcmp(boxType, BoxFactory::MEHD_BOX) == 0)
    {
        return new MehdBox(boxSize, boxType);
    }
    else if (strcmp(boxType, BoxFactory::TREX_BOX) == 0)
    {
        return new TrexBox(boxSize, boxType);
    }
    else if (strcmp(boxType, BoxFactory::MFHD_BOX) == 0)
    {
        return new MfhdBox(boxSize, boxType);
    }
    else if (strcmp(boxType, BoxFactory::TFHD_BOX) == 0)
    {
        return new TfhdBox(boxSize, boxType);
    }
    else if (strcmp(boxType, BoxFactory::TFDT_BOX) == 0)
    {
        return new TfdtBox(boxSize, boxType);
    }
    else if (strcmp(boxType, BoxFactory::TRUN_BOX) == 0)
    {
        return new TrunBox(boxSize, boxType);
    }
    else if (strcmp(boxType, BoxFactory::TRPY_BOX) == 0)
    {
        return new TrpyBox(boxSize, boxType);
    }
    else if (strcmp(boxType, BoxFactory::TOTL_BOX) == 0)
    {
        return new TotlBox(boxSize, boxType);
    }
    else if (strcmp(boxType, BoxFactory::NUMP_BOX) == 0)
    {
        return new NumpBox(boxSize, boxType);
    }
    else if (strcmp(boxType, BoxFactory::TPYL_BOX) == 0)
    {
        return new TpylBox(boxSize, boxType);
    }
    else if (strcmp(boxType, BoxFactory::NPCK_BOX) == 0)
    {
        return new NpckBox(boxSize, boxType);
    }
    else if (strcmp(boxType, BoxFactory::TPAY_BOX) == 0)
    {
        return new TpayBox(boxSize, boxType);
    }
    else if (strcmp(boxType, BoxFactory::MAXR_BOX) == 0)
    {
        return new MaxrBox(boxSize, boxType);
    }
    else if (strcmp(boxType, BoxFactory::DMED_BOX) == 0)
    {
        return new DmedBox(boxSize, boxType);
    }
    else if (strcmp(boxType, BoxFactory::DIMM_BOX) == 0)
    {
        return new DimmBox(boxSize, boxType);
    }
    else if (strcmp(boxType, BoxFactory::DREP_BOX) == 0)
    {
        return new DrepBox(boxSize, boxType);
    }
    else if (strcmp(boxType, BoxFactory::PMAX_BOX) == 0)
    {
        return new PmaxBox(boxSize, boxType);
    }
    else if (strcmp(boxType, BoxFactory::DMAX_BOX) == 0)
    {
        return new DmaxBox(boxSize, boxType);
    }
    else if (strcmp(boxType, BoxFactory::TMIN_BOX) == 0)
    {
        return new TminBox(boxSize, boxType);
    }
    else if (strcmp(boxType, BoxFactory::TMAX_BOX) == 0)
    {
        return new TmaxBox(boxSize, boxType);
    }
    else if (strcmp(boxType, BoxFactory::PAYT_BOX) == 0)
    {
        return new PaytBox(boxSize, boxType);
    }
    else if (strcmp(boxType, BoxFactory::RTP_BOX) == 0)
    {
        return new RtpBox(boxSize, boxType);
    }
    else if (strcmp(boxType, BoxFactory::SDP_BOX) == 0)
    {
        return new SdpBox(boxSize, boxType);
    }
    else if (strcmp(boxType, BoxFactory::NAME_BOX) == 0)
    {
        return new NameBox(boxSize, boxType);
    }
    else if (strcmp(boxType, BoxFactory::ELST_BOX) == 0)
    {
        return new ElstBox(boxSize, boxType);
    }
	else if (strcmp(boxType, BoxFactory::MDAT_BOX) == 0)
	{
		return new MdatBox(boxSize, boxType);
	}	
    else
    {
        return new Box(boxSize, boxType, true);
    }

    return nullptr;
}

bool BoxFactory::isContainer(const BoxType_t & boxType)
{
    return (
            (strcmp(boxType, BoxFactory::MOOV_BOX) == 0)
           || (strcmp(boxType, BoxFactory::MOOF_BOX) == 0)
           || (strcmp(boxType, BoxFactory::TRAK_BOX) == 0)
           || (strcmp(boxType, BoxFactory::TREF_BOX) == 0)
           || (strcmp(boxType, BoxFactory::MDIA_BOX) == 0)
           || (strcmp(boxType, BoxFactory::EDTS_BOX) == 0)
           || (strcmp(boxType, BoxFactory::MINF_BOX) == 0)
           || (strcmp(boxType, BoxFactory::DINF_BOX) == 0)
           || (strcmp(boxType, BoxFactory::STBL_BOX) == 0)
           || (strcmp(boxType, BoxFactory::MVEX_BOX) == 0)
           || (strcmp(boxType, BoxFactory::TRAF_BOX) == 0)
           || (strcmp(boxType, BoxFactory::MFRA_BOX) == 0)
           || (strcmp(boxType, BoxFactory::SKIP_BOX) == 0)
           || (strcmp(boxType, BoxFactory::UDTA_BOX) == 0)
           || (strcmp(boxType, BoxFactory::HNTI_BOX) == 0)
           || (strcmp(boxType, BoxFactory::HINF_BOX) == 0)
          );
}

bool BoxFactory::isBasicBox(const BoxType_t & boxType)
{
	return (
             (strcmp(boxType, BoxFactory::FREE_BOX) == 0)
          || (strcmp(boxType, BoxFactory::TPAY_BOX) == 0)
          || (strcmp(boxType, BoxFactory::MAXR_BOX) == 0)
          || (strcmp(boxType, BoxFactory::DMED_BOX) == 0)
          || (strcmp(boxType, BoxFactory::DIMM_BOX) == 0)
          || (strcmp(boxType, BoxFactory::DREP_BOX) == 0)
          || (strcmp(boxType, BoxFactory::PMAX_BOX) == 0)
          || (strcmp(boxType, BoxFactory::DMAX_BOX) == 0)
            );
}

}//namespace  mp4Parser
