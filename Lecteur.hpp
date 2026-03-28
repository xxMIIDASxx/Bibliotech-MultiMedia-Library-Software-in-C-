#ifndef LECTEUR_HPP
#define LECTEUR_HPP

#include <memory>
#include <string>

class Media;

class Lecteur {
private:
    std::shared_ptr<Media> mediaActuel;
    bool enLecture;
    int position;
    
public:
    Lecteur();
    
    void chargerMedia(std::shared_ptr<Media> media);
    void jouer();
    void pause();
    void stop();
    void avancer(int secondes);
    void reculer(int secondes);
};

#endif