#include "Menu.hpp"
#include <iostream>
#include <iomanip>
#include <limits>
#include <fstream>
#include <memory>
#include <cstdlib>
#include <algorithm>
#include <cctype>

// Platform-specific includes for password hiding
#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

using namespace std;

static void sleepMilliseconds(unsigned int ms) {
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}

// Helper function to clear input buffer (keep as standalone function)
void clearCinBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Helper function to get hidden password input
string getHiddenPassword() {
    string password;
    
#ifdef _WIN32
    // Windows implementation using _getch()
    char ch;
    while ((ch = _getch()) != '\r') {  // Enter key
        if (ch == '\b') {  // Backspace
            if (!password.empty()) {
                password.pop_back();
                cout << "\b \b";  // Move back, space, move back
            }
        } else if (ch >= 32 && ch <= 126) {  // Printable characters
            password.push_back(ch);
            cout << '*';
        }
    }
    cout << endl;
#else
    // Linux/Unix implementation using termios
    termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~ECHO;  // Disable echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    
    getline(cin, password);
    
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);  // Restore echo
    cout << endl;
#endif
    
    return password;
}

Menu::Menu() : isAuthenticated(false), isAdmin(false), currentUsername("") {
    // Clear any existing input buffer
    clearCinBuffer();
    
    // Charger les utilisateurs existants
    chargerUtilisateurs();
    
    // Si aucun utilisateur n'existe, creer un admin par defaut
    if (utilisateurs.empty()) {
        utilisateurs.push_back(Utilisateur("admin", "admin123", true));
        sauvegarderUtilisateurs();
        cout << COLOR_YELLOW << "[INFO] COMPTE ADMINISTRATEUR PAR DEFAUT CREE." << ANSI_RESET << endl;
        cout << "  NOM D'UTILISATEUR: ADMIN" << endl;
        cout << "  MOT DE PASSE: ADMIN123" << endl;
        sleepMilliseconds(2000);
    }
    
    // Ajouter des medias de test
    bibliotheque.ajouterMedia(make_shared<Livre>("The Lord Of The Rings: The Fellowship Of The Ring", "J.R.R Tolkien", 1954, 571));
    bibliotheque.ajouterMedia(make_shared<Audio>("Stan", "Eminem", 2009, 8.08));
    bibliotheque.ajouterMedia(make_shared<Video>("Inception", "Christopher Nolan", 2010, 148.0, "4K"));
    bibliotheque.ajouterMedia(make_shared<LivreAudio>("Harry Potter and The Philosopher's Stone", "J.K. Rowling", 1997, 320, 1245.5));
    
}

void Menu::clearScreen() const {
    cout << "\033[2J\033[1;1H";
}

void Menu::pause() const {
    cout << "\n" << COLOR_YELLOW << "APPUYEZ SUR ENTREE POUR CONTINUER..." << ANSI_RESET;
    clearCinBuffer();
}

bool Menu::utilisateurExiste(const string& username) {
    // Username is case-sensitive
    for (const auto& user : utilisateurs) {
        if (user.getUsername() == username) {
            return true;
        }
    }
    return false;
}

void Menu::sauvegarderUtilisateurs() {
    ofstream fichier("utilisateurs.dat");
    if (!fichier) {
        cerr << COLOR_RED << "[ERREUR] IMPOSSIBLE DE SAUVEGARDER LES UTILISATEURS!" << ANSI_RESET << endl;
        return;
    }
    
    for (const auto& user : utilisateurs) {
        fichier << user.getUsername() << "|"
                << user.getPassword() << "|"
                << (user.getIsAdmin() ? "1" : "0") << "\n";
    }
    fichier.close();
}

void Menu::chargerUtilisateurs() {
    ifstream fichier("utilisateurs.dat");
    if (!fichier) {
        return;
    }
    
    string ligne;
    while (getline(fichier, ligne)) {
        size_t pos1 = ligne.find('|');
        size_t pos2 = ligne.find('|', pos1 + 1);
        
        if (pos1 != string::npos && pos2 != string::npos) {
            string username = ligne.substr(0, pos1);
            string password = ligne.substr(pos1 + 1, pos2 - pos1 - 1);
            bool isAdmin = (ligne.substr(pos2 + 1) == "1");
            
            utilisateurs.push_back(Utilisateur(username, password, isAdmin));
        }
    }
    fichier.close();
}

