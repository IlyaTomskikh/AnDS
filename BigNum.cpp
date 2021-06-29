#include <iostream>
#include <time.h>
#include <string.h>
#include <algorithm>
#include <limits>
#define BASE_SIZE (sizeof(BASE)*8)
#define MAX_BASE_VALUE ((DBASE) 1 << BASE_SIZE)
using namespace std;
typedef unsigned char BASE;
typedef unsigned short DBASE;


class BigNum
{
private:
	int len;
	int maxLen;
	BASE *digits;
public:
	
//	DEC CIN/COUT
	friend istream & operator>> (istream &in, BigNum &that)
	{
		string s;
		in >> s;
		DBASE tmp = 0;
		BASE k = 0;
		delete[] that.digits;
		that.len = 1;
		that.maxLen = (s.length() - 1) / (BASE_SIZE / 4) + 1;
		that.digits = new BASE[that.maxLen];
		for (int i = 0; i < that.maxLen; ++i) that.digits[i] = 0;
		for (int i = 0; i < s.length(); ++i)
		{
			char _ch;
			if (s[i] >= '0' && s[i] <= '9')
			{
				_ch = s[i] - '0';
				that *= 10;
				that +=_ch;
			}
		}
		that.lenNorm();
		return in;
	}
	friend ostream & operator<< (ostream &out, BigNum &that)
	{
		if (!(that.digits[0] == 0 && that.len == 1))
		{
			string s = "";
			BigNum tmp(that);
			while (tmp.digits[0] != 0 || tmp.len > 1)
			{
				char _ch = tmp % 10 + '0';
				s.push_back(_ch);
				tmp /= 10;
			}
			reverse(s.begin(), s.end());
			out << s;
		}
		else out << 0;
		return out;
	}


	BigNum(int l = 1, bool flag = true): len(l), maxLen(l)
	{
		digits = new BASE[len];
		if (flag) for(int i = 0; i < len; ++i) digits[i] = 0;
		else
		{
			for(int i = 0; i < len; ++i)
			{
				digits[i] = rand();
				if (BASE_SIZE >= sizeof(int) * 8) 
				{
					digits[i] = digits[i] << BASE_SIZE / 2;
					digits[i] += rand();
				}
			}
			lenNorm();
		}
	}
	BigNum(const BigNum &that): len(that.len), maxLen(that.maxLen)
	{
		digits = new BASE[maxLen];
		for (int i = 0; i < maxLen; ++i) digits[i] = that.digits[i];
	}
	~BigNum()
	{
		len ^= len;
		maxLen ^= maxLen;
		delete[] digits;
		digits = nullptr;
	}
	BigNum & operator= (const BigNum &that)
	{
		if (this != &that)
		{	
			len = that.len;
			maxLen = that.maxLen;
			delete[] digits;
			digits = new BASE[maxLen];
			for (int i = 0; i < maxLen; i++) digits[i] = that.digits[i];
		}
		return *this;
	}

	
	BASE & operator[](int ix)
	{
		return digits[ix];
	}

//	HEX CIN/COUT
/*
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
		if (that.digits)
		{
			
			for (int i = that.len; i--;)
			{
				cout.width(BASE_SIZE / 4);
				cout.fill('0');
				out << hex << (int)that.digits[i] << dec;
			}
		}
		else out << ' ';
		return out;
	}
*/

