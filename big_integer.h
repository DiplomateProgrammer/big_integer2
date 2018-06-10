#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <cstddef>
#include <iosfwd>
#include <string>
#include "vector/vector.h"


struct big_integer
{
    big_integer();
    big_integer(big_integer const& other)  = default;
    big_integer(int a);
    explicit big_integer(std::string const& str);
    ~big_integer();

    big_integer& operator=(big_integer const& other);

    big_integer& operator+=(big_integer const& rhs);
    big_integer& operator-=(big_integer const& rhs);
    big_integer& operator*=(big_integer const& rhs);
    big_integer& operator/=(big_integer const& rhs);
    big_integer& operator%=(big_integer const& rhs);

    big_integer& operator&=(big_integer const& rhs);
    big_integer& operator|=(big_integer const& rhs);
    big_integer& operator^=(big_integer const& rhs);

    big_integer& operator<<=(int rhs);
    big_integer& operator>>=(int rhs);

    big_integer operator+() const;
    big_integer operator-() const;
    big_integer operator~() const;

    big_integer& operator++();
    big_integer operator++(int);

    big_integer& operator--();
    big_integer operator--(int);

    friend bool operator==(big_integer const& a, big_integer const& b);
    friend bool operator!=(big_integer const& a, big_integer const& b);
    friend bool operator<(big_integer const& a, big_integer const& b);
    friend bool operator>(big_integer const& a, big_integer const& b);
    friend bool operator<=(big_integer const& a, big_integer const& b);
    friend bool operator>=(big_integer const& a, big_integer const& b);

    friend std::string to_string(big_integer const& a);

private:
	my::vector digits;
	int sign;
	big_integer multiply_short(big_integer const& num, unsigned int x);
	unsigned int trial(big_integer const& r, big_integer const& d, unsigned int k, unsigned int m);
	bool smaller(big_integer const& r, big_integer const& d, unsigned int k, unsigned int m);
	static big_integer digit_quot(big_integer const& num, unsigned int x);
	unsigned int digit_rem(big_integer const& num, unsigned int x);
	std::pair<big_integer, big_integer> divide(big_integer const& rhs);
	std::pair<big_integer, big_integer> division(big_integer const& rhs);
	void to_twos_complement();
	void to_normal();
	static void normalize(big_integer& a);
	bool is_zero() const;
	static void abs(big_integer& a);
	void add_absolute(big_integer& a, big_integer const& rhs);
	void sub_absolute(big_integer& a, big_integer const& rhs);
	void multiply_absolute(big_integer& a, big_integer const& b);
	big_integer& 
	logic_operation(big_integer const& rhs, unsigned int (op)(unsigned int const&, unsigned int const&));
	static unsigned int and2 (unsigned int const &a, unsigned int const &b);
	static unsigned int or2 (unsigned int const &a, unsigned int const &b);
	static unsigned int xor2 (unsigned int const &a, unsigned int const &b);
};

big_integer operator+(big_integer a, big_integer const& b);
big_integer operator-(big_integer a, big_integer const& b);
big_integer operator*(big_integer a, big_integer const& b);
big_integer operator/(big_integer a, big_integer const& b);
big_integer operator%(big_integer a, big_integer const& b);

big_integer operator&(big_integer a, big_integer const& b);
big_integer operator|(big_integer a, big_integer const& b);
big_integer operator^(big_integer a, big_integer const& b);

big_integer operator<<(big_integer a, int b);
big_integer operator>>(big_integer a, int b);

bool operator==(big_integer const& a, big_integer const& b);
bool operator!=(big_integer const& a, big_integer const& b);
bool operator<(big_integer const& a, big_integer const& b);
bool operator>(big_integer const& a, big_integer const& b);
bool operator<=(big_integer const& a, big_integer const& b);
bool operator>=(big_integer const& a, big_integer const& b);

std::string to_string(big_integer const& a);
std::ostream& operator<<(std::ostream& s, big_integer const& a);

#endif // BIG_INTEGER_H
