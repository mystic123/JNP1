/*
 *  Języki i narzędzia programowania I
 *  Zadanie zaliczeniowe nr 7.
 *  Autor: Paweł Kapica, pk334579
 *  Grupa 1
 * 
 */

#include <stack>
#include <string>
#include <functional>
#include <exception>
#include <cassert>
#include <map>
#include <utility>

class SyntaxError : public std::exception
{
	virtual const char* what() const throw()
	{
		return "Syntax error.";
	}
};

class OperatorAlreadyDefined : public std::exception
{
	virtual const char* what() const throw()
	{
		return "Operator already defined.";
	}
};

class UnknownOperator : public std::exception
{
	virtual const char* what() const throw()
	{
		return "Unknown operator.";
	}
};

class DivisionByZero : public std::exception
{
	virtual const char* what() const throw()
	{
		return "Division by zero.";
	}
};

typedef std::function<int(void)> Lazy;

class LazyCalculator {
public:
	LazyCalculator():operators() {
		define('+',[](Lazy a, Lazy b) {return a()+b();});
		define('-',[](Lazy a, Lazy b) {return a()-b();});
		define('*',[](Lazy a, Lazy b) {return a()*b();});
		define('/',[](Lazy a, Lazy b) { if (b()==0)
						  throw new DivisionByZero();
						else
						  return a()/b(); } );
	}

	Lazy parse(const std::string& s) const {
		std::stack<Lazy> st;
		for (size_t i=0; i<s.size(); i++) {
			if (!isLiteral(s[i])) {
				//when character is not literal
				if (operators.find(s[i]) == operators.end()) {
					throw UnknownOperator();
				}
				else {
					if (st.size() >= 2) {
						Lazy a = st.top();
						st.pop();
						Lazy b = st.top();
						st.pop();
						std::function<int(Lazy,Lazy)> fn = operators.find(s[i])->second;
						st.push([fn,a,b](){ return fn(b,a); });
					}
					else {
						throw SyntaxError();
					}
				}
			}
			else {
				int x = stoi(s.substr(i,1));
				st.push([x](){ return x; });
			}
		}
		if (st.size() == 1) {
			return st.top();
		}
		else {
			throw SyntaxError();
		}
	}

	int calculate(const std::string& s) const {
		return parse(s)();
	}

	void define(char c, std::function<int(Lazy, Lazy)> fn) {
		if (isLiteral(c)) {
			throw OperatorAlreadyDefined();
		}
		else if (operators.find(c) == operators.end()) {
			operators.insert(std::make_pair(c,fn));
		}
		else {
			throw OperatorAlreadyDefined();
		}
	}

private:
	bool isLiteral(const char& c) const
	{
		int i = 0;
		while (i < onlyLitNum && onlyLiterals[i] != c) {
			i++;
		}
		return (i<onlyLitNum);
	}

	constexpr static int onlyLitNum = 3;
	constexpr static char onlyLiterals[onlyLitNum] = {'0', '2', '4'};
	std::map<char, std::function<int(Lazy, Lazy)> > operators;
};

std::function<void(void)> operator*(int n, std::function<void(void)> fn) {
    return [=]() {
       for (int i = 0; i < n; i++)
           fn();
    };
}

int manytimes(Lazy n, Lazy fn) {
    (n() * fn)();  // Did you notice the type cast?
    return 0;
}

int main() {
    LazyCalculator calculator;
    
    // The only literals...
    assert(calculator.calculate("0") == 0);
    assert(calculator.calculate("2") == 2);
    assert(calculator.calculate("4") == 4);

    // Built-in operators.
    assert(calculator.calculate("42+") == 6);
    assert(calculator.calculate("24-") == -2);
    assert(calculator.calculate("42*") == 8);
    assert(calculator.calculate("42/") == 2);

    assert(calculator.calculate("42-2-") == 0);
    assert(calculator.calculate("242--") == 0);
    assert(calculator.calculate("22+2-2*2/0-") == 2);

    // The fun.
    calculator.define('!', [](Lazy a, Lazy b) { return a()*10 + b(); });
    assert(calculator.calculate("42!") == 42);
    std::string buffer;
    calculator.define(',', [](Lazy a, Lazy b) { a(); return b(); });
    calculator.define('P', [&buffer](Lazy a, Lazy b) { buffer += "pomidor"; return 0; });
    assert(calculator.calculate("42P42P42P42P42P42P42P42P42P42P42P42P42P42P42P42P,,,,42P42P42P42P42P,,,42P,42P,42P42P,,,,42P,,,42P,42P,42P,,42P,,,42P,42P42P42P42P42P42P42P42P,,,42P,42P,42P,,,,,,,,,,,,") == 0);
    assert(buffer.length() == 42 * std::string("pomidor").length());

    std::string buffer2 = std::move(buffer);
    buffer.clear();
    calculator.define('$', manytimes);
    assert(calculator.calculate("42!42P$") == 0);
    // Notice, how std::move worked.
    assert(buffer.length() == 42 * std::string("pomidor").length());
	
    calculator.define('?', [](Lazy a, Lazy b) { return a() ? b() : 0; });
    assert(calculator.calculate("042P?") == 0);
    assert(buffer == buffer2);
	
    assert(calculator.calculate("042!42P$?") == 0);
    assert(buffer == buffer2);
	
    for (auto bad: {"", "42", "4+", "424+"}) {
        try {
            calculator.calculate(bad);
            assert(false);
        } catch (SyntaxError) { }
    }
    try {
        calculator.define('!', [](Lazy a, Lazy b) { return a()*10 + b(); });
        assert(false);
    } catch (OperatorAlreadyDefined) { }
    try {
        calculator.calculate("02&");
        assert(false);
    } catch (UnknownOperator) { }
}