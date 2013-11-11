

#include "WiiEvent.h"


namespace mray
{

const int WiiEvent::EventID=100;

WiiEvent::WiiEvent():Event(EventID),event(EWiiEvent_Unkown),WiimoteNumber(0)
{
}
WiiEvent::~WiiEvent()
{
}


}
