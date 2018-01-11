#include "Morpion.h"

int main()
{
    srand(time(NULL));

    game *game1 = NULL; // Aucune partie
    uint8_t choice; // Choix de l'utilisateur

    printf("\e[1;1H\e[2J"); // Nettoyage ecran
    printf("\n\t\tBonjour et bienvenue au jeu du Morpion IA (Intelligence Artificielle).\n\n"
                   "\t\tL'équipe vous souhaite bon jeu.\n");

    menu(game1); // Acces au menu

    printf("\e[1;1H\e[2J"); // Nettoyage ecran
    printf("\n\t\t\tL'équipe vous souhaite une agréable journée\n");

    return EXIT_SUCCESS;
}