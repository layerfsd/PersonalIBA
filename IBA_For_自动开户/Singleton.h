
#pragma once

//#include <afxmt.h>
template<typename T>
class CSingletonCleaner  
{  
public:  
	CSingletonCleaner(T* m_Instance)
	: m_Instance(m_Instance)
	{

	}  

	~CSingletonCleaner()  
	{  
		if (m_Instance != NULL)
		{
			delete m_Instance;
			m_Instance = NULL;
		}
	}  

private:
	T* m_Instance;
};

#define SINGLETON_DECLARE(ClassName) \
private: \
	friend class CSingletonCleaner; \
	static ClassName* m_Instance; \
	static CCriticalSection m_csSingleton; \
private: \
	ClassName(const ClassName &rhs); \
	ClassName &operator=(const ClassName &rhs); \
	ClassName(void); \
	~ClassName(void); \


#define SINGLETON_IMPLEMENT(ClassName) \
ClassName* ClassName::m_Instance = NULL; \
ClassName* ClassName::GetInstance() \
{ \
	if (NULL == m_Instance) \
	{ \
		if (NULL == m_Instance) \
		{ \
			m_Instance = new ClassName(); \
			static CSingletonCleaner<ClassName> Cleaner(m_Instance); \
		} \
	}\
	return m_Instance; \
}\

//#define SINGLETON_DECLARE(ClassName) \
//public: \
//	static ClassName* GetInstance(); \
//private: \
//	friend class CSingletonCleaner; \
//	static ClassName* m_Instance; \
//	static CCriticalSection m_csSingleton; \
//private: \
//	ClassName(const ClassName &rhs); \
//	ClassName &operator=(const ClassName &rhs); \
//	ClassName(void); \
//	~ClassName(void); \
//
//
//#define SINGLETON_IMPLEMENT(ClassName) \
//	ClassName* ClassName::m_Instance = NULL; \
//	CCriticalSection ClassName::m_csSingleton; \
//	ClassName* ClassName::GetInstance() \
//{ \
//	if (NULL == m_Instance) \
//{ \
//	CSingleLock lock(&m_csSingleton, TRUE); \
//	if (NULL == m_Instance) \
//{ \
//	m_Instance = new ClassName(); \
//	static CSingletonCleaner<ClassName> cl(m_Instance); \
//} \
//}\
//	return m_Instance; \
//}\