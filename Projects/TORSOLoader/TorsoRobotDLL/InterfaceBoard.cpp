//
// InterfaceBoard.c: ƒCƒ“ƒ^[ƒtƒF[ƒXĞƒ{[ƒh—pƒNƒ‰ƒX’è‹`
// 2007/7/27 Torso—p‚É‘å•‘‚«Š·‚¦(íœ)
//  New DeleteéŒ¾‚ª‚È‚º‚©‚¤‚Ü‚­“®‚©‚È‚©‚Á‚½‚Ì‚Åíœ->Œˆ‚ß‘Å‚¿‚ÅéŒ¾
//

//#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <windows.h>
#include "InterfaceBoard.h"
#include "variables.h"
#include <string.h>
#include <math.h>
//#include <assert.h>
#include <iostream>
#include <stdlib.h>
#include <fstream>

using namespace std;

#pragma warning ( disable : 4996 )  // VS2005‚Å‚ÍŒx‚³‚ê‚é‚Ì‚Å–³Œø‰»

//--------------------------------
//ƒCƒ“ƒ^[ƒtƒF[ƒXĞDAƒ{[ƒhƒNƒ‰ƒX
//ˆø”FƒfƒoƒCƒX–¼(—áF"FBIDA1")Aƒ`ƒƒƒlƒ‹”(—áF4)AƒŒƒ“ƒW(—áFDA_10V,FBIDA.h‚Å’è‹`)
//--------------------------------
InterfaceDABoard::InterfaceDABoard(char *name, int ChNum, int Range)
{
    int n;
    ChannelNum = ChNum;

    strcpy(DeviceName,name);
    DeviceHandle=DaOpen((LPCTSTR)DeviceName);
    if(DeviceHandle == INVALID_HANDLE_VALUE){
        DaClose(DeviceHandle);
        cout << "Error. Can't Open Device" << DeviceName << endl;
        exit(-1);
    }
    DaGetDeviceInfo(DeviceHandle,&DeviceInfo);
    DaGetSamplingConfig(DeviceHandle, &DeviceConfig);
    for(n=0;n<ChannelNum;n++){
        Volt[n]=0.0;
    }
    //ƒ`ƒƒƒlƒ‹İ’èBƒ`ƒƒƒlƒ‹”AƒŒƒ“ƒW,ƒTƒ“ƒvƒ‹ƒ‚[ƒhAü”g”,ŒJ‚è•Ô‚µ‰ñ”B

    DeviceConfig.ulChCount=ChNum;
    for(n=0 ; n<ChannelNum ; n++){
        DeviceConfig.SmplChReq[n].ulChNo = n+1;
        DeviceConfig.SmplChReq[n].ulRange = Range;
    }
    DeviceConfig.ulSamplingMode = DA_IO_SAMPLING;
    DeviceConfig.fSmplFreq =10000.0;
    DeviceConfig.ulSmplRepeat = 0;
    DeviceConfig.ulTrigMode = DA_FREERUN;
    DaSetSamplingConfig(DeviceHandle , &DeviceConfig );
}

// ƒRƒ“ƒXƒgƒ‰ƒNƒ^‚ÌƒI[ƒo[ƒ[ƒhì¬(•ÏXFWatanabe)
// 

InterfaceDABoard::InterfaceDABoard(char *name)
{
    strcpy(DeviceName,name);
    DeviceHandle=DaOpen((LPCTSTR)DeviceName);
    if(DeviceHandle == INVALID_HANDLE_VALUE){
        DaClose(DeviceHandle);
        cout << "Error. Can't Open Device" << DeviceName << endl;
        exit(-1);
    }
    DaGetDeviceInfo(DeviceHandle,&DeviceInfo);

}

