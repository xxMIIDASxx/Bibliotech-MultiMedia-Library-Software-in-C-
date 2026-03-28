@echo off
echo ========================================
echo    COMPILATION BIBLIOTECH v2.0
echo ========================================
echo.

echo Compilation en cours...
g++ -std=c++17 -Wall -Wextra -pedantic -o bibliotech.exe ^
    main.cpp ^
    Menu.cpp ^
    Bibliotheque.cpp ^
    Media.cpp ^
    FichierManager.cpp ^
    Lecteur.cpp ^
    Statistiques.cpp ^
    Utilisateur.cpp

if %errorlevel% EQU 0 (
    echo.
    echo [SUCCES] Compilation reussie!
    echo.
    echo ========================================
    echo    LANCEMENT DE BIBLIOTECH
    echo ========================================
    echo.
    bibliotech.exe
) else (
    echo.
    echo [ERREUR] Erreur de compilation!
    echo.
    echo Verification des fichiers...
    echo.
    
    if not exist "main.cpp" echo 1. [ERREUR] main.cpp manquant
    if not exist "Menu.cpp" echo 2. [ERREUR] Menu.cpp manquant
    if not exist "Bibliotheque.cpp" echo 3. [ERREUR] Bibliotheque.cpp manquant
    if not exist "Media.cpp" echo 4. [ERREUR] Media.cpp manquant
    if not exist "FichierManager.cpp" echo 5. [ERREUR] FichierManager.cpp manquant
    if not exist "Lecteur.cpp" echo 6. [ERREUR] Lecteur.cpp manquant
    if not exist "Statistiques.cpp" echo 7. [ERREUR] Statistiques.cpp manquant
    if not exist "Utilisateur.cpp" echo 8. [ERREUR] Utilisateur.cpp manquant
    
    echo.
    echo Solutions:
    echo 1. Verifiez que tous les fichiers .cpp sont dans le meme dossier
    echo 2. Installez MinGW ^(g++^) si non installe
    echo 3. Si erreur avec _getch sur Windows, essayez: g++ -std=c++17 -Wall -Wextra -pedantic -o bibliotech.exe *.cpp -luser32
    echo 4. Si erreur avec termios sur Linux/Mac, verifiez les permissions
    echo.
    pause
)