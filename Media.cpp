#include "Media.hpp"

using namespace std;

// ========== Media ==========
Media::Media(const string& t, const string& a, int an) 
    : titre(t), auteur(a), annee(an), disponible(true) {
    if (an < 0 || an > 2025) {
        throw MediaException("Annee invalide: " + to_string(an));
    }
}

void Media::afficher() const {
    cout << "========================================================\n";
    cout << BOLD << "Titre: " << titre << ANSI_RESET << endl;
    cout << "  Auteur: " << COLOR_YELLOW << auteur << ANSI_RESET;
    cout << "  |  Annee: " << annee << endl;
    cout << "  Type: " << COLOR_GREEN << "Base Media" << ANSI_RESET;
    cout << "  |  Disponible: " << (disponible ? COLOR_GREEN "Oui" : COLOR_RED "Non") << ANSI_RESET << endl;
}

string Media::getTitre() const { return titre; }
string Media::getAuteur() const { return auteur; }
int Media::getAnnee() const { return annee; }
bool Media::estDisponible() const { return disponible; }
void Media::setDisponible(bool dispo) { disponible = dispo; }

void Media::emprunter() { 
    if (disponible) {
        disponible = false; 
        cout << COLOR_GREEN << "[OK] " << titre << " emprunte avec succes!" << ANSI_RESET << endl;
    } else {
        throw MediaException("Media non disponible: " + titre);
    }
}

void Media::retourner() { 
    disponible = true; 
    cout << COLOR_GREEN << "[OK] " << titre << " retourne avec succes!" << ANSI_RESET << endl;
}

bool Media::operator<=(const Media& autre) const {
    return annee <= autre.annee;
}

bool Media::operator==(const Media& autre) const {
    return titre == autre.titre && auteur == autre.auteur && annee == autre.annee;
}

ostream& operator<<(ostream& os, const Media& media) {
    media.afficher();
    return os;
}

// ========== Livre ==========
Livre::Livre(const string& t, const string& a, int an, int p) 
    : Media(t, a, an), nbPages(p) {}

void Livre::afficher() const {
    Media::afficher();
    cout << "  Pages: " << nbPages;
    cout << "  |  Type: " << COLOR_CYAN << "Livre" << ANSI_RESET << endl;
    cout << "========================================================\n";
}

string Livre::getType() const { return "Livre"; }

// ========== Audio ==========
Audio::Audio(const string& t, const string& a, int an, double d) 
    : Media(t, a, an), duree(d) {}

void Audio::afficher() const {
    Media::afficher();
    cout << "  Duree: " << duree << " min";
    cout << "  |  Type: " << COLOR_CYAN << "Audio" << ANSI_RESET << endl;
    cout << "========================================================\n";
}

double Audio::dureeTotale() const { return duree; }
string Audio::getType() const { return "Audio"; }

// ========== Video ==========
Video::Video(const string& t, const string& a, int an, double d, const string& f) 
    : Media(t, a, an), Audio(t, a, an, d), format(f) {}

void Video::afficher() const {
    Media::afficher();
    cout << "  Duree: " << duree << " min";
    cout << "  |  Format: " << format;
    cout << "  |  Type: " << COLOR_CYAN << "Video" << ANSI_RESET << endl;
    cout << "========================================================\n";
}

string Video::getType() const { return "Video"; }

// ========== LivreAudio ==========
LivreAudio::LivreAudio(const string& t, const string& a, int an, int p, double d)
    : Media(t, a, an), Livre(t, a, an, p), Audio(t, a, an, d) {
}

void LivreAudio::afficher() const {
    cout << "========================================================\n";
    cout << BOLD << "Titre: " << Media::titre << ANSI_RESET << endl;
    cout << "  Auteur: " << COLOR_YELLOW << Media::auteur << ANSI_RESET;
    cout << "  |  Annee: " << Media::annee << endl;
    cout << "  Pages: " << Livre::getNbPages();
    cout << "  |  Duree: " << Audio::getDuree() << " min";
    cout << "  |  Type: " << COLOR_CYAN << "Livre Audio" << ANSI_RESET << endl;
    cout << "  Disponible: " << (Media::disponible ? COLOR_GREEN "Oui" : COLOR_RED "Non") << ANSI_RESET << endl;
    cout << "========================================================\n";
}