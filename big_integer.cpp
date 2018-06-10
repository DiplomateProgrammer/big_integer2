//ûû
#include "big_integer.h"

using namespace std;
#include <cstring>
#include <stdexcept>
#include <string>
//#include <vector>
#include <algorithm>
const unsigned long long BASE = (unsigned long long)UINT_MAX + 1;
const unsigned int logic_base = UINT_MAX;
const unsigned int bits_in_base = sizeof(unsigned int) * 8;

	big_integer::big_integer(): digits(my::vector(1, 0)), sign(0) {}

	big_integer::big_integer(int a)
	{
		if (a < 0)
		{
			sign = -1;
		}
		if (a == 0)
		{
			sign = 0;
		}
		if (a > 0)
		{
			sign = 1;
		}
		digits = my::vector(1, (unsigned int)std::abs(a));
	}

	big_integer::big_integer(std::string const& str)
	{
		big_integer multiplier = 1;
		sign = 1;
		for (size_t i = str.size() - 1; i < str.size(); i--)
		{
			if ((i == 0) && (str[i] == '-')) 
			{
				sign = -1;
				break;
			}
			*this += multiply_short(multiplier, (unsigned int)(str[i] - '0'));
			multiplier = multiply_short(multiplier, 10);
		}
		normalize(*this);
		
	}

	big_integer::~big_integer()
	{

	}

	big_integer& big_integer::operator=(big_integer const& other)
	{
		digits = other.digits;
		sign = other.sign;
		return *this;
	}
	void big_integer::add_absolute(big_integer& a, big_integer const& rhs)
	{
		unsigned int carry = 0;
		a.sign = 1;
		for (size_t i = 0; i < min(a.digits.size(), rhs.digits.size()); i++)
		{
			unsigned long long test_carry = (unsigned long long)a.digits[i] + (unsigned long long)rhs.digits[i] + carry;
			a.digits[i] += rhs.digits[i] + carry;
			(test_carry > logic_base) ? carry = 1 : carry = 0;
		}
		for (size_t i = min(a.digits.size(), rhs.digits.size()); i < max(a.digits.size(), rhs.digits.size()); i++)
		{
			if (a.digits.size() < rhs.digits.size())
			{
				a.digits.push_back(rhs.digits[i]);
			}
			unsigned long long test = (unsigned long long)a.digits[i] + (unsigned long long)carry;
			a.digits[i] += carry;
			(test > logic_base) ? carry = 1 : carry = 0;
		}
		if (carry)
		{
			a.digits.push_back(carry);
		}
		normalize(a);
	}

	void big_integer::sub_absolute(big_integer& a, big_integer const& rhs)
	{
		unsigned int carry = 0;
		if (a > rhs)
		{
			a.sign = 1;
		}
		else
		{
			a.sign = -1;
		}
		size_t minlen = min(rhs.digits.size(), a.digits.size());
		for (size_t i = 0; i < minlen; i++)
		{
			unsigned int carryold = carry;
			unsigned int digit, other;
			if (a.sign == 1) 
			{
				digit = a.digits[i];
				other = rhs.digits[i];
			}
			else 
			{
				digit = rhs.digits[i];
				other = a.digits[i];
			}
			(other + carry > digit) ? carry = 1 : carry = 0;
			a.digits[i] = digit - other - carryold;
		}

		while (carry) 
		{
			if (a.digits.size() <= minlen)
			{
				a.digits.push_back(0);
			}
			unsigned int digit;
			a.sign == 1 ? digit = a.digits[minlen] : digit = rhs.digits[minlen];
			unsigned int carrynew = (digit == 0) ? 1 : 0;
			(digit != 0) ? digit -= carry : digit = UINT_MAX;
			a.digits[minlen] = digit;
			carry = carrynew;
			minlen++;
		}
		normalize(a);
		if (a.is_zero())
		{
			a.sign = 0;
		}
	}
	big_integer& big_integer::operator+=(big_integer const& rhs)
	{
		if ((sign >= 0) && (rhs.sign >= 0))
		{
			add_absolute(*this, rhs);
		}
		else if ((sign == -1) && (rhs.sign == -1))
		{
			sign = -sign;
			add_absolute(*this, -rhs);
			sign = -sign;
		}
		else if ((sign == -1) && (rhs.sign >= 0))
		{
			sign = -sign;
			sub_absolute(*this, rhs);
			sign = -sign;
		}
		else
		{
			sub_absolute(*this, -rhs);
		}
		return *this;
	}

	big_integer& big_integer::operator-=(big_integer const& rhs)
	{
		if ((sign >= 0) && (rhs.sign >= 0))
		{
			sub_absolute(*this, rhs);
		}
		else if ((sign == -1) && (rhs.sign == -1))
		{
			sign = -sign;
			sub_absolute(*this, -rhs);
			sign = -sign;
		}
		else if ((sign == -1) && (rhs.sign >= 0))
		{
			sign = -sign;
			add_absolute(*this, rhs);
			sign = -sign;
		}
		else
		{
			add_absolute(*this, -rhs);
		}
		return *this;
	}

	big_integer& big_integer::operator*=(big_integer const& rhs)
	{
		int startsign = sign;
		multiply_absolute(*this, rhs);
		sign = rhs.sign * startsign;
		return *this;
	}
	big_integer big_integer::multiply_short(big_integer const& num, unsigned int x)
	{
		big_integer tmp(num);
		unsigned int remainder = 0;
		for (size_t i = 0; i < tmp.digits.size(); i++)
		{
			unsigned long long product = tmp.digits[i] * (unsigned long long)(x) + remainder;
			tmp.digits[i] = (unsigned int)(product & (logic_base));
			remainder = product >> (bits_in_base);
		}
		if (remainder) tmp.digits.push_back(remainder);
		tmp.sign = 1;
		normalize(tmp);
		return tmp;
	}
	void big_integer::multiply_absolute(big_integer& a, big_integer const& b)
	{
		big_integer result;
		result.sign = 1;
		result.digits.resize(a.digits.size() + b.digits.size() + 2, 0);
		for (size_t i = 0; i < a.digits.size(); i++) 
		{
			unsigned long long storage = 0, borrow = 0, temp = 0;
			for (size_t j = 0; j < b.digits.size(); j++) 
			{
				storage = (unsigned long long)a.digits[i] * (unsigned long long)b.digits[j];
				temp = (storage&logic_base) + result.digits[i + j] + borrow;
				result.digits[i + j] = (unsigned int)(temp&logic_base);
				borrow = (storage >> (bits_in_base)) + (temp >> (bits_in_base));
			}
			result.digits[i + b.digits.size()] += (unsigned int)borrow;
		}
		normalize(result);
		std::swap(a, result);
	}
	unsigned int big_integer::trial(big_integer const& r, big_integer const& d, unsigned int k, unsigned int m)
	{
		unsigned int km = k + m;
		if (r == 0) return 0;
		unsigned long long r2 = ((unsigned long long)r.digits[km] << (bits_in_base)) + ((unsigned long long)r.digits[km - 1]);
		unsigned long long d1 = (unsigned long long)d.digits[m - 1];
		unsigned long long left = r2 / d1;
		return std::min((unsigned int)(left), UINT_MAX);
	}
	bool big_integer::smaller(big_integer const& r, big_integer const& d, unsigned int k, unsigned int m)
	{
		size_t i = m, j = 0;
		while (i != j) 
		{
			if (r.digits[i + k] != d.digits[i])
			{
				j = i;
			}
			else
			{
				--i;
			}
		}
		return r.digits[i + k] < d.digits[i];
	}
	big_integer big_integer::digit_quot(big_integer const& num, unsigned int x)
	{
		unsigned long long tmp = 0;
		big_integer result;
		for (size_t i = num.digits.size() - 1; i < num.digits.size(); i--) {
			tmp = (tmp << (bits_in_base)) + num.digits[i];
			result.digits.push_back(tmp / x);
			tmp %= x;
		}
		result.digits.reverse();
		normalize(result);
		return result;
	}
	unsigned int big_integer::digit_rem(big_integer const& num, unsigned int x)
	{
		unsigned long long tmp = 0;
		for (size_t i = num.digits.size() - 1; i < num.digits.size(); i--) 
		{
			tmp = ((tmp << (bits_in_base)) + num.digits[i]) % x;
		}
		return (unsigned int)tmp;
	}
	pair<big_integer, big_integer> big_integer::divide(big_integer const& rhs)
	{
		big_integer r, q, d;
		q.sign = 1;
		size_t n = digits.size(), m = rhs.digits.size();
		unsigned int f = (unsigned int)(BASE / (rhs.digits[rhs.digits.size() - 1] + 1));
		if (f != 0)
		{
			r = multiply_short(*this, f);
			d = multiply_short(rhs, f);
		}
		else
		{
			r = *this << (bits_in_base);
			d = rhs << (bits_in_base);
		}
		q.digits.resize(n - m + 2, 0);
		r.digits.push_back(0);
		for (size_t k = n - m; k < n - m + 1; k--)
		{
			unsigned int qt = trial(r, d, k, m);
			if (qt == 0) continue;
			big_integer dq = multiply_short(d, qt);
			dq.digits.push_back(0);
			while (smaller(r, dq, k, m)) 
			{
				--qt;
				dq -= d;
			}
			q.digits[k] = qt;
			r -= dq << ((bits_in_base)*k);
		}
		if (f != 0)
		{
			r = digit_quot(r, f);
		}
		else
		{
			r = r >> (bits_in_base);
		}
		normalize(q);
		std::pair<big_integer, big_integer> ans{q, r};
		return ans;
	}
	pair<big_integer, big_integer> big_integer::division(big_integer const& rhs)
	{
		if (rhs.is_zero()) 
		{
			throw(std::runtime_error("Division by zero!"));
		}
		if (*this < rhs)
		{
			return {0, *this};
		}
		if (*this == rhs)
		{
			return {1, 0};
		}
		if (rhs.digits.size() == 1) 
		{
			return {digit_quot(*this, rhs.digits[0]), digit_rem(*this, rhs.digits[0])};
		}
		return divide(rhs);
	}
	big_integer& big_integer::operator/=(big_integer const& rhs)
	{
		bool signum = (this->sign == rhs.sign);
		abs(*this);
		if (rhs.sign < 0) 
		{
			big_integer tmp = rhs;
			abs(tmp);
			*this = division(tmp).first;
		}
		else 
		{
			*this = division(rhs).first;
		}
		if (signum) 
		{
			this->sign = 1;
		}
		else
		{
			this->sign = -1;
		}
		normalize(*this);
		return *this;
	}

	big_integer& big_integer::operator%=(big_integer const& rhs)
	{
		bool signum = ((this->sign <= 0) && (rhs.sign>0));
		abs(*this);
		if (rhs.sign < 0) 
		{
			big_integer tmp = rhs;
			abs(tmp);
			*this = division(tmp).second;
		}
		else 
		{
			*this = division(rhs).second;
		}
		if (signum) 
		{
			this->sign = -1;
		}
		else
		{
			this->sign = 1;
		}
		normalize(*this);
		return *this;
	}

	big_integer& big_integer::logic_operation(big_integer const& rhs, unsigned int (op)(unsigned int const&, unsigned int const&))
	{
		big_integer temp = rhs;
		this->to_twos_complement();
		temp.to_twos_complement();
		if (digits.size() < temp.digits.size())
		{
			size_t old_size = digits.size();
			digits.resize(temp.digits.size());
			for (size_t i = old_size; i < digits.size(); i++)
			{
				digits[i] = 0;
			}
		}
		if (digits.size() > temp.digits.size())
		{
			size_t old_size = temp.digits.size();
			temp.digits.resize(digits.size());
			for (size_t i = old_size; i < temp.digits.size(); i++)
			{
				temp.digits[i] = 0;
			}
		}
		for (size_t i = 0; i < digits.size(); i++)
		{
			digits[i] = op(digits[i], temp.digits[i]);
		}
		if (this->is_zero())
		{
			sign = 0;
		}
		this->to_normal();
		return *this;
	}
	unsigned int big_integer::and2(unsigned int const &a, unsigned int const &b)
	{
		return a & b;
	}
	unsigned int big_integer::or2 (unsigned int const &a, unsigned int const &b)
	{
		return a | b;
	}
	unsigned int big_integer:: xor2 (unsigned int const &a, unsigned int const &b)
	{
		return a ^ b;
	}
	big_integer& big_integer::operator&=(big_integer const& rhs)
	{
		return logic_operation(rhs, &and2);
	}

	big_integer& big_integer::operator|=(big_integer const& rhs)
	{
		return logic_operation(rhs, &or2);
	}

	big_integer& big_integer::operator^=(big_integer const& rhs)
	{
		return logic_operation(rhs, &xor2);
	}

	big_integer& big_integer::operator<<=(int rhs)
	{
		unsigned int delta = 0;
		my::vector result(digits.size() + (rhs + 31) / 32);
		size_t j = 0;
		while (rhs >= int(bits_in_base)) {
			rhs -= (bits_in_base);
			result[j++] = 0;
		}
		if (!rhs) 
		{
			for (size_t i = 0; i < digits.size(); i++) 
			{
				result[j++] = digits[i];
			}
			digits = result;
			normalize(*this);
			return *this;
		}
		for (size_t i = 0; i < digits.size(); i++) 
		{
			unsigned int newdigit = delta | (digits[i] << rhs);
			result[j++] = newdigit;
			delta = (unsigned int)((unsigned long long)(digits[i] >> (bits_in_base - rhs)));
		}
		if (delta)
		{
			result[j++] = delta;
		}
		digits = result;
		normalize(*this);
		return *this;
	}

	big_integer& big_integer::operator>>=(int rhs)
	{
		unsigned int delta = 0;
		if (sign == -1) 
		{
			*this -= 1;
		}
		my::vector result;
		size_t start = 0;
		while (rhs >= int(bits_in_base)) 
		{
			start = rhs / (bits_in_base);
			rhs %= bits_in_base;
		}
		for (size_t i = digits.size() - 1; i >= start && i < digits.size(); i--) 
		{
			unsigned int newdigit = (digits[i] >> rhs)&((unsigned int)((unsigned int)1 << ((unsigned int)bits_in_base - (unsigned int)rhs)) - 1);
			newdigit |= delta;
			delta = (digits[i] & ((unsigned int)((unsigned int)((unsigned int)1 << (unsigned int)rhs) - 1))) << (unsigned int)(bits_in_base - rhs);
			result.push_back(newdigit);
		}
		result.reverse();
		digits = result;
		if (sign < 0) 
		{
			*this -= 1;
		}
		normalize(*this);
		return *this;
	}

	big_integer big_integer::operator+() const
	{
		return *this;
	}

	big_integer big_integer::operator-() const
	{
		big_integer ans = big_integer(*this);
		ans.sign = -ans.sign;
		return ans;
	}

	big_integer big_integer::operator~() const
	{
		big_integer ans = *this;
		ans.to_twos_complement();
		for (size_t i = 0; i < ans.digits.size(); i++)
		{
			ans.digits[i] = ~ans.digits[i];
		}
		if (ans.is_zero())
		{
			ans.sign = 0;
		}
		ans.to_normal();
		return ans;
	}

	big_integer& big_integer::operator++()
	{
		*this += 1;
		return *this;
	}

	big_integer big_integer::operator++(int)
	{
		big_integer tmp = *this;
		++(*this);
		return tmp;
	}

	big_integer& big_integer::operator--()
	{
		*this -= 1;
		return *this;
	}

	big_integer big_integer::operator--(int)
	{
		big_integer tmp = *this;
		--(*this);
		return tmp;
	}
	void big_integer::to_twos_complement()
	{
		if (sign == -1) 
		{
			for (size_t i = 0; i < digits.size(); i++) 
			{
				digits[i] = ~digits[i];
			}
			sign = 1;
			*this += 1;
		}
	}
	void big_integer::to_normal()
	{
		if (digits[digits.size() - 1] > (logic_base >> 1)) 
		{
			for (size_t i = 0; i < digits.size(); i++)
			{
				digits[i] = ~digits[i];
			}
			*this += 1;
			sign = -1;
		}
	}
	void big_integer::normalize(big_integer& a)
	{
		size_t x = a.digits.size() - 1;
		while (x > 0 && a.digits[x] == 0 )
		{
			x--;
		}
		a.digits.resize(x + 1);
		if (a.digits.size() == 1 && a.digits[0] == 0) 
		{
			a.sign = 0;
		}
	}
	bool big_integer::is_zero() const
	{
		if ((digits.size() == 1) && (digits[0]) == 0) return true;
		else return false;
	}
	void big_integer::abs(big_integer& a)
	{
		if (a.sign == -1) 
		{
			a.sign = 1;
		}
	}