// initialize‚ğƒRƒ“ƒXƒgƒ‰ƒNƒ^‚©‚ç•ª—£
// ƒ`ƒƒƒ“ƒlƒ‹‘g‚Ì“ñŸŒ³”z—ñ‚ğ—^‚¦‚ÄAˆêŠ‡æ“¾‚µ‚½‚¢ƒ`ƒƒƒ“ƒlƒ‹‘g•ª
// ‚¾‚¯\‘¢‘Ì‚ğì¬‚·‚é
// ¡‰ñ‚Í•ªŠ„‚È‚µ‚È‚Ì‚ÅˆêŸŒ³”z—ñ‚É‚µ‚Ä‚¢‚é‚±‚Æ‚É’ˆÓII
// (”Ä—p«‚ğ‚½‚¹‚é‚È‚ç‚ÎAD‚Æ“¯—l‚É**ChArray, *ChNum‚Å’è‹`‚·‚×‚«)
void InterfaceDABoard::initialize(int *ChArray, int ChNum, int Range)
{
    int n;
	ChannelNum = ChNum;

	for(n=0;n<ChannelNum;n++){
        Volt[n]=0.0;
    }
    //ƒ`ƒƒƒlƒ‹İ’èBƒ`ƒƒƒlƒ‹”AƒŒƒ“ƒW,ƒTƒ“ƒvƒ‹ƒ‚[ƒhAü”g”,ŒJ‚è•Ô‚µ‰ñ”B

    DeviceConfig.ulChCount=ChNum;
    for(n=0 ; n<ChannelNum ; n++){
        DeviceConfig.SmplChReq[n].ulChNo = ChArray[n];
        DeviceConfig.SmplChReq[n].ulRange = Range;
    }
    DeviceConfig.ulSamplingMode = DA_IO_SAMPLING;
    DeviceConfig.fSmplFreq =10000.0;
    DeviceConfig.ulSmplRepeat = 0;
    DeviceConfig.ulTrigMode = DA_FREERUN;
    DaSetSamplingConfig(DeviceHandle , &DeviceConfig );
}


//‚ ‚éƒ`ƒƒƒlƒ‹‚ÌƒŒƒ“ƒW‚ğİ’èB‚à‚µ‚»‚Ìƒ`ƒƒƒlƒ‹‚ªŠJ‚¢‚Ä‚¢‚È‚¯‚ê‚ÎŠJ‚­
void InterfaceDABoard::SetChannel(int Channel, int Range)
{
    if(Channel > ChannelNum){
        ChannelNum = Channel;
        DeviceConfig.ulChCount=ChannelNum;
    }
    DeviceConfig.SmplChReq[Channel].ulChNo = Channel+1;
    DeviceConfig.SmplChReq[Channel].ulRange = Range;
    DaSetSamplingConfig(DeviceHandle , &DeviceConfig );
}


InterfaceDABoard::~InterfaceDABoard(void)
{
    DaClose(DeviceHandle);
}


//ƒ{[ƒh‚Ìó‘Ô‚ğ•\¦
void InterfaceDABoard::Show(void)
{
    cout << "Board Information" << endl
         << "Type " <<DeviceInfo.ulBoardType << endl
         << "ID " << DeviceInfo.ulBoardID << endl // ƒ{[ƒh¯•Ê”Ô†
         << "Channel Number " << DeviceInfo.ulChCount << endl// ƒ{[ƒhƒAƒiƒƒOo—Íƒ`ƒƒƒ“ƒlƒ‹”
         << "Resolution " << DeviceInfo.ulResolution << endl// ƒ{[ƒh•ª‰ğ”\
         << "Digital Input Number " << DeviceInfo.ulDi << endl// ƒ{[ƒhƒfƒWƒ^ƒ‹“ü—Í“_”
         << "Digital Output Number " << DeviceInfo.ulDo << endl;// ƒ{[ƒhƒfƒWƒ^ƒ‹o—Í“_”
}

//
//Channel‚ÉValue[V]‚ğo—Í‚µ‚Â‚ÂA‚»‚êˆÈŠO‚ÌChannel‚Ì’l‚Í•Û‚·‚éB
//
void InterfaceDABoard::DAOut(int Channel,double Value)
{
	UINT MaxCh=DeviceConfig.ulChCount;
	UINT MaxSmplNum=1;

	if(Channel>(int)MaxCh-1 ||Channel<0){
        cout << "No such DA Channel " << Channel << endl;
        exit(-1);
	}

	Volt[Channel]=(float)Value;

	//o—Í
	switch(DeviceInfo.ulResolution){
		case 8 :
			DaDataConv(DA_DATA_PHYSICAL,Volt,MaxSmplNum,&DeviceConfig,
						DA_DATA_BIN8,Data8,&MaxSmplNum,&DeviceConfig,0,0,NULL);
			nRet=DaOutputDA(DeviceHandle,MaxCh,DeviceConfig.SmplChReq,Data8);
			break;
		case 12:
			DaDataConv(DA_DATA_PHYSICAL,Volt,MaxSmplNum,&DeviceConfig,
						DA_DATA_BIN12,Data12,&MaxSmplNum,&DeviceConfig,0,0,NULL);
			nRet=DaOutputDA(DeviceHandle,MaxCh,DeviceConfig.SmplChReq,Data12);
			break;
		case 16:
			DaDataConv(DA_DATA_PHYSICAL,Volt,MaxSmplNum,&DeviceConfig,
						DA_DATA_BIN16,Data16,&MaxSmplNum,&DeviceConfig,0,0,NULL);
			nRet=DaOutputDA(DeviceHandle,MaxCh,DeviceConfig.SmplChReq,Data16);
			break;
		case 24:
			DaDataConv(DA_DATA_PHYSICAL,Volt,MaxSmplNum,&DeviceConfig,
						DA_DATA_BIN24,Data24,&MaxSmplNum,&DeviceConfig,0,0,NULL);
			nRet=DaOutputDA(DeviceHandle,MaxCh,DeviceConfig.SmplChReq,Data24);
			break;
	}

    if(nRet!=0){
        DisplayErrorMessage();
        exit(-1);
    }
}


