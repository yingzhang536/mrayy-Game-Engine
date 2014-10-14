

#include "stdafx.h"
#include "RoombaController.h"
#include "IOSystem.h"

#include <windows.h>

namespace mray
{

#define INITIALIZE_SENSORS()                    \
  streamedSensors_ (),                          \
    wheeldropCaster_ (),                        \
    wheeldropLeft_ (),                          \
    wheeldropRight_ (),                         \
    bumpLeft_ (),                               \
    bumpRight_ (),                              \
    wall_ (),                                   \
    cliffLeft_ (),                              \
    cliffFrontLeft_ (),                         \
    cliffFrontRight_ (),                        \
    cliffRight_ (),                             \
    deviceDetect_ (),                           \
    digitalInput3_ (),                          \
    digitalInput2_ (),                          \
    digitalInput1_ (),                          \
    digitalInput0_ (),                          \
    analogSignal_ (),                           \
    homeBaseChargerAvailable_ (),               \
    internalChargerAvailable_ (),               \
    virtualWall_ (),                            \
    leftWheelOvercurrent_ (),                   \
    rightWheelOvercurrent_ (),                  \
    ld2Overcurrent_ (),                         \
    ld1Overcurrent_ (),                         \
    ld0Overcurrent_ (),                         \
    ir_ (),                                     \
    advanceButton_ (),                          \
    playButton_ (),                             \
    distance_ (),                               \
    angle_ (),                                  \
    chargingState_ (),                          \
    batteryVoltage_ (),                         \
    batteryCurrent_ (),                         \
    batteryTemperature_ (),                     \
    batteryCharge_ (),                          \
    batteryCapacity_ (),                        \
    wallSignal_ (),                             \
    cliffLeftSignal_ (),                        \
    cliffFrontLeftSignal_ (),                   \
    cliffFrontRightSignal_ (),                  \
    cliffRightSignal_ (),                       \
    songNumber_ (),                             \
    songPlaying_ (),                            \
    streamPackets_ (),                          \
    requestedVelocity_ (),                      \
    requestedRadius_ (),                        \
    requestedLeftVelocity_ (),                  \
    requestedRightVelocity_ ()
RoombaControllerImpl::RoombaControllerImpl():
INITIALIZE_SENSORS(), currentMode_(IROBOT_CREATE_OFF)
{
	m_serialPort = OS::IOSystem::getInstance().GetSerialPortService()->CreateSerialPort();

}
RoombaControllerImpl::~RoombaControllerImpl()
{
	Disconnect();
	delete m_serialPort;
}


static void SendByte(OS::ISerialPort* s, uchar b)
{
	if (!s->IsOpen())
		return;
	try
	{
		s->Write(&b, sizeof(b));
	}
	catch (std::exception* e)
	{
	}
}

/// Push a 16 bits integer (high byte first) in a stream.
static void
sendInt16(OS::ISerialPort* s, short i)
{
	if (!s->IsOpen())
		return;
	const unsigned char v[] = { (i & 0xFFFF) >> 8, i & 0xFFFF };
	try
	{
		s->Write(v, sizeof(v));
	}
	catch (std::exception* e)
	{
	}
}

/// Receive a 16 bits integer (high byte first) in a stream.
static short
receiveInt16(OS::ISerialPort* s)
{
	if (!s->IsOpen())
		return 0;
	 unsigned char v[2] ;
	 try
	 {
		s->Read(v, sizeof(v));
	 }
	 catch (std::exception* e)
	 {
	 }
	return (v[1] << 8) + v[0];
}
bool RoombaControllerImpl::Connect(const core::string& port)
{
	Disconnect();
	try
	{
		m_serialPort->OpenByName(port, 115200);
	}
	catch (std::exception* e)
	{
		return false;
	}
	SendStartCommand();
	Sleep(100);	
	if (IsConnected())
		SendFullModeCommand();
	return IsConnected();
}
bool RoombaControllerImpl::IsConnected()
{
	return m_serialPort->IsOpen();
}
void RoombaControllerImpl::Disconnect()
{
	m_serialPort->Close();
}

//Switch to passive mode.
void RoombaControllerImpl::SendStartCommand()
{

	if (currentMode_ != IROBOT_CREATE_OFF)
		return;
	const unsigned char op = OPCODE_START;
	SendByte(m_serialPort, op);
	currentMode_ = IROBOT_CREATE_PASSIVE;
}

//Switch to safe mode.
void RoombaControllerImpl::SendSafeCommand()
{
	if (currentMode_ < IROBOT_CREATE_PASSIVE)
		return;
	const unsigned char op = OPCODE_SAFE;
	SendByte(m_serialPort, op);
	currentMode_ = IROBOT_CREATE_SAFE;

}

//Switch to full mode.
void RoombaControllerImpl::SendFullModeCommand()
{
	if (currentMode_ < IROBOT_CREATE_PASSIVE)
		return;
	const unsigned char op = OPCODE_FULL;
	SendByte(m_serialPort, op);
	currentMode_ = IROBOT_CREATE_FULL;

}


void RoombaControllerImpl::Drive(const math::vector2di& speed, int radius)
{

	int velocity = speed.x;
	if (currentMode_ < IROBOT_CREATE_SAFE)
		return;
	velocity = math::clamp(velocity, VELOCITY_MIN, VELOCITY_MAX);
	radius = math::clamp(radius, RADIUS_MIN, RADIUS_MAX);

	const unsigned char op = OPCODE_DRIVE;

	SendByte(m_serialPort, op);
	sendInt16(m_serialPort, velocity);
	sendInt16(m_serialPort, radius);
}
void RoombaControllerImpl::DriveStop()
{
	Drive(0, 0);
}
//drive each wheel seperately
void RoombaControllerImpl::DriveWheels(int leftWheel, int rightWheel)
{
	if (currentMode_ < IROBOT_CREATE_SAFE)
		return;
	leftWheel = math::clamp(leftWheel, VELOCITY_MIN, VELOCITY_MAX);
	rightWheel = math::clamp(rightWheel, VELOCITY_MIN, VELOCITY_MAX);

	const unsigned char op = OPCODE_DRIVE_DIRECT;
	SendByte(m_serialPort, op);
	sendInt16(m_serialPort, rightWheel);
	sendInt16(m_serialPort, leftWheel);

}

///  Change led status.
///  Switch on/off advance or play leds.
///  c Power led color.
///  i Power led light intensity.
void RoombaControllerImpl::SetLED(Led l, int c, int i)
{
	if (currentMode_ < IROBOT_CREATE_SAFE)
		return;
	if (l < LED_NONE || l > LED_ALL)
		return;

	const unsigned char b = l;
	const unsigned char op[] = { OPCODE_LEDS, b, c, i };
	m_serialPort->Write(op, sizeof(op));
}

//sid Song id.
void RoombaControllerImpl::SetSong(unsigned char sid, const song_t& song)
{
	if (currentMode_ < IROBOT_CREATE_PASSIVE)
		return;
	if (sid > SONG_MAX)
		return;
	if (song.size() > SONG_MAX_SIZE)
		return;

	std::stringstream ss;
	for (song_t::const_iterator it = song.begin(); it != song.end(); ++it)
	{
		if (it->first < NO_NOTE || it->first > NOTE_MAX)
			return;
		ss << it->first << it->second;
	}
	const unsigned char size = song.size();
	const unsigned char op[] = { OPCODE_SONG, sid, size };
	m_serialPort->Write(op, sizeof(op));
	m_serialPort->Write(ss.str().c_str(), size);

}

void RoombaControllerImpl::PlaySong(uint v)
{
	if (currentMode_ < IROBOT_CREATE_SAFE)
		return;
	if (v > SONG_MAX)
		return;

	const unsigned char op[] = { OPCODE_PLAY, v };
	m_serialPort->Write(op, sizeof(op));

}

void RoombaControllerImpl::RequestSensorPacket(SensorPacket packet)
{
	if (currentMode_ < IROBOT_CREATE_PASSIVE)
		return;
	if (packet < SENSOR_GROUP_0
		|| packet > SENSOR_REQUESTED_LEFT_VELOCITY)
		return;

	const unsigned char p = packet;
	const unsigned char op[] = { OPCODE_SENSORS, p };
	m_serialPort->Write(op, sizeof(op));
	queriedSensors_.push(packet);

}

/// Internal macro.
#define MAKE_SENSOR_CMD(OP, CMD)                                \
  if (currentMode_ < IROBOT_CREATE_PASSIVE)                     \
    return;                               \
  if (packets.size () > 255)                                    \
    return;                                   \
                                                                \
  std::stringstream ss;                                         \
  for (sensorPackets_t::const_iterator it = packets.begin ();   \
       it != packets.end (); ++it)                              \
    {                                                           \
      if (*it < SENSOR_GROUP_0                                  \
          || *it > SENSOR_REQUESTED_LEFT_VELOCITY)              \
        continue;                               \
      const unsigned char p = *it;                              \
      ss << p;                                                  \
      CMD;                                                      \
    }                                                           \
                                                                \
	const unsigned char size = packets.size();                   \
	const unsigned char op[] = { (OP),size};\
	m_serialPort->Write(op, sizeof(op));\
	m_serialPort->Write(ss.str().c_str(), size);

void RoombaControllerImpl::QueryListCommand(const sensorPackets_t& packets)
{
	MAKE_SENSOR_CMD(OPCODE_QUERY_LIST,
		queriedSensors_.push(*it));

}

// t is in tenth of second
void RoombaControllerImpl::RobotWait(uchar t)
{

	if (currentMode_ < IROBOT_CREATE_PASSIVE)
		return;

	const unsigned char op[] = { OPCODE_WAIT_TIME, t };
	m_serialPort->Write(op, sizeof(op));
}

void RoombaControllerImpl::UpdateSensors()
{

}

#define MK_SENSOR_GETTER(TYPE, NAME)            \
  TYPE                                          \
  RoombaControllerImpl::NAME ()                               \
  {                                             \
    UpdateSensors ();                           \
    return NAME##_;                             \
  }

MK_SENSOR_GETTER(bool, wheeldropCaster)
MK_SENSOR_GETTER(bool, wheeldropLeft)
MK_SENSOR_GETTER(bool, wheeldropRight)
MK_SENSOR_GETTER(bool, bumpLeft)
MK_SENSOR_GETTER(bool, bumpRight)
MK_SENSOR_GETTER(bool, wall)
MK_SENSOR_GETTER(bool, cliffLeft)
MK_SENSOR_GETTER(bool, cliffFrontLeft)
MK_SENSOR_GETTER(bool, cliffFrontRight)
MK_SENSOR_GETTER(bool, cliffRight)
MK_SENSOR_GETTER(bool, deviceDetect)
MK_SENSOR_GETTER(bool, digitalInput3)
MK_SENSOR_GETTER(bool, digitalInput2)
MK_SENSOR_GETTER(bool, digitalInput1)
MK_SENSOR_GETTER(bool, digitalInput0)
MK_SENSOR_GETTER(short, analogSignal)
MK_SENSOR_GETTER(bool, homeBaseChargerAvailable)
MK_SENSOR_GETTER(bool, internalChargerAvailable)
MK_SENSOR_GETTER(bool, virtualWall)
MK_SENSOR_GETTER(bool, leftWheelOvercurrent)
MK_SENSOR_GETTER(bool, rightWheelOvercurrent)
MK_SENSOR_GETTER(bool, ld2Overcurrent)
MK_SENSOR_GETTER(bool, ld1Overcurrent)
MK_SENSOR_GETTER(bool, ld0Overcurrent)
MK_SENSOR_GETTER(unsigned char, ir)
MK_SENSOR_GETTER(bool, advanceButton)
MK_SENSOR_GETTER(bool, playButton)
MK_SENSOR_GETTER(short, distance)
MK_SENSOR_GETTER(short, angle)
MK_SENSOR_GETTER(RoombaControllerImpl::ChargingState, chargingState)
MK_SENSOR_GETTER(short, batteryVoltage)
MK_SENSOR_GETTER(short, batteryCurrent)
MK_SENSOR_GETTER(short, batteryTemperature)
MK_SENSOR_GETTER(short, batteryCharge)
MK_SENSOR_GETTER(short, batteryCapacity)
MK_SENSOR_GETTER(short, wallSignal)
MK_SENSOR_GETTER(short, cliffLeftSignal)
MK_SENSOR_GETTER(short, cliffFrontLeftSignal)
MK_SENSOR_GETTER(short, cliffFrontRightSignal)
MK_SENSOR_GETTER(short, cliffRightSignal)
MK_SENSOR_GETTER(unsigned char, songNumber)
MK_SENSOR_GETTER(bool, songPlaying)
MK_SENSOR_GETTER(unsigned char, streamPackets)
MK_SENSOR_GETTER(short, requestedVelocity)
MK_SENSOR_GETTER(short, requestedRadius)
MK_SENSOR_GETTER(short, requestedLeftVelocity)
MK_SENSOR_GETTER(short, requestedRightVelocity)


//////////////////////////////////////////////////////////////////////////



bool RoombaController::Connect(const core::string& port)
{
	return m_impl->Connect(port);
}
bool RoombaController::IsConnected()
{
	return m_impl->IsConnected();
}
void RoombaController::Disconnect()
{
	 m_impl->Disconnect();

}

void RoombaController::Drive(const math::vector2di& speed, int rotationSpeed)
{
	int l = 0;
	int r = 0;
	l = r = speed.x;
	l -= rotationSpeed;
	r += rotationSpeed;

	l /= 2;
	r /= 2;
	 m_impl->DriveWheels(l,r);

}
void RoombaController::DriveStop()
{
	m_impl->DriveStop();
}




}





