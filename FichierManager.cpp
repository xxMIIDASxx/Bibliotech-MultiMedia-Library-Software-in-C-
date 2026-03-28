#include "FichierManager.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <algorithm>
#include <cctype>

using namespace std;

// Helper function for case-insensitive string comparison
static bool equalsIgnoreCase(const string& str1, const string& str2) {
    if (str1.size() != str2.size()) return false;
    for (size_t i = 0; i < str1.size(); ++i) {
        if (tolower(str1[i]) != tolower(str2[i])) return false;
    }
    return true;
}

bool FichierManager::sauvegarder(const Bibliotheque& biblio, const string& fichier) {
    ofstream file(fichier);
    if (!file.is_open()) {
        cerr << COLOR_RED << "[ERREUR] Impossible d'ouvrir le fichier " << fichier << ANSI_RESET << endl;
        return false;
    }
    
    file << "# SAUVEGARDE BIBLIOTHEQUE MULTIMEDIA\n";
    file << "# Format: Type|Titre|Auteur|Annee|Disponible|Attributs\n\n";
    
    const auto& medias = biblio.getMedias();
    for (const auto& m : medias) {
        string type = m->getType();
        file << type << "|" 
             << m->getTitre() << "|"
             << m->getAuteur() << "|"
             << m->getAnnee() << "|"
             << (m->estDisponible() ? "1" : "0");
        
        // Convert type to lowercase for comparison
        string typeLower = type;
        transform(typeLower.begin(), typeLower.end(), typeLower.begin(), ::tolower);
        
        if (equalsIgnoreCase(type, "Livre")) {
            auto livre = dynamic_cast<Livre*>(m.get());
            if (livre) file << "|" << livre->getNbPages();
        } 
        else if (equalsIgnoreCase(type, "Audio")) {
            auto audio = dynamic_cast<Audio*>(m.get());
            if (audio) file << "|" << audio->getDuree();
        }
        else if (equalsIgnoreCase(type, "Video")) {
            auto video = dynamic_cast<Video*>(m.get());
            if (video) file << "|" << video->getDuree() << "|" << video->getFormat();
        }
        else if (equalsIgnoreCase(type, "LivreAudio")) {
            auto livreAudio = dynamic_cast<LivreAudio*>(m.get());
            if (livreAudio) file << "|" << livreAudio->getNbPages() << "|" << livreAudio->getDuree();
        }
        
        file << "\n";
    }
    
    file.close();
    cout << COLOR_GREEN << "[OK] Bibliotheque sauvegardee dans '" << fichier << "'" << ANSI_RESET << endl;
    return true;
}

bool FichierManager::charger(Bibliotheque& biblio, const string& fichier) {
    ifstream file(fichier);
    if (!file.is_open()) {
        cerr << COLOR_RED << "[ERREUR] Fichier non trouve: " << fichier << ANSI_RESET << endl;
        return false;
    }
    
    string line;
    int count = 0;
    
    auto& medias = const_cast<vector<shared_ptr<Media>>&>(biblio.getMedias());
    medias.clear();
    
    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        
        stringstream ss(line);
        string token;
        vector<string> tokens;
        
        while (getline(ss, token, '|')) {
            tokens.push_back(token);
        }
        
        if (tokens.size() < 5) continue;
        
        string type = tokens[0];
        string titre = tokens[1];
        string auteur = tokens[2];
        int annee = stoi(tokens[3]);
        bool disponible = (tokens[4] == "1");
        
        try {
            // Convert type to lowercase for comparison
            string typeLower = type;
            transform(typeLower.begin(), typeLower.end(), typeLower.begin(), ::tolower);
            
            if (equalsIgnoreCase(type, "Livre") && tokens.size() >= 6) {
                int pages = stoi(tokens[5]);
                auto livre = make_shared<Livre>(titre, auteur, annee, pages);
                livre->setDisponible(disponible);
                biblio.ajouterMedia(livre);
                count++;
            }
            else if (equalsIgnoreCase(type, "Audio") && tokens.size() >= 6) {
                double duree = stod(tokens[5]);
                auto audio = make_shared<Audio>(titre, auteur, annee, duree);
                audio->setDisponible(disponible);
                biblio.ajouterMedia(audio);
                count++;
            }
            else if (equalsIgnoreCase(type, "Video") && tokens.size() >= 7) {
                double duree = stod(tokens[5]);
                string format = tokens[6];
                auto video = make_shared<Video>(titre, auteur, annee, duree, format);
                video->setDisponible(disponible);
                biblio.ajouterMedia(video);
                count++;
            }
            else if (equalsIgnoreCase(type, "LivreAudio") && tokens.size() >= 7) {
                int pages = stoi(tokens[5]);
                double duree = stod(tokens[6]);
                auto livreAudio = make_shared<LivreAudio>(titre, auteur, annee, pages, duree);
                livreAudio->setDisponible(disponible);
                biblio.ajouterMedia(livreAudio);
                count++;
            }
        } catch (const exception& e) {
            cerr << COLOR_RED << "[ERREUR] Erreur lors du chargement d'une ligne: " << e.what() << ANSI_RESET << endl;
        }
    }
    
    file.close();
    cout << COLOR_GREEN << "[OK] " << count << " medias charges depuis '" << fichier << "'" << ANSI_RESET << endl;
    return true;
}

void FichierManager::testerSauvegarde() {
    cout << "\n=== Test Sauvegarde ===" << endl;
    Bibliotheque biblio;
    biblio.ajouterMedia(make_shared<Livre>("Test Livre", "Auteur Test", 2023, 300));
    sauvegarder(biblio, "test_sauvegarde.txt");
}

void FichierManager::testerChargement() {
    cout << "\n=== Test Chargement ===" << endl;
    Bibliotheque biblio;
    charger(biblio, "test_sauvegarde.txt");
    biblio.afficherTous();
}

void FichierManager::testerFichierCSV() {
    cout << "\n=== Test CSV ===" << endl;
    testerSauvegarde();
    testerChargement();
}