//”z—ñValue(•¨——Ê[V])‚ğo—ÍBƒ`ƒƒƒlƒ‹“¯
//”z—ñƒTƒCƒY‚Íƒ`ƒƒƒlƒ‹”•ª–³‚¯‚ê‚Î‚È‚ç‚È‚¢
//(“Á‚ÉƒGƒ‰[ˆ—‚µ‚Ä‚¢‚È‚¢‚Ì‚Å—eˆÕ‚ÉSegmentation Fault‚·‚éB’ˆÓj
void InterfaceDABoard::DAOut(double* Value)
{
	UINT MaxCh=DeviceConfig.ulChCount;
	UINT MaxSmplNum=1;

    //Segmentation Fault‹N‚«‚â‚·‚¢‚Æ‚±‚ëBƒ`ƒƒƒlƒ‹”•ªƒf[ƒ^‚ğw’è‚µ‚Ä‚¢‚é‚±‚Æ‚ğŠm”F‚¹‚æ
    for(int n=0;n<(int)MaxCh;n++){
        Volt[n]=(float)Value[n];
    }

	//o—Í
	switch(DeviceInfo.ulResolution){
		case 8 :
			DaDataConv(DA_DATA_PHYSICAL,Volt,MaxSmplNum,&DeviceConfig,
						DA_DATA_BIN8,Data8,&MaxSmplNum,&DeviceConfig,0,0,NULL);
			nRet=DaOutputDA(DeviceHandle,MaxCh,DeviceConfig.SmplChReq,Data8);
			break;
		case 12:
			nRet=DaDataConv(DA_DATA_PHYSICAL,Volt,MaxSmplNum,&DeviceConfig,
						DA_DATA_BIN12,Data12,&MaxSmplNum,&DeviceConfig,0,0,NULL);
			nRet=DaOutputDA(DeviceHandle,MaxCh,DeviceConfig.SmplChReq,Data12);
			break;
		case 16:
			DaDataConv(DA_DATA_PHYSICAL,Volt,MaxSmplNum,&DeviceConfig,
						DA_DATA_BIN16,Data16,&MaxSmplNum,&DeviceConfig,0,0,NULL);
			nRet=DaOutputDA(DeviceHandle,MaxCh,DeviceConfig.SmplChReq,Data16);
			break;
		case 24:
			DaDataConv(DA_DATA_PHYSICAL,Volt,MaxSmplNum,&DeviceConfig,
						DA_DATA_BIN24,Data24,&MaxSmplNum,&DeviceConfig,0,0,NULL);
			nRet=DaOutputDA(DeviceHandle,MaxCh,DeviceConfig.SmplChReq,Data24);
			break;
	}

    if(nRet!=0){
        DisplayErrorMessage();
        exit(-1);
    }
}

