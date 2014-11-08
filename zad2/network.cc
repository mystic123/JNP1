#include "network.h"
#include <iostream>
#include <map>
#include <set>
#include <tuple>


#ifdef DEBUG_MODE
static const bool debug = true;
#else
static const bool debug = false;
#endif

using namespace std;

typedef multimap<string, string > t_links; /* multimapa przechowujaca informacje o polaczeniach */
typedef set<string> t_nodes; /* zbior przechowujacy wezly */
typedef tuple<t_nodes, t_links, bool> t_network; /* krotka ktora przechowuje wszystkie informacje o sieci */
typedef map<UL, t_network> t_all_network; /* mapa id_sieci -> siec */

/*
 * ************* Help functions *************
 */

/* zwraca referencje na strukture przechowujaca wszystkie sieci */
t_all_network& getData()
{
    static t_all_network ans;
    return ans;
}

/* zwraca referencje na zmienna odpowiedzialna za indexowanie sieci */
UL& currentId()
{
    static UL ans;
    return ans;
}

/* sprawdza czy istnieje siec o podanym id */
bool isNetwork(UL id)
{
    return getData().find(id) != getData().end();
}

/* sprawdza czy istnieje wezel label w sieci o podanym id */
bool isNode(UL id, const char *label)
{
    t_nodes nodes = get<0>(getData()[id]);
    t_nodes::iterator it = nodes.find(label);
    return it != nodes.end();
}

/* sprawdza czy istnieje polaczenie w sieci o podanym id */
bool isLink(UL id, const char *slabel, const char *tlabel)
{
    t_links links = get<1>(getData()[id]);
    pair<t_links::iterator, t_links::iterator> tmp;
    tmp = links.equal_range(slabel);
    t_links::iterator begin, end;
    begin = tmp.first;
    end = tmp.second;
    while (begin != end && begin->second != tlabel)
    {
            begin++;
    }
    return begin != end;
}

/*
 * ************* Base functions *************
 */

unsigned long network_new(int growing)
{
    UL id = currentId()++;
    static ios_base::Init _; //stream initialization
    if(debug)
    {
        cerr << "network_new(" << growing << ")\n";
        cerr << "network_new: network " << id << " created\n";
    }
    t_nodes nodes;
    t_links links;
    bool grow = (growing != 0 ? true : false);
    t_network network = make_tuple(nodes, links, grow);
    getData().insert(make_pair(id, network));
    
    return id;
}

void network_delete(unsigned long id)
{
    if(debug)
    {
        cerr << "network_delete(" << id << ")\n";
        cerr << "network_delete: ";
    }
    if(isNetwork(id))
    {
        getData().erase(id);
        if(debug)
            cerr << "network " << id << " deleted\n";
    }
    else if(debug)
    {
        cerr << "network " << id << " does not exist\n";
    }
}

size_t network_nodes_number(unsigned long id)
{
    if(debug)
    {
        cerr << "network_nodes_number(" << id << ")\n";
        cerr << "network_nodes_number: ";
    }
    if(isNetwork(id))
    {
        size_t result = (get<0>(getData()[id])).size();
        if(debug)
            cerr << "network " << id << " contains " << result << " nodes\n";
        return result;
    }
    else
    {
        if(debug)
            cerr << "network " << id << " does not exist\n";
        return 0;
    }
}

size_t network_links_number(unsigned long id)
{
    if(debug)
    {
        cerr << "network_links_number(" << id << ")\n";
        cerr << "network_links_number: ";
    }
    if(isNetwork(id))
    {
        size_t result = (get<1>(getData()[id])).size();
        if(debug)
            cerr << "network_links_number(" << id << ") = " << result << "\n";
        return result;
    }
    else
    {
        if(debug)
            cerr << "network " << id << " does not exist\n";
        return 0;
    }
}

void network_add_node(unsigned long id, const char* label)
{
    if(debug)
    {
        cerr << "network_add_node(" << id << ", " << label << ")\n";
        cerr << "network_add_node: ";
    }
    if(isNetwork(id))
    {
        if(label != NULL)
        {
            if(!isNode(id, label))
            {
                (get<0>(getData()[id])).insert(label);
                if (debug)
                    cerr << "node "<< label << " added\n";
            }
            else if(debug)
                cerr << "node " << label << " already exists\n";
        }
        else if(debug)
            cerr << "attempt to add null node label\n";
    }
    else if(debug)
        cerr << "network " << id << " does not exist\n";
}

void network_add_link(unsigned long id, const char* slabel, const char* tlabel)
{
    if(debug)
    {
        cerr << "network_add_link(" << id << ", " << slabel << ", " << tlabel << ")\n";
    }
    if(isNetwork(id))
    {
        if(slabel != NULL && tlabel != NULL)
        {
            if(!isLink(id, slabel, tlabel))
            {
                if(!isNode(id, slabel))
                    network_add_node(id, slabel);
                if(!isNode(id, tlabel))
                    network_add_node(id, tlabel);
                (get<1>(getData()[id])).insert(make_pair(slabel, tlabel));
                if(debug)
                    cerr << "network_add_link: link " << slabel << ", " << tlabel << " created\n";
            }
            else if(debug)
                cerr << "network_add_link: link " << slabel << ", " << tlabel << " already exists\n";
        }
        else if(debug)
            cerr << "network_add_link: atempt to add link with null node label\n";
    }
    else if(debug)
        cerr << "network_add_link: network " << id << " does not exist\n";
}