big_integer operator+(big_integer a, big_integer const& b)
{
	return a += b;
}

big_integer operator-(big_integer a, big_integer const& b)
{
	return a -= b;
}

big_integer operator*(big_integer a, big_integer const& b)
{
	return a *= b;
}

big_integer operator/(big_integer a, big_integer const& b)
{
	return a /= b;
}

big_integer operator%(big_integer a, big_integer const& b)
{
	return a %= b;
}

big_integer operator&(big_integer a, big_integer const& b)
{
	return a &= b;
}

big_integer operator|(big_integer a, big_integer const& b)
{
	return a |= b;
}

big_integer operator^(big_integer a, big_integer const& b)
{
	return a ^= b;
}

big_integer operator<<(big_integer a, int b)
{
	return a <<= b;
}

big_integer operator>>(big_integer a, int b)
{
	return a >>= b;
}

bool operator==(big_integer const& a, big_integer const& b)
{
	if (a.sign != b.sign)
	{
		return false;
	}
	if (a.digits.size() != b.digits.size())
	{
		return false;
	}
	for (size_t i = a.digits.size() - 1; i < a.digits.size(); i--)
	{
		if (a.digits[i] != b.digits[i])
		{
			return false;
		}
	}
	return true;
}

bool operator!=(big_integer const& a, big_integer const& b)
{
	return !(a == b);
}