bool Menu::authentifier() {
    clearScreen();
    cout << "\n" << BOLD << UNDERLINE << COLOR_GREEN 
         << "AUTHENTIFICATION" << ANSI_RESET << endl << endl;
    
    string username, password;
    int attempts = 3;
    
    while (attempts > 0) {
        cout << "NOM D'UTILISATEUR: ";
        getline(cin, username);
        
        cout << "MOT DE PASSE: ";
        password = getHiddenPassword();  // Use hidden password input
        
        bool found = false;
        for (auto& user : utilisateurs) {
            // Username is case-sensitive
            if (user.getUsername() == username && user.authentifier(password)) {
                isAuthenticated = true;
                isAdmin = user.getIsAdmin();
                currentUsername = username;
                cout << COLOR_GREEN << "\n[OK] CONNEXION REUSSIE! ";
                cout << (isAdmin ? "(MODE ADMINISTRATEUR)" : "(MODE CLIENT)") << ANSI_RESET << endl;
                sleepMilliseconds(1000);
                return true;
            }
        }
        
        if (!found) {
            attempts--;
            if (attempts > 0) {
                cout << COLOR_RED << "\n[ERREUR] IDENTIFIANTS INCORRECTS. TENTATIVES RESTANTES: " 
                     << attempts << ANSI_RESET << endl;
                sleepMilliseconds(500);
            }
        }
    }
    
    cout << COLOR_RED << "\n[ERREUR] ACCES REFUSE. TROP DE TENTATIVES ECHOUEES." << ANSI_RESET << endl;
    sleepMilliseconds(2000);
    return false;
}

void Menu::creerUtilisateur() {
    if (!isAuthenticated || !isAdmin) {
        cout << COLOR_RED << "[ERREUR] ACCES REFUSE! ADMINISTRATEUR REQUIS." << ANSI_RESET << endl;
        pause();
        return;
    }
    
    clearScreen();
    cout << "\n" << BOLD << UNDERLINE << COLOR_GREEN 
         << "CREATION D'UN NOUVEL UTILISATEUR" << ANSI_RESET << endl << endl;
    
    string username, password;
    char choixAdmin;
    bool isAdminUser = false;
    
    // Clear buffer before taking new input
    clearCinBuffer();
    
    do {
        cout << "NOM D'UTILISATEUR: ";
        getline(cin, username);
        
        if (utilisateurExiste(username)) {
            cout << COLOR_RED << "[ERREUR] CE NOM D'UTILISATEUR EXISTE DEJA!" << ANSI_RESET << endl;
        }
    } while (utilisateurExiste(username));
    
    cout << "MOT DE PASSE: ";
    password = getHiddenPassword();  // Use hidden password input
    
    cout << "EST-CE UN ADMINISTRATEUR? (O/N): ";
    cin >> choixAdmin;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    
    isAdminUser = (choixAdmin == 'O' || choixAdmin == 'o');
    
    utilisateurs.push_back(Utilisateur(username, password, isAdminUser));
    sauvegarderUtilisateurs();
    
    cout << COLOR_GREEN << "\n[OK] UTILISATEUR CREE AVEC SUCCES!" << ANSI_RESET << endl;
    cout << "NOM D'UTILISATEUR: " << username << endl;
    cout << "TYPE: " << (isAdminUser ? "ADMINISTRATEUR" : "CLIENT") << endl;
    
    pause();
}

void Menu::changerPassword() {
    if (!isAuthenticated) {
        cout << COLOR_RED << "[ERREUR] VOUS DEVEZ ETRE AUTHENTIFIE!" << ANSI_RESET << endl;
        pause();
        return;
    }
    
    clearScreen();
    cout << "\n" << BOLD << UNDERLINE << COLOR_CYAN 
         << "CHANGER MOT DE PASSE" << ANSI_RESET << endl << endl;
    
    string ancien, nouveau;
    cout << "NOM D'UTILISATEUR: " << currentUsername << endl;
    
    cout << "ANCIEN MOT DE PASSE: ";
    ancien = getHiddenPassword();  // Use hidden password input
    
    cout << "NOUVEAU MOT DE PASSE: ";
    nouveau = getHiddenPassword();  // Use hidden password input
    
    for (auto& user : utilisateurs) {
        if (user.getUsername() == currentUsername) {
            user.changerPassword(ancien, nouveau);
            sauvegarderUtilisateurs();
            pause();
            return;
        }
    }
    
    cout << COLOR_RED << "\n[ERREUR] UTILISATEUR NON TROUVE!" << ANSI_RESET << endl;
    pause();
}

