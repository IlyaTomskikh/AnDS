#include <iostream>
#include <time.h>
#include <string.h>
#define BASE_SIZE (sizeof(BASE)*8)
using namespace std;
typedef unsigned short BASE;
typedef unsigned int DBASE;


class BigNum
{
private:
	int len;
	int maxLen;
	BASE *digits;
public:
	BigNum(int l = 1, bool flag = true): len(l), maxLen(l)
	{
		digits = new BASE[maxLen];
		if (flag) for(int i = 0; i < maxLen; ++i) digits[i] = 0;
		else
		{
			for(int i = 0; i < maxLen; ++i) digits[i] = rand();
			for(int i = maxLen; i--;) if (digits[i] == 0) --len;
		}
	}
	BigNum(const BigNum &that): len(that.len), maxLen(that.maxLen)
	{
		digits = new BASE[maxLen];
		for (int i = 0; i < maxLen; ++i) digits[i] = that.digits[i];
	}
	~BigNum()
	{
		len = 0;
		maxLen = 0;
		delete[] digits;
		digits = NULL;
	}
	BigNum & operator= (const BigNum &that)
	{
		if (this != &that)
		{
			len = that.len;
			maxLen = that.maxLen;
			delete[] digits;
			digits = new BASE[maxLen];
			for (int i = 0; i < maxLen; ++i) digits[i] = that.digits[i];
		}
		return *this;
	}
	
	BASE & operator[](int ix)
	{
		return digits[ix];
	}

	friend istream & operator>> (istream &in, BigNum &that)
	{
		string str;
		in >> str;
		if (str.length() > that.maxLen) return in;
		delete[] that.digits;
		that.len = (str.length() - 1) / (BASE_SIZE / 4) + 1;
		that.digits = new BASE[that.len];
		for (int i = that.len; i--;) that.digits[i] = 0;
		int j = 0, k = 0;
		BASE tmp = 0;
		for (int i = str.length(); i--;)
		{
			if (str[i] <= '9' && str[i] >= '0') tmp = str[i] - '0';
			else
			{
				if (str[i] <= 'f' && str[i] >= 'a') tmp = str[i] - 'a' + 10;
				else
				{
					if (str[i] <= 'F' && str[i] >= 'A') tmp = str[i] - 'A' + 10;
					else break;
				}
			}
			that.digits[j] |= tmp << (k*4);
			++k;
			if (k >= BASE_SIZE / 4)
			{
				//cout << "that.digits[" << j << "] = " << hex << that.digits[j] << dec << endl;
				k = 0;
				++j;
			}
		}
		that.lenNorm();
		return in;
	}
	friend ostream & operator<< (ostream &out, BigNum &that)
	{
		cout.width(BASE_SIZE / 4);
		cout.fill('0');
		if (that.digits != NULL) for (int i = that.len; i--;) out << hex << int(that.digits[i]);
		else out << ' ';
		cout << dec;
		return out;
	}
	
