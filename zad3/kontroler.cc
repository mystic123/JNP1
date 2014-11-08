#include "kontroler.h"
#include "sejf.h"

using namespace std;

static const string burg("ALARM: WLAMANIE\n");
static const string manip("ALARM: ZMANIPULOWANY\n");
static const string ok("OK\n");

Kontroler::Kontroler(const Sejf &_parent):
	parent(_parent)
{
}

Kontroler::operator bool() const
{
	return (parent.getAccess() > 0);
}

ostream& operator<<(ostream &os, const Kontroler &c)
{
	switch(c.parent.getState())
	{
		case BURGLARY:
			os << burg;
			break;
		case MANIPULATION:
			os << manip;
			break;
		default:
			os << ok;
			break;
	}

	return os;
}