void Menu::deconnecter() {
    isAuthenticated = false;
    isAdmin = false;
    currentUsername = "";
    cout << COLOR_GREEN << "\n[OK] DECONNEXION REUSSIE!" << ANSI_RESET << endl;
    sleepMilliseconds(1000);
}

void Menu::ajouterMedia() {
    if (!isAuthenticated || !isAdmin) {
        cout << COLOR_RED << "[ERREUR] ACCES REFUSE! ADMINISTRATEUR REQUIS." << ANSI_RESET << endl;
        pause();
        return;
    }
    
    clearScreen();
    cout << "\n" << BOLD << UNDERLINE << COLOR_GREEN 
         << "AJOUTER UN MEDIA" << ANSI_RESET << endl << endl;
    
    string type, titre, auteur, format;
    int annee, pages;
    double duree;
    
    cout << "TYPE (LIVRE/AUDIO/VIDEO/LIVREAUDIO): ";
    getline(cin, type);
    
    cout << "TITRE: ";
    getline(cin, titre);
    
    cout << "AUTEUR: ";
    getline(cin, auteur);
    
    cout << "ANNEE: ";
    cin >> annee;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    
    // Convert type to lowercase for case-insensitive comparison
    string typeLower = type;
    transform(typeLower.begin(), typeLower.end(), typeLower.begin(), ::tolower);
    
    if (typeLower == "livre") {
        cout << "NOMBRE DE PAGES: ";
        cin >> pages;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        bibliotheque.ajouterMedia(make_shared<Livre>(titre, auteur, annee, pages));
    } else if (typeLower == "audio") {
        cout << "DUREE (MINUTES): ";
        cin >> duree;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        bibliotheque.ajouterMedia(make_shared<Audio>(titre, auteur, annee, duree));
    } else if (typeLower == "video") {
        cout << "DUREE (MINUTES): ";
        cin >> duree;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "FORMAT: ";
        getline(cin, format);
        bibliotheque.ajouterMedia(make_shared<Video>(titre, auteur, annee, duree, format));
    } else if (typeLower == "livreaudio") {
        cout << "NOMBRE DE PAGES: ";
        cin >> pages;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "DUREE (MINUTES): ";
        cin >> duree;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        bibliotheque.ajouterMedia(make_shared<LivreAudio>(titre, auteur, annee, pages, duree));
    } else {
        cout << COLOR_RED << "[ERREUR] TYPE DE MEDIA NON RECONNU!" << ANSI_RESET << endl;
        pause();
        return;
    }
    
    pause();
}

void Menu::supprimerMedia() {
    if (!isAuthenticated || !isAdmin) {
        cout << COLOR_RED << "[ERREUR] ACCES REFUSE! ADMINISTRATEUR REQUIS." << ANSI_RESET << endl;
        pause();
        return;
    }
    
    clearScreen();
    cout << "\n" << BOLD << UNDERLINE << COLOR_RED 
         << "SUPPRIMER UN MEDIA" << ANSI_RESET << endl << endl;
    
    string titre;
    
    cout << "TITRE DU MEDIA A SUPPRIMER: ";
    getline(cin, titre);
    
    cout << "\n" << COLOR_YELLOW << "ATTENTION: CETTE ACTION EST IRREVERSIBLE!" << ANSI_RESET << endl;
    cout << "CONFIRMEZ-VOUS LA SUPPRESSION DE \"" << titre << "\"? (O/N): ";
    
    char confirmation;
    cin >> confirmation;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    
    if (confirmation == 'O' || confirmation == 'o') {
        bibliotheque.supprimerMedia(titre);
    } else {
        cout << COLOR_GREEN << "\n[OK] SUPPRESSION ANNULEE!" << ANSI_RESET << endl;
    }
    
    pause();
}