	bool operator== (const BigNum &that)
	{
		if (len != that.len) return false;
		for (int i = 0; i < len; ++i) if (digits[i] != that.digits[i]) return false;
		return true;
	}
	bool operator!= (const BigNum &that)
	{
		return !(*this == that);
	}
	bool operator< (const BigNum &that)
	{
		if (this == &that) return false;
		if (len != that.len) return len < that.len;
		for (int i = len; i--;) if (digits[i] != that.digits[i]) return digits[i] < that.digits[i];
		return false;
	}
	bool operator> (const BigNum &that)
	{
		if (this == &that) return false;
		if (len != that.len) return len > that.len;
		for (int i = len; i--;) if (digits[i] != that.digits[i]) return digits[i] > that.digits[i];
		return false;
	}
	bool operator<= (const BigNum &that)
	{
		return !(*this > that);
	}
	bool operator>= (const BigNum &that)
	{
		return !(*this < that);
	}
	BigNum operator+ (const BigNum &that)
	{
		BigNum sum(max(maxLen, that.maxLen) + 1);
		int nm = min(len, that.len), j = 0, k = 0;
		DBASE tmp = 0;
		while (j < nm)
		{
			tmp = digits[j] + that.digits[j] + k;
			sum.digits[j] = (BASE) tmp;
			k = (BASE) (tmp >> BASE_SIZE);
			++j;
		}
		while (j < len)
		{
			tmp = digits[j] + k;
			sum.digits[j] = (BASE) tmp;
			k = (BASE) (tmp >> BASE_SIZE);
			++j;
		}
		while (j < that.len)
		{
			tmp = that.digits[j] + k;
			sum.digits[j] = (BASE) tmp;
			k = (BASE) (tmp >> BASE_SIZE);
			++j;
		}
		sum.digits[j] = k;
		sum.lenNorm();
		return sum;
	}
	BigNum operator+= (const BigNum &that)
	{
		*this = *this + that;
		return *this;
	}
	BigNum operator+ (BASE num)
	{
		int k = num, i = 0;
		BigNum sum(maxLen + 1);
		while (i < len)
		{
			DBASE tmp = digits[i] + k;
			k = (BASE) (tmp >> BASE_SIZE);
			sum.digits[i] = tmp;
			++i;
		}
		sum.digits[i] = k;
		return sum;
	}
	BigNum operator+= (BASE num)
	{
		*this = *this + num;
		return *this;
	}
	BigNum operator- (const BigNum &that)
	{
		BigNum sub(max(len, that.len), true);
		if (*this >= that)
		{
			DBASE tmp;
			int k = 0, j = 0;
			while (j < that.len)
			{
				tmp = (1 << BASE_SIZE) + digits[j];
				tmp -= (that.digits[j] + k);
				sub.digits[j] = (BASE) tmp;
				k = !(tmp >> BASE_SIZE);
				// ! is safer then ~
				++j;
			}
			while (j < len)
			{
				tmp = (1 << BASE_SIZE) + digits[j];
				tmp -= k;
				sub.digits[j] = (BASE) tmp;
				k = !(tmp >> BASE_SIZE);
				++j;
			}
			sub.lenNorm();
		}
		else
		{
			cerr << "right operand must be less or equal" << endl;
			exit(-1);
		}
		return sub;
	}
	BigNum operator-= (const BigNum &that)
	{
		*this = *this - that;
		return *this;
	}
	BigNum operator* (BASE num)
	{
		BigNum mul1(len + 1, true);
		DBASE tmp = 0;
		int k = 0, j = 0;
		for (; j < len; ++j, k = tmp >> BASE_SIZE)
		{
			tmp = (digits[j] * num) + k;
			mul1.digits[j] = (BASE) tmp;
		}
		mul1.digits[j] = k;
		mul1.lenNorm();
		return mul1;
	}
	BigNum operator*= (BASE num)
	{
		*this = *this * num;
		return *this;
	}
	BigNum operator* (BigNum &that)
	{
		BigNum mul(len + that.len, true);
		DBASE tmp, k;
		for (int i = 0; i < len; ++i)
		{
			k ^= k;
			for (int j = 0; j < that.len; ++j)
			{
				tmp = (DBASE) that[j] * digits[i] + mul[i + j] + k;
				mul[i + j] = tmp;
				k = tmp >> BASE_SIZE;
			}
			mul[that.len + i] += k;
		}
		mul.lenNorm();
		return mul;
	}
	BigNum operator*= (BigNum &that)
	{
		*this = (*this) * (that);
		return *this;
	}
	BigNum operator/ (const BASE num)
	{
		if (num <= 0) return 0;
		BigNum div1(len, true);
		int j = 0;
		BASE r = 0;
		DBASE tmp;
		do
		{
			tmp = (r << BASE_SIZE) + digits[len - 1 - j];
			div1.digits[len - 1 - j] = tmp / num;
			r = tmp % num;
			++j;
		}while (j < len);
		return div1;
	}
	BASE operator% (const BASE num)
	{
		if (num <= 0) return 0;
		BASE module1;
		BigNum div1(len, true);
		int j = 0;
		DBASE tmp;
		do
		{
			tmp = (module1 << BASE_SIZE) + digits[len - 1 - j];
			div1.digits[len - 1 - j] = tmp / num;
			module1 = tmp % num;
			++j;
		}while (j < len);
		return module1;
	}
	/*
	BigNum operator/ (BigNum &that)
	{
		BigNum zer(that.len, true), one(that.len, true);
		one.digits[0] = 1;
		if (*this == that) return one;
		if (that <= zer || *this < that) return zer;
		BigNum div2(len, true);
		BigNum r(that.len, true);
		DBASE d = (that.digits[that.len - 1] + 1) >> BASE_SIZE;
		*this = *this / 10 * d;
		that = that * d;
		int m = len - that.len;
		BASE _r, _q;
		do
		{
			_q = (digits[m + that.len] << BASE_SIZE + digits[m + that.len - 1]) / that.digits[that.len - 1];
			_r = (digits[m + that.len] << BASE_SIZE + digits[m + that.len - 1]) % that.digits[that.len - 1];
			do
			{
				if (_q << BASE_SIZE == _q * _q || _q * that.digits[that.len - 2] > _r << BASE_SIZE + digits[m + that.len - 2])
				{
					--_q;
					_r += that.digits[that.len - 1];
				}
			} while(_r << BASE_SIZE == _r * _r);
			//тут какое-то волшебство с вычетанием из определенной части числа
		} //while ();
		
		div2.lenNorm();
		return div2;
	}
	
	BigNum operator% (BigNum &that)
	{
		BigNum zer(that.len, true);
		if (that <= zer || *this == that) return zer;
		if (*this < that) return *this;
		BigNum q(len - that.len, true);
		BigNum module2(that.len, true);

		if (module2 >= that) return zer;
		return module2;
	}
	*/
/*
	friend istream & operator>> (istream &in, BigNum &that)
	{
		string s;
		in >> s;
		DBASE tmp;
		BASE k;
		that.maxLen = (s.length() - 1) / (BASE_SIZE / 4) + 2;
		that.len = 1;
		delete[] that.digits;
		that.digits = new BASE[that.maxLen];
		for (int i = that.maxLen - 1; i--;) that.digits[i] = 0;
		for (int i = 0; i < that.maxLen; ++i)
		{
			if (s[i] >= '0' && s[i] <= '9')
			{
				that *= 10;
				that += (s[i] - '0');
			}
			else break;
		}
		that.lenNorm();
		return in;
	}
	friend ostream & operator<< (ostream &out, BigNum &that)
	{
		string s = "";
		for (int i = that.len - 1; i--;) s.push_back(char(that % ((BASE) 10) + 48));
		out << s;
		return out;
	}
*/
	void lenNorm()
	{
		len = maxLen;
		for(int i = maxLen - 1; i > 0; --i) if (digits[i] == 0) --len;
	}
};

int main()
{
	srand(time(0));
	BigNum a(4, false), b(4, false), c, d(4, false);
	cout << "a = " << a << endl << "b = " << b << endl << "d = " << d << endl;
	c = a + b;
	cout << "a + b = " << c << endl;
	c -= a;
	cout << "a + b - a = " << c << endl;
	c = a * b;
	cout << "a * b = " << c << endl;
	c *= d;
	cout << "(a * b) * d = " << c << endl;
	//calculatori.ru
	return 1;
}
