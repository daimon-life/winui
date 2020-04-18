#pragma once




typedef std::map<void*,void*> MAP_PAINTMGR;
template<class T>
class CImgObject : public T
{
public:
	CImgObject(void):
	  m_dwf(0)
	  {
		  ::InitializeCriticalSection(&m_Lock);
		  XMutex tmp(m_Lock);
		  m_mapInfo.clear();
	  }
	  ~CImgObject(void)
	  {

	  }

	  LONG AddRef()
	  {
		  InterlockedIncrement(&m_dwf);
		  return m_dwf;
	  }
	  LONG Release()
	  {
		  InterlockedDecrement(&m_dwf);
		  LONG cef = m_dwf;
		  ATLASSERT(m_dwf == m_mapInfo.size());
		  ATLTRACE(_T("我是,我现%d\r\n"),m_dwf);
		  if (cef == 0)
		  {
			  ::DeleteCriticalSection(&m_Lock);
			  OnDelete();
		  }

		  return cef;
	  }

	  CImgObject& operator=(T* pT) const
	  {
		  (T*)this = pT;
		  return *this;
	  }

	  T* GetReal(void* pvoid)
	  {
		  XMutex tmp(m_Lock);
		  MAP_PAINTMGR::iterator it = m_mapInfo.find(pvoid);
		  if (it == m_mapInfo.end())
		  {
			  m_mapInfo[pvoid] = pvoid;
			  AddRef();
		  }
		  return this;
	  }

	  BOOL ReleaseMgr(void* pvoid)
	  {
		  ::EnterCriticalSection(&m_Lock);
		  MAP_PAINTMGR::iterator it = m_mapInfo.find(pvoid);
		  if (it == m_mapInfo.end())
		  {
			  return FALSE;
		  }
		  else
		  {
			  m_mapInfo.erase(it);
			  ::LeaveCriticalSection(&m_Lock);
			  return Release() == 0;
			  //ATLASSERT(m_dwf == m_mapInfo.size());
		  }

		  ::LeaveCriticalSection(&m_Lock);
		  return TRUE;
	  }

	  virtual void OnDelete() = 0;
protected:
	LONG m_dwf;//引入计数。来判断是否要删除此对象

	MAP_PAINTMGR m_mapInfo;//用来存放被引用的paintmgr;

	CRITICAL_SECTION m_Lock;
};
