/*
 * JNP Zadanie 6 - Gruba Ryba
 * grupa: mp334965+pk334579
 * plik mojagrubaryba.h
 *
 * Zalozenia projektowe:
 * Interfejs Board reprezentujacy plansze do gry. Dziedziczy po nim clasa DefaultBoard z domyslna plansza (jak w tresci zadania).
 *
 * Interfejs Field reprezentujacy pole gry. Dziedzicza po nim wszystkie pola wystepujace na planszy. Dodatkowo wyodrebniona klasa
 * Nieruchomosc reprezentujaca pola ktore mozna kupic.
 *
 * Klasa Player reprezentujaca gracza ludzkiego lub komputerowego. Dziedzicza z niej klasy HumanPlayer i ComputerPlayer.
 *
 * Oczywiscie klasa MojaGrubaRyba.
 *
 */

#ifndef MOJAGRUBARYBA_H
#define MOJAGRUBARYBA_H

#include "grubaryba.h"
#include <vector>
#include <string>
#include <iostream>

const unsigned int startMoney = 1000;
const unsigned int startReward = 50;
const unsigned int minPlayers = 2;
const unsigned int maxPlayers = 8;

class Field;

class Board
{
public:
	Board() {}
	virtual ~Board() {}
	virtual std::vector<std::shared_ptr<Field> > route(std::shared_ptr<Field> start, unsigned int dist) = 0;
	virtual std::weak_ptr<Field> getStart() = 0;
	virtual void reset() = 0;
};

class DefaultBoard: public Board
{
public:
	DefaultBoard();
	~DefaultBoard() {}
	std::weak_ptr<Field> getStart();
	std::vector<std::shared_ptr<Field >> route(std::shared_ptr<Field> start, unsigned int dist);
	void reset();

private:
	DefaultBoard(const DefaultBoard&);
	void operator=(const DefaultBoard&);
	std::vector<std::shared_ptr<Field> > fields;
};

class Player;

class Field
{
public:
	Field(std::string name) : name(name) {}
	virtual ~Field() {};
	std::string getName() {
		return name;
	}
	virtual void doAction(Player&) = 0;
	virtual void passing(Player&) {}
	virtual void reset() {}

private:
	Field(const Field&);
	void operator=(const Field&);
	const std::string name;
};

class Blankfield : public Field
{
public:
	Blankfield(std::string name) : Field(name) {}
	virtual ~Blankfield() {}
	void doAction(Player&) {}
	//void passing(Player&) {}
};

class Nieruchomosc: public Field
{
public:
	Nieruchomosc(std::string name, unsigned int price): Field(name), price(price), sold(false), owner(nullptr) {};
	virtual ~Nieruchomosc() {}
	unsigned int getPrice() {
		return price;
	}
	void setFree() {
		sold = 0;
		owner = nullptr;
	}
	bool isSold() {
		return sold;
	}
	virtual void doAction(Player&);
	void reset();
	//virtual void passing(Player&) {}

protected:
	virtual unsigned int calcValue() = 0;
	const unsigned int price;
	bool sold;
	Player* owner;
};

class Koralowiec: public Nieruchomosc
{
public:
	Koralowiec(std::string name, unsigned int price): Nieruchomosc(name,price) {}
	virtual ~Koralowiec() {}
	//void passing(Player&) {}
private:
	unsigned int calcValue();
};

class ObiektUP: public Nieruchomosc
{
public:
	ObiektUP(std::string name, unsigned int price): Nieruchomosc(name, price) {}
	virtual ~ObiektUP() {}
	//void passing(Player&) {}
private:
	unsigned int calcValue();
};

class Start: public Field
{
public:
	Start() : Field("Start") {}
	virtual ~Start() {}
	void doAction(Player&);
	void passing(Player&);
};

class Nagroda: public Field
{
public:
	Nagroda(std::string name, unsigned int reward): Field(name), reward(reward) {}
	virtual ~Nagroda() {}
	void doAction(Player&);
	//void passing(Player&) {}

private:
	const unsigned int reward;
};

class Kara: public Field
{
public:
	Kara(std::string name, unsigned int fine): Field(name), fine(fine) {}
	virtual ~Kara() {}
	void doAction(Player&);
	//void passing(Player&) {}

private:
	const unsigned int fine;
};

class Depozyt: public Field
{
public:
	Depozyt(std::string name, unsigned int toll): Field(name), toll(toll), treasury(0) {}
	virtual ~Depozyt() {}
	void doAction(Player&);
	void passing(Player&);
	void reset();

private:
	const unsigned int toll;
	unsigned int treasury;
};

