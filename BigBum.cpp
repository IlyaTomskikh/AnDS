#include <iostream>
#include <time.h>
#include <string.h>
using namespace std;
typedef unsigned short BASE;
typedef unsigned int DBASE;
#define BASE_SIZE (sizeof(BASE)*8)

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
		if (flag)
		{
			//len = 1;
			for(int i = 0; i < maxLen; ++i) digits[i] = 0;
		}
		else
		{
			srand(time(0));
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
	BigNum & operator= (BigNum &that)
	{
		if (this == &that) return *this;
		len = that.len;
		maxLen = that.maxLen;
		delete[] digits;
		digits = new BASE[maxLen];
		for (int i = 0; i < maxLen; ++i) digits[i] = that.digits[i];
		return *this;
	}
	friend istream & operator>> (istream &in, BigNum &that)
	{
		std::string str;
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
			if (str[i] <= 'f' && str[i] >= 'a') tmp = str[i] - 'a' + 10;
			if (str[i] <= 'F' && str[i] >= 'A') tmp = str[i] - 'A' + 10;
			that.digits[j] |= tmp << (k*4);
			++k;
			if (k >= BASE_SIZE / 4)
			{
				//cout << "that.digits[" << j << "] = " << hex << that.digits[j] << dec << endl;
				k = 0;
				++j;
			}
		}
		for(int i = that.maxLen; i--;) if (that.digits[i] == 0) --(that.len);
		return in;
	}
	friend ostream & operator<< (ostream &out, BigNum &that)
	{
		cout.width(BASE_SIZE / 4);
		cout.fill('0');
		if (that.digits != NULL) for (int i = that.len; i--;) out << hex << that.digits[i];
		else out << ' ';
		cout << dec;
		return out;
	}
	bool operator== (BigNum &that)
	{
		if (len != that.len) return false;
		for (int i = 0; i < len; ++i) if (digits[i] != that.digits[i]) return false;
		return true;
	}
	bool operator!= (BigNum &that)
	{
		return !(*this == that);
	}
	bool operator< (BigNum &that)
	{
		if (this == &that) return false;
		if (len != that.len) return len < that.len;
		for (int i = 0; i < len; ++i) if (digits[i] != that.digits[i]) return digits[i] < that.digits[i];
		return false;
	}
	bool operator> (BigNum &that)
	{
		if (this == &that) return false;
		if (len != that.len) return len < that.len;
		for (int i = 0; i < len; ++i) if (digits[i] != that.digits[i]) return digits[i] > that.digits[i];
		return false;
	}
	bool operator<= (BigNum &that)
	{
		return !(*this > that);
	}
	bool operator>= (BigNum &that)
	{
		return !(*this < that);
	}
	BigNum operator+ (BigNum &that)
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
	BigNum operator+= (BigNum &that)
	{
		return (*this + that);
	}
	BigNum operator- (BigNum &that)
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
				++j;
			}
			while (j < len)
			{
				tmp = (1 << BASE_SIZE) + that.digits[j];
				tmp -= k;
				sub.digits[j] = (BASE) tmp;
				k = !(tmp >> BASE_SIZE);
				++j;
			}
			sub.lenNorm();
		}
		return sub;
	}
	BigNum operator-= (BigNum &that)
	{
		return (*this - that);
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
	BigNum operator* (BigNum &that)
	{
		BigNum mul2 (len + that.len - 1, true);
		for (int j = 0; j < len; ++j)
		{
			if (that.digits[j] != 0)
			{
				int i = 0, k = 0;
				DBASE tmp;
				for (; i < that.len; ++i, k = tmp >> BASE_SIZE)
				{
					tmp = digits[i] * (that.digits[j]) + mul2.digits[i + j] + k;
					mul2.digits[i + j] = (BASE) tmp;
				}
			}
		}
		mul2.lenNorm();
		return mul2;
	}
	BigNum operator*= (BigNum &that)
	{
		return ((*this) * that);
	}
	BigNum operator/ (BASE num)
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
	BASE operator% (BASE num)
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
	BigNum operator/ (BigNum &that)
	{
		BigNum zer(that.len, true), one(that.len, true);
		one.digits[0] = 1;
		if (*this == that) return one;
		if (that <= zer || *this < that) return zer;
		BigNum div2(len, true);
		BigNum r(that.len, true);

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

	friend istream & operator>> (istream &in, BigNum &that)
	{
		std::string s;
		in >> s;
		if (s.length() > that.maxLen) return in;
		delete[] that.digits;
		that.len = (s.length() - 1) / (BASE_SIZE / 4) + 1;
		that.digits = new BASE[that.len];
		int j = 0;
		//создаем большое число, которое будет состоять из одной ячейки типа BASE
		BigNum t(1, true);
		for (int i = that.len; i--;) that.digits[i] = 0;
		do
		{
			t.digits[0] = s[j] - '0';
			that = that * 10 + t;
			++j;
		} while (j < s.length());
		return in;
	}
	friend ostream & operator<< (ostream &out, BigNum &that)
	{
		//cout.width(BASE_SIZE / 4);
		//cout.fill('0');
		if (that.digits != NULL) for (int i = that.len; i--;) out << that.digits[i];
		else out << ' ';
		return out;
	}

	void lenNorm()
	{
		for(int i = maxLen; i--;) if (digits[i] == 0) --len;
	}
};

int main()
{
	return 0;
}