void Menu::rechercherParTitre() {
    if (!isAuthenticated) {
        cout << COLOR_RED << "[ERREUR] VOUS DEVEZ ETRE AUTHENTIFIE!" << ANSI_RESET << endl;
        pause();
        return;
    }
    
    clearScreen();
    cout << "\n" << BOLD << UNDERLINE << COLOR_CYAN 
         << "RECHERCHER PAR TITRE" << ANSI_RESET << endl << endl;
    
    string titre;
    
    cout << "TITRE A RECHERCHER: ";
    getline(cin, titre);
    
    cout << "\n" << BOLD << "RESULTATS POUR \"" << titre << "\":" << ANSI_RESET << endl;
    bibliotheque.rechercherParTitre(titre);
    pause();
}

void Menu::rechercherParType() {
    if (!isAuthenticated) {
        cout << COLOR_RED << "[ERREUR] VOUS DEVEZ ETRE AUTHENTIFIE!" << ANSI_RESET << endl;
        pause();
        return;
    }
    
    clearScreen();
    cout << "\n" << BOLD << UNDERLINE << COLOR_CYAN 
         << "RECHERCHER PAR TYPE" << ANSI_RESET << endl << endl;
    
    string type;
    
    cout << "TYPE (LIVRE/AUDIO/VIDEO/LIVREAUDIO): ";
    getline(cin, type);
    
    cout << "\n" << BOLD << "RESULTATS POUR LE TYPE \"" << type << "\":" << ANSI_RESET << endl;
    bibliotheque.rechercherParType(type);
    pause();
}

void Menu::afficherTous() {
    if (!isAuthenticated) {
        cout << COLOR_RED << "[ERREUR] VOUS DEVEZ ETRE AUTHENTIFIE!" << ANSI_RESET << endl;
        pause();
        return;
    }
    
    clearScreen();
    bibliotheque.afficherTous();
    pause();
}

void Menu::trierParTitre() {
    if (!isAuthenticated) {
        cout << COLOR_RED << "[ERREUR] VOUS DEVEZ ETRE AUTHENTIFIE!" << ANSI_RESET << endl;
        pause();
        return;
    }
    
    bibliotheque.trierParTitre();
    pause();
}

void Menu::trierParAnnee() {
    if (!isAuthenticated) {
        cout << COLOR_RED << "[ERREUR] VOUS DEVEZ ETRE AUTHENTIFIE!" << ANSI_RESET << endl;
        pause();
        return;
    }
    
    bibliotheque.trierParAnnee();
    pause();
}

void Menu::emprunterMedia() {
    if (!isAuthenticated) {
        cout << COLOR_RED << "[ERREUR] VOUS DEVEZ ETRE AUTHENTIFIE!" << ANSI_RESET << endl;
        pause();
        return;
    }
    
    clearScreen();
    cout << "\n" << BOLD << UNDERLINE << COLOR_GREEN 
         << "EMPRUNTER UN MEDIA" << ANSI_RESET << endl << endl;
    
    string titre;
    
    cout << "TITRE DU MEDIA A EMPRUNTER: ";
    getline(cin, titre);
    
    bibliotheque.emprunterMedia(titre);
    pause();
}

void Menu::retournerMedia() {
    if (!isAuthenticated) {
        cout << COLOR_RED << "[ERREUR] VOUS DEVEZ ETRE AUTHENTIFIE!" << ANSI_RESET << endl;
        pause();
        return;
    }
    
    clearScreen();
    cout << "\n" << BOLD << UNDERLINE << COLOR_GREEN 
         << "RETOURNER UN MEDIA" << ANSI_RESET << endl << endl;
    
    string titre;
    
    cout << "TITRE DU MEDIA A RETOURNER: ";
    getline(cin, titre);
    
    bibliotheque.retournerMedia(titre);
    pause();
}