//ƒGƒ‰[•\¦F‚±‚Ì•Ó‚Ì’l‚Ígpcda.h“à‚É’è‹`‚³‚ê‚Ä‚¢‚é
void InterfaceDABoard::DisplayErrorMessage()
{
    cout <<"DA Error: ";
    switch(nRet) {
        case DA_ERROR_NOT_DEVICE:
            cout << "The device couldn't be found."<<endl;
            break;
        case DA_ERROR_NOT_OPEN:
            cout << "The system couldn't found the device."<<endl;
            break;
        case DA_ERROR_INVALID_HANDLE:
            cout << "Invalid device handle is specified."<<endl;
            break;
        case DA_ERROR_ALREADY_OPEN:
            cout << "The device has been already opened."<<endl;
            break;
        case DA_ERROR_NOT_SUPPORTED:
            cout << "It is not supported."<<endl;
            break;
        case DA_ERROR_NOW_SAMPLING:
            cout << "The analog output is running."<<endl;
            break;
        case DA_ERROR_STOP_SAMPLING:
            cout << "The analog output is stopped."<<endl;
            break;
        case DA_ERROR_START_SAMPLING:
            cout << "The analog output couldn't be performed."<<endl;
            break;
        case DA_ERROR_SAMPLING_TIMEOUT:
            cout << "The timeout interval elapsed while the analog output is running."<<endl;
            break;
        case DA_ERROR_INVALID_PARAMETER:
            cout << "Invalid parameters are specified."<<endl;
            break;
        case DA_ERROR_ILLEGAL_PARAMETER:
            cout << "Invalid analog output conditions are specified."<<endl;
            break;
        case DA_ERROR_NULL_POINTER:
            cout << "A NULL pointer is specified."<<endl;
            break;
        case DA_ERROR_SET_DATA:
            cout << "The analog output data couldn't be configured."<<endl;
            break;
        case DA_ERROR_FILE_OPEN:
            cout << "Opening the file failed."<<endl;
            break;
        case DA_ERROR_FILE_CLOSE:
            cout << "Closing the file failed."<<endl;
            break;
        case DA_ERROR_FILE_READ:
            cout << "Reading the file failed."<<endl;
            break;
        case DA_ERROR_FILE_WRITE:
            cout << "Writing the file failed."<<endl;
            break;
        case DA_ERROR_INVALID_DATA_FORMAT:
            cout << "Invalid data format is specified."<<endl;
            break;
        case DA_ERROR_INVALID_AVERAGE_OR_SMOOTHING:
            cout << "Invalid averaging configuration or invalid smoothing configuration is specified."<<endl;
            break;
        case DA_ERROR_INVALID_SOURCE_DATA:
            cout <<  "Invalid source data are specified."<<endl;
            break;
        case DA_ERROR_NOT_ALLOCATE_MEMORY:
            cout << "Enough memory couldn't be allocated."<<endl;
            break;
        case DA_ERROR_NOT_LOAD_DLL:
            cout << "Loading the DLL failed."<<endl;
            break;
        case DA_ERROR_CALL_DLL:
            cout << "Calling the DLL failed."<<endl;
            break;
        default:
            cout << "Unexpected error is occurred."<<endl;
            break;
    }

}


