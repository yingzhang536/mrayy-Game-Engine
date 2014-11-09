
#include "stdafx.h"
#include "OmniBaseController.h"


namespace mray
{

OmniBaseController::OmniBaseController()
{
	comROBOT = 0;
	connected = false;
}

OmniBaseController::~OmniBaseController()
{
	Disconnect();
	delete comROBOT;
}



void robot_OnDataArrival(int size, char *buffer){

	//todo:handle return
	//comROBOT->sendData("S", 2);

}


void robot_SerialEventManager(uint32 object, uint32 event){
	char *buffer;
	int   size;
	Tserial_event *com;

	com = (Tserial_event *)object;
	if (com != 0){
		switch (event){
		case  SERIAL_CONNECTED:
			//printf("Com Port Connected! \n");
			break;
		case  SERIAL_DATA_ARRIVAL:
			size = com->getDataInSize();
			buffer = com->getDataInBuffer();
			robot_OnDataArrival(size, buffer);
			com->dataHasBeenRead();
			break;
		}
	}


}

bool OmniBaseController::Connect(const core::string& port)
{
	Disconnect();

	comROBOT = new Tserial_event();
	comROBOT->setManager(robot_SerialEventManager);
	connected=comROBOT->connect((char*)port.c_str(), 115200, SERIAL_PARITY_ODD, 8, FALSE, TRUE)==0;
	if (!comROBOT->isconnected())
	{
		printf("Failed to connect robot\n");
		delete comROBOT;
		comROBOT = 0;
	}
	else
	{
		comROBOT->setRxSize(15);
	}
	return comROBOT->isconnected();
}

bool OmniBaseController::IsConnected()
{
	return comROBOT!=0;
}

void OmniBaseController::Disconnect()
{
	if (!comROBOT)
		return;

	comROBOT->disconnect();
	connected = FALSE;
	delete comROBOT;
	comROBOT = 0;
}


void OmniBaseController::Drive(const math::vector2di& speed, int rotationSpeed)
{
	if (!comROBOT)
		return;
	int packet_size;
	char sCommand[128];
	sprintf_s(sCommand, 128, "V,%d,%d,%d\r\n", speed.x, speed.y, rotationSpeed);
// 	
// 	sprintf_s(sCommand, 128, "q\r\n");

//	printf("%s\n", sCommand);

	packet_size = strlen(sCommand);
	comROBOT->sendData(sCommand, packet_size);

}
void OmniBaseController::DriveStop()
{
	if (!comROBOT)
		return;

	int packet_size;
	char sCommand[128];
	 	
 	sprintf_s(sCommand, 128, "q\r\n");

	packet_size = strlen(sCommand);
	comROBOT->sendData(sCommand, packet_size);
}

}


