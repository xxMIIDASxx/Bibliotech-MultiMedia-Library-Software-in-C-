#ifndef MEDIA_HPP
#define MEDIA_HPP

#include <iostream>
#include <string>
#include <memory>
#include <stdexcept>
#include <algorithm>  // Added for transform
#include <cctype>     // Added for tolower

// ========== ANSI Escape Codes ==========
#define ANSI_RESET   "\033[0m"
#define BOLD         "\033[1m"
#define UNDERLINE    "\033[4m"
#define COLOR_RED    "\033[31m"
#define COLOR_GREEN  "\033[32m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_CYAN   "\033[36m"
#define COLOR_WHITE  "\033[37m"
#define BG_CYAN      "\033[46m"
#define BG_YELLOW    "\033[43m"
#define BG_RED       "\033[41m"
#define BG_GREEN     "\033[42m"

// ========== Exceptions ==========
class MediaException : public std::exception {
private:
    std::string message;
    
public:
    MediaException(const std::string& msg) : message(msg) {}
    const char* what() const noexcept override { return message.c_str(); }
};

// ========== Classe de base abstraite ==========
class Media {
protected:
    std::string titre;
    std::string auteur;
    int annee;
    bool disponible;

public:
    Media(const std::string& t, const std::string& a, int an);
    virtual ~Media() = default;

    virtual void afficher() const;
    virtual double dureeTotale() const { return 0.0; }
    virtual std::string getType() const = 0;

    // Getters
    std::string getTitre() const;
    std::string getAuteur() const;
    int getAnnee() const;
    bool estDisponible() const;
    void emprunter();
    void retourner();
    void setDisponible(bool dispo);

    // Surcharge d'opérateurs
    bool operator<=(const Media& autre) const;
    bool operator==(const Media& autre) const;
    friend std::ostream& operator<<(std::ostream& os, const Media& media);
};

// ========== Livre avec héritage virtuel ==========
class Livre : virtual public Media {
protected:
    int nbPages;

public:
    Livre(const std::string& t, const std::string& a, int an, int p);
    void afficher() const override;
    std::string getType() const override;
    int getNbPages() const { return nbPages; }
};

// ========== Audio avec héritage virtuel ==========
class Audio : virtual public Media {
protected:
    double duree;

public:
    Audio(const std::string& t, const std::string& a, int an, double d);
    void afficher() const override;
    double dureeTotale() const override;
    std::string getType() const override;
    double getDuree() const { return duree; }
};

// ========== Video ==========
class Video : public Audio {
    std::string format;

public:
    Video(const std::string& t, const std::string& a, int an, double d, const std::string& f);
    void afficher() const override;
    std::string getType() const override;
    std::string getFormat() const { return format; }
};

// ========== LivreAudio (héritage multiple) ==========
class LivreAudio : public Livre, public Audio {
public:
    LivreAudio(const std::string& t, const std::string& a, int an, int p, double d);
    void afficher() const override;
    double dureeTotale() const override { return Audio::dureeTotale(); }
    std::string getType() const override { return "LivreAudio"; }
    
    // Résolution d'ambiguïté pour l'héritage multiple
    std::string getTitre() const { return Media::titre; }
    std::string getAuteur() const { return Media::auteur; }
    int getAnnee() const { return Media::annee; }
    bool estDisponible() const { return Media::disponible; }
    void setDisponible(bool dispo) { Media::disponible = dispo; }
    
    // Gestion des emprunts/retours
    void emprunter() { 
        if (Media::disponible) {
            Media::disponible = false; 
            std::cout << COLOR_GREEN << "[OK] " << Media::titre << " emprunte avec succes!" << ANSI_RESET << std::endl;
        } else {
            throw MediaException("Media non disponible: " + Media::titre);
        }
    }
    
    void retourner() { 
        Media::disponible = true; 
        std::cout << COLOR_GREEN << "[OK] " << Media::titre << " retourne avec succes!" << ANSI_RESET << std::endl;
    }
    
    // Accesseurs spécifiques
    int getNbPages() const { return Livre::getNbPages(); }
    double getDuree() const { return Audio::getDuree(); }
};

#endif