//ƒGƒ‰[•\¦F‚±‚Ì•Ó‚Ì’l‚Ífbienc.h“à‚É’è‹`‚³‚ê‚Ä‚¢‚é
void InterfaceCNTBoard::DisplayErrorMessage()
{
    cout << "CNTBoard::";
    switch(nRet){
        case PENC_ERROR_SUCCESS:   // ³íI—¹
            cout << "³íI—¹"<<endl;
            break;
        case PENC_ERROR_NOT_DEVICE:
            cout << "w’è‚³‚ê‚½ƒfƒoƒCƒX‚ğŒ©‚Â‚¯‚é‚±‚Æ‚ª‚Å‚«‚Ü‚¹‚ñ"<<endl;
            break;
        case PENC_ERROR_NOT_OPEN:
            cout << "ƒVƒXƒeƒ€‚ªƒfƒoƒCƒX‚ğƒI[ƒvƒ“‚Å‚«‚Ü‚¹‚ñ"<<endl;
            break;
        case PENC_ERROR_INVALID_HANDLE:
            cout << "ƒfƒoƒCƒXƒnƒ“ƒhƒ‹‚ª³‚µ‚­‚ ‚è‚Ü‚¹‚ñ"<<endl;
            break;
        case PENC_ERROR_ALREADY_OPEN:
            cout << "‚·‚Å‚ÉOPEN‚µ‚Ä‚¢‚éƒfƒoƒCƒX‚ğOPEN‚µ‚æ‚¤‚Æ‚µ‚Ü‚µ‚½"<<endl;
            break;
        case PENC_ERROR_HANDLE_EOF:
            cout << "EOF‚É’B‚µ‚Ü‚µ‚½"<<endl;
            break;
        case PENC_ERROR_MORE_DATA:
            cout << "‚³‚ç‚É‘½‚­‚Ìƒf[ƒ^‚ª—˜—p‰Â”\‚Å‚·"<<endl;
            break;
        case PENC_ERROR_INSUFFICIENT_BUFFER:
            cout << "ƒVƒXƒeƒ€ƒR[ƒ‹‚É“n‚³‚ê‚½ƒf[ƒ^—Ìˆæ‚ª¬‚³‚·‚¬‚Ü‚·"<<endl;
            break;
        case PENC_ERROR_IO_PENDING:
            cout << "”ñ“¯ŠúI/O‘€ì‚ªis’†‚Å‚·"<<endl;
            break;
        case PENC_ERROR_NOT_SUPPORTED:
            cout << "ƒTƒ|[ƒg‚³‚ê‚Ä‚¢‚È‚¢‹@”\‚Å‚·"<<endl;
            break;
        case PENC_ERROR_INITIALIZE_IRQ:
            cout << "Š„‚è‚İ‚Ì‰Šú‰»‚É¸”s‚µ‚Ü‚µ‚½"<<endl;
            break;
        case PENC_ERROR_INVALID_CHANNEL:
            cout << "•s³‚Èƒ`ƒƒƒ“ƒlƒ‹‚ğw’è‚µ‚Ü‚µ‚½"<<endl;
            break;
        case PENC_ERROR_INVALID_MODE:
            cout << "•s³‚Èƒ‚[ƒh‚ğw’è‚µ‚Ü‚µ‚½"<<endl;
            break;
        case PENC_ERROR_INVALID_DIRECT:
            cout << "•s³‚ÈƒJƒEƒ“ƒ^•ûŒü‚ğw’è‚µ‚Ü‚µ‚½"<<endl;
            break;
        case PENC_ERROR_INVALID_COUNTER:
            cout << "•s³‚ÈƒJƒEƒ“ƒ^’l‚ğw’è‚µ‚Ü‚µ‚½"<<endl;
            break;
        case PENC_ERROR_INVALID_COMPARATOR:
            cout << "•s³‚È”äŠrƒJƒEƒ“ƒ^‚ğw’è‚µ‚Ü‚µ‚½"<<endl;
            break;
        case PENC_ERROR_INVALID_ZMODE:
            cout << "•s³‚È‚y‘Š˜_—’l‚ğw’è‚µ‚Ü‚µ‚½"<<endl;
            break;
        case PENC_ERROR_INVALID_MASK:
            cout << "•s³‚ÈƒCƒxƒ“ƒgƒ}ƒXƒN’l‚ğw’è‚µ‚Ü‚µ‚½"<<endl;
            break;
        case PENC_ERROR_INVALID_ITIMER:
            cout << "•s³‚ÈƒCƒ“ƒ^[ƒoƒ‹ƒ^ƒCƒ}İ’è’l‚ğw’è‚µ‚Ü‚µ‚½"<<endl;
            break;
        case PENC_ERROR_ALREADY_REGISTRATION:
            cout << "ƒCƒxƒ“ƒg‚Í‚·‚Å‚É“o˜^Ï‚İ‚Å‚·"<<endl;
            break;
        case PENC_ERROR_ALREADY_DELETE:
            cout << "ƒCƒxƒ“ƒg‚Í‚·‚Å‚Éíœ‚³‚ê‚Ä‚¢‚Ü‚·"<<endl;
            break;
        case PENC_ERROR_MEMORY_NOTALLOCATED:
            cout << "ì‹Æ—pƒƒ‚ƒŠ‚ÌŠm•Û‚É¸”s‚µ‚Ü‚µ‚½"<<endl;
            break;
        case PENC_ERROR_MEMORY_FREE:
            cout << "ƒƒ‚ƒŠ‚Ì‰ğ•ú‚É¸”s‚µ‚Ü‚µ‚½"<<endl;
            break;
        case PENC_ERROR_TIMER:
            cout << "ƒ^ƒCƒ}ƒŠƒ\[ƒX‚Ìæ“¾‚É¸”s‚µ‚Ü‚µ‚½"<<endl;
            break;
        case PENC_ERROR_DRVCAL:
            cout << "ƒhƒ‰ƒCƒo‚ªŒÄ‚Ño‚¹‚Ü‚¹‚ñ"<<endl;
            break;
        case PENC_ERROR_NULL_POINTER:
            cout << "ƒhƒ‰ƒCƒoADLLŠÔ‚ÅNULLƒ|ƒCƒ“ƒ^‚ª“n‚³‚ê‚Ü‚µ‚½"<<endl;
            break;
        case PENC_ERROR_PARAMETER:
            cout << "ˆø”ƒpƒ‰ƒ[ƒ^‚Ì’l‚ª•s³‚Å‚·"<<endl;
            break;
        default:
            cout << "—\Šú‚µ‚È‚¢ƒGƒ‰[‚ª”­¶‚µ‚Ü‚µ‚½"<<endl;
            break;
    }
}


