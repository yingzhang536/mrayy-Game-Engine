

#include "stdafx.h"
#include "ARServiceProvider.h"

#include "INetwork.h"
#include "IReliableSocket.h"
#include "IThreadManager.h"
#include "IThreadFunction.h"
#include "dispObjSM.h"
#include "dispObjSMParser.h"

namespace mray
{
namespace NCam
{

	class ARProcessThread :public OS::IThreadFunction
	{
		ARServiceProvider* m_owner;
	public:
		ARProcessThread(ARServiceProvider* provider)
		{
			m_owner = provider;
		}
		virtual void execute(OS::IThread*caller, void*arg)
		{
			while (caller->isActive())
			{
				m_owner->UpdateThread();
				OS::IThreadManager::getInstance().sleep(20);
			}
		}
	};

ARServiceProvider::ARServiceProvider()
{
	OS::IThreadFunction* function = new ARProcessThread(this);
	m_commandsMutex = OS::IThreadManager::getInstance().createMutex();
	m_thread = OS::IThreadManager::getInstance().createThread(function);
	m_connection = network::INetwork::getInstance().createTCPSocket();
	m_connection->SetManualSocket(true);
}

ARServiceProvider::~ARServiceProvider()
{
	Disconnect();
	delete m_connection;
	delete m_commandsMutex;
}


bool ARServiceProvider::Connect(const core::string& host, uint port)
{
	network::INetwork::getInstance().getHostAddress(host.c_str(), m_serverAddr);
	m_serverAddr.port=port;

	if (m_connection->connect(m_serverAddr) != 0)
	{
		m_thread->start(0);
		return true;
	}
	return false;
}

void ARServiceProvider::Disconnect()
{
	m_connection->closeConnection();
	m_thread->terminate();
}

bool ARServiceProvider::IsConnected()
{
	return m_connection->isConnected();
}


void ARServiceProvider::Update()
{
	std::list<IARCommand*> commands;
	m_commandsMutex->lock();
	commands = m_commands;
	m_commands.clear();
	m_commandsMutex->unlock();
	std::list<IARCommand*>::iterator it= commands.begin();
	for (; it != commands.end(); ++it)
	{
		if ((*it)->type == EARCommandType::EArData)
		{
			FIRE_LISTENR_METHOD(OnARContents, ((ARCommandAddData*)*it));
		}
		else if ((*it)->type == EARCommandType::EDeleteGroup)
		{
			FIRE_LISTENR_METHOD(OnDeletedGroup, ((ARCommandDeleteGroup*)*it));
		}
		delete *it;
	}

}

bool ParserCommon(DispObjHdr*hdr, IARObject* obj)
{
	uint origin=hdr->attr.origin;
	switch (origin) {
	case ORIGIN_3DSPACE_3D_WORLD:
		obj->coordinates= EARCoordinates::EWorldSpace;
		break;
	case ORIGIN_3DSPACE_3D_OWNCAR:
		obj->coordinates = EARCoordinates::EOwnCar;
		break;
	case ORIGIN_WINDOW_3D_WORLD:
		obj->coordinates = EARCoordinates::EWindow3DWorld;
		break;
	case ORIGIN_WINDOW_3D_OWNCAR:
		obj->coordinates = EARCoordinates::EWindow3DOwnCar;
		break;
	case ORIGIN_WINDOW_2D_PIXEL:
		obj->coordinates = EARCoordinates::EWindow2DPixel;
		break;
	case ORIGIN_WINDOW_2D_NORMALIZE:
		obj->coordinates = EARCoordinates::EWindow2DNormalized;
		break;
	};

	obj->pos.set(hdr->attr.posDir.pos.x,
		hdr->attr.posDir.pos.z, -hdr->attr.posDir.pos.y);

	obj->dir.set(hdr->attr.posDir.pitch,
		hdr->attr.posDir.heading, hdr->attr.posDir.roll);

	return true;
}

IARObject* ParseObject(DispObjHdr* pHdr)
{

	DispObjSMParser parser;
	DOSM_GLObject glObj;
	DOSM_Predefined predefined;
	DOSM_String str;
	DOSM_VehicleData vehcle;
	IARObject* ret = 0;
	if (parser.asGLObject(pHdr, &glObj)) {

		ARMesh* mesh = new ARMesh();
		ret = mesh;

		ParserCommon(glObj.pHdr, mesh);
	
		mesh->lineWidth = glObj.pGL->lineWidth;
		mesh->cullface = glObj.pGL->backCulling;

		switch (glObj.pGL->glObjType)
		{
		case DISPOBJ_GL_LINES:
			mesh->meshType = EARMeshType::ELines;
			break;
		case DISPOBJ_GL_LINE_LOOP:
			mesh->meshType = EARMeshType::ELineLoop;
			break;
		case DISPOBJ_GL_LINE_STRIP:
			mesh->meshType = EARMeshType::ELineStrip;
			break;
		case DISPOBJ_GL_TRIANGLES:
			mesh->meshType = EARMeshType::ETriangles;
			break;
		case DISPOBJ_GL_TRIANGLE_STRIP:
			mesh->meshType = EARMeshType::ETriangleStrip;
			break;
		case DISPOBJ_GL_TRIANGLE_FAN:
			mesh->meshType = EARMeshType::ETriangleFan;
			break;
		default:
			break;
		}

		switch (glObj.pGL->normalType)
		{
		case DISPOBJ_NORMAL_NONE:
			mesh->normalType = EARNormalType::ENone;
			break;
		case DISPOBJ_NORMAL_VERTEX:
			mesh->normalType = EARNormalType::EVertex;
			break;
		case DISPOBJ_NORMAL_OVERALL:
			mesh->normalType = EARNormalType::EOverall;
			break;
		case DISPOBJ_NORMAL_AUTO_SMOOTH:
			mesh->normalType = EARNormalType::EAutoSmooth;
			break;
		case DISPOBJ_NORMAL_AUTO_FLAT:
			mesh->normalType = EARNormalType::EAutoFlat;
			break;
		default:
			break;
		}
		switch (glObj.pGL->colorType)
		{
		case DISPOBJ_COLOR_NONE:
			mesh->colorType = EARColorType::ENone;
			break;
		case DISPOBJ_COLOR_VERTEX:
			mesh->colorType = EARColorType::EVertex;
			break;
		case DISPOBJ_COLOR_OVERALL:
			mesh->colorType = EARColorType::EOverall;
			break;
		default:
			break;
		}

		mesh->verticies.resize(glObj.points.size());
		for (size_t i = 0; i < glObj.points.size(); i++) {
			DispObjXYZ* pXYZ = &glObj.points[i];
			mesh->verticies[i].set(pXYZ->x, pXYZ->y, pXYZ->z);
		}
		mesh->normals.resize(glObj.normals.size());
		for (size_t i = 0; i < glObj.normals.size(); i++) {
			DispObjXYZ* pXYZ = &glObj.normals[i];
			mesh->normals[i].set(pXYZ->x, pXYZ->y, pXYZ->z);
		}
		mesh->colors.resize(glObj.colors.size());
		for (size_t i = 0; i < glObj.colors.size(); i++) {
			DispObjRGBA* pXYZ = &glObj.colors[i];
			mesh->colors[i].Set(pXYZ->r, pXYZ->g, pXYZ->b, pXYZ->a);
		}
	}
	else if (parser.asPredefined(pHdr, &predefined)) {
		ARPredef* mesh = new ARPredef();
		ret = mesh;
		ParserCommon(predefined.pHdr, mesh);

		mesh->name = predefined.pPredefined->name;
		mesh->scale.set(predefined.pPredefined->scaleX, predefined.pPredefined->scaleY, predefined.pPredefined->scaleZ);

	}
	else if (parser.asString(pHdr, &str)) {
		ARString* mesh = new ARString();
		ret = mesh;
		ParserCommon(str.pHdr, mesh);

		mesh->fontSize = str.pString->fontSize;
		mesh->fontType = str.pFont;
		switch (str.pString->fontSizeType)
		{
		case DISPOBJ_FONTSIZETYPE_PIXEL:
			mesh->fontSizeType = EARFontSize::EPixel;
			break;;
		case DISPOBJ_FONTSIZETYPE_NORMALIZE:
			mesh->fontSizeType = EARFontSize::ENormalized;
			break;;
		default:
			break;
		}

		mesh->colors.resize(str.pString->numColor);
		for (int i = 0; i < str.pString->numColor; i++) {
			DispObjRGBA* pXYZ = &str.pString->color[i];
			mesh->colors[i].Set(pXYZ->r, pXYZ->g, pXYZ->b, pXYZ->a);
		}
		mesh->text = str.pData;
	}
	else if(parser.asVehicleData(pHdr,&vehcle)) {
		ARVehicle* v = new ARVehicle();
		ret = v;
		ParserCommon(vehcle.pHdr, v);

	}
	ret->id = pHdr->oh.objId;
	return ret;
}

void ARServiceProvider::UpdateThread()
{

#define CMD_GET_AR 0
#define CMD_DISCONNECT 1

#define DATA_END	0	// end of data
#define DATA_AR		1	// AR data for 1 group
#define DATA_DELETED_GROUP		2	// deleted group ID


	if (!m_connection->isConnected())
		return;
	uint packet[] = { 4, CMD_GET_AR };
	m_connection->send(packet, sizeof(packet), true, m_serverAddr);

	bool doneFlag = false;
	bool hadData = false;
	char *data = NULL;
	while (!doneFlag) {
		//network::SPacket* recPack = m_connection->popMessage();
		uint dataLen = 0;
		if(!m_connection->receive(m_serverAddr, &dataLen, sizeof(dataLen), 0) )
			continue;
		
		if (!dataLen)
		{
			//problem occurred, we should receive DATA_END
			break;
		}
		uint *data = new uint[dataLen];
		if (!m_connection->receive(m_serverAddr, data, dataLen, 0))
		{
			delete [] data;
			continue;;
		}

		uint *p = (uint *)data;
		uint dataType = *p++;

		IARCommand* cmd=0;

		if (dataType == DATA_END) {
			doneFlag = true;
			continue;	// i.e. breaks loop
		}
		else if (dataType == DATA_DELETED_GROUP) {
			hadData = true;

			ARCommandDeleteGroup* c = new ARCommandDeleteGroup();
			cmd = c;
			c->groupID = *p;

		}
		else if (dataType == DATA_AR) {
			hadData = true;
			ARCommandAddData* c = new ARCommandAddData();
			cmd = c;
			ARGroup* group = new ARGroup();
			c->group = group;
			group->groupID = *p++;
			DispSMHdr *pSMHdr = (DispSMHdr *)p;
			group->seqID = pSMHdr->seq;

			DispObjHdr *pObj = pSMHdr->toFirstDispObj();
			for (size_t i = 0; i < pSMHdr->numObjects;
				i++, pObj = pObj->toNext()) {
				IARObject* o = ParseObject(pObj);
				if (o)
					group->objects.push_back(o);
			}
		}
		if (cmd)
		{
			m_commandsMutex->lock();
			m_commands.push_back(cmd);
			m_commandsMutex->unlock();
		}

		delete [] data;
	}
}

}
}
