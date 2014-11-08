#ifndef VIRUS_GENEALOGYH
#define VIRUS_GENEALOGYH

#include <iostream>
#include <cstdlib>
#include <map>
#include <vector>
#include <set>
#include <exception>
#include <boost/shared_ptr.hpp>

/* Wyjatek zglaszany przy probie odwolania do nieistniejacego wirusa */
class VirusNotFoundException: public std::exception
{
public:
	virtual const char* what() const throw() 
	{
		return "VirusNotFound";
	}
};

/* Wyjatek zglaszany przy probie utworzenia wirusa, ktory juz istnieje */
class VirusAlreadyCreatedException: public std::exception
{
public:
	virtual const char* what() const throw() 
	{
		return "VirusAlreadyCreated";
	}
};

/* Wyjatek zglaszany przy probie usuniecia wezla wirusa macierzystego */
class TriedToRemoveStemVirusException: public std::exception
{
public:
	virtual const char* what() const throw() 
	{
		return "TriedToRemoveStemVirus";
	}
};

template <class Virus>
class VirusGenealogy
{

public:
	/* Tworzy nową genealogię i węzęł wirusa macierzystego o identyfikatorze
	 * stem_id.
	 */
	VirusGenealogy(typename Virus::id_type const &stem_id): stem_id(stem_id)
	{
		VirusMutation vir;
		vir.vir.reset(new Virus(stem_id));

		all_mutations.insert(mut_map_val(stem_id,vir));
	}

	/* Zwraca identyfikator wirusa macierzystego.*/
	typename Virus::id_type get_stem_id() const
	{
		return stem_id;
	}

	/* Zwraca listę identyfikatorów bezpośrednich następników wirusa
	 * o podanym identyfikatorze. Zgłasza wyjątek VirusNotFound,
	 * jeśli dany wirus nie istnieje.
	 */
	std::vector<typename Virus::id_type> get_children(typename Virus::id_type const &id) const
	{
		mapIter it = all_mutations.find(id);
		if (it != all_mutations.end()) {
			std::vector<typename Virus::id_type> tmp(it->second.children.begin(), it->second.children.end());
			return tmp;
		}
		else {
			throw VirusNotFoundException();
		}
	}

	/* Zwraca listę identyfikatorów bezpośrednich poprzedników wirusa
	 * o podanym identyfikatorze. Zgłasza wyjątek VirusNotFound, 
	 * jeśli dany wirus nie istnieje.
	 */
	std::vector<typename Virus::id_type> get_parents(typename Virus::id_type const &id) const
	{
		mapIter it = all_mutations.find(id);
		if (it != all_mutations.end()) {
			std::vector<typename Virus::id_type> tmp(it->second.parents.begin(), it->second.parents.end());
			return tmp;
		}
		else {
			throw VirusNotFoundException();
		}
	}

	/* Sprawdza, czy wirus o podanym identyfikatorze istnieje.*/
	bool exists(typename Virus::id_type const &id) const
	{
		return (all_mutations.find(id) != all_mutations.end());
	}

	/* Zwraca referencję do obiektu reprezentującego wirus o podanym
	 * identyfikatorze. Zgłasza wyjątek VirusNotFound, jeśli żądany 
	 * wirus nie istnieje.
	 */
	Virus& operator[](typename Virus::id_type const &id) const
	{
		mapIter it = all_mutations.find(id);
		if (it != all_mutations.end()) {
			return *it->second.vir.get();
		}
		else {
			throw VirusNotFoundException();
		}
	}

	/* Tworzy węzęł reprezentujący nowy wirus o identyfikatorze id powstały 
	 * z wirusów o podanym identyfikatorze parent_id. Zgłasza wyjątek 
	 * VirusAlreadyCreated, jeśli wirus o identyfikatorze id już istnieje.
	 * Zgłasza wyjątek VirusNotFound, jeśli któryś z wyspecyfikowanych
	 * poprzedników nie istnieje.
	 */
	void create(typename Virus::id_type const &id, typename Virus::id_type const &parent_id)
	{
		mapIter it = all_mutations.find(id);
		mapIter it2 = all_mutations.find(parent_id);
		if (it == all_mutations.end()) {
			if (it2 != all_mutations.end()) {
				VirusMutation vir;
				vir.vir.reset(new Virus(id));
				vir.parents.insert(parent_id);
				it2->second.children.insert(id);
				all_mutations.insert(mut_map_val(id,vir));
			}
			else {
				throw VirusNotFoundException();
			}
		}
		else {
			throw VirusAlreadyCreatedException();
		}
	}

