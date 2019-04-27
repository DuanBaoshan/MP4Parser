#include <inttypes.h>
#include <iostream>

#include "debug.h"

#include "mp4parser.h"

using namespace std;
using namespace mp4Parser;

typedef struct BoxCount
{
    int moovCount;
    int moofCount;
    int mediaCount;
    int mvhdCount;
}BoxCount_t;


static int OutputStatistic(MP4Parser& mp4Parser);

static int OutputTrackInfo(MP4Parser& mp4Parser);

static int OutputMoofInfo(MP4Parser& mp4Parser);

static int OutputMdat(MP4Parser& mp4Parser);

static int TransferFunc(BoxPtr_t& box, void *p_userData);


int main(int argc, char *argv[])
{
    cout << "Hello Mp4Parser!" << endl;

    if (argc < 2)
    {
        cout << "usage: Mp4Parser mp4_file [output_file]" << endl;
        return -1;
    }

    MP4Parser parser;

    cout << "Will parse file:'" << argv[1] << "'..." << endl;
	parser.parse(argv[1]);
    cout << "End parse file:'" << argv[1] << "'." << endl;

    cout << endl << "Begin output result..." << endl;
	std::string outputFile = "";
    if (argc >= 3)
    {
		outputFile = argv[2];
    }

	parser.setDstFile(outputFile);
	OutputStatistic(parser);
    parser.outputBoxList();
	OutputMdat(parser);

    cout << "End output result." << endl;

    BoxCount_t boxCount;
    memset(&boxCount, 0, sizeof(BoxCount_t));
    parser.transfer(TransferFunc, &boxCount);

    cout << "moovCount:" << boxCount.moovCount << ", moofCount:"
         << boxCount.moofCount << ", mediaCount:" << boxCount.mediaCount << ", mvhdCount:" << boxCount.mvhdCount << endl;

    return 0;
}

static int OutputStatistic(MP4Parser& mp4Parser)
{

	mp4Parser.redirectOut2File();

	cout << "File:'" << mp4Parser.srcFile() << "'" << endl;
	cout << "*******************************" << endl;
    OutputTrackInfo(mp4Parser);
    OutputMoofInfo(mp4Parser);
    cout << "*******************************" << endl;

	mp4Parser.restore2Stdout();

    return 0;
}

static int OutputTrackInfo(MP4Parser& mp4Parser)
{
    BoxPtr_t trakBox;
    int trakCount = 0;

    trakCount = mp4Parser.getBoxCount("trak");
    cout << "There are " << trakCount << " tracks." << endl;


    trakBox = mp4Parser.getBox("trak");
    while (trakBox)
    {
        BoxPtr_t tkhdBox = trakBox->findBoxInChildren("tkhd");
        if (tkhdBox)
        {
            TkhdBoxPtr_t tkhdBoxPtr = std::dynamic_pointer_cast<TkhdBox>(tkhdBox);
            cout << "Track ID:" << tkhdBoxPtr->trackID() << endl;
            cout << "    Duration:" << tkhdBoxPtr->duration() << endl;
        }

        BoxPtr_t hdlrBox = trakBox->findBoxInChildren("hdlr");
        if (tkhdBox)
        {
            HdlrBoxPtr_t hdlrBoxPtr = std::dynamic_pointer_cast<HdlrBox>(hdlrBox);
            uint32_t hdlrType = hdlrBoxPtr->handlerType();
            cout << "    Handler type:" << HdlrBox::HANDLER_TYPE_MAP[(HandlerType_e)hdlrType].c_str()
				 << "(0x" << hex << uppercase << hdlrType << ")" << dec << endl;
            cout << "    Handler Name:" << hdlrBoxPtr->name() << endl;
        }

        BoxPtr_t mdhdBox = trakBox->findBoxInChildren("mdhd");
        if (mdhdBox)
        {
            MdhdBoxPtr_t mdhdBoxPtr = std::dynamic_pointer_cast<MdhdBox>(mdhdBox);
            cout << "    Language:'" << mdhdBoxPtr->language() << "'" << endl;
        }

        trakBox = mp4Parser.getNextBox(trakBox);
    }

    return 0;
}

