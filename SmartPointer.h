#ifndef _SMART_POINTER_H
#define _SMART_POINTER_H

#include <stdio.h>

class ReferenceCount {
public:
    ReferenceCount() : _refCnt(0) {}
    void incrReference() {
		_refCnt++;
	}
    void decrReference() {
		if (--_refCnt == 0) 
            delete this;
    }

protected:
    virtual ~ReferenceCount() {}

private:
    ReferenceCount(const ReferenceCount&);
    void operator=(const ReferenceCount&);
    unsigned _refCnt;
};

template <class T>
class SmartPointer {
public:
    SmartPointer() : _ptr(NULL) {}

    SmartPointer(T *other) : _ptr(other) 
    { 
        if (_ptr) {
    		_ptr->incrReference();
        }
    }

    SmartPointer(const SmartPointer<T>& other) : _ptr(other._ptr)
    {
        if (_ptr) {
    		_ptr->incrReference();
        }
    }

    SmartPointer<T>& operator=(T *other) 
    {
        if (_ptr != other) {
    		if (_ptr) {
    			_ptr->decrReference();
    		}
    		_ptr = other;
    		if (_ptr) {
    			_ptr->incrReference();
    		}
        }
        return *this ; 
    }

    SmartPointer<T>& operator=(const SmartPointer<T>& other) 
    {
        return operator=(other._ptr);
    }

    ~SmartPointer() {
        if (_ptr) {
    		_ptr->decrReference();
        }
    }

    T*get() const
	{
		return _ptr;
	}

    T& operator*() const
	{
		return *_ptr;
	}

    T*operator->() const
	{
		return _ptr;
	}

private:
    T*_ptr;
};

#endif