//--------------------------------
//ƒCƒ“ƒ^[ƒtƒF[ƒXĞƒJƒEƒ“ƒ^ƒ{[ƒhƒNƒ‰ƒX
//ˆø”FƒfƒoƒCƒX–¼(—áF"FBIPENC1")Aƒ`ƒƒƒlƒ‹”(—áF2)
//PCI-6204,PCI-6205Œn—ñê—p
//--------------------------------
InterfaceCNTBoard::InterfaceCNTBoard(char *name, int Channel)
{
    ChannelNum = Channel;

    strcpy(DeviceName,name);
    DeviceHandle=PencOpen((LPCTSTR)DeviceName, PENC_FLAG_SHARE);
    if(DeviceHandle == INVALID_HANDLE_VALUE){
        PencClose(DeviceHandle);
        cout << "Error. Can't Open Device" << DeviceName << endl;
        exit(-1);
    }
    // ƒfƒtƒHƒ‹ƒg‚Ì‰Šú‰»î•ñ
    // ƒJƒEƒ“ƒgƒAƒbƒvAˆê’vŒŸo‚µ‚È‚¢Aƒ\ƒtƒgƒ‰ƒbƒ`AˆÊ‘Š·“ü—Íƒpƒ‹ƒXA‚S’ü”{AZ‘ŠƒNƒŠƒA‚¹‚¸
    InitMode(CountUp | NotUseEqual |SoftLatch , DiffPhasePulse | Multi_4 , NotClear);
}

InterfaceCNTBoard::~InterfaceCNTBoard(void)
{
    PencClose(DeviceHandle);
}

// ƒ`ƒƒƒ“ƒlƒ‹‚Ì‰Šú‰»İ’è(nChƒ`ƒƒƒ“ƒlƒ‹‚ğCountMode‚Åİ’è‚³‚ê‚½ƒ‚[ƒh‚É‚æ‚èA
// PulseMode‚Åİ’è‚³‚ê‚½ƒpƒ‹ƒX‚ğ“Ç‚İ‚Ş.Z‘Š‚Ìİ’è‚ÍZMode‚É‚æ‚èw’èj
void InterfaceCNTBoard::InitMode(int nCh, int CountMode, int PulseMode, int ZPhaseMode){
//    assert(nCh < ChannelNum);       // nCH‚Åw’è‚³‚ê‚éƒ`ƒƒƒ“ƒlƒ‹‚ÍA‰Šú‰»‚µ‚½ƒ`ƒƒƒ“ƒlƒ‹”‚æ‚è‚à¬‚³‚¢•K—v‚ª‚ ‚éB
    int CountDirection = 0;
    int EqualMode = 0;
    int LatchMode = 0;
    int InputPulseMode = 0x00;

    if((CountMode & 0xC0) == CountUp){
        CountDirection = 0;         // Up•ûŒü‚ÉƒJƒEƒ“ƒg
    }else if((CountMode & 0xC0) == CountDown){
        CountDirection = 1;         // Down•ûŒü‚ÉƒJƒEƒ“ƒg
    }
    if((CountMode & 0x21) == UseEqual){
        EqualMode = 1;          // ˆê’vŒŸo—LŒø
    }else if((CountMode & 0x21) == NotUseEqual){
        EqualMode = 0;          // ˆê’vŒŸo–³Œø
    }


    if((CountMode & 0x22) == SoftLatch){
        LatchMode = 0;          // ƒ\ƒtƒgƒ‰ƒbƒ`
    }else if((CountMode & 0x24) == ExtLatch){
        LatchMode = 1;          // ŠO•”ƒ‰ƒbƒ`
    }

    if((PulseMode & 0xF0) == SinglePulse){
        InputPulseMode = 0x00;              // ’P‘Šƒpƒ‹ƒXƒ‚[ƒh‚Ì‚Æ‚«
        if((PulseMode & 0x01) == Multi_1){
            InputPulseMode += 0x00;         //•W€
        }else{
            InputPulseMode += 0x01;         // ‚Q’ü”{B’P‘Šƒpƒ‹ƒXƒ‚[ƒh‚Ì‚Æ‚«‚Í‚Q’ü”{‚Ü‚Å
        }
    }else if((PulseMode & 0xF0 ) == DiffPhasePulse){
        InputPulseMode = 0x04;              // ˆÊ‘Š·ƒpƒ‹ƒXƒ‚[ƒh‚Ì‚Æ‚«
        if((PulseMode & 0x03) == Multi_1){
            InputPulseMode += 0x00;         //•W€
        }else if((PulseMode & 0x03) == Multi_2){
            InputPulseMode += 0x01;         // ‚Q’ü”{
        }else if((PulseMode & 0x03) == Multi_4){
            InputPulseMode += 0x02;         // 4’ü”{
        }
        if((PulseMode & 0x0C) == SyncClear){
            InputPulseMode += 0x08;         // “¯ŠúƒNƒŠƒA
        }else if((PulseMode & 0x0C) == ASyncClear){
            InputPulseMode += 0x00;         //@”ñ“¯ŠúƒNƒŠƒA
        }
    }else if((PulseMode & 0xF0 ) == TwoPulse){
        InputPulseMode = 0x08;
    }

    if((nRet=PencReset(DeviceHandle,nCh+1)) != 0) DisplayErrorMessage();
    if((nRet=PencSetMode(DeviceHandle, nCh+1, InputPulseMode, CountDirection, EqualMode, LatchMode)) != 0) DisplayErrorMessage();
    if((nRet=PencSetZMode(DeviceHandle, nCh+1, ZPhaseMode) != 0)) DisplayErrorMessage();
    if((nRet=PencSetCounter(DeviceHandle,nCh+1,0) != 0)) DisplayErrorMessage();
}