void network_remove_node(unsigned long id, const char* label)
{
    if (debug) {
        cerr << "network_remove_node( " << id <<  "," << label << ")\n";
        cerr << "network_remove_node: ";
    }
    if (label != NULL)
    {
        t_all_network::iterator it = getData().find(id);
        if (it != getData().end())
        {
            bool growing = get<2>(it->second);
            if (!growing)
            {
                /* usuwa wskazany node */
                (get<0>(it->second)).erase(label);
                /* usuwa link jesli node jest kluczem */
                (get<1>(it->second)).erase(label);
                /* przechodzimy po mapie i usuwamy wpisy w ktorych node jest wartoscia */
                t_links *links = &(get<1>(it->second));
                for(t_links::iterator it = links->begin(); it != links->end(); it++)
                    if((*it).second == label)
                        links->erase(it);
                if (debug)
                    cerr << "node " << label << " removed\n";
            }
            else if (debug)
                cerr << "attempt to remove node from growing network\n";
        }
        else if (debug)
            cerr << "network " << id << " does not exist\n";
    }
    else if (debug)
        cerr << "attempt to remove null node label\n";
}

void network_remove_link(unsigned long id, const char* slabel, const char* tlabel)
{
    if (debug)
    {
        cerr << "network_remove_link(" << id << "," << slabel << "," << tlabel << ")\n";
        cerr << "network_remove_link: ";
    }
    if (slabel != NULL && tlabel !=NULL)
    {
        t_all_network::iterator it = getData().find(id);
        if (it != getData().end())
        {
            bool growing = get<2>(it->second);
            if (!growing)
            {
                t_links::iterator begin, end;
                //szuka tych linkow, gdzie slabel jest kluczem
                pair<t_links::iterator, t_links::iterator> tmp = (get<1>(it->second)).equal_range(slabel);
                begin = tmp.first;
                end = tmp.second;
                 //szuka i usuwa wlasciwy link
                bool endOfLoop = false;
                while (begin != end && !endOfLoop)
                {
                    if(begin->second != tlabel)
                        begin++;
                    else
                        endOfLoop = true;
                }
                if(begin != end)
                    (get<1>(it->second)).erase(begin);
                if (debug)
                    cerr << "link " << slabel << "->" << tlabel << " removed\n";
            }
            else if (debug)
                cerr << "attempt to remove link from growing network\n";
        }
        else if (debug)
            cerr << "network " << id << " does not exist\n";
    }
    else if (debug)
        cerr << "attempt to remove link with null node label\n";
}

void network_clear(unsigned long id)
{
    if (debug)
    {
        cerr << "network_clear(" << id << ")\n";
        cerr << "network_clear: ";
    }
    t_all_network::iterator it = getData().find(id);
    if (it != getData().end())
    {
        bool growing = get<2>(it->second);
        if (!growing)
        {
            //czyszczenie wezlow i linkow
            get<0>(it->second).clear();
            get<1>(it->second).clear();
            if (debug)
                cerr << "network "<<id<<" cleared\n";
        }
        else if (debug)
            cerr << "attempt to clear not growing network\n";
    }
    else if (debug)
        cerr << "network " << id << " does not exist\n";
}

size_t network_out_degree(unsigned long id, const char* label)
{
    if (debug) {
        cerr << "network_out_degree(" << id << "," << label << ")\n";
        cerr << "network_out_degree: ";
    }
    if (label != NULL) {
        t_all_network::iterator it = getData().find(id);
        if (it != getData().end()) {
            t_links links = get<1>(it->second);
            size_t result = links.count(label);
            if (debug)
                cerr << "return "<< result <<  "\n";
            return result;
        }
        else if (debug)
            cerr << "network " << id << " does not exist\n";
    }
    else if (debug)
        cerr << "attempt to get null node degree\n";
    return 0;
}

size_t network_in_degree(unsigned long id, const char* label)
{
    if (debug)
    {
        cerr << "network_out_degree(" << id << "," << label << ")\n";
        cerr << "network_out_degree: ";
    }
    if (label != NULL)
    {
        t_all_network::iterator it = getData().find(id);
        if (it != getData().end())
        {
            //przeglada wszystkie linki i liczy te ktore prowadza do label
            t_links links = get<1>(it->second);
            t_links::iterator lIt = links.begin();
            UL result = 0;
            while (lIt != links.end())
            {
                if (lIt->second == label)
                    result++;
                lIt++;
            }
            if (debug)
                cerr << "return " << result <<  "\n";
            return result;
        }
        else if (debug)
            cerr << "network " << id << " does not exist\n";
    }
    else if (debug)
        cerr << "attempt to get null node degree\n";
    return 0;
}