bool operator<(big_integer const& a, big_integer const& b)
{
	if (a.sign != b.sign)
	{
		return a.sign < b.sign;
	}
	if (a.digits.size() > b.digits.size())
	{
		return false;
	}
	if (b.digits.size() > a.digits.size())
	{
		return true;
	}
	size_t len = std::min(a.digits.size(), b.digits.size());
	for (size_t i = len - 1; i < len; i--) 
	{
		if (a.digits[i] < b.digits[i])
		{
			return true;
		}
		if (a.digits[i] > b.digits[i])
		{
			return false;
		}
	}
	return false;
}

bool operator>(big_integer const& a, big_integer const& b)
{
	return !((a < b) || (a == b));
}

bool operator<=(big_integer const& a, big_integer const& b)
{
	return !(a > b);
}

bool operator>=(big_integer const& a, big_integer const& b)
{
	return !(a < b);
}

std::string to_string(big_integer const& a)
{
	string str = "";
	big_integer tmp = a;
	if (tmp.is_zero())
	{
		tmp.sign = 0;
	}
	int sign = tmp.sign;
	big_integer::abs(tmp);
	while (tmp > 0) 
	{
		str += to_string(tmp.digit_rem(tmp, 10));
		tmp = tmp.digit_quot(tmp, 10);
	}
	if (sign == -1)
	{
		str.push_back('-');
	}
	if (a == 0)
	{
		str = "0";
	}
	std::reverse(str.begin(), str.end());
	return str;
}

std::ostream& operator<<(std::ostream& s, big_integer const& a)
{
	return s << to_string(a);
}
