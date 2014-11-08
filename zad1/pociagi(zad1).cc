/* Zadanie Pociągi
 * 
 * Autorzy: Paweł Kapica
 * 			Michał Kiełek
 * 
 * flagi kompilacji: -O2 -Wall -std=c++11 -lboost_regex
 */
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <boost/regex.hpp>
#include <boost/tokenizer.hpp>

using namespace boost;
using namespace std;

//deklaracje typow
	typedef boost::char_separator<char> charSep;
	typedef boost::tokenizer<charSep> myTokenizer;
	typedef myTokenizer::iterator tokensIterator;
	typedef int timeVar; //przechowuje godzine odjazdu, musi byc signed
	typedef unsigned long long delayVar; //przechowuje czas opoznienia
	typedef pair<timeVar, delayVar> trainEntry;

	const regex entryRegex("[ \t]*[0-9]{3,9}[ \t]+" //nr pociagu
	"(0?[1-9]|[1-2][0-9]|3[0-1]).(0?[1-9]|1[0-2]).[0-9]+[ \t]+" //data
	"(0?[0-9]|1[0-9]|2[0-3]).[0-5][0-9]([ \t]*|[ \t]+[0-9]{0,19}[ \t]*)"); //godzina 
															//i opoznienie
	const regex comRegex("[ \t]*(L|M|S)[ \t]+(0?[0-9]|1[0-9]|2[0-3]).[0-5][0-9]"
							"[ \t]+(0?[0-9]|1[0-9]|2[0-3]).[0-5][0-9][ \t]*"); 
	
	const charSep dotSep(".");
	const charSep spaceSep(" \t");
	const int HOURS = 24;
	const int MINUTES = 60;
	const int FEBRUARY = 2;
	const int FEBDAYS = 28;
	const int FEBLEAPDAYS = 29;
	const int LMONTHDAYS = 31;
	const int JULY = 7;
	
//	porównuje czasy łącznie z opóźnieniem, uwzględnia opóźnienie
//	zmieniające dzień na następny
bool sortTime(const trainEntry& x, const trainEntry& y)
{ 
	return (((x.first + x.second)%(HOURS * MINUTES)) < 
									((y.first + y.second)%(HOURS * MINUTES)));
}

//klasyczny binsearch wyszukujący daną liczbę lub mniejszą
//z porównywaniem jak w funkcj wyżej
int binSearchLow(const vector<trainEntry>& table, unsigned int value)
{
	int l, p, s;
	l = 0;
	p = table.size() - 1;
	while (l != p){
		s = (l + p + 1) / 2;
		if ((table[s].first + table[s].second)%(HOURS * MINUTES) <= value) {
			l = s;
		}
		else {
			p = s - 1;
		}
	}
	return l;
}
//klasyczny binsearch wyszukujący daną liczbę lub większą
int binSearchUp(const vector<trainEntry>& table, unsigned int value)
{
	int l, p, s;
	l = 0;
	p = table.size() - 1;
	while (l != p){
		s = (l + p) / 2;
		if ((table[s].first + table[s].second)%(HOURS * MINUTES) >= value) {
			p = s;
		}
		else {
			l = s + 1;
		}
	}
	return l;
}

	
bool checkDate(const string& input)
{
	myTokenizer tokens(input,dotSep);
	tokensIterator dateIt = tokens.begin();
	int day, month;
	unsigned long long year;
	day = stoi(*dateIt++);
	month = stoi(*dateIt++);
	year = stoull(*dateIt);
	bool leapYear = ((year%4 == 0 && year%100 != 0) || year%400 == 0);
	if (month == FEBRUARY && (day > FEBLEAPDAYS || (day > FEBDAYS && !leapYear))) {
		return 0; //luty za duzo dni
	}
	else if (day == LMONTHDAYS && ((month <= JULY && month%2 == 0) ||
							(month > JULY && month%2 == 1))) {
		return 0; //niewlasciwa ilosc dni w miesiacu
	}
	else { 
		return 1;
	}
}

timeVar parseHour(const string& input)
{
	myTokenizer tokens(input,dotSep);
	tokensIterator dateIt = tokens.begin();
	timeVar hour,min;
	hour = stoi(*dateIt++);
	min = stoi(*dateIt);
	return hour*MINUTES+min;
}

