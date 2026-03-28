#include "Utilisateur.hpp"
#include <iostream>

using namespace std;

// ANSI Color Codes
#define ANSI_RESET   "\033[0m"
#define COLOR_GREEN  "\033[32m"
#define COLOR_RED    "\033[31m"

Utilisateur::Utilisateur(const string& user, const string& pass, bool admin)
    : username(user), password(pass), isAdmin(admin) {}

bool Utilisateur::authentifier(const string& pass) const {
    return password == pass;
}

void Utilisateur::changerPassword(const string& ancien, const string& nouveau) {
    if (authentifier(ancien)) {
        password = nouveau;
        cout << COLOR_GREEN << "[OK] Mot de passe change avec succes!" << ANSI_RESET << endl;
    } else {
        cout << COLOR_RED << "[ERREUR] Ancien mot de passe incorrect!" << ANSI_RESET << endl;
    }
}