	void create(typename Virus::id_type const &id, std::vector<typename Virus::id_type> const &parent_ids)
	{
		if (parent_ids.empty()) {
			throw VirusNotFoundException();
		}
		mapIter it = all_mutations.find(id);
		if (it == all_mutations.end()) {
			//sprawdzam czy wszyscy ojcowie istnieją i tworzę nowy vector
			std::vector<typename Virus::id_type> toAdd;
			for (size_t i=0; i<parent_ids.size(); i++) {
				mapIter it2 = all_mutations.find(parent_ids[i]);
				if (it2 != all_mutations.end()) {
					toAdd.push_back(parent_ids[i]);
				}
				else {
					throw VirusNotFoundException();
				}
			}
			//jeśli nie było wyjątku to mogę dodać
			VirusMutation vir;
			vir.vir.reset(new Virus(id));
			for (size_t i=0; i<toAdd.size(); i++) {
				vir.parents.insert(toAdd[i]);
				all_mutations.find(toAdd[i])->second.children.insert(id);
			}
			all_mutations.insert(mut_map_val(id,vir));
		}
		else {
			throw VirusAlreadyCreatedException();
		}
	}
	
	/* Dodaje nową krawędź w grafie genealogii. Zgłasza wyjątek VirusNotFound,
	 * jeśli któryś z podanych wirusów nie istnieje.
	 */
	void connect(typename Virus::id_type const &child_id, typename Virus::id_type const &parent_id)
		{
			mapIter child = all_mutations.find(child_id);
			mapIter parent = all_mutations.find(parent_id);
			if (child != all_mutations.end() && parent != all_mutations.end()) {
				child->second.parents.insert(parent_id);
				parent->second.children.insert(child_id);
			}
			else {
				throw VirusNotFoundException();
			}
		}

	/* Usuwa wirus o podanym identyfikatorze. Zgłasza wyjątek VirusNotFound, 
	 * jeśli żądany wirus nie istnieje. Zgłasza wyjątek TriedToRemoveStemVirus 
	 * przy próbie usunięcia wirusa macierzystego.
	 */
	void remove(typename Virus::id_type const &id)
	{
		if (id != stem_id) {
			mapIter it = all_mutations.find(id);
			if (it != all_mutations.end()) {
				//usuwanie z rodziców
				for (setIter i = it->second.parents.begin(); i != it->second.parents.end(); i++) {
					setIter tmp = all_mutations.find(*i)->second.children.find(id);
					all_mutations.find(*i)->second.children.erase(tmp);
				}
				//usuwanie z dzieci
				for (setIter i = it->second.children.begin(); i != it->second.children.end(); i++) {
					setIter tmp = all_mutations.find(*i)->second.parents.find(id);
					all_mutations.find(*i)->second.parents.erase(tmp);
					if (all_mutations.find(*i)->second.parents.empty()) {
						remove(all_mutations.find(*i)->second.vir->get_id());
					}
				}
				all_mutations.erase(it);
			}
			else {
				throw VirusNotFoundException();
			}
		}
		else {
			throw TriedToRemoveStemVirusException();
		}
	}
	
private:
	/* Definicja struktury genealogii wirusa */
	typedef struct {
		boost::shared_ptr<Virus> vir;
		mutable std::set<typename Virus::id_type> parents;
		mutable std::set<typename Virus::id_type> children;
	} VirusMutation;
	
	/* Definicja typu: mapa mutacji */
	typedef typename std::map<typename Virus::id_type, VirusMutation> mutations_map;

	/* Value_type do wstawiania nowych elementow do mapy */ 
	typedef typename std::map<typename Virus::id_type, VirusMutation>::value_type mut_map_val;

	/* Iterator po mapie wirusow */
	typedef typename std::map<typename Virus::id_type, VirusMutation>::const_iterator mapIter;
	
	/* Iterator po secie wirusow */
	typedef typename std::set<typename Virus::id_type>::const_iterator setIter;
	
	/* Wierzcholek wirusa macierzystego */
	typename Virus::id_type stem_id;
	
	/* Atrybut prywatny, mapa wszystkich mutacji */
	mutations_map all_mutations;
	
	/* Prywatny konstruktor kopiujący i operator przypisania */
	VirusGenealogy(const VirusGenealogy&);
	void operator=(const VirusGenealogy&);
};

#endif //VIRUS_GENEALOGYH