void Menu::afficherStatistiques() {
    if (!isAuthenticated) {
        cout << COLOR_RED << "[ERREUR] VOUS DEVEZ ETRE AUTHENTIFIE!" << ANSI_RESET << endl;
        pause();
        return;
    }
    
    clearScreen();
    cout << "\n" << BOLD << UNDERLINE << COLOR_CYAN 
         << "STATISTIQUES" << ANSI_RESET << endl << endl;
    
    cout << "DUREE MOYENNE DES CONTENUS AUDIOVISUELS: " 
         << fixed << setprecision(2) << bibliotheque.dureeMoyenneAudioVideo() << " MINUTES\n\n";
    
    bibliotheque.exporterStatistiques("statistiques.txt");
    
    pause();
}

void Menu::previsualiserMedia() {
    if (!isAuthenticated) {
        cout << COLOR_RED << "[ERREUR] VOUS DEVEZ ETRE AUTHENTIFIE!" << ANSI_RESET << endl;
        pause();
        return;
    }
    
    clearScreen();
    cout << "\n" << BOLD << UNDERLINE << COLOR_CYAN 
         << "PREVISUALISER UN MEDIA" << ANSI_RESET << endl << endl;
    
    string titre;
    
    cout << "TITRE DU MEDIA A PREVISUALISER: ";
    getline(cin, titre);
    
    cout << "\n" << COLOR_YELLOW << "CHARGEMENT DU MEDIA: " << titre << ANSI_RESET << endl;
    cout << "LECTURE EN COURS..." << endl;
    cout << "[##########] 100%" << endl;
    
    cout << "\n" << COLOR_YELLOW << "APPUYEZ SUR ENTREE POUR ARRETER LA LECTURE..." << ANSI_RESET;
    cin.get();
}

void Menu::controleLecture() {
    if (!isAuthenticated) {
        cout << COLOR_RED << "[ERREUR] VOUS DEVEZ ETRE AUTHENTIFIE!" << ANSI_RESET << endl;
        pause();
        return;
    }
    
    int choix;
    do {
        clearScreen();
        cout << "\n" << BOLD << UNDERLINE << COLOR_CYAN 
             << "CONTROLE DE LECTURE" << ANSI_RESET << endl << endl;
        cout << "  1. " << COLOR_WHITE << "[JOUER]" << ANSI_RESET << " DEMARRER LA LECTURE\n";
        cout << "  2. " << COLOR_WHITE << "[PAUSE]" << ANSI_RESET << " METTRE EN PAUSE\n";
        cout << "  3. " << COLOR_WHITE << "[STOP]" << ANSI_RESET << " ARRETER\n";
        cout << "  4. " << COLOR_WHITE << "[AVANCER]" << ANSI_RESET << " AVANCER 10S\n";
        cout << "  5. " << COLOR_WHITE << "[RECULER]" << ANSI_RESET << " RECULER 10S\n";
        cout << "  0. " << COLOR_WHITE << "[RETOUR]" << ANSI_RESET << " RETOUR AU MENU\n";
        cout << BOLD << "\n>> " << ANSI_RESET;
        
        if (!(cin >> choix)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            choix = -1;
        }
        
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        switch(choix) {
            case 1:
                cout << "\n" << COLOR_GREEN << "[JOUER] LECTURE DEMARREE" << ANSI_RESET << endl;
                break;
            case 2:
                cout << "\n" << COLOR_YELLOW << "[PAUSE] LECTURE EN PAUSE" << ANSI_RESET << endl;
                break;
            case 3:
                cout << "\n" << COLOR_RED << "[STOP] LECTURE ARRETEE" << ANSI_RESET << endl;
                break;
            case 4:
                cout << "\n" << COLOR_CYAN << "[AVANCER] AVANCE DE 10 SECONDES" << ANSI_RESET << endl;
                break;
            case 5:
                cout << "\n" << COLOR_CYAN << "[RECULER] RECULE DE 10 SECONDES" << ANSI_RESET << endl;
                break;
            case 0:
                cout << "\n" << COLOR_YELLOW << "RETOUR AU MENU PRINCIPAL" << ANSI_RESET << endl;
                break;
            default:
                cout << COLOR_RED << "\n[ERREUR] CHOIX INVALIDE!" << ANSI_RESET << endl;
        }
        
        if (choix != 0) {
            cout << "APPUYEZ SUR ENTREE POUR CONTINUER...";
            cin.get();
        }
        
    } while (choix != 0);
}

