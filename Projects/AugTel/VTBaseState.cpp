

#include "stdafx.h"
#include "VTBaseState.h"

#include "CommunicationManager.h"
#include "UDPCommunicationLayer.h"
#include "GameComponentCreator.h"
#include "VTSharedMemory.h"



namespace mray
{
namespace AugTel
{

#define VT_USING_SHAREDMEM
	class TelesarCommunicationHandler
	{
		static int _refCount;
		static TelesarCommunicationHandler* _instance;
	public:

		VT::ICommunicationLayer* commLayer;

	protected:

		TelesarCommunicationHandler()
		{
			commLayer = 0;
		}
		~TelesarCommunicationHandler()
		{
			delete commLayer;
		}
		void _init()
		{

			//init Virtual Telesar Library

#ifdef VT_USING_SHAREDMEM
			commLayer = new VT::VTSharedMemory(mT("TelesarV"));
#else
			commLayer = new VT::UDPCommunicationLayer(1234);
#endif
			VT::CommunicationManager::getInstance().AddCommunicationLayer(mT("TelesarV"), commLayer);


			commLayer->Start();

			game::GameComponentCreator::getInstance().AddAlias("KinematicJointDOF6Component", "PhysicalJointComponent");
			game::GameComponentCreator::getInstance().AddAlias("NullPhysicsComponent", "RigidBodyComponent");
		}

	public:
		static void Ref()
		{
			_refCount++;
			if (_refCount == 1)
			{
				_instance = new TelesarCommunicationHandler();
				_instance->_init();
			}
		}
		static void Unref()
		{
			_refCount--;
			if (_refCount == 0)
			{
				delete _instance;
				_instance = 0;
			}
		}
		static TelesarCommunicationHandler* Instance(){ return _instance; }

		void Update(float dt)
		{
			commLayer->Update(dt);
		}
	};
	int TelesarCommunicationHandler::_refCount = 0;
	TelesarCommunicationHandler* TelesarCommunicationHandler::_instance = 0;

VTBaseState::VTBaseState()
{
	TelesarCommunicationHandler::Ref();
}

VTBaseState::~VTBaseState()
{
	TelesarCommunicationHandler::Unref();
}


void VTBaseState::OnInit()
{
}

void VTBaseState::OnEnter()
{
}

void VTBaseState::OnExit()
{
}

void VTBaseState::Update(float dt)
{
	TelesarCommunicationHandler::Instance()->Update(dt);
}

void VTBaseState::CalibratePosition()
{
	TelesarCommunicationHandler::Instance()->commLayer->InjectCommand("calib", "");
}


void VTBaseState::CalibrateGloves()
{

}


}
}