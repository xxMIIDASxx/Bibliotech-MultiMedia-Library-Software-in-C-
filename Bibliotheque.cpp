#include "Bibliotheque.hpp"
#include <algorithm>
#include <fstream>
#include <numeric>
#include <iostream>
#include <iomanip>
#include <cctype>  // Added for tolower

using namespace std;

// Helper function for case-insensitive string comparison
static bool equalsIgnoreCase(const string& str1, const string& str2) {
    if (str1.size() != str2.size()) return false;
    for (size_t i = 0; i < str1.size(); ++i) {
        if (tolower(str1[i]) != tolower(str2[i])) return false;
    }
    return true;
}

void Bibliotheque::ajouterMedia(shared_ptr<Media> m) {
    medias.push_back(m);
    cout << COLOR_GREEN << "[OK] Media ajoute: " << m->getTitre() << ANSI_RESET << endl;
}

void Bibliotheque::supprimerMedia(const string& titre) {
    size_t avant = medias.size();
    medias.erase(remove_if(medias.begin(), medias.end(),
        [&](const shared_ptr<Media>& m) { return equalsIgnoreCase(m->getTitre(), titre); }), medias.end());
    
    if (medias.size() < avant) {
        cout << COLOR_GREEN << "[OK] " << (avant - medias.size()) << " media(s) supprime(s): " << titre << ANSI_RESET << endl;
    } else {
        cout << COLOR_RED << "[ERREUR] Aucun media trouve avec ce titre: " << titre << ANSI_RESET << endl;
    }
}

void Bibliotheque::rechercherParTitre(const string& titre) const {
    bool trouve = false;
    string titreLower = titre;
    transform(titreLower.begin(), titreLower.end(), titreLower.begin(), ::tolower);
    
    for (const auto& m : medias) {
        string mediaTitre = m->getTitre();
        string mediaTitreLower = mediaTitre;
        transform(mediaTitreLower.begin(), mediaTitreLower.end(), mediaTitreLower.begin(), ::tolower);
        
        if (mediaTitreLower.find(titreLower) != string::npos) {
            m->afficher();
            trouve = true;
        }
    }
    
    if (!trouve) {
        cout << COLOR_YELLOW << "[INFO] Aucun media trouve avec \"" << titre << "\"" << ANSI_RESET << endl;
    }
}

void Bibliotheque::rechercherParType(const string& type) const {
    bool trouve = false;
    string typeLower = type;
    transform(typeLower.begin(), typeLower.end(), typeLower.begin(), ::tolower);
    
    for (const auto& m : medias) {
        string mediaType = m->getType();
        string mediaTypeLower = mediaType;
        transform(mediaTypeLower.begin(), mediaTypeLower.end(), mediaTypeLower.begin(), ::tolower);
        
        if (mediaTypeLower == typeLower) {
            m->afficher();
            trouve = true;
        }
    }
    
    if (!trouve) {
        cout << COLOR_YELLOW << "[INFO] Aucun media trouve de type \"" << type << "\"" << ANSI_RESET << endl;
    }
}

void Bibliotheque::trierParTitre() {
    sort(medias.begin(), medias.end(),
        [](const shared_ptr<Media>& a, const shared_ptr<Media>& b) {
            string aLower = a->getTitre();
            string bLower = b->getTitre();
            transform(aLower.begin(), aLower.end(), aLower.begin(), ::tolower);
            transform(bLower.begin(), bLower.end(), bLower.begin(), ::tolower);
            return aLower < bLower;
        });
    cout << COLOR_GREEN << "[OK] Medias tries par titre (A->Z)" << ANSI_RESET << endl;
}

void Bibliotheque::trierParAnnee() {
    sort(medias.begin(), medias.end(),
        [](const shared_ptr<Media>& a, const shared_ptr<Media>& b) {
            return *a <= *b;
        });
    cout << COLOR_GREEN << "[OK] Medias tries par annee (plus ancien->plus recent)" << ANSI_RESET << endl;
}

