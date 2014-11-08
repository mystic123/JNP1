/*
 * JNP Zadanie 6 - Gruba Ryba
 * grupa: mp334965+pk334579
 * plik mojagrubaryba.cc
 */

#ifdef DEBUG_MODE
static const bool debug = true;
#else
static const bool debug = false;
#endif

#include "mojagrubaryba.h"

const std::string ComputerPlayer::compName = "Gracz";

DefaultBoard::DefaultBoard(): Board()
{
	std::shared_ptr<Start> start = std::shared_ptr<Start>(new Start());
	std::shared_ptr<Koralowiec> anem = std::shared_ptr<Koralowiec>(new Koralowiec("Anemonia", 160));
	std::shared_ptr<Blankfield> wyspa = std::shared_ptr<Blankfield>(new Blankfield("Wyspa"));
	std::shared_ptr<Koralowiec> aporina = std::shared_ptr<Koralowiec>(new Koralowiec("Aporina", 220));
	std::shared_ptr<Akwarium> akwarium = std::shared_ptr<Akwarium>(new Akwarium("Akwarium", 3));
	std::shared_ptr<ObiektUP> grota = std::shared_ptr<ObiektUP>(new ObiektUP("Grota", 300));
	std::shared_ptr<Koralowiec> menella = std::shared_ptr<Koralowiec>(new Koralowiec("Menella", 280));
	std::shared_ptr<Depozyt> laguna = std::shared_ptr<Depozyt>(new Depozyt("Laguna", 15));
	std::shared_ptr<ObiektUP> statek = std::shared_ptr<ObiektUP>(new ObiektUP("Statek", 250));
	std::shared_ptr<Nagroda> blazenki = std::shared_ptr<Nagroda>(new Nagroda("Blazenki", 120));
	std::shared_ptr<Koralowiec> penn = std::shared_ptr<Koralowiec>(new Koralowiec("Pennatula", 400));
	std::shared_ptr<Kara> rekin = std::shared_ptr<Kara>(new Kara("Rekin", 180));
	fields.push_back(start);
	fields.push_back(anem);
	fields.push_back(wyspa);
	fields.push_back(aporina);
	fields.push_back(akwarium);
	fields.push_back(grota);
	fields.push_back(menella);
	fields.push_back(laguna);
	fields.push_back(statek);
	fields.push_back(blazenki);
	fields.push_back(penn);
	fields.push_back(rekin);
}

std::weak_ptr< Field > DefaultBoard::getStart()
{
	return std::weak_ptr<Field>(fields[0]);
}

std::vector<std::shared_ptr<Field> > DefaultBoard::route(std::shared_ptr<Field> start, unsigned int dist)
{
	std::vector<std::shared_ptr<Field> >::iterator it = fields.begin();
	while (it != fields.end() && (*it).get() != start.get()) {
		it++;
	}
	it++;
	std::vector<std::shared_ptr<Field> > l;
	while (dist>0 && it != fields.end()) {
		l.push_back(*it);
		dist--;
		it++;
	}
	it = fields.begin();
	while (dist>0 && it != fields.end()) {
		l.push_back(*it);
		dist--;
		it++;
	}
	return l;
}

void DefaultBoard::reset()
{
	for (std::vector<std::shared_ptr<Field> >::iterator it = fields.begin(); it != fields.end(); it++) {
		(*it)->reset();
	}
}

void Nieruchomosc::doAction(Player& pl)
{
	if (debug)
		std::cout<<this->getName()<<".doAction("<<pl.getName()<<")"<<std::endl;
	if (!sold) {
		if (pl.wantBuy(*this)) {
			if (pl.pay(price) == price) {
				pl.buyEstate(this);
				owner = &pl;
				sold = 1;
			}
		}
	}
	else
		if (owner != &pl) {
			owner->takeMoney(pl.pay(calcValue()));
		}
}

void Nieruchomosc::reset()
{
	sold = 0;
	owner = nullptr;
}

unsigned int Koralowiec::calcValue()
{
	return (price*2)/10;
}

unsigned int ObiektUP::calcValue()
{
	return (price*4)/10;
}

void Start::doAction(Player& pl)
{
	if (debug)
		std::cout<<"Start::doAction("<<pl.getName()<<")"<<std::endl;
	pl.takeMoney(startReward);
}

void Start::passing(Player& pl)
{
	if (debug)
		std::cout<<"Start::passing("<<pl.getName()<<")"<<std::endl;
	this->doAction(pl);
}

void Nagroda::doAction(Player& pl)
{
	if (debug)
		std::cout<<"Nagroda::doAction("<<pl.getName()<<")"<<std::endl;
	pl.takeMoney(reward);
}

void Kara::doAction(Player& pl)
{
	if (debug)
		std::cout<<"Kara::doAction("<<pl.getName()<<")"<<std::endl;
	pl.pay(fine);
}

void Depozyt::doAction(Player& pl)
{
	if (debug)
		std::cout<<"Depozyt::doAction("<<pl.getName()<<")"<<std::endl;
	pl.takeMoney(treasury);
	treasury = 0;
}

void Depozyt::passing(Player& pl)
{
	if (debug)
		std::cout<<"Depozyt::passing("<<pl.getName()<<")"<<std::endl;
	pl.pay(toll);
	treasury += toll;
}

void Depozyt::reset()
{
	treasury = 0;
}

void Akwarium::doAction(Player& pl)
{
	if (debug)
		std::cout<<"Akwarium::doAction("<<pl.getName()<<")"<<std::endl;
	pl.setWait(time);
}