static int OutputMoofInfo(MP4Parser& mp4Parser)
{
    int boxCount = 0;

    boxCount = mp4Parser.getBoxCount("moof");
    if (boxCount == 0)
    {
        return 0;
    }

    cout << endl << "There are " << boxCount << " moof boxes." << endl;
    BoxPtr_t moofBox = mp4Parser.getBox("moof");
    while (moofBox)
    {
        BoxPtr_t mfhdBox = moofBox->findBoxInChildren("mfhd");
        MfhdBoxPtr_t mfhdBoxPtr = std::dynamic_pointer_cast<MfhdBox>(mfhdBox);
        cout << "    SequenceNumber:" <<  mfhdBoxPtr->sequenceNumber() << endl;

        int trafCount = moofBox->getBoxCountInChildren("traf");
        cout << "    There are:" << trafCount << " traf." << endl;

        BoxPtr_t trafBox = moofBox->findBoxInChildren("traf");
        for (int i = 0; i < trafCount; i++)
        {
            BoxPtr_t tfhdBox = trafBox->findBoxInChildren("tfhd");
            TfhdBoxPtr_t tfhdBoxPtr = std::dynamic_pointer_cast<TfhdBox>(tfhdBox);
            cout << "    Traf " << i + 1 << endl;
            cout << "        trackID:" << tfhdBoxPtr->trackID() << endl;
            if (tfhdBoxPtr->baseDataOffsetPresent())
            {
                cout << "        baseDataOffset:" << tfhdBoxPtr->baseDataOffset() << endl;
            }
            if (tfhdBoxPtr->defaultSampleFlagsPresent())
            {
                cout << "        defaultSampleFlags:0x" << hex << uppercase << tfhdBoxPtr->defaultSampleFlags() << dec << endl;
            }

            int trunCount = trafBox->getBoxCountInChildren("trun");
            cout << "        There are:" << trunCount << " trun." << endl;
            BoxPtr_t trunBox = trafBox->findBoxInChildren("trun");
            for (int j = 0; j < trunCount; j++)
            {
                TrunBoxPtr_t trunBoxPtr = std::dynamic_pointer_cast<TrunBox>(trunBox);
                cout << "        trun " << j + 1 << ":" << endl;
                cout << "            sampleCount:" << trunBoxPtr->sampleCount() << endl;
                if (trunBoxPtr->dataOffsetPresent())
                {
                    cout << "            dataOffset:" << trunBoxPtr->dataOffset() << endl;
                }

                trunBox = trafBox->findNextBoxInChildren(trunBox);
            }

            trafBox = moofBox->findNextBoxInChildren(trafBox);
        }

        cout << endl;

        moofBox = mp4Parser.getNextBox(moofBox);
    }

    return 0;
}

static int OutputMdat(MP4Parser& mp4Parser)
{
	BoxPtr_t box = mp4Parser.getBox("mdat");
	if (box)
	{
		BoxBodyPtr_t body;
		MdatBoxPtr_t mdatBox = std::dynamic_pointer_cast<MdatBox>(box);

		mdatBox->getActualBody(body);

		mp4Parser.redirectOut2File();

		cout << endl << endl <<"Mdat body size:" << body->bodySize << endl;
		int bodySize = body->bodySize > 128 ? 128 : body->bodySize;

		for (int i = 0; i < bodySize; i++)
		{
			if (i != 0 && (i % 8) == 0)
			{
				cout << endl;
			}

			int value = body->p_body[i];
			cout << _HEX_2_DIGIT_ << value << " " << dec;
		}

		mp4Parser.restore2Stdout();
	}

	cout << endl;
	return 0;
}

static int TransferFunc(BoxPtr_t& box, void *p_userData)
{
    BoxCount_t *p_boxCount = static_cast<BoxCount_t*>(p_userData);
    if (strcmp(box->type(), mp4Parser::BoxFactory::MOOV_BOX) == 0)
    {
        p_boxCount->moovCount++;
    }
    else if (strcmp(box->type(), mp4Parser::BoxFactory::MOOF_BOX) == 0)
    {
        p_boxCount->moofCount++;
    }
    else if (strcmp(box->type(), mp4Parser::BoxFactory::MDIA_BOX) == 0)
    {
        p_boxCount->mediaCount++;
    }
    else if (strcmp(box->type(), mp4Parser::BoxFactory::MVHD_BOX) == 0)
    {
        p_boxCount->mvhdCount++;
    }

    return 0;
}

