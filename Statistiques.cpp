#include "Statistiques.hpp"
#include <iostream>
#include <fstream>
#include <map>
#include <iomanip>
#include <vector>
#include <memory>

using namespace std;

void Statistiques::afficherStatsParType(const Bibliotheque& biblio, const string& fichier) {
    ofstream out(fichier);
    if (!out) {
        cerr << "[ERREUR] Impossible d'ouvrir le fichier " << fichier << endl;
        return;
    }
    
    out << "=== STATISTIQUES DETAILLEES PAR TYPE ===\n\n";
    
    const auto& medias = biblio.getMedias();
    
    map<string, int> compteurType;
    map<string, double> dureeTotale;
    map<string, int> disponibleParType;
    
    for (const auto& m : medias) {
        string type = m->getType();
        compteurType[type]++;
        
        if (type == "Audio" || type == "Video" || type == "LivreAudio") {
            dureeTotale[type] += m->dureeTotale();
        }
        
        if (m->estDisponible()) {
            disponibleParType[type]++;
        }
    }
    
    out << "Nombre total de medias: " << medias.size() << "\n\n";
    
    out << "Details par type:\n";
    out << "=================\n";
    
    for (const auto& entry : compteurType) {
        string type = entry.first;
        int count = entry.second;
        
        out << "\nType: " << type << "\n";
        out << "  Nombre: " << count << "\n";
        out << "  Pourcentage: " << fixed << setprecision(1) 
            << (count * 100.0 / medias.size()) << "%\n";
        
        if (dureeTotale.find(type) != dureeTotale.end()) {
            out << "  Duree totale: " << dureeTotale[type] << " min\n";
            out << "  Duree moyenne: " << (dureeTotale[type] / count) << " min\n";
        }
        
        out << "  Disponibles: " << disponibleParType[type] << "/" << count 
            << " (" << (disponibleParType[type] * 100.0 / count) << "%)\n";
    }
    
    out.close();
    cout << COLOR_GREEN << "[OK] Statistiques detaillees exportees dans '" << fichier << "'" << ANSI_RESET << endl;
}