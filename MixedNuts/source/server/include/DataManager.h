#ifndef _DATAMANAGER_H_
#define _DATAMANAGER_H_ 

#include <map>
#include <boost/thread/recursive_mutex.hpp>
#include "definitions.h"

template<class T>
class DataManager
{
public:
	DataManager(void){}
	~DataManager(void){}	

	T GetDataByID(IDType id)
	{
		T data;			
		boost::recursive_mutex::scoped_lock lock(m_Mutex);		//scoped lock mutex
		typename std::map<IDType, T>::const_iterator pos = m_DataList.find(id);	//find by key std::map
		if (pos != m_DataList.end())
			data = pos->second;
		return data;
	}

	void Add( IDType id, T client)
	{		
		//scoped lock mutex
		boost::recursive_mutex::scoped_lock lock(m_Mutex);		
		
		//checking if ID exist
		typename std::map<IDType, T>::const_iterator pos = 
			m_DataList.find(id);	
		
		if (pos == m_DataList.end()) {
			m_DataList.insert(typename std::map<IDType, T>::value_type(id,client));
		}
	}

	void Remove(IDType id)
	{
		try{
			boost::recursive_mutex::scoped_lock lock(m_Mutex);  //scoped lock mutex
			m_DataList.erase(id);		//erase by key std::map				
		}catch(...){}
	}

	void Lock()
	{
		m_Mutex.lock();
	}

	void BeginIterate()
	{
		Lock();
		m_iterator = m_DataList.begin();
	}

	T GetNextData()
	{
		return (m_iterator++)->second;
	}
	
	bool IsIteratorEnd(){
		return(m_iterator == m_DataList.end());
	}

	void EndIterate()
	{
		Unlock();
	}

	void Unlock()
	{
		m_Mutex.unlock();
	}

private:
	boost::recursive_mutex m_Mutex;
	std::map<IDType, T> m_DataList;
	typename std::map<IDType, T>::iterator m_iterator;			//NOTE: typename is needed to avoid error
};

#endif 
