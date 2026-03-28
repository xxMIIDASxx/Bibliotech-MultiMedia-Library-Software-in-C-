#ifndef MENU_HPP
#define MENU_HPP

#include "Bibliotheque.hpp"
#include "Media.hpp"
#include "FichierManager.hpp"
#include "Utilisateur.hpp"
#include <memory>
#include <string>
#include <vector>
#include <limits>  // Added for numeric_limits

class Menu {
private:
    Bibliotheque bibliotheque;
    bool isAuthenticated;
    bool isAdmin;
    std::string currentUsername;
    std::vector<Utilisateur> utilisateurs;
    
    // Authentication functions
    bool authentifier();
    void changerPassword();
    void deconnecter();
    
    // User management functions
    void creerUtilisateur();
    void sauvegarderUtilisateurs();
    void chargerUtilisateurs();
    bool utilisateurExiste(const std::string& username);
    
    // Media operations
    void ajouterMedia();
    void supprimerMedia();
    void rechercherParTitre();
    void rechercherParType();
    void afficherTous();
    void trierParTitre();
    void trierParAnnee();
    void emprunterMedia();
    void retournerMedia();
    
    // Statistics
    void afficherStatistiques();
    
    // Player functions
    void previsualiserMedia();
    void controleLecture();
    
    // File operations
    void sauvegarder();
    void charger();
    
    // Display functions
    void afficherMenuPrincipal();
    void afficherMenuAdmin();
    void afficherMenuClient();
    
    // Helper functions
    void pause() const;
    void clearScreen() const;
    
public:
    Menu();
    void executer();
};

#endif