//w przypadku blednego wpisu zwracana jest para <-1,0>
trainEntry parseEntry(unsigned long long lineCount, const string& entry)
{
	timeVar hour = -1;
	delayVar delay = 0;
	bool error;
	trainEntry result (hour,delay);
	error = !regex_match(entry, entryRegex);
	if (!error) {
		myTokenizer tokens(entry, spaceSep);
		//liczenie tokenow
		tokensIterator mainIt = tokens.begin();
		int tokCount = 0;
		while (tokCount <= 4 && mainIt != tokens.end()) {
			tokCount++;
			mainIt++;
		}
		mainIt = tokens.begin();
		mainIt++;
		error = !checkDate(*mainIt);
		if (!error) {
			mainIt++;
			hour = parseHour(*mainIt);
			if (tokCount == 4) {
				++mainIt;
				delay = stoull(*mainIt);
			}
			result.first = hour;
			result.second = delay;
		}
	}
	if (error) {
		cerr<<"ERROR "<<lineCount<<": "<<entry<<endl;
	}
	return result;
}
//wykonuje komendę L
void comL(int tp, int tk, const vector<trainEntry>& trains)
{	int ip, ik;
	ip = binSearchUp(trains, tp); 
	ik = binSearchLow(trains, tk);
	if (ip == ik) {
		//oba binsearch-e zwróciły ten sam indeks, a wartość pod nim 
		//nie mieści się pomiędzy czasem początkowy i końcowym
		int time = (trains[ip].first + trains[ip].second) % (HOURS * MINUTES);
		if ((time < tp) || ( time > tk)){
			cout << 0 << endl;
		}
		else {
			cout << (ik - ip + 1) << endl;
		}	
	}
	else {
		cout << (ik - ip + 1) << endl;
	}
}
//wykonuje komendę M
void comM(int tp, int tk, const vector<trainEntry>& trains)
{	int ip, ik;
	ip = binSearchUp(trains, tp); 
	ik = binSearchLow(trains, tk);
	delayVar maxDelay = 0;
	for (int i = ip; i <= ik; ++i) {
		if (trains[i].second > maxDelay) {
			maxDelay = trains[i].second;
		}
	}
	if (ip == ik) {
		//oba binsearch-e zwróciły ten sam indeks, a wartość pod nim 
		//nie mieści się pomiędzy czasem początkowy i końcowym
		int time = (trains[ip].first + trains[ip].second) % (HOURS * MINUTES);
		if ((time < tp) || ( time > tk)){
			cout << 0 << endl;
		}
		else {
			cout << maxDelay << endl;
		}	
	}
	else {
		cout << maxDelay << endl;
	}
}
//wykonuje komendę S
void comS(int tp, int tk, const vector<trainEntry>& trains)
{	int ip, ik;
	ip = binSearchUp(trains, tp); 
	ik = binSearchLow(trains, tk);
	delayVar delay = 0;
	for (int i = ip; i <= ik; ++i) {
		delay += trains[i].second;
	}
	if (ip == ik) {
		//oba binsearch-e zwróciły ten sam indeks, a wartość pod nim 
		//nie mieści się pomiędzy czasem początkowy i końcowym
		int time = (trains[ip].first + trains[ip].second) % (HOURS * MINUTES);
		if ((time < tp) || ( time > tk)){
			cout << 0 << endl;
		}
		else {
			cout << delay << endl;
		}	
	}
	else {
		cout << delay << endl;
	}	
}
//parsowanie komend
void parseCommand(unsigned long long lineCount, const string& com, 
						vector<trainEntry>& trains, bool& wasCom)
{	bool error = 0;
	string comand;
	timeVar tp, tk;
	myTokenizer tokens(com, spaceSep);
	//liczenie tokenow
	tokensIterator comIt = tokens.begin();
	comand = *comIt++;
	tp = parseHour(*comIt++);
	tk = parseHour(*comIt++);
	if (tk < tp ) {
		error = 1;
	}
	else {
		//brak pociągów
		if (trains.empty()) {
			cout << 0 << endl;
		}
		else {
			if(comand =="L") {
				comL(tp, tk, trains);
			}
			else if (comand == "M") {
				comM(tp, tk, trains);
			}
			else if (comand == "S") {
				comS(tp, tk, trains);
			}
		}
	}
	if (error) {
		cerr<<"ERROR "<<lineCount<<": "<<com<<endl;
	}
	else {
		if (!wasCom) {
			//oznaczenie o pierwszej komendzie, sortowanie wektora i tym samym
			//blokowanie dodawania nowych pociągów zmienną wasCom
			wasCom = 1;
			sort(trains.begin(), trains.end(), sortTime);
		}
	}
}



int main()
{
	ios_base::sync_with_stdio(false);
	vector<trainEntry> trains;
	string entry;
	unsigned long long lineCount = 0;
	bool wasCom = 0;
	trainEntry cos;
	while (getline(cin,entry)) {
		++lineCount;
		if (regex_match(entry, entryRegex)) {
			if (!wasCom) {
				cos = parseEntry(lineCount, entry);
				if (cos.first != -1) {
					trains.push_back(cos);
				}
			}
			else { 
				cerr<<"ERROR "<<lineCount<<": "<<entry<<endl;
			}
		}
		else if (regex_match(entry, comRegex)) {
			parseCommand(lineCount,entry, trains, wasCom);
		}
		else {
			cerr<<"ERROR "<<lineCount<<": "<<entry<<endl;
		}
	}
	 return 0;
 }