// ƒ`ƒƒƒ“ƒlƒ‹‚Ì‰Šú‰»İ’è(‘Sƒ`ƒƒƒ“ƒlƒ‹‚ğCountMode‚Åİ’è‚³‚ê‚½ƒ‚[ƒh‚É‚æ‚èA
// PulseMode‚Åİ’è‚³‚ê‚½ƒpƒ‹ƒX‚ğ“Ç‚İ‚Ş.Z‘Š‚Ìİ’è‚ÍZMode‚É‚æ‚èw’èj
void InterfaceCNTBoard::InitMode(int CountMode, int PulseMode, int ZPhaseMode)
{
    int i;
    for(i=0;i<ChannelNum;i++)
        InitMode(i,CountMode,PulseMode,ZPhaseMode);
}

//ƒ{[ƒhó‘Ô•\¦
void InterfaceCNTBoard::Show(void)
{
    int n;
    int multi;
    int Mode, Direction, Equal, Latch, ZMode;
    cout << "PCI-6204 compatible CNT Board" << endl;

    for(n=0;n<ChannelNum;n++){
        cout << "ƒ`ƒƒƒ“ƒlƒ‹" << n << "  " ;
        PencGetMode(DeviceHandle, n+1, &Mode, &Direction, &Equal, &Latch);
        PencGetZMode(DeviceHandle, n+1, &ZMode);
        multi = (int)pow(2.0,(Mode & 0x03));
        switch(Mode & 0x0C){
        case 0x00:
            cout << "ƒQ[ƒg‚Â‚«’P‘Šƒpƒ‹ƒX  " << multi << "’ü”{  " << "”ñ“¯ŠúƒNƒŠƒA" << endl;
            break;
        case 0x04:
            cout << "ˆÊ‘Š·ƒpƒ‹ƒX@@@@@" << multi << "’ü”{  " << "”ñ“¯ŠúƒNƒŠƒA" << endl;
            break;
        case 0x0C:
            cout << "ˆÊ‘Š·ƒpƒ‹ƒX@@@@@" << multi << "’ü”{  " << "  “¯ŠúƒNƒŠƒA" << endl;
            break;
        case 0x08:
            cout << "‚Qƒpƒ‹ƒX@@@@@@@" << multi << "’ü”{  " << "”ñ“¯ŠúƒNƒŠƒA" << endl;
            break;
        }
        if(Direction == 0)
            cout << "   ƒJƒEƒ“ƒgƒAƒbƒv  " ;
        else
            cout << "   ƒJƒEƒ“ƒgƒ_ƒEƒ“  " ;

        if(Equal == 0)
            cout << "ˆê’vŒŸo–³Œø  ";
        else
            cout << "ˆê’vŒŸo—LŒø  ";

        if(Latch == 0)
            cout << "ƒ\ƒtƒgƒ‰ƒbƒ`  ";
        else
            cout << "ŠO•”ƒ‰ƒbƒ`    ";

        if((ZMode & 0x10) == 0x00)
            cout << "Z‘Š’Êí  ";
        else
            cout << "Z‘Š”½“]  ";

        switch(ZMode & 0x03){
        case 0x00:
            cout << "Z‘ŠƒNƒŠƒA–³Œø" << endl;
            break;
        case 0x01:
            cout << "Z‘ŠƒNƒŠƒA—LŒø" << endl;
            break;
        case 0x02:
            cout << "ŠO•”ƒ‰ƒbƒ`&Z‘Š‚É‚æ‚éƒNƒŠƒA—LŒø" << endl;
            break;
        }

    }

}

