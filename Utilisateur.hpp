#ifndef UTILISATEUR_HPP
#define UTILISATEUR_HPP

#include <string>

class Utilisateur {
private:
    std::string username;
    std::string password;
    bool isAdmin;
    
public:
    Utilisateur(const std::string& user, const std::string& pass, bool admin = false);
    
    bool authentifier(const std::string& pass) const;
    void changerPassword(const std::string& ancien, const std::string& nouveau);
    
    std::string getUsername() const { return username; }
    std::string getPassword() const { return password; }
    bool getIsAdmin() const { return isAdmin; }
};

#endif