	short cmp(const BigNum &that)
	{
		if (this == &that) return 0;
		if (len < that.len) return -1;
		if (len > that.len) return 1;
		for (int i = len - 1; i >= 0; i--)
		{
			if (digits[i] < that.digits[i]) return -1;
			if (digits[i] > that.digits[i]) return 1;
		}
		return 0;
	}
	bool operator== (const BigNum &that)
	{
		for (int i = 0; i < len; ++i) if (digits[i] != that.digits[i]) return false;
		return true;
	}
	bool operator!= (const BigNum &that)
	{
		return !(*this == that);
	}
	bool operator< (const BigNum &that)
	{
		if (cmp(that) < 0) return true;
		else return false;
	}
	bool operator> (const BigNum &that)
	{
		if (cmp(that) > 0) return true;
		else return false;
	}
	bool operator<= (const BigNum &that)
	{
		if (cmp(that) <= 0) return true;
		else return false;
	}
	bool operator>= (const BigNum &that)
	{
		if (cmp(that) >= 0) return true;
		else return false;
	}
	BigNum operator+ (const BigNum &that)
	{
		BigNum sum(max(len, that.len) + 1);
		int nm = min(len, that.len), j = 0, k = 0;
		DBASE tmp = 0;
		while (j < nm)
		{
			tmp = DBASE(digits[j]) + DBASE(that.digits[j]) + DBASE(k);
			sum.digits[j] = (BASE)tmp;
			k = (BASE)(tmp >> BASE_SIZE);
			++j;
		}
		while (j < len)
		{
			tmp = DBASE(digits[j]) + DBASE(k);
			sum.digits[j] = (BASE)tmp;
			k = (BASE)(tmp >> BASE_SIZE);
			++j;
		}
		while (j < that.len)
		{
			tmp = DBASE(that.digits[j]) + DBASE(k);
			sum.digits[j] = (BASE)tmp;
			k = (BASE)(tmp >> BASE_SIZE);
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
		DBASE tmp = 0;
		BASE k = 0;
		int newLen = maxLen;
		if (len + 1 > maxLen) newLen = len + 1;
		BigNum result(newLen);
		tmp = DBASE(digits[0]) + DBASE(num) + DBASE(k);
		k = (BASE)(tmp >> BASE_SIZE);
		result.digits[0] = (BASE)tmp;
		for (int i = 1; i < len; i++)
		{
			tmp = DBASE(digits[i]) + DBASE(k);
			k = (BASE)(tmp >> BASE_SIZE);
			result.digits[i] = (BASE)tmp;
		}
		result.digits[len] = k;
		result.lenNorm();
		return result;
	}
	BigNum operator+= (BASE num)
	{
		*this = *this + num;
		return *this;
	}
	BigNum operator- (const BigNum &that)
	{
		BigNum sub(len);
		if (*this >= that)
		{
			DBASE tmp;
			int k = 0, j = 0;
			while (j < that.len)
			{
				tmp = MAX_BASE_VALUE + digits[j];
				tmp = tmp - that.digits[j] - k;
				sub.digits[j] = (BASE)tmp;
				k = !(tmp >> BASE_SIZE);
				// ! is safer then ~
				++j;
			}
			while (j < len)
			{
				tmp = MAX_BASE_VALUE + digits[j];
				tmp -= k;
				sub.digits[j] = (BASE)tmp;
				k = !(tmp >> BASE_SIZE);
				++j;
			}
			sub.lenNorm();
		}
		else
		{
			cerr << "right operand must be less or equal" << endl;
			return *this;
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
		BigNum mul(len + 1);
		mul.len = len + 1;
		DBASE tmp = 0;
		BASE k = 0;
		for (int j = 0; j < len; ++j)
		{
			tmp = (DBASE(digits[j]) * DBASE(num)) + DBASE(k);
			mul.digits[j] = (BASE)tmp;
			k = tmp >> BASE_SIZE;
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
		BigNum mul(len + that.len);
		DBASE tmp;
		for (int i = 0; i < len; ++i)
		{
			int k = 0;
			for (int j = 0; j < that.len; ++j)
			{
				tmp = DBASE(that.digits[j]) * DBASE(digits[i]) + DBASE(mul.digits[i + j]) + DBASE(k);
				mul.digits[i + j] = (BASE)tmp;
				k = (BASE)(tmp >> BASE_SIZE);
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
		DBASE tmp, k = 0;
		BigNum q(len);
		for(int i = len - 1; i >= 0; --i)
		{
			tmp = (k << BASE_SIZE) + digits[i];
			k = tmp % num;
			q.digits[i] = tmp / num;
		}
		q.lenNorm();
		return q;
	}
	BigNum operator/= (const BASE num)
	{
		*this = *this / num;
		return *this;
	}
	BASE operator% (const BASE num)
	{
		DBASE tmp, r = 0;
		for (int i = len - 1; i >= 0; --i)
		{
			tmp = ((DBASE) r << BASE_SIZE) + digits[i];
			r = (BASE) (tmp % num);
		}
		return r;
	}
	BASE operator%= (const BASE num)
	{
		BASE mod = *this % num;
		return mod;
	}
	
	BigNum operator/ (BigNum &that)
	{
		if (that.len == 1)
		{
			BigNum res = *this / that.digits[0];
			return res;
		}
		if (that.digits[that.len - 1] == 0 || that.len < 1 || *this < that)
		{
			cout << "that.digits[that.len - 1] == 0 || that.len < 1 || *this < that" << endl;
			return *this;
		}
		return divmod(*this, that).first;
	}
	BigNum operator/= (BigNum &that)
	{
		*this = *this / that;
		return *this;
	}
	
	BigNum operator% (BigNum &that)
	{
		if (that.len == 1)
		{
			BigNum r(1);
			r.digits[0] = *this % that.digits[0];
			return r;
		}
		if (that.digits[that.len - 1] == 0 || that.len < 1 || *this < that)
		{
			cout << "that.digits[that.len - 1] == 0 || that.len < 1 || *this < that" << endl;
			return *this;
		}
		return divmod(*this, that).second;
	}
	BigNum operator%= (BigNum &that)
	{
		*this = *this % that;
		return *this;
	}

	friend pair<BigNum, BigNum> divmod(BigNum &a, BigNum &that)	//a div that && a mod that
	{
		BigNum divident = a,
		divisor = that,
		q(a.len);
		q.len = 1;
		//нормализация (D1)
		DBASE q_ = 0, r_ = 0, b = MAX_BASE_VALUE,
		d = MAX_BASE_VALUE / ((DBASE)that.digits[that.len - 1] + (DBASE)1);;
		if(d != 1)
		{
			divident *= d;
			divisor *= d;
		}
		BigNum tmpDivident = divident;
		if (a.len == divident.len)
		{
			delete[] divident.digits;
			divident.digits = new BASE[tmpDivident.len + 1];
			divident.maxLen++;
			divident.len++;
			for(int i = 0; i < tmpDivident.len; ++i) divident.digits[i] = tmpDivident.digits[i];
			divident.digits[divident.len - 1] = 0;
		}
		//начальная установка индекса (D2)
		int j = a.len - that.len, k = divisor.len;
		for(; j >= 0; --j)	//шаг цикла (D7)
		{
			//вычисление q^ и r^, проверка сложного условия (D3)
			q_ = (((DBASE(divident.digits[j + k]) << BASE_SIZE)) + divident.digits[j + k - 1]) / divisor.digits[k - 1];
			r_ = (((DBASE(divident.digits[j + k]) << BASE_SIZE)) + divident.digits[j + k - 1]) % divisor.digits[k - 1];
			if ((q_ == b) || ((q_ * divisor.digits[k - 2]) > (b * r_ + divident.digits[j + k - 2])))
			{
				--q_;
				r_ = r_ + divisor.digits[k - 1];
				if ((r_ < b) && ((q_ == b) || (q_ * divisor.digits[k - 2] > b * r_ + divident.digits[j + k - 2]))) --q_;
			}
			int dl = divisor.len;
			BigNum tmpDivisor = divisor * q_;
			//умножить-вычесть (D4)
			DBASE tmp = 0, k__ = 0;
			int ix = j, jx = dl, i = ix, l = 0;
			while (i <= ix + jx)
			{
				tmp = MAX_BASE_VALUE + DBASE(divident.digits[i]);
				tmp -= DBASE(tmpDivisor.digits[l]) + DBASE(k__);
				divident.digits[i] = (BASE) tmp;
				k__ = !(tmp >> BASE_SIZE);
				++i;
				++l;
			}
			divident.lenNorm();
			DBASE fl = k__;
			//присваивание и проверка условия (D5)
			q.digits[j] = q_;
			q.len++;
			if (fl < 0)	
			{
				//компенсировать сложение (D6)
				i = j;
				l = 0;
				int ln = i + divisor.len;
				for (; i <= ln; ++i)
				{
					DBASE tmp = DBASE(divident.digits[i]) + DBASE(divisor.digits[l]) + DBASE(k__);
					k__ = (BASE) (tmp >> BASE_SIZE);
					divident.digits[i] = (BASE) tmp;
					++l;
				}
				divident.digits[i] = k__;
				divident.lenNorm();
				q.digits[j]--;
			}
		}
		divident.len = divisor.len;
		divident.maxLen = divisor.maxLen;
		BigNum r = divident / d;	//D8 - денормализация для модуля
		q.lenNorm();
		return make_pair(q, r);
	}
	
	void lenNorm()
	{
		while (len - 1 > 0 && digits[len - 1] == 0) --len;
	}
};

int main()
{
	srand(time(0));
	BigNum a(2, false), b(2, false), c, d;
	cout << a << " <-a b-> " << b << endl;
	d = a / b;
	c = a % b;
	cout << "a / b = " << d << endl
	<< "a % b = " << c << endl;
	if ((b * d + c) == a) cout << "division and module are correct" << endl;
	else
	{
		cout << "divisor must not exceed dividend" << endl;
		BigNum tmpBN_ = a;
		a = b;
		b = tmpBN_;
		d = a / b;
		c = a % b;
		cout << "a / b = " << d << endl
		<< "a % b = " << c << endl;
		if ((b * d + c) == a) cout << "division and module are correct" << endl;
	}
	
	cout << "TEST" << endl;
	int M = 1000, T = 1000, tmp=0;
	BigNum A, B, C, D, tmpBN;
	do
	{
		int n = rand() % M + 1,
		m = rand() % n + 1;
		A = BigNum(n, false);
		B = BigNum(m, false);
		if (A < B)
		{
			tmpBN = A;
			A = B;
			B = tmpBN;
		}
		C = A / B;
		D = A % B;
		BigNum asdf = C * B, qwerty = C * B + D;	//4isto 4toby vyvodit'
		if (A != C * B + D)
		{
			cout << "A = " << A << endl << "B = " << B << endl
			 << "C = " << C << endl
			 << "D = " << D << endl
			 << "C * B = " << asdf << endl
			 << "C * B + D = " << qwerty << endl
			 << "1 condition" << endl;
		}		
		if (A - D != C * B) cout << "2 condition" << endl;
		if (D >= B) cout << "3 condition" << endl;
		cout << "T = " << T << " n = " << n << " m = " << m << endl;
		--T;
	}
	while (A == C * B + D && A-D == C * B && D < B && T > 0);
	//if (T > 0)
		//cout << "failed at T=" << T + 1 << endl
		//<< "A = " << A << endl
		//<< "B = " << B << endl
		//<< "C = " << C << endl
		//<< "D = " << D << endl;
	//calculatori.ru
	
	return 1;
}
