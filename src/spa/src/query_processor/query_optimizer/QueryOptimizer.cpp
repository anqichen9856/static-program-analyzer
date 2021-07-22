#include "QueryOptimizer.h"

QueryOptimizer::QueryOptimizer(Query *query) {
    this->query = query;
    for (auto selected : query->selected) {
        if (QueryParser::isSynAttrPair(selected)) {
            string synonym = QueryParser::getSyn(selected);
            selected_synonym.insert(synonym);
        } else {
            selected_synonym.insert(selected);
        }
    }
}

void QueryOptimizer::optimizeQuery() {
    rankByClauseType(query->clauses);
    groupSynNoSyn();
    groupByConnectedSyn();
    sortGroups();
    query->no_select = no_select;
    query->with_select = with_select;
}

void QueryOptimizer::groupSynNoSyn() {
    for (auto clause : query->clauses) {
        if (clause->getNumOfSyn() == 0) {
            no_syn.push_back(clause);
        } else if (clause->getNumOfSyn() == 1) {
            with_one_syn.push_back(clause);
        } else {
            with_two_syns.push_back(clause);
        }
    }
}

void QueryOptimizer::groupByConnectedSyn() {
    while (!with_one_syn.empty()) {
        auto first_cl = with_one_syn.front();
        with_one_syn.pop_front();

        list<Clause *> group = {first_cl};
        string syn = *(first_cl->getSynonyms().begin());
        set<string> syn_queue = {syn};
        set<string> group_syn_set = {};

        while (!syn_queue.empty()) {
            string curr_syn = *(syn_queue.begin());
            syn_queue.erase(syn_queue.begin());
            group_syn_set.insert(curr_syn);
            vector<Clause*> to_remove;
            for (auto cl : with_one_syn) {
                if (*(cl->getSynonyms().begin()) == curr_syn) {
                    group.push_back(cl);
                    to_remove.push_back(cl);
                }
            }
            for (auto elem : to_remove) {
                with_one_syn.remove(elem);
            }
            to_remove.clear();
            for (auto cl : with_two_syns) {
                auto syns = cl->getSynonyms();
                if (syns.find(curr_syn) != syns.end()) {
                    group.push_back(cl);
                    to_remove.push_back(cl);
                    for (auto syn : syns) {
                        if (syn != curr_syn) {
                            syn_queue.insert(syn);
                        }
                    }
                }
            }
            for (auto elem : to_remove) {
                with_two_syns.remove(elem);
            }
        }
        syn_in_groups.insert({groups.size(), group_syn_set});
        groups.insert({groups.size(), group});
    }

    while (!with_two_syns.empty()) {
        auto first_cl = with_two_syns.front();
        with_two_syns.pop_front();

        list<Clause *> group = {first_cl};
        set<string> syn_queue = {};
        for (string syn : first_cl->getSynonyms()) {
            syn_queue.insert(syn);
        }
        set<string> group_syn_set;

        while (!syn_queue.empty()) {
            string curr_syn = *(syn_queue.begin());
            syn_queue.erase(syn_queue.begin());
            group_syn_set.insert(curr_syn);

            vector<Clause*> to_remove;
            for (auto cl : with_two_syns) {
                auto syns = cl->getSynonyms();
                if (syns.find(curr_syn) != syns.end()) {
                    group.push_back(cl);
                    to_remove.push_back(cl);
                    for (auto syn : syns) {
                        if (syn != curr_syn) {
                            syn_queue.insert(syn);
                        }
                    }
                }
            }
            for (auto elem : to_remove) {
                with_two_syns.remove(elem);
            }
        }
        syn_in_groups.insert({groups.size(), group_syn_set});
        groups.insert({groups.size(), group});
    }
}

void QueryOptimizer::sortGroups() {
    if (!no_syn.empty()) {
        no_select.push_back(no_syn);
    }

    for (int i = 0; i < groups.size(); i++) {
        bool contains = false;
        for (auto selected : selected_synonym) {
            if (syn_in_groups[i].find(selected) != syn_in_groups[i].end()) {
                with_select.push_back(groups[i]);
                contains = true;
                break;
            }
        }
        if (!contains) {
            no_select.push_back(groups[i]);
        }
    }

}

void QueryOptimizer::rankByClauseType(vector<Clause*> &lst) {
    auto comp = [](Clause *c1, Clause *c2) {
        return c1->getRank() < c2->getRank();
    };
    sort(lst.begin(), lst.end(), comp);
}





