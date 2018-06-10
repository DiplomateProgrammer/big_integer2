#include "vector.h"
#include <minmax.h>
namespace my
{
	vector::vector()
	{
		small = true;
		sz = 0;
	}
	vector::vector(size_t sz)
	{
		if (sz <= cutoff)
		{
			small = true;
			this->sz = sz;
			for (size_t i = 0; i < sz; i++) data.stat[i] = 0;
		}
		else
		{
			small = false;
			this->sz = sz;
			data.big.capacity = sz;
			new (&data.big) data_type::big_type(new type[sz], sz);
			for (size_t i = 0; i < sz; i++) data.big.dyn.get()[i] = 0;
		}
	}
	vector::vector(size_t sz, type value)
	{
		if (sz <= cutoff)
		{
			small = true;
			this->sz = sz;
			for (size_t i = 0; i < sz; i++) data.stat[i] = value;
		}
		else
		{
			small = false;
			this->sz = sz;
			data.big.capacity = sz;
			new (&data.big) data_type::big_type(new type[sz], sz);
			for (size_t i = 0; i < sz; i++) data.big.dyn.get()[i] = value;
		}
	}
	vector::vector(vector const &other)
	{
		if (other.small)
		{
			small = true;
			sz = other.sz;
			for (size_t i = 0; i < sz; i++) data.stat[i] = other.data.stat[i];
		}
		else
		{
			small = false;
			sz = other.sz;
			new (&data.big) data_type::big_type(other.data.big);
		}
	}
	vector::~vector()
	{
		if (!small)
		{
			data.big.~big_type();
		}
	}
	void vector::resize_technical(size_t sz, MODE mode, type value)
	{
		if (sz <= cutoff) //into small
		{
			if (!small) //from big
			{
				type new_buffer[cutoff];
				for (size_t i = 0; i < sz; i++) new_buffer[i] = data.big.dyn.get()[i];
				data.big.~big_type();
				for (size_t i = 0; i < sz; i++) data.stat[i] = new_buffer[i];
			}
			small = true;
			if (mode == RESIZE && this->sz < sz)
			{
				for (size_t i = this->sz; i < sz; i++) data.stat[i] = value;
			}
			else if (mode == ASSIGN)
			{
				for (size_t i = 0; i < sz; i++) data.stat[i] = value;
			}
		}
		else //into big
		{
			if (small) //from small
			{
				type *new_buffer = new type[sz];
				for (size_t i = 0; i < this->sz; i++) new_buffer[i] = data.stat[i];
				new (&data.big) data_type::big_type(new_buffer, sz);
			}
			else //from big
			{
				if (this->sz < sz) //to bigger
				{
					type *new_buffer = new type[sz];
					for (size_t i = 0; i < this->sz; i++) new_buffer[i] = data.big.dyn.get()[i];
					data.big.~big_type();
					new(&data.big) data_type::big_type(new_buffer, sz);
				}
			}
			small = false;
			if (mode == RESIZE && this->sz < sz)
			{
				for (size_t i = this->sz; i < sz; i++) data.big.dyn.get()[i] = value;
			}
			else if (mode == ASSIGN)
			{
				if (this->sz >= sz && data.big.dyn.use_count() > 1)
				{
					data.big.~big_type();
					new(&data.big) data_type::big_type(new type[sz], sz);
				}
				for (size_t i = 0; i < sz; i++) data.big.dyn.get()[i] = value;
			}
		}
		this->sz = sz;
	}
	void vector::resize(size_t sz)
	{
		resize_technical(sz, NONE, 0);
	}
	void vector::resize(size_t sz, type value)
	{
		resize_technical(sz, RESIZE, value);
	}
	void vector::assign(size_t sz, type value)
	{
		resize_technical(sz, ASSIGN, value);
	}
	type const vector::operator[](size_t num) const
	{
		if (small) return data.stat[num];
		else return data.big.dyn.get()[num];
	}
	type &vector::operator[](size_t num)
	{
		if (small) return data.stat[num];
		else if (data.big.dyn.use_count() == 1) return data.big.dyn.get()[num];
		else
		{
			type *new_buffer = new type[data.big.capacity];
			size_t capacity = data.big.capacity;
			for (size_t i = 0; i < sz; i++) new_buffer[i] = data.big.dyn.get()[i];
			data.big.~big_type();
			new (&data.big) data_type::big_type(new_buffer, capacity);
			return data.big.dyn.get()[num];
		}
	}
	vector vector::operator=(vector const &other)
	{
		if (!small) data.big.~big_type();
		if (other.small)
		{
			for (size_t i = 0; i < other.sz; i++) data.stat[i] = other.data.stat[i];
		}
		else
		{
			new (&data.big) data_type::big_type(other.data.big);
		}
		small = other.small;
		sz = other.sz;
		return *this;
	}
	size_t vector::size() const
	{
		return sz;
	}
	void vector::push_back(type value)
	{
		if (sz < cutoff) data.stat[sz] = value; //small
		else if (sz == cutoff) //border
		{
			type *new_buffer = new type[cutoff + 1];
			for (size_t i = 0; i < cutoff; i++) new_buffer[i] = data.stat[i];
			new_buffer[cutoff] = value;
			new(&data.big) data_type::big_type(new_buffer, cutoff + 1);
			small = false;
		}
		else //big
		{
			if (data.big.capacity == sz || data.big.dyn.use_count() > 1)
			{
				size_t new_size;
				if (data.big.capacity == sz) new_size = data.big.capacity * 2;
				else new_size = data.big.capacity;
				type *new_buffer = new type[new_size];
				for (size_t i = 0; i < sz; i++) new_buffer[i] = data.big.dyn.get()[i];
				new_buffer[sz] = value;
				data.big.~big_type();
				new(&data.big) data_type::big_type(new_buffer, new_size);
			}
			else data.big.dyn.get()[sz] = value;
		}
		sz++;
	}
	/*void vector::pop_back()
	{
		if (!small && data.big.dyn.use_count() > 1)
		{
			type *new_buffer = new type[data.big.capacity];
			size_t capacity = data.big.capacity;
			for (size_t i = 0; i < sz - 1; i++) new_buffer[i] = data.big.dyn.get()[i];
			data.big.~big_type();
			new(&data.big) data_type::big_type(new_buffer, capacity);
		}
		sz--;
	}*/
	void vector::reverse()
	{
		if (small)
		{
			type new_buffer[cutoff];
			for (size_t i = sz - 1; i < sz; i--) new_buffer[sz - i - 1] = data.stat[i];
			for (size_t i = 0; i < sz; i++) data.stat[i] = new_buffer[i];
		}
		else
		{
			if (data.big.dyn.use_count() == 1)
			{
				type *new_buffer = new type[sz];
				for (size_t i = sz - 1; i < sz; i--) new_buffer[sz - i - 1] = data.big.dyn.get()[i];
				for (size_t i = 0; i < sz; i++) data.big.dyn.get()[i] = new_buffer[i];
				delete[] new_buffer;
			}
			else
			{
				type *new_buffer = new type[data.big.capacity];
				size_t capacity = data.big.capacity;
				for (size_t i = sz - 1; i < sz; i--) new_buffer[sz - i - 1] = data.big.dyn.get()[i];
				data.big.~big_type();
				new (&data.big) data_type::big_type(new_buffer, capacity);
			}
		}
	}
	type vector::back() const
	{
		if (small) return data.stat[sz - 1];
		else return data.big.dyn.get()[sz - 1];
	}
}