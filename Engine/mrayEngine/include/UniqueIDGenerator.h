
#ifndef UniqueIDGenerator_h__
#define UniqueIDGenerator_h__



namespace mray
{
	

template<class T>
class UniqueIDGenerator
{
protected:
	typedef std::set<uint> IDSet;
	typedef std::map<T, IDSet> IDMap;

	IDMap m_objects;

	uint _GetAvailableID(const IDSet& idSet)
	{
		uint id = 0;
		while (true)
		{
			if (idSet.find(id) == idSet.end())
				break;
			++id;
		}
		return id;
	}
public:
	UniqueIDGenerator(){}
	virtual ~UniqueIDGenerator()
	{
		Clear();
	}

	uint AddObject(const T&obj)
	{
		IDMap::iterator it = m_objects.find(obj);
		uint id = 0;
		if (it != m_objects.end())
		{
			id = _GetAvailableID(it->second);
			it->second.insert(id);
		}
		else
		{
			m_objects[obj].insert(id);
		}

		return id;
	}
	void RemoveObject(uint id)
	{
		m_objects.erase(id);
	}

	bool GetObjectByID(uint id, T&out)
	{
		IDObjectMap::iterator it = m_objects.find(id);
		if (it != m_objects.end()){
			out = it->second;
			return true;
		}
		return false;
	}

	void Clear()
	{
		m_objects.clear();
		m_lastID = 0;
	}


	
};

}

#endif // UniqueIDGenerator_h__
