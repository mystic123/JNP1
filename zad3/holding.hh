#ifndef HOLDING_H
#define HOLDING_H

/*
 * Plik holding.hh
 * Zadanie zaliczeniowe nr 4
 * Grupa kk282146+pk334579
 */

#include <iostream>

using namespace std;

//funkcje pomocnicze
constexpr unsigned int differ(unsigned int a, unsigned int b)
{
	return (a > b) ? a - b : 0;
}

constexpr unsigned int divide(unsigned int a, unsigned int b)
{
	return (b != 0) ? a / b : 0;
}

template<unsigned int _acc, unsigned int _hs, unsigned int _exo>
struct Company {
	static const unsigned int acc = _acc;
	static const unsigned int hs = _hs;
	static const unsigned int exo = _exo;
};

typedef Company<1, 0, 0> Accountancy;
typedef Company<0, 1, 0> Hunting_shop;
typedef Company<0, 0, 1> Exchange_office;

template<class C1, class C2>
struct add_comp {
	typedef Company < C1::acc + C2::acc, C1::hs + C2::hs, C1::exo + C2::exo > type;
};

template<class C1, class C2>
struct remove_comp {
	typedef Company<differ(C1::acc, C2::acc), differ(C1::hs, C2::hs), differ(C1::exo, C2::exo) > type;
};

template<class C, unsigned int n>
struct multiply_comp {
	typedef Company<C::acc* n, C::hs* n, C::exo* n> type;
};

template<class C, unsigned int n>
struct split_comp {
	typedef Company<divide(C::acc, n), divide(C::hs, n), divide(C::exo, n) > type;
};

template<class C>
struct additive_expand_comp {
	typedef Company < C::acc + 1, C::hs + 1, C::exo + 1 > type;
};

template<class C>
struct additive_rollup_comp {
	typedef Company<differ(C::acc, 1), differ(C::hs, 1), differ(C::exo, 1) > type;
};

template<class Company>
class Group
{
public:
	typedef Company company_type;
	static company_type company;

	Group() : group_size(1), acc_val(acc_def_val), hs_val(hs_def_val),
		exo_val(exo_def_val)
	{ }

	Group(unsigned int k) : group_size(k), acc_val(acc_def_val), hs_val(hs_def_val),
		exo_val(exo_def_val)
	{ }

	Group(Group<Company> const& x) : group_size(x.get_size()), acc_val(x.get_acc_val()),
		hs_val(x.get_hs_val()), exo_val(x.get_exo_val())
	{ }

	unsigned int get_size() const {
		return group_size;
	}

	void set_acc_val(unsigned int i) {
		acc_val = i;
	}
	void set_hs_val(unsigned int i) {
		hs_val = i;
	}
	void set_exo_val(unsigned int i) {
		exo_val = i;
	}

	unsigned int get_acc_val() const {
		return acc_val;
	}
	unsigned int get_hs_val() const {
		return hs_val;
	}
	unsigned int get_exo_val() const {
		return exo_val;
	}

	unsigned int get_value() const {
		return group_size * (Company::acc * acc_val + Company::hs * hs_val + Company::exo * exo_val);
	}

	Group<Company>& operator+= (const Group<Company>& x) {
		unsigned int new_size =  group_size + x.get_size();
		acc_val = divide(acc_val * group_size + x.get_acc_val() * x.get_size(),
		                 new_size);
		hs_val = divide(hs_val * group_size + x.get_hs_val() * x.get_size(),
		                new_size);
		exo_val = divide(exo_val * group_size + x.get_exo_val() * x.get_size(),
		                 new_size);
		group_size = new_size;
		return *this;
	}

	Group<Company>& operator-= (const Group<Company>& x) {
		unsigned int new_size =  group_size + x.get_size();
		acc_val = divide(differ(acc_val * group_size, x.get_acc_val() * x.get_size()),
		                 new_size);
		hs_val = divide(differ(hs_val * group_size, x.get_hs_val() * x.get_size()),
		                new_size);
		exo_val = divide(differ(exo_val * group_size, x.get_exo_val() * x.get_size()),
		                 new_size);
		group_size = new_size;
		return *this;
	}

	Group<Company> operator+ (const Group<Company>& x) {
		Group<Company> result = Group<Company> (group_size + x.get_size());
		result.set_acc_val((x.get_size() *x.get_acc_val() + acc_val * group_size) /
		                   (group_size + x.get_size()));
		result.set_hs_val((x.get_size() *x.get_hs_val() + hs_val * group_size) /
		                  (group_size + x.get_size()));
		result.set_exo_val((x.get_size() *x.get_exo_val() + exo_val * group_size) /
		                   (group_size + x.get_size()));
		return result;
	}

	Group<Company> operator- (const Group<Company>& x) {
		unsigned int new_size = differ(group_size, x.get_size());
		Group<Company> result = Group<Company> (new_size);
		result.set_acc_val(divide(differ(acc_val * group_size, x.get_size() * x.get_acc_val())
		                          , new_size));
		result.set_hs_val(divide(differ(hs_val * group_size, x.get_size() * x.get_hs_val())
		                         , new_size));
		result.set_exo_val(divide(differ(exo_val * group_size, x.get_size() * x.get_exo_val())
		                          , new_size));
		return result;
	}

