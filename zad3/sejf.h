#ifndef SEJF_H
#define SEJF_H

#include <string>
#include "kontroler.h"

using namespace std;

enum state
{
	OK,
	BURGLARY,
	MANIPULATION
};

class Sejf
{
	friend class Kontroler;
	public:
		Sejf(const string &t, unsigned int access = defAccess);
		Sejf(string &&t, unsigned int acces = defAccess);
		Sejf(const Sejf&) = delete;
		void operator=(const Sejf&) = delete;

		Sejf(Sejf&& s);
		Sejf& operator=(Sejf&& s)
		{
			text = std::move(s.text);
			accessCounter = s.accessCounter;
			return *this;
		}

		const Kontroler &kontroler() const;
		state getState() const { return this->st; }
		unsigned int getAccess() const { return this->accessCounter; }

		int16_t operator[](int i);
		Sejf& operator*=(int i);
		Sejf& operator+=(int i);
		Sejf& operator-=(int i);

	private:
		static const int defAccess = 42;

		Kontroler k;
		string text;
		unsigned int accessCounter;
		state st;
};

#endif // SEJF_H
