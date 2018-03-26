/*
	DArray - Dynamic Array
	Simple replacement for std::vector
*/
#pragma once

template <class T>
class DArray
{
public:
	DArray()
	{
		// arbitrarily start at 8 items
		m_size = 8;
		m_itemCount = 0;

		m_items = new T[m_size];
	}
	~DArray() { delete[] m_items; }

	// copy constructors
	DArray(const DArray& da)
	{
		for (int i = 0; i < da.getCount(); ++i)
			this->add(da[i]);
	}
	DArray& operator=(DArray& da)
	{
		for (int i = 0; i < da.getCount(); ++i)
			this->add(da[i]);
		return *this;
	}

	// I'm not sure how move constructors work so leave them as default
	DArray(DArray&& da) = default;
	DArray& operator=(DArray&& da) = default;

	T& operator[](int index) { return m_items[index]; }

	void add(const T& item)
	{
		// just stick it on the end of the array
		m_items[m_itemCount] = item;
		m_itemCount++;

		// resize if we get close to max size
		if (m_itemCount >= m_size - 1)
		{
			m_size *= 2;

			// make a whole new array with the new size
			T* resized = new T[m_size];
			// and copy all the stuff over
			for (int i = 0; i < m_itemCount; ++i)
				resized[i] = m_items[i];

			// get rid of the old array
			delete[] m_items;
			// and make it point to the new one
			m_items = resized;

			// apparently realloc doesn't like to work :(
			//m_items = (T*)realloc(m_items, m_size);
		}
	}

	void remove(int index)
	{
		// removing the last item is just popping off the top
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

	// searches for item in the array and removes it if found
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

	// pop and clear don't actually do much
	// they just change the item count so that unwanted items aren't
	//   accessed and will be overwritten
	void pop() { m_itemCount--; }
	void clear() { m_itemCount = 0; }

	inline int getCurrentSize() { return m_size; }
	inline int getCount() { return m_itemCount; }

	// use subscript operator to get array elements, not this!
	inline T* _getArray() { return m_items; }
private:
	T*		m_items;
	int		m_size;
	int		m_itemCount;
};