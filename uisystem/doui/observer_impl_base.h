#ifndef OBSERVER_IMPL_BASE_HPP0
#define OBSERVER_IMPL_BASE_HPP0

#include <map>

template <typename ReturnT, typename ParamT>
class ReceiverImplBase;

template <typename ReturnT, typename ParamT>
class ObserverImplBase
{
public:
	virtual void AddReceiver(ReceiverImplBase<ReturnT, ParamT>* receiver) = 0;
	virtual void RemoveReceiver(ReceiverImplBase<ReturnT, ParamT>* receiver) = 0;
	virtual ReturnT Broadcast(ParamT param) = 0;
	virtual ReturnT RBroadcast(ParamT param) = 0;
	virtual ReturnT Notify(ParamT param) = 0;
};

template <typename ReturnT, typename ParamT>
class UILIB_API ReceiverImplBase
{
public:
	virtual void AddObserver(ObserverImplBase<ReturnT, ParamT>* observer) = 0;
	virtual void RemoveObserver() = 0;
	virtual ReturnT Receive(ParamT param) = 0;
	virtual ReturnT Respond(ParamT param, ObserverImplBase<ReturnT, ParamT>* observer) = 0;
};

template <typename ReturnT, typename ParamT>
class ReceiverImpl;

template <typename ReturnT, typename ParamT>
class ObserverImpl : public ObserverImplBase<ReturnT, ParamT>
{
	template <typename ReturnT, typename ParamT>
	friend class Iterator;
public:
	ObserverImpl()
	{}

	virtual ~ObserverImpl()	{}

	virtual void AddReceiver(ReceiverImplBase<ReturnT, ParamT>* receiver)
	{
		if (receiver == NULL)
			return;

		receivers_.push_back(receiver);
		receiver->AddObserver(this);
	}

	virtual void RemoveReceiver(ReceiverImplBase<ReturnT, ParamT>* receiver)
	{
		if (receiver == NULL)
			return;

		ReceiversVector::iterator it = receivers_.begin();
		for (; it != receivers_.end(); ++it)
		{
			if (*it == receiver)
			{
				receivers_.erase(it);
				break;
			}
		}
	}

	virtual ReturnT Broadcast(ParamT param)
	{
		ReceiversVector::iterator it = receivers_.begin();
		for (; it != receivers_.end(); ++it)
		{
			(*it)->Receive(param);
		}

		return ReturnT();
	}

	virtual ReturnT RBroadcast(ParamT param)
	{
		ReceiversVector::reverse_iterator it = receivers_.rbegin();
		for (; it != receivers_.rend(); ++it)
		{
			(*it)->Receive(param);
		}

		return ReturnT();
	}

	virtual ReturnT Notify(ParamT param)
	{
		ReceiversVector::iterator it = receivers_.begin();
		for (; it != receivers_.end(); ++it)
		{
			(*it)->Respond(param, this);
		}

		return ReturnT();
	}

	template <typename ReturnT, typename ParamT>
	class Iterator
	{
		ObserverImpl<ReturnT, ParamT> & _tbl;
		DWORD index;
		ReceiverImplBase<ReturnT, ParamT>* ptr;
	public:
		Iterator( ObserverImpl & table )
			: _tbl( table ), index(0), ptr(NULL)
		{}

		Iterator( const Iterator & v )
			: _tbl( v._tbl ), index(v.index), ptr(v.ptr)
		{}

		ReceiverImplBase<ReturnT, ParamT>* next()
		{
			if ( index >= _tbl.receivers_.size() )
				return NULL;

			for ( ; index < _tbl.receivers_.size(); )
			{
				ptr = _tbl.receivers_[ index++ ];
				if ( ptr )
					return ptr;
			}
			return NULL;
		}
	};

protected:
	typedef std::vector<ReceiverImplBase<ReturnT, ParamT>*> ReceiversVector;
	ReceiversVector receivers_;
};


template <typename ReturnT, typename ParamT>
class UILIB_API ReceiverImpl : public ReceiverImplBase<ReturnT, ParamT>
{
protected:
    typedef std::vector<ObserverImplBase<ReturnT, ParamT>*> ObserversVector;

public:
	ReceiverImpl()
	{
        m_pObServersList = new ObserversVector();
    }
	virtual ~ReceiverImpl()
    {
        if (m_pObServersList)
        {
            delete m_pObServersList;
            m_pObServersList = NULL;
        }
    }

public:
	virtual void AddObserver(ObserverImplBase<ReturnT, ParamT>* observer)
	{
        if (m_pObServersList)
        {
            m_pObServersList->push_back(observer);
        }
	}
	virtual void RemoveObserver()
	{
        if (m_pObServersList)
        {
            ObserversVector::iterator vtIt;
            for (vtIt = m_pObServersList->begin(); vtIt != m_pObServersList->end(); vtIt++)
            {
                (*vtIt)->RemoveReceiver(this);
            }
        }
	}
	virtual ReturnT Receive(ParamT param)
	{
		return ReturnT();
	}
	virtual ReturnT Respond(ParamT param, ObserverImplBase<ReturnT, ParamT>* observer)
	{
		return ReturnT();
	}

protected:
	ObserversVector *m_pObServersList;
};


#endif // OBSERVER_IMPL_BASE_HPP0