void Menu::sauvegarder() {
    if (!isAuthenticated || !isAdmin) {
        cout << COLOR_RED << "[ERREUR] ACCES REFUSE! ADMINISTRATEUR REQUIS." << ANSI_RESET << endl;
        pause();
        return;
    }
    
    clearScreen();
    cout << "\n" << BOLD << UNDERLINE << COLOR_GREEN 
         << "SAUVEGARDER" << ANSI_RESET << endl << endl;
    
    string fichier = "sauvegarde.txt";
    bibliotheque.exporterRapport(fichier);
    pause();
}

void Menu::charger() {
    if (!isAuthenticated || !isAdmin) {
        cout << COLOR_RED << "[ERREUR] ACCES REFUSE! ADMINISTRATEUR REQUIS." << ANSI_RESET << endl;
        pause();
        return;
    }
    
    clearScreen();
    cout << "\n" << BOLD << UNDERLINE << COLOR_CYAN 
         << "CHARGER DEPUIS FICHIER" << ANSI_RESET << endl << endl;
    
    string fichier;
    cout << "NOM DU FICHIER A CHARGER: ";
    cin >> fichier;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    
    if (FichierManager::charger(bibliotheque, fichier)) {
        cout << COLOR_GREEN << "[OK] CHARGEMENT REUSSI!" << ANSI_RESET << endl;
    } else {
        cout << COLOR_RED << "[ERREUR] ECHEC DU CHARGEMENT!" << ANSI_RESET << endl;
    }
    
    pause();
}

void Menu::afficherMenuPrincipal() {
    clearScreen();
    cout << "\n" << BOLD << UNDERLINE << COLOR_GREEN 
         << "BIENVENUE DANS BIBLIOTECH - VOTRE BIBLIOTHEQUE MEDIAS" << ANSI_RESET << endl;
    
    if (!isAuthenticated) {
        cout << "\n" << BOLD << "SELECTIONNEZ VOTRE ROLE:" << ANSI_RESET << endl;
        cout << "----------------------------------------\n";
        cout << "  1. " << COLOR_WHITE << "MODE CLIENT" << ANSI_RESET << "\n";
        cout << "  2. " << COLOR_WHITE << "MODE ADMIN" << ANSI_RESET << "\n";
        cout << "  0. " << COLOR_RED << "QUITTER LE PROGRAMME" << ANSI_RESET << "\n";
        cout << "----------------------------------------\n";
    } else if (isAdmin) {
        afficherMenuAdmin();
    } else {
        afficherMenuClient();
    }
    
    cout << BOLD << "\n>> " << ANSI_RESET;
}