void Bibliotheque::afficherTous() const {
    if (medias.empty()) {
        cout << COLOR_YELLOW << "[INFO] La bibliotheque est vide." << ANSI_RESET << endl;
        return;
    }
    
    cout << "\n" << BOLD << BG_CYAN << COLOR_WHITE << " COLLECTION COMPLETE (" 
         << medias.size() << " medias)" << ANSI_RESET << endl;
    for (const auto& m : medias) {
        m->afficher();
    }
}

void Bibliotheque::emprunterMedia(const string& titre) {
    string titreLower = titre;
    transform(titreLower.begin(), titreLower.end(), titreLower.begin(), ::tolower);
    
    for (auto& m : medias) {
        string mediaTitre = m->getTitre();
        string mediaTitreLower = mediaTitre;
        transform(mediaTitreLower.begin(), mediaTitreLower.end(), mediaTitreLower.begin(), ::tolower);
        
        if (mediaTitreLower == titreLower && m->estDisponible()) {
            m->emprunter();
            return;
        }
    }
    cout << COLOR_RED << "[ERREUR] Media non trouve ou non disponible: " << titre << ANSI_RESET << endl;
}

void Bibliotheque::retournerMedia(const string& titre) {
    string titreLower = titre;
    transform(titreLower.begin(), titreLower.end(), titreLower.begin(), ::tolower);
    
    for (auto& m : medias) {
        string mediaTitre = m->getTitre();
        string mediaTitreLower = mediaTitre;
        transform(mediaTitreLower.begin(), mediaTitreLower.end(), mediaTitreLower.begin(), ::tolower);
        
        if (mediaTitreLower == titreLower && !m->estDisponible()) {
            m->retourner();
            return;
        }
    }
    cout << COLOR_RED << "[ERREUR] Media non trouve ou non emprunte: " << titre << ANSI_RESET << endl;
}

double Bibliotheque::dureeMoyenneAudioVideo() const {
    double total = 0;
    int count = 0;
    for (const auto& m : medias) {
        string type = m->getType();
        string typeLower = type;
        transform(typeLower.begin(), typeLower.end(), typeLower.begin(), ::tolower);
        
        if (typeLower == "audio" || typeLower == "video" || typeLower == "livreaudio") {
            total += m->dureeTotale();
            count++;
        }
    }
    return count > 0 ? total / count : 0;
}

void Bibliotheque::exporterStatistiques(const string& fichier) const {
    ofstream out(fichier);
    out << "Statistiques de la bibliotheque :\n";
    out << "Nombre total de medias : " << medias.size() << "\n";
    out << "Duree moyenne des contenus audiovisuels : " << fixed << setprecision(2) << dureeMoyenneAudioVideo() << " min\n";
    
    int livres = 0, audios = 0, videos = 0, livreAudios = 0;
    for (const auto& m : medias) {
        string type = m->getType();
        string typeLower = type;
        transform(typeLower.begin(), typeLower.end(), typeLower.begin(), ::tolower);
        
        if (typeLower == "livre") livres++;
        else if (typeLower == "audio") audios++;
        else if (typeLower == "video") videos++;
        else if (typeLower == "livreaudio") livreAudios++;
    }
    
    out << "\nRepartition par type :\n";
    out << "  Livres : " << livres << "\n";
    out << "  Audios : " << audios << "\n";
    out << "  Videos : " << videos << "\n";
    out << "  LivreAudios : " << livreAudios << "\n";
    
    out.close();
    cout << COLOR_GREEN << "[OK] Statistiques exportees dans '" << fichier << "'" << ANSI_RESET << endl;
}

void Bibliotheque::exporterRapport(const string& fichier) const {
    ofstream out(fichier);
    out << "=== RAPPORT COMPLET DE LA BIBLIOTHEQUE ===\n\n";
    
    for (const auto& m : medias) {
        out << m->getType() << " | " << m->getTitre() << " | " 
            << m->getAuteur() << " | " << m->getAnnee() << " | " 
            << (m->estDisponible() ? "Disponible" : "Emprunte") << "\n";
    }
    
    out.close();
    cout << COLOR_GREEN << "[OK] Rapport exporte dans '" << fichier << "'" << ANSI_RESET << endl;
}