void Player::sellEstate(Nieruchomosc* est)
{
	if (debug)
		std::cout<<this->getName()<<".sellEstate("<<est->getName()<<")"<<std::endl;
	std::vector<Nieruchomosc*>::iterator it = estates.begin();
	while (it != estates.end() && *it != est) {
		it++;
	}
	money+=(*it)->getPrice()/2;
	(*it)->setFree();
	estates.erase(it);
}

void Player::buyEstate(Nieruchomosc* est)
{
	if (debug)
		std::cout<<this->getName()<<".buyEstate("<<est->getName()<<")"<<std::endl;
	estates.push_back(est);
}

void Player::bankrupt()
{
	if (debug)
		std::cout<<this->getName()<<".bankrupt()"<<std::endl;
	for (size_t i=0; i<estates.size(); i++) {
		sellEstate(estates[i]);
	}
	isbankru = 1;
	estates.clear();
}

void Player::reset()
{
	money = startMoney;
	estates.clear();
	wait = 0;
	isbankru = 0;
}

unsigned int HumanPlayer::pay(unsigned int price)
{
	if (money >= price) {
		money-=price;
		return price;
	}
	else {
		for (size_t i = 0; i < estates.size(); i++) {
			if (hum->wantSell(estates[i]->getName())) {
				sellEstate(estates[i]);
			}
		}
		if (money >= price) {
			money-=price;
			return price;
		}
		else {
			bankrupt();
			return money;
		}
	}
}

void DumbCompPlayer::reset()
{
	Player::reset();
	fieldCount = 0;
}

MojaGrubaRyba::MojaGrubaRyba(): GrubaRyba(), board(new DefaultBoard()), dice1(nullptr), dice2(nullptr), stillPlay(1), diceSet(0)
{
}

void MojaGrubaRyba::setDie(std::shared_ptr<Die> die)
{
	if (die.get() != nullptr) {
		dice1 = die;
		dice2 = die->clone();
		diceSet = 1;
	}
}

void MojaGrubaRyba::addComputerPlayer(ComputerLevel level)
{
	if (players.size() < maxPlayers) {
		switch (level) {
		case GrubaRyba::ComputerLevel::DUMB: {
			std::shared_ptr<DumbCompPlayer> d = std::shared_ptr<DumbCompPlayer>(new DumbCompPlayer(players.size()+1, startMoney));
			players.push_back(d);
			break;
		}
		case GrubaRyba::ComputerLevel::SMARTASS: {
			std::shared_ptr<SmartCompPlayer> s = std::shared_ptr<SmartCompPlayer>(new SmartCompPlayer(players.size()+1, startMoney));
			players.push_back(s);
			break;
		}
		}
	}
	else {
		throw TooManyPlayersException(maxPlayers);
	}
}
void MojaGrubaRyba::addHumanPlayer(std::shared_ptr<Human> human)
{
	if (human.get() != nullptr) {
		if (players.size() < maxPlayers) {
			std::shared_ptr<HumanPlayer> g = std::shared_ptr<HumanPlayer>(new HumanPlayer(human, startMoney));
			players.push_back(g);
		}
		else {
			throw TooManyPlayersException(maxPlayers);
		}
	}
}
void MojaGrubaRyba::play(unsigned int rounds)
{
	if (diceSet) {
		if (players.size() >= 2) {
			for (std::vector<std::shared_ptr<Player> >::iterator it = players.begin(); it != players.end(); it++) {
				(*it)->setLocation(board->getStart());
			}
			unsigned int round = 0;
			while (round++ < rounds && stillPlay) {
				for (std::vector<std::shared_ptr<Player> >::iterator it = players.begin(); it != players.end(); it++) {
					if (countBankrupted() < players.size() - 1) {
						if (!(*it)->isBankrupt()) {
							(*it)->updateWait();
							if ((*it)->getWait() == 0) {
								unsigned int dist = 0;
								dist+=dice1->roll();
								dist+=dice2->roll();
								std::vector<std::shared_ptr<Field> > v = board->route((*it)->getLocation().lock(), dist);
								unsigned int i = 0;
								while (i < v.size()-1) {
									v[i]->passing(*(*it));
									i++;
								}
								(*it)->setLocation(std::weak_ptr<Field>(v[i]));
								v[i]->doAction(*(*it));
							}
						}
					}
					else {
						stillPlay = 0;
					}
				}
				printStatus(round);
			}
			reset();
		}
		else {
			throw TooFewPlayersException(minPlayers);
		}
	}
	else {
		throw NoDieException();
	}
}

unsigned int MojaGrubaRyba::countBankrupted()
{
	unsigned int result = 0;
	for (std::vector<std::shared_ptr<Player> >::iterator it = players.begin(); it != players.end(); it++) {
		if ((*it)->isBankrupt()) {
			result++;
		}
	}
	return result;
}

void MojaGrubaRyba::reset()
{
	board->reset();
	for (std::vector<std::shared_ptr<Player> >::iterator it = players.begin(); it != players.end(); it++) {
		(*it)->reset();
	}
	stillPlay = 1;
}

void MojaGrubaRyba::printStatus(unsigned int round)
{
	std::cout<<"Runda: "<<round<<std::endl;
	for (std::vector<std::shared_ptr<Player> >::iterator it = players.begin(); it != players.end(); it++) {
		std::cout<<(*it)->getName();
		if ((*it)->isBankrupt()) {
			std::cout<<" *** bankrut *** "<<std::endl;
		}
		else
			if ((*it)->getWait() == 0) {
				std::cout<<" pole: "<<(*it)->getLocation().lock()->getName();
				std::cout<<" gotowka: "<<(*it)->getMoney()<<std::endl;
			}
			else {
				std::cout<<" pole: "<<(*it)->getLocation().lock()->getName();
				std::cout<<" *** czekanie: "<<(*it)->getWait()<<" *** "<<std::endl;
			}
	}
}