void Menu::afficherMenuAdmin() {
    cout << "\n" << BOLD << BG_CYAN << COLOR_WHITE 
         << " MODE ADMINISTRATEUR " << ANSI_RESET << endl;
    cout << "\n" << BOLD << COLOR_GREEN << "SELECTIONNEZ UNE OPERATION:" << ANSI_RESET << endl;
    cout << "========================================\n";
    cout << "  " << COLOR_CYAN << "GESTION UTILISATEURS:" << ANSI_RESET << "\n";
    cout << "    1. " << COLOR_WHITE << "CREER UN NOUVEL UTILISATEUR" << ANSI_RESET << "\n";
    cout << "    2. " << COLOR_WHITE << "CHANGER MOT DE PASSE" << ANSI_RESET << "\n";
    cout << "    3. " << COLOR_WHITE << "SE DECONNECTER" << ANSI_RESET << "\n";
    cout << "\n  " << COLOR_CYAN << "GESTION MEDIAS:" << ANSI_RESET << "\n";
    cout << "    4. " << COLOR_WHITE << "AJOUTER UN MEDIA" << ANSI_RESET << "\n";
    cout << "    5. " << COLOR_WHITE << "SUPPRIMER UN MEDIA" << ANSI_RESET << "\n";
    cout << "    6. " << COLOR_WHITE << "AFFICHER TOUS LES MEDIAS" << ANSI_RESET << "\n";
    cout << "    7. " << COLOR_WHITE << "TRIER PAR TITRE" << ANSI_RESET << "\n";
    cout << "    8. " << COLOR_WHITE << "TRIER PAR ANNEE" << ANSI_RESET << "\n";
    cout << "\n  " << COLOR_CYAN << "RECHERCHE:" << ANSI_RESET << "\n";
    cout << "    9. " << COLOR_WHITE << "RECHERCHER PAR TITRE" << ANSI_RESET << "\n";
    cout << "   10. " << COLOR_WHITE << "RECHERCHER PAR TYPE" << ANSI_RESET << "\n";
    cout << "\n  " << COLOR_CYAN << "EMPRUNTS:" << ANSI_RESET << "\n";
    cout << "   11. " << COLOR_WHITE << "EMPRUNTER UN MEDIA" << ANSI_RESET << "\n";
    cout << "   12. " << COLOR_WHITE << "RETOURNER UN MEDIA" << ANSI_RESET << "\n";
    cout << "\n  " << COLOR_CYAN << "STATISTIQUES:" << ANSI_RESET << "\n";
    cout << "   13. " << COLOR_WHITE << "AFFICHER LES STATISTIQUES" << ANSI_RESET << "\n";
    cout << "\n  " << COLOR_CYAN << "LECTURE:" << ANSI_RESET << "\n";
    cout << "   14. " << COLOR_WHITE << "PREVISUALISER UN MEDIA" << ANSI_RESET << "\n";
    cout << "   15. " << COLOR_WHITE << "CONTROLE DE LECTURE" << ANSI_RESET << "\n";
    cout << "\n  " << COLOR_CYAN << "FICHIERS:" << ANSI_RESET << "\n";
    cout << "   16. " << COLOR_WHITE << "SAUVEGARDER" << ANSI_RESET << "\n";
    cout << "   17. " << COLOR_WHITE << "CHARGER" << ANSI_RESET << "\n";
    cout << "\n  " << COLOR_RED << "QUITTER:" << ANSI_RESET << "\n";
    cout << "    0. " << COLOR_RED << "QUITTER L'APPLICATION" << ANSI_RESET << "\n";
    cout << "========================================\n";
}

void Menu::afficherMenuClient() {
    cout << "\n" << BOLD << BG_CYAN << COLOR_WHITE 
         << " MODE CLIENT " << ANSI_RESET << endl;
    cout << "\n" << BOLD << COLOR_GREEN << "SELECTIONNEZ UNE OPERATION:" << ANSI_RESET << endl;
    cout << "========================================\n";
    cout << "  " << COLOR_CYAN << "RECHERCHE:" << ANSI_RESET << "\n";
    cout << "    1. " << COLOR_WHITE << "RECHERCHER PAR TITRE" << ANSI_RESET << "\n";
    cout << "    2. " << COLOR_WHITE << "RECHERCHER PAR TYPE" << ANSI_RESET << "\n";
    cout << "    3. " << COLOR_WHITE << "AFFICHER TOUS LES MEDIAS" << ANSI_RESET << "\n";
    cout << "\n  " << COLOR_CYAN << "TRI:" << ANSI_RESET << "\n";
    cout << "    4. " << COLOR_WHITE << "TRIER PAR TITRE" << ANSI_RESET << "\n";
    cout << "    5. " << COLOR_WHITE << "TRIER PAR ANNEE" << ANSI_RESET << "\n";
    cout << "\n  " << COLOR_CYAN << "EMPRUNTS:" << ANSI_RESET << "\n";
    cout << "    6. " << COLOR_WHITE << "EMPRUNTER UN MEDIA" << ANSI_RESET << "\n";
    cout << "    7. " << COLOR_WHITE << "RETOURNER UN MEDIA" << ANSI_RESET << "\n";
    cout << "\n  " << COLOR_CYAN << "STATISTIQUES:" << ANSI_RESET << "\n";
    cout << "    8. " << COLOR_WHITE << "AFFICHER LES STATISTIQUES" << ANSI_RESET << "\n";
    cout << "\n  " << COLOR_CYAN << "LECTURE:" << ANSI_RESET << "\n";
    cout << "    9. " << COLOR_WHITE << "PREVISUALISER UN MEDIA" << ANSI_RESET << "\n";
    cout << "   10. " << COLOR_WHITE << "CONTROLE DE LECTURE" << ANSI_RESET << "\n";
    cout << "\n  " << COLOR_CYAN << "COMPTE:" << ANSI_RESET << "\n";
    cout << "   11. " << COLOR_WHITE << "CHANGER MOT DE PASSE" << ANSI_RESET << "\n";
    cout << "   12. " << COLOR_WHITE << "SE DECONNECTER" << ANSI_RESET << "\n";
    cout << "\n  " << COLOR_RED << "QUITTER:" << ANSI_RESET << "\n";
    cout << "    0. " << COLOR_RED << "QUITTER L'APPLICATION" << ANSI_RESET << "\n";
    cout << "========================================\n";
}

