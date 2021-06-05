#include <iostream>
#include <time.h>
#include <string.h>
#include <limits>
#define BASE_SIZE (sizeof(BASE)*8)
using namespace std;
typedef unsigned short BASE;
typedef unsigned int DBASE;
BASE MAX_BASE_VALUE = ~0;


class BigNum
{
private:
	int len;
	int maxLen;
	BASE *digits;
public:

	int weirdSub(const BigNum &that, int ix)
	{
		DBASE tmp;
		int k = 0;
		for (int i = 0; i < that.len; ++i)
		{
			tmp = digits[i + ix] + MAX_BASE_VALUE - that.digits[i] - k;
			digits[ix + i] = (BASE) tmp;
			k = !(tmp >> BASE_SIZE);
		}
		for (int i = that.len; k && i <= len; ++i, k = !(tmp >> BASE_SIZE))
		{
			tmp = digits[ix + i] + MAX_BASE_VALUE - k;
			digits[i + ix] = (BASE) tmp;
		}
		lenNorm();
		return k;
	}
	void weirdAdd(BigNum &that, int ix)
	{
		int k = 0;
		for (int i = 0; i < that.len - 2; ++i)
		{
			DBASE tmp = digits[ix + i] + that.digits[i] + k;
			k = tmp >> BASE_SIZE;
			digits[i + ix] = tmp;
		}
		lenNorm();
	}

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
	BigNum & operator= (const BASE that)
	{
		delete[] digits;
		len = 1;
		digits = new BASE[1];
		digits[0] = that;
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
	BigNum operator* (const BASE num)
	{
		BigNum mul(len + 1, true);
		DBASE tmp = 0;
		BASE k = 0;
		for (int j = 0; j < len; ++j)
		{
			tmp = ((DBASE) digits[j]) * ((DBASE) num) + k;
			k = tmp >> BASE_SIZE;
			mul.digits[j] = (BASE) tmp;
		}
		mul.digits[len] = k;
		mul.lenNorm();
		return mul;
	}
	BigNum operator*= (const BASE num)
	{
		*this = *this * num;
		return *this;
	}
	BigNum operator* (const BigNum &that)
	{
		BigNum mul(len + that.len, true);
		DBASE tmp, k;
		for (int i = 0; i < len; ++i)
		{
			k ^= k;
			for (int j = 0; j < that.len; ++j)
			{
				tmp = (DBASE) that.digits[j] * digits[i] + mul[i + j] + k;
				mul.digits[i + j] = tmp;
				k = tmp >> BASE_SIZE;
			}
			mul.digits[that.len + i] += k;
		}
		mul.lenNorm();
		return mul;
	}
	BigNum operator*= (const BigNum &that)
	{
		*this = (*this) * (that);
		return *this;
	}
	BigNum operator/ (const BASE num)
	{
		if (num <= 0) return 0;
		BigNum d(len, true);
		DBASE tmp = 0, k = 0;
		for (int i = len - 1; i >= 0; --i)
		{
			tmp = k << BASE_SIZE + digits[i];
			k = tmp % num;
			d.digits[i] = tmp / num;
		}
		d.lenNorm();
		return d;
	}
	BigNum operator/= (const BASE num)
	{
		*this = *this / num;
		return *this;
	}
	BASE operator% (const BASE num)
	{
		if (num <= 0) return 0;
		DBASE tmp, mod;
		for (int i = len - 1; i >= 0; --i)
		{
			tmp = mod << BASE_SIZE + digits[i];
			mod = tmp % num;
		}
		return mod;
	}
	BASE operator%= (const BASE num)
	{
		BASE mod = *this % num;
		return mod;
	}
	
	BigNum operator/ (BigNum &that)
	{
		if (that.len == 1 && that.digits[0] != 0)
		{
			BigNum res = *this / that.digits[0];
			return res;
		}
		if (that.len == 1 && that.digits[0] == 0 || *this == that) return 1;
		if (*this < that) return 0;
		BigNum q(len - that.len + 1);
		DBASE d = MAX_BASE_VALUE / (that.digits[that.len - 1] + 1), q_, r_;
		BigNum u(*this), v(that);	//чтобы было проще идти по алгоритму (соответствие переменных)
		u *= d;
		v *= d;
		int j = len - that.len;
		if (len == u.len)
		{
			if (u.len == u.maxLen)
			{
				BigNum tmp(u);
				delete[] u.digits;
				u.digits = new BASE[u.maxLen + 1];
				for (int i = 0; i < tmp.len; ++i) u.digits[i] = tmp.digits[i];
				u.maxLen++;
			}
			u.digits[len] = 0;
			u.len = len + 1;
		}
		while (j >= 0)
		{
			q_ = (u.digits[j + that.len] * MAX_BASE_VALUE + u.digits[j + that.len - 1]) / (v.digits[that.len - 1]);
			r_ = (u.digits[j + that.len] * MAX_BASE_VALUE + u.digits[j + that.len - 1]) % (v.digits[that.len - 1]);
			if (q_ >= MAX_BASE_VALUE || q_ * v.digits[that.len - 2] > MAX_BASE_VALUE * r_ + u.digits[j + that.len - 2])
			{
				--q_;
				r_ += v.digits[that.len - 1];
				if ((r_ < MAX_BASE_VALUE) &&(q_ >= MAX_BASE_VALUE || q_ * v.digits[that.len - 2] > MAX_BASE_VALUE * r_ + u.digits[j + that.len - 2])) 
				{
					--q_;
					r_ += v.digits[that.len - 1];
				}
			}
			q.digits[j] = q_;
			int k = u.weirdSub(v * q_, j);
			if (k)
			{
				q.digits[j]--;
				u.weirdAdd(v, j);
			}
			--j;
		}
		q.lenNorm();
		return q;
	}

	BigNum operator/= (BigNum &that)
	{
		*this = *this / that;
		return *this;
	}
	
	BigNum operator% (BigNum &that)
	{
		if (that.len == 1 && that.digits[0] != 0)
		{
			BigNum r(1, 1);
			r.digits[0] = *this % that.digits[0];
			return r;
		}
		if (that.digits[0] == 0) throw invalid_argument("ERROR: DIVISION BY ZERO");
		if (that == *this)
		{
			BigNum r = 0;
			return r;
		}
		if(*this < that) return *this;
		BigNum r(that.len - 1), u(*this), v(that);
		DBASE d = MAX_BASE_VALUE / (that.digits[that.len - 1] + 1);
		u *= d;
		v *= d;
		if (len == u.len)
		{
			if (u.len == u.maxLen)
			{
				BigNum tmp(u);
				delete[] u.digits;
				u.digits = new BASE[u.maxLen + 1];
				for (int i = 0; i < tmp.len; ++i) u.digits[i] = tmp.digits[i];
				u.maxLen++;
			}
			u.digits[len] = 0;
			u.len = len + 1;
		}
		int j = len - that.len;
		DBASE q_, r_;
		do
		{
			q_ = ((u.digits[j + that.len] * MAX_BASE_VALUE+u.digits[j + that.len - 1]) / v.digits[that.len - 1]);
			r_ = ((u.digits[j + that.len] * MAX_BASE_VALUE+u.digits[j + that.len - 1]) % v.digits[that.len - 1]);
			if (q_ >= MAX_BASE_VALUE || q_ * v.digits[that.len - 2] > MAX_BASE_VALUE * r_ + u.digits[j + that.len - 2])
			{
				--q_;
				r_ += v.digits[that.len - 1];
				if ((r_ < MAX_BASE_VALUE) &&(q_ >= MAX_BASE_VALUE || q_ * v.digits[that.len - 2] > MAX_BASE_VALUE * r_ + u.digits[j + that.len - 2])) 
				{
					--q_;
					//r_ += v.digits[that.len - 1];
				}
			}
			int k = u.weirdSub(v * q_, j);
			if (k) u.weirdAdd(v, j);
			--j;
		} while (j >= 0);
		r = u / d;
		r.lenNorm();
		return r;
	}
	BigNum operator%= (BigNum &that)
	{
		*this = *this % that;
		return *this;
	}
	

/*

	friend istream & operator>> (istream &in, BigNum &that)
	{
		string s;
		in >> s;
		if (s.length() > that.maxLen)
		{
			cerr << "s.length() > that.maxLen" << endl;
			exit(0);
		}
		DBASE tmp = 0;
		BASE k = 0;
		delete[] that.digits;
		that.len = s.length();
		that.digits = new BASE[that.maxLen];
		for (int i = 0; i < that.maxLen; ++i) that.digits[i] = 0;
		for (int i = 0; i < that.len; ++i)
		{
			if (s[i] >= '0' && s[i] <= '9')
			{
				char _ch = s[i] - '0';
				that *= 10;
				that +=_ch;
			}
			else break;
		}
		that.lenNorm();
		return in;
	}
	friend ostream & operator<< (ostream &out, BigNum &that)
	{
		string s = "";
		BigNum tmp = that;
		while (tmp.digits[0] != 0 || tmp.len > 1)
		{
			s.push_back(tmp % 10 + '0');
			tmp /= 10;
		}
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
	BigNum a(4, false), b(4, false), c, d(4, false), cop(a);
	cout << "a = " << a << endl << "b = " << b << endl << "d = " << d << endl << "copy of a = " << cop << endl;
	c = a + b;
	cout << "a + b = " << c << endl;
	c -= a;
	cout << "a + b - a = " << c << endl;
	c = a * b;
	cout << "a * b = " << c << endl;
	c *= d;
	cout << "(a * b) * d = " << c << endl;
	cout << "a = " << a << " b = " << b << endl;
	c = a / b;
	cout << "a / b = " << c << endl;
	c = b / a;
	cout << "b / a = " << c << endl;
	c = a % b;
	cout << "a % b = " << c << endl;
	c = b % a;
	cout << "b % a = " << c << endl;
	//calculatori.ru
	return 1;
}
