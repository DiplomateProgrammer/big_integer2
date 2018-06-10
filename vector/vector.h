#ifndef MYVECTOR_H
#define MYVECTOR_H
#include <memory>
namespace my
{
	using type = unsigned int;
	constexpr int cutoff = 150;
	enum MODE { NONE, RESIZE, ASSIGN };
	struct vector 
	{
		vector();
		vector(size_t);
		vector(size_t, type);
		vector(vector const&);
		~vector();
		void resize(size_t);
		void resize(size_t, type);
		void assign(size_t, type);
		type const operator[](size_t) const;
		type& operator[](size_t);
		vector operator=(vector const &);
		size_t size() const;
		void push_back(type);
		//void pop_back();
		void reverse();
		type back() const;
	private:
		union data_type
		{
			struct big_type
			{
				std::shared_ptr<type> dyn;
				size_t capacity;
				big_type(type *x, size_t cap): dyn(x, std::default_delete<type[]>()), capacity(cap) {}
				big_type(big_type const &other) : dyn(other.dyn), capacity(other.capacity) {};
				~big_type() = default;
			} big;
			type stat[cutoff];
			data_type(){}
			~data_type() {}
		} data;
		size_t sz;
		bool small;
		void resize_technical(size_t, MODE, type);
	};
}
#endif