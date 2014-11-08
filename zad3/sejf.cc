#include "sejf.h"
#include <iostream>
#include <cstdint>

using namespace std;

Sejf::Sejf(const string &t, unsigned int access):
	k(*this),
	text(t),
	accessCounter(access)
{
}

Sejf::Sejf(string&& t, unsigned int access):
	k(*this),
	text(std::move(t)),
	accessCounter(access)
{
}


Sejf::Sejf(Sejf&& s):
	k(*this),
	text(std::move(s.text)),
	accessCounter(s.accessCounter)
{
}

const Kontroler& Sejf::kontroler() const
{
	return k;
}

int16_t Sejf::operator[](int i)
{
	if(i >= 0)
	{
		unsigned int j = static_cast<int16_t>(i);
		if(j < this->text.size() && accessCounter)
		{
			accessCounter--;
			return this->text[j];
		}
		else
		{
			if(j < this->text.size())
			{
			this->st = BURGLARY;
			}
		}
	}

	return -1;
}

Sejf& Sejf::operator*=(int i)
{
	if(i == 0 && this->accessCounter == 0)
	{
		if(this->st != BURGLARY)
		{
			this->st = MANIPULATION;
		}
	}
	if(i >= 1)
	{
		this->accessCounter *= i;
		if(this->st != BURGLARY)
		{
			this->st = MANIPULATION;
		}
	}

	return *this;
}

Sejf& Sejf::operator+=(int i)
{
	if(i >= 0)
	{
		this->accessCounter += i;
		if(this->st != BURGLARY)
		{
			this->st = MANIPULATION;
		}
	}

	return *this;
}

Sejf& Sejf::operator-=(int i)
{
	if(i >= 0)
	{
		unsigned int j = static_cast<unsigned int>(i); //TODO check
		if(j > 0)
		{
			// Jeżeli mamy zmniejszyć o więcej niż liczba dostępnych prób przerywamy
			if(this->accessCounter < static_cast<unsigned int>(i))
			{
				return *this;
			}
			this->accessCounter -= i;
			if(this->st != BURGLARY)
			{
				this->st = MANIPULATION;
			}
		}
		if(j == 0 && this->st != BURGLARY)
		{
			this->st = MANIPULATION;
		}
	}

	return *this;
}