class Akwarium: public Field
{
public:
	Akwarium(std::string name, unsigned int time): Field(name), time(time) {}
	virtual ~Akwarium() {}
	void doAction(Player&);
	//void passing(Player&) {}

private:
	const unsigned int time;
};

class Player
{
public:
	Player(unsigned int startMoney) : money(startMoney), wait(0), isbankru(0) {}
	virtual ~Player() {}
	unsigned int getMoney() {
		return money;
	}
	std::weak_ptr<Field> getLocation() {
		return location;
	}
	void setLocation(std::weak_ptr<Field> l) {
		location = l;
	}
	void takeMoney(unsigned int x) {
		money+=x;
	}
	void sellEstate(Nieruchomosc*);
	void setWait(unsigned int w) {
		wait = w;
	}
	unsigned int getWait() {
		return wait;
	}
	void buyEstate(Nieruchomosc*);
	bool isBankrupt() {
		return isbankru;
	}
	void updateWait() {
		if (wait>0)
			wait--;
	}
	virtual std::string getName() = 0;
	virtual bool wantBuy(Nieruchomosc&) = 0;
	virtual bool wantSell(Nieruchomosc&) = 0;
	virtual unsigned int pay(unsigned int price) = 0;
	void reset();

protected:
	void bankrupt();

	unsigned int money;
	std::weak_ptr<Field> location;
	std::vector<Nieruchomosc*> estates;
	unsigned int wait;
	bool isbankru;

private:
	Player(const Player&);
	void operator=(const Player&);

};

class HumanPlayer: public Player
{
public:
	HumanPlayer(std::shared_ptr<Human> human, unsigned int startMoney) : Player(startMoney), hum(human) {}
	virtual ~HumanPlayer() {}
	bool wantBuy(Nieruchomosc& est) {
		return hum->wantBuy(est.getName());
	}
	bool wantSell(Nieruchomosc& est) {
		return hum->wantSell(est.getName());
	}
	unsigned int pay(unsigned int price);
	std::string getName() {
		return hum->getName();
	}

private:
	std::shared_ptr<Human> hum;
};

class ComputerPlayer: public Player
{
public:
	ComputerPlayer(unsigned int id, unsigned int startMoney) : Player(startMoney) {
		name = compName+std::to_string(id);
	}
	virtual ~ComputerPlayer() {}
	virtual bool wantBuy(Nieruchomosc&) = 0;
	bool wantSell(Nieruchomosc& est) {
		return false;
	}
	std::string getName() {
		return name;
	}
	unsigned int pay(unsigned int price) {
		if (money>=price) {
			money-=price;
			return price;
		}
		else {
			bankrupt();
			return 0;
		}
	}

private:
	ComputerPlayer(const ComputerPlayer&);
	void operator=(const ComputerPlayer&);
	static const std::string compName;
	std::string name;
};

class DumbCompPlayer: public ComputerPlayer
{
public:
	DumbCompPlayer(unsigned int id, unsigned int startMoney) : ComputerPlayer(id, startMoney), fieldCount(0) {};
	virtual ~DumbCompPlayer() {}
	bool wantBuy(Nieruchomosc&) {
		fieldCount++;
		return (fieldCount%3 == 0);
	}
	void reset();

private:
	unsigned int fieldCount;
};

class SmartCompPlayer: public ComputerPlayer
{
public:
	SmartCompPlayer(unsigned int id, unsigned int startMoney) : ComputerPlayer(id, startMoney) {};
	virtual ~SmartCompPlayer() {}
	bool wantBuy(Nieruchomosc&) {
		return true;
	}
};

class MojaGrubaRyba: public GrubaRyba
{
public:
	MojaGrubaRyba();
	virtual ~MojaGrubaRyba() {}
	void setDie(std::shared_ptr<Die> die);
	void addComputerPlayer(ComputerLevel level);
	void addHumanPlayer(std::shared_ptr<Human> human);
	void play(unsigned int rounds);

private:
	unsigned int countBankrupted();
	void reset();
	void printStatus(unsigned int);

	std::shared_ptr<Board> board;
	std::vector<std::shared_ptr<Player> > players;
	std::shared_ptr<Die> dice1;
	std::shared_ptr<Die> dice2;
	bool stillPlay;
	bool diceSet;

	MojaGrubaRyba(const MojaGrubaRyba&);
	void operator=(const MojaGrubaRyba&);
};

#endif