//ƒGƒ“ƒR[ƒ_’l‚ğ•Ô‚·ŠÖ”“ñ‚Â
//‚ ‚éƒ`ƒƒƒlƒ‹‚©‚ç‚Ì’l‚ğ“Ç‚ŞBƒ`ƒƒƒlƒ‹w’è‚Í0`
DWORD InterfaceCNTBoard::Get(int Channel)
{
    static DWORD dwCounter;
    if((nRet=PencGetCounter(DeviceHandle,Channel+1,&dwCounter))!=0)DisplayErrorMessage();
    return dwCounter;

}
//‚·‚×‚Ä‚Ìƒ`ƒƒƒlƒ‹‚©‚ç‚Ì’l‚ğ“¯‚É“Ç‚İAˆø”‚Ì”z—ñ‚ÉŠi”[
void InterfaceCNTBoard::Get(DWORD *EncValue)
{
    int bit = (1<<ChannelNum) - 1; //ƒ`ƒƒƒlƒ‹”•ª‚Ìƒrƒbƒg‚ğ—§‚Ä‚é
    if((nRet=PencGetCounterEx(DeviceHandle, bit, &EncValue[0]))!=0)
		DisplayErrorMessage();
}

//Œ»İ‚Ì‚·‚×‚Ä‚ÌƒGƒ“ƒR[ƒ_’l‚ğ“¯‚É0‚Éİ’èB
//ƒIƒvƒVƒ‡ƒ“‚Æ‚µ‚ÄƒIƒtƒZƒbƒg’l‚àˆø”‚Æ‚µ‚Äİ’è‰Â”

void InterfaceCNTBoard::SetCount(DWORD Offset)
{
    //ƒGƒ“ƒR[ƒ_‚ÌƒIƒtƒZƒbƒg’l‚ğİ’è
    DWORD *dw= new DWORD[ChannelNum];

    for(int n=0;n<ChannelNum;n++){
        dw[n] = Offset;
    }
    //ƒŠƒZƒbƒg
    int bit = (1<<ChannelNum) - 1; //ƒ`ƒƒƒlƒ‹”•ª‚Ìƒrƒbƒg‚ğ—§‚Ä‚é
    if((nRet=PencSetCounterEx(DeviceHandle, bit, dw))!=0)DisplayErrorMessage();
}

//w’èƒ`ƒƒƒlƒ‹‚Ì’l‚ğOffset’l‚Éİ’èBƒ`ƒƒƒlƒ‹”Ô†w’è‚Í‚O`
void InterfaceCNTBoard::SetCount(int Channel, DWORD Offset)
{
    //ƒŠƒZƒbƒg
    if((nRet=PencSetCounter(DeviceHandle,Channel+1,Offset) != 0)) DisplayErrorMessage();
}



InterfaceDIOBoard::InterfaceDIOBoard(char *name)
{	
	strcpy(DeviceName,name);
	
    DeviceHandle = DioOpen((LPCTSTR)DeviceName, 0);
    if (DeviceHandle == INVALID_HANDLE_VALUE) {
		DioClose(DeviceHandle);
        printf("ƒfƒoƒCƒX–¼ FBIDIO1‚Íg—p‚Å‚«‚Ü‚¹‚ñ\n");
        exit(-1);             //ƒvƒƒOƒ‰ƒ€I—¹
    }
}

InterfaceDIOBoard::~InterfaceDIOBoard(void)
{
	DioClose(DeviceHandle);
}

void InterfaceDIOBoard::DInput(int *Value, int offset, int num)
{

    int buffer[64];

	DioInputPoint(DeviceHandle, buffer, offset, num);

	for(int i=0; i<num; i++) Value[i] = buffer[i];

}

void InterfaceDIOBoard::DOutput(BYTE value)
{

}