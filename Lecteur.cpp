#include "Lecteur.hpp"
#include "Media.hpp"
#include <iostream>

using namespace std;

Lecteur::Lecteur() : enLecture(false), position(0) {}

void Lecteur::chargerMedia(shared_ptr<Media> media) {
    mediaActuel = media;
    position = 0;
    enLecture = false;
    cout << "[OK] Media charge: " << media->getTitre() << "\n";
}

void Lecteur::jouer() {
    if (mediaActuel) {
        enLecture = true;
        cout << "[JOUER] Lecture de: " << mediaActuel->getTitre() << "\n";
    } else {
        cout << "[ERREUR] Aucun media charge!\n";
    }
}

void Lecteur::pause() {
    if (enLecture) {
        enLecture = false;
        cout << "[PAUSE] Lecture mise en pause a " << position << "s\n";
    }
}

void Lecteur::stop() {
    enLecture = false;
    position = 0;
    cout << "[STOP] Lecture arretee\n";
}

void Lecteur::avancer(int secondes) {
    position += secondes;
    cout << "[AVANCER] Avance de " << secondes << " secondes\n";
}

void Lecteur::reculer(int secondes) {
    if (position - secondes >= 0) {
        position -= secondes;
    } else {
        position = 0;
    }
    cout << "[RECULER] Recule de " << secondes << " secondes\n";
}