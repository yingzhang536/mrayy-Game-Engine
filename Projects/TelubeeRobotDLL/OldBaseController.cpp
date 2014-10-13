
#include "stdafx.h"
#include "OldBaseController.h"


namespace mray
{

OldBaseController::OldBaseController()
{
	comROBOT = new Tserial_event();
	connected = false;
}

OldBaseController::~OldBaseController()
{
	Disconnect();
	delete comROBOT;
}




bool OldBaseController::Connect(const core::string& port)
{
	Disconnect();
	connected=comROBOT->connect((char*)port.c_str(), 115200, SERIAL_PARITY_ODD, 8, FALSE, TRUE);

	return connected;
}

bool OldBaseController::IsConnected()
{
	return connected;
}

void OldBaseController::Disconnect()
{
	comROBOT->disconnect();
	comROBOT = FALSE;
}


void OldBaseController::Drive(const math::vector2di& speed, int rotationSpeed)
{
	int packet_size;
	char sCommand[128];
	sprintf_s(sCommand, 128, "V,%d,%d,%d\r\n", speed.x, speed.y, rotationSpeed);
// 	
// 	sprintf_s(sCommand, 128, "q\r\n");

	packet_size = strlen(sCommand);
	comROBOT->sendData(sCommand, packet_size);

}
void OldBaseController::DriveStop()
{

	int packet_size;
	char sCommand[128];
	 	
 	sprintf_s(sCommand, 128, "q\r\n");

	packet_size = strlen(sCommand);
	comROBOT->sendData(sCommand, packet_size);
}

}