void Menu::executer() {
    int choix;
    bool continuer = true;
    
    do {
        afficherMenuPrincipal();
        
        // Clear any errors
        cin.clear();
        
        if (!(cin >> choix)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << COLOR_RED << "\n[ERREUR] ENTREE INVALIDE!" << ANSI_RESET << endl;
            pause();
            continue;
        }
        
        // Clear the rest of the line
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        if (!isAuthenticated) {
            switch(choix) {
                case 1: // Client mode
                    isAuthenticated = true;
                    isAdmin = false;
                    currentUsername = "Guest";
                    cout << COLOR_GREEN << "\n[OK] BIENVENUE EN MODE CLIENT!" << ANSI_RESET << endl;
                    sleepMilliseconds(1000);
                    break;
                case 2: // Admin mode
                    if (authentifier()) {
                        isAuthenticated = true;
                        isAdmin = true;
                    }
                    break;
                case 0:
                    cout << "\n" << BOLD << BG_YELLOW << COLOR_WHITE 
                         << "MERCI D'AVOIR UTILISE BIBLIOTECH! AU REVOIR!" << ANSI_RESET << endl;
                    continuer = false;
                    break;
                default:
                    cout << COLOR_RED << "\n[ERREUR] CHOIX INVALIDE!" << ANSI_RESET << endl;
                    pause();
            }
        } else if (isAdmin) {
            switch(choix) {
                case 1: creerUtilisateur(); break;
                case 2: changerPassword(); break;
                case 3: deconnecter(); break;
                case 4: ajouterMedia(); break;
                case 5: supprimerMedia(); break;
                case 6: afficherTous(); break;
                case 7: trierParTitre(); break;
                case 8: trierParAnnee(); break;
                case 9: rechercherParTitre(); break;
                case 10: rechercherParType(); break;
                case 11: emprunterMedia(); break;
                case 12: retournerMedia(); break;
                case 13: afficherStatistiques(); break;
                case 14: previsualiserMedia(); break;
                case 15: controleLecture(); break;
                case 16: sauvegarder(); break;
                case 17: charger(); break;
                case 0:
                    cout << "\n" << BOLD << BG_YELLOW << COLOR_WHITE 
                         << "MERCI D'AVOIR UTILISE BIBLIOTECH! AU REVOIR!" << ANSI_RESET << endl;
                    continuer = false;
                    break;
                default:
                    cout << COLOR_RED << "\n[ERREUR] CHOIX INVALIDE!" << ANSI_RESET << endl;
                    pause();
            }
        } else {
            switch(choix) {
                case 1: rechercherParTitre(); break;
                case 2: rechercherParType(); break;
                case 3: afficherTous(); break;
                case 4: trierParTitre(); break;
                case 5: trierParAnnee(); break;
                case 6: emprunterMedia(); break;
                case 7: retournerMedia(); break;
                case 8: afficherStatistiques(); break;
                case 9: previsualiserMedia(); break;
                case 10: controleLecture(); break;
                case 11: changerPassword(); break;
                case 12: deconnecter(); break;
                case 0:
                    cout << "\n" << BOLD << BG_YELLOW << COLOR_WHITE 
                         << "MERCI D'AVOIR UTILISE BIBLIOTECH! AU REVOIR!" << ANSI_RESET << endl;
                    continuer = false;
                    break;
                default:
                    cout << COLOR_RED << "\n[ERREUR] CHOIX INVALIDE!" << ANSI_RESET << endl;
                    pause();
            }
        }
        
    } while (continuer);
}