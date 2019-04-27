## Welcom to MP4Parser
### Synopsis
MP4Parser can read raw data from a mp4 file, then parse out mp4 box in compliance of ISO_IEC_14496-12_2004(E). It will store all the boxes in a list, and provide a series of function so that you can easily find the box you instersted in. It also can output all the box information to stdout or file with beauty style. Finally, it's very easy to add new mp4 box to MP4Parser module.    
Until now MP4Parser support the following mp4 boxes:
- audio_sample_entry_box
- mfhdbox
- stypbox
- tfdtbox                 
- tkhdbox           
- mehdbox
- mvhdbox
- tfhdbox
- namebox           
- cttsbox
- nmhdbox
- tmaxbox
- dimmbox
- npckbox
- tminbox
- dmaxbox
- numpbox
- totlbox
- dmedbox
- paytbox
- tpaybox
- drefbox
- pmaxbox
- tpylbox
- drepbox
- rtpbox
- tref_typebox
- elstbox
- sample_entry_box
- trexbox
- ftypbox
- sdpbox
- trpybox
- hdlrbox
- smhdbox
- trunbox
- hint_sample_entry_box
- stcobox
- urlbox
- hmhdbox
- stscbox
- sttsbox
- maxrbox
- stsdbox
- visual_sample_entry_box
- mdatbox
- stssbox
- vmhdbox
- mdhdbox
- stszbox
### How to use MP4Parser
**1. Transfer the box list:**    
   **(1).Use MP4Parser's transfer function:**
```
    int TransferFunc(BoxPtr_t& box, void *p_userData)
    {
        int count = 0;
        if (strcmp(box->type(), mp4Parser::BoxFactory::MOOV_BOX) == 0)
        {
            count++;
        }
        
        int *p_count = static_cast<int*>p_userData;
        *p_count = count;
        
        return 0;
    }
    
    int main()
    {
        MP4Parser mp4Parser;
        int count = 0;
        
        mp4Parser.parse("/your/mp4file/path/file.mp4");
        mp4Parser.transfer(TransferFunc, &count);
        cout << "Moov box count:" << count << endl;        
    }
```   
   **(2).Use your own transfer funciton:**
```    
    void TransferBox(const BoxPtr_t &box)
    {
        //do something;

        if (!box->hasChildren())
        {
            return;
        }

        BoxPtr_t child = box->firstChild();
        while (child)
        {
            TransferBox(child);
            child = child->nextSibling();
        }

    }
    
    int main()
    {	
        MP4Parser mp4Parser;
        mp4Parser.parse("/your/mp4file/path/file.mp4");
        
        BoxPtr_t box = mp4Parser.getFirstBox();
        while(box)
        {
            TransferBox(box);
            box = box->nextSibling();
        }
    }
```    

**2. Find your intersted box:**
```
    int main()
    {
        MP4Parser mp4Parser;
        BoxPtr_t trakBox;
        int trakCount = 0;        
        
        mp4Parser.parse("/your/mp4file/path/file.mp4");

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
```
**3. Output box information:**
```
    int main()
    {
        MP4Parser mp4Parser;
        BoxPtr_t trakBox;       
        
        mp4Parser.parse("/your/mp4file/path/file.mp4"); 
        mp4Parser.outputBoxList();//Output to the stdout.
        
        mp4Parser.setDstFile("/your/resultfile/path/resut.txt");
        mp4Parser.outputBoxList();//Output to a file.
        
        mp4Parser.redirectOut2File();
        cout << "This line will be written into the result file." << endl;
        mp4Parser.outputBoxList();//Output to a file.
    
        mp4Parser.restore2Stdout();
        cout << "This line will not be written into the result file." << endl;
        
        mp4Parser.setDstFile("");
        mp4Parser.outputBoxList();//Output to the stdout again.        
    }
```
    
**4. Add new box:**   
   (1).Add a new box class, if it is a container box, then inherits from class BoxContainer,
       or else inherits from class Box.  
   (2).Implement the virtual function parseBody() and showBody().   
   (3).Implment other private function of its own.  
   (4).Add new box name variable to BoxFactory class in boxfactory.h, for example:
       static const char* NEW_BOX;  
   (5).Implement the new box name in BoxFactory class in boxfactory.cpp, for example:
       const char* BoxFactory::FREE_BOX = "new_box";  
   (6).Return a new object from createBox() when use the new box name to create it.  
      
**5.How to compile:**   
   (1).You can use CodeBlocks to open the Mp4Parser.cbp to compile on Window platform.   
   (2).You also can use make command to compile on Linux platform.
