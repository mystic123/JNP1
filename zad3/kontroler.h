#ifndef KONTROLER_H
#define KONTROLER_H

#include <iostream>
#include <string>

class Sejf;

using namespace std;

class Kontroler
{
	public:
		explicit operator bool() const;

	private:
		friend class Sejf;
		friend ostream& operator<<(ostream &os, const Kontroler &c);
		Kontroler(const Sejf& _parent);
		const Sejf& parent;
};

ostream& operator<<(ostream &os, const Kontroler &c);

#endif // CONTROLLER_H
