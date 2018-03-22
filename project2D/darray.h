#pragma once

#include <cstdlib>

#define DARRAY_DEFAULT_SIZE 8

template <class T>
class DArray
{
public:
	DArray()
	{
		m_size = DARRAY_DEFAULT_SIZE;
		m_itemCount = 0;

		m_items = new T[m_size];
	}
	~DArray()
	{
		delete[] m_items;
	}

	T& operator[](int index) { return m_items[index]; }

	void add(const T& item)
	{
		m_items[m_itemCount] = item;
		m_itemCount++;

		// resize if we get close to max size
		if (m_itemCount >= m_size - 1)
		{
			m_size *= 2;
			
			T* resized = new T[m_size];
			for (int i = 0; i < m_itemCount; ++i)
				resized[i] = m_items[i];

			delete[] m_items;
			m_items = resized;
			// apparently realloc doesn't like to work
			//m_items = (T*)realloc(m_items, m_size);
		}
	}

	void remove(int index)
	{
		// if we're removing the last item in the array, we can just
		//   decrease the count so it gets overridden next time we add
		if (index == m_itemCount - 1)
		{
			pop();
			return;
		}

		// make sure we're in the bounds of the array
		if (index < 0 || index >= m_itemCount)
			return;

		// shift all elements left, overwriting the element at index
		for (int i = index + 1; i < m_itemCount; ++i)
			m_items[i - 1] = m_items[i];
		m_itemCount--;
	}

	void remove(const T& item)
	{
		for (int i = 0; i < m_itemCount; ++i)
		{
			if (m_items[i] == item)
			{
				remove(i);
				break;
			}
		}
	}

	void pop()
	{
		m_itemCount--;
	}

	void clear()
	{
		m_itemCount = 0;
	}

	inline int getCurrentSize() { return m_size; }
	inline int getCount() { return m_itemCount; }

	// shouldn't be used which is why the _ is here
	inline T* _getArray() { return m_items; }
private:
	T*		m_items;
	int		m_size;
	int		m_itemCount;
};