	Group<Company>& operator*= (unsigned int x) {
		group_size *= x;
		acc_val = divide(acc_val, x);
		hs_val = divide(hs_val, x);
		exo_val = divide(exo_val, x);
		return *this;
	}

	Group<Company>& operator/= (unsigned int x) {
		group_size = divide(group_size, x);
		acc_val *= x;
		hs_val *= x;
		exo_val *= x;
		return *this;
	}

	Group<Company> operator* (unsigned int x) {
		Group<Company> result(group_size * x);
		result.set_acc_val(divide(acc_val, x));
		result.set_hs_val(divide(hs_val, x));
		result.set_exo_val(divide(exo_val, x));
		return result;
	}

	Group<Company> operator/ (unsigned int x) {
		Group<Company> result(divide(group_size, x));
		result.set_acc_val(acc_val * x);
		result.set_hs_val(hs_val * x);
		result.set_exo_val(exo_val * x);
		return result;
	}

	template<class C>
	bool operator== (const Group<C>& x) const {
		bool hs, exo;
		hs = (group_size* Company::hs == x.get_size() * C::hs);
		exo = (group_size* Company::exo == x.get_size() * C::exo);
		if (hs && exo) {
			return (group_size* Company::acc == x.get_size() * C::acc);
		}
		else {
			return false;
		}
	}

	template<class C>
	bool operator!= (const Group<C>& x) const {
		return (!(*this == x));
	}

	template<class C>
	bool operator< (const Group<C>& x) const {
		return ((group_size * Company::hs < x.get_size() * C::hs) &&
		        (group_size * Company::exo < x.get_size() * C::exo));
	}

	template<class C>
	bool operator> (const Group<C>& x) const {
		return ((group_size * Company::hs > x.get_size() * C::hs) &&
		        (group_size * Company::exo > x.get_size() * C::exo));
	}

	template<class C>
	bool operator<= (const Group<C>& x) const {
		return ((*this < x) || *this == x);
	}
	template<class C>
	bool operator>= (const Group<C>& x) const {
		return ((*this > x) || *this == x);
	}


private:

	unsigned int group_size;
	unsigned int acc_val;
	unsigned int hs_val;
	unsigned int exo_val;
//domyslne wartosci dla values
	static const unsigned int acc_def_val = 15;
	static const unsigned int hs_def_val = 150;
	static const unsigned int exo_def_val = 50;
};

template <class C>
ostream& operator<< (ostream& os, const Group<C>& x)
{
	os << "Number of companies: " << x.get_size() << "; Value: " << x.get_value() <<
	   "\nAccountancies value: " << x.get_acc_val() << ", Hunting shops value: " << x.get_hs_val() <<
	   ", Exchange offices value: " << x.get_exo_val() << "\nAccountancies: " << C::acc <<
	   ", Hunting shops: " << C::hs << ", Exchange offices: " << C::exo << endl;
	return os;
}

template<class C>
Group<typename additive_expand_comp<C>::type> const
additive_expand_group(Group<C> const& s1)
{
	Group<typename additive_expand_comp<C>::type> result(s1.get_size());
	result.set_acc_val(s1.get_acc_val());
	result.set_hs_val(s1.get_hs_val());
	result.set_exo_val(s1.get_exo_val());
	return result;
}

template<class C>
Group<typename multiply_comp<C, 10>::type> const
multiplicative_expand_group(Group<C> const& s1)
{
	Group<typename multiply_comp<C, 10>::type> result(s1.get_size());
	result.set_acc_val(s1.get_acc_val());
	result.set_hs_val(s1.get_hs_val());
	result.set_exo_val(s1.get_exo_val());
	return result;
}

template<class C>
Group<typename additive_rollup_comp<C>::type> const
additive_rollup_group(Group<C> const& s1)
{
	Group<typename additive_rollup_comp<C>::type> result(s1.get_size());
	result.set_acc_val(s1.get_acc_val());
	result.set_hs_val(s1.get_hs_val());
	result.set_exo_val(s1.get_exo_val());
	return result;
}

template<class C>
Group<typename split_comp<C, 10>::type> const
multiplicative_rollup_group(Group<C> const& s1)
{
	Group<typename split_comp<C, 10>::type> result(s1.get_size());
	result.set_acc_val(s1.get_acc_val());
	result.set_hs_val(s1.get_hs_val());
	result.set_exo_val(s1.get_exo_val());
	return result;
}

template<class C1, class C2, class C3>
bool solve_auction(Group<C1> const& g1, Group<C2> const& g2, Group<C3> const& g3)
{
	return ((g1 > g2) && (g1 > g3)) || ((g2 > g1) && (g2 > g3)) ||
	       ((g3 > g1) && (g3 > g2));
}

#endif // HOLDING_H
