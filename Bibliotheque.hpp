#ifndef BIBLIOTHEQUE_HPP
#define BIBLIOTHEQUE_HPP

#include "Media.hpp"
#include <vector>
#include <memory>
#include <functional>

class Bibliotheque {
    std::vector<std::shared_ptr<Media>> medias;

public:
    void ajouterMedia(std::shared_ptr<Media> m);
    void supprimerMedia(const std::string& titre);
    void rechercherParTitre(const std::string& titre) const;
    void rechercherParType(const std::string& type) const;
    void trierParTitre();
    void trierParAnnee();
    void afficherTous() const;
    void emprunterMedia(const std::string& titre);
    void retournerMedia(const std::string& titre);
    double dureeMoyenneAudioVideo() const;
    void exporterStatistiques(const std::string& fichier) const;
    void exporterRapport(const std::string& fichier) const;
    
    const std::vector<std::shared_ptr<Media>>& getMedias() const { return medias; }
};

#endif