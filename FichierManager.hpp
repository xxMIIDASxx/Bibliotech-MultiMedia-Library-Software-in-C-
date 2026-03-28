#ifndef FICHIERMANAGER_HPP
#define FICHIERMANAGER_HPP

#include "Bibliotheque.hpp"
#include <string>

class FichierManager {
public:
    static bool sauvegarder(const Bibliotheque& biblio, const std::string& fichier);
    static bool charger(Bibliotheque& biblio, const std::string& fichier);
    
    static void testerSauvegarde();
    static void testerChargement();
    static void testerFichierCSV();
};

#endif