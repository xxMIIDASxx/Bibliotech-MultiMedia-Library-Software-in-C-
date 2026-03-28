#ifndef STATISTIQUES_HPP
#define STATISTIQUES_HPP

#include "Bibliotheque.hpp"
#include <string>

class Statistiques {
public:
    static void afficherStatsParType(const Bibliotheque& biblio, 
                                    const std::string& fichier = "stats_detaillees.txt");
};

#endif