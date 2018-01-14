#include "Morpion.h"

void color_printf(COLOR_TERMINAL font, COLOR_TERMINAL background, const char * format, ...)
{
    int32_t res;
    va_list args;
    printf("\x1B[1;%02d;%02dm", font + 30, background + 40);
    va_start(args, format);
    res = vprintf(format, args);
    va_end(args);
    printf("\x1B[00m");
}

/*
 * Creation d'un plateau de jeu
 */
board *create_board(uint8_t size)
{
    board *board1 = malloc(sizeof(board)); // Allocation de la structure contenant le plateau
    if(board1 == NULL)
    {
        printf("Probleme d'allocation de la structure du plateau.\n");
        exit(EXIT_FAILURE); // Sorti du programme en cas d'erreur
    }

    board1->size = size; // Affectation taille du plateau
    board1->table = malloc(size * sizeof(uint8_t)); // Allocation du plateau vide
    if(board1->table == NULL)
    {
        printf("Problème d'allocation du plateau\n");
        exit(EXIT_FAILURE); // Sorti du programme en cas d'erreur
    }

    return board1;
}

/*
 * Detruit le plateau de jeu
 */
void destroy_board(board *board1)
{
    free(board1->table); // Liberation du plateau
    free(board1); // Liberation de la structure du plateau
}

/*
 * Remet le plateau à 0
 */
void reset_board(board *board1)
{
    uint8_t index;
    for(index = 0; index < board1->size; index++)
    {
        board1->table[index] = EMPTY;
    }
}

/*
 * Obtient la valeur de la case du plateau
 */
uint8_t get_square(board *board1, uint8_t row, uint8_t column)
{
    uint8_t coordinate = (uint8_t) (row * SIZE + column); // Calcule la position dans le tableau
    uint8_t value_square = board1->table[coordinate]; // Recupere la valeur de la case du tableau

    return value_square;
}

/*
 * Place le joueur sur la case du tableau
 */
void set_square(board *board1, uint8_t row, uint8_t column, uint8_t value)
{
    uint8_t coordinate = (uint8_t) (row * SIZE + column); // Calcule la position dans le tableau
    board1->table[coordinate] = value; // Place le joueur dans la case du tableau
}

/*
 * Retourne vrai si l'indice est valide
 */
_Bool index_valid(board *board1, uint8_t index)
{
    if((index >= 0) && (index < board1->size))
        return true;
    else
        return false;
}

/*
 * Retourne vrai si la case est vide
 */
_Bool empty_square(board *board1, uint8_t row, uint8_t column)
{
    if(get_square(board1, row, column) == EMPTY)
        return true;
    else
        return false;
}

/*
 * Creation d'un joueur
 */
player *create_player()
{
    player *player1 = malloc(sizeof(player));
    if(player1 == NULL)
    {
        printf("Erreur lors de la creation du joueur.\n");
        exit(EXIT_FAILURE);
    }

    player1->name = NULL;
    player1->shape = EMPTY;

    return player1;
}

/*
 * Change le nom du joueur
 */
void change_player_name(player *player1, char *name)
{
    player1->name = name;
}

/*
 * Creation d'une partie
 */
game *create_game()
{
    game *game1 = malloc(sizeof(game)); // Creation partie
    if(game1 == NULL)
    {
        printf("Erreur lors de la creation de la partie\n");
        exit(EXIT_FAILURE);
    }

    game1->tray = create_board(SIZE * SIZE); // Creation plateau
    if(game1->tray == NULL)
    {
        printf("Erreur lors de la creation du plateau\n");
        exit(EXIT_FAILURE);
    }

    game1->player1 = create_player();
    game1->player2 = create_player();

    return game1;
}


/*
 * Destruit la partie
 */
void destroy_game(game *game1)
{
    if(game1 != NULL)
    {
        destroy_board(game1->tray); // Destruction du plateau
        free(game1->player1);
        free(game1->player2);
        free(game1); // Detrustion du jeu
    }
}

/*
 * Choisi au hasard le joueur
 */
void player_random(game *game1)
{
    if(rand()%2 == 1)
    {
        game1->current_player = game1->player1;
    }
    else
    {
        game1->current_player = game1->player2;
    }
}

/*
 * Change le tour du joueur courant
 */
void change_current_player(game *game1)
{
    if(game1->current_player == game1->player1)
    { // Si c'est le tour du joueur 1
        game1->current_player = game1->player2; // On passe au joueur 2
    }
    else
    { // Si c'est le tour du joueur 2
        game1->current_player = game1->player1; // On passe au joueur 1
    }
}

/*
 * Retourne qui a l'etat la partie (DRAW, EMPTY, ROUND or CROSS)
 */
uint8_t state_game(board *board1)
{
    uint8_t row = 0,
            column = 0,
            result_round = 0,
            result_cross = 0,
            result;

    // Recherche diagonale gauche
    result = search_diagonal_left(board1, row, column, result_round, result_cross);

    if(result != EMPTY)
    { // Si le resultat est rond ou croix
        return result; // On retourne le resultat
    }

    // Recherche diagonale droite
    result = search_diagonal_right(board1, row, column, result_round, result_cross);

    if(result != EMPTY)
    { // Si le resultat est rond ou croix
        return result; // On retourne le resultat
    }

    // Recherche par ligne
    result = search_row(board1, row, column, result_round, result_cross);

    if(result != EMPTY)
    { // Si le resultat est rond ou croix
        return result; // On retourne le resultat
    }

    // Recherche par colonne
    return search_column(board1, row, column, result_round, result_cross);

}

/*
 * Recherche si on a la diagonale gauche gagnante
 */
uint8_t search_diagonal_left(board *board1, uint8_t row, uint8_t column, uint8_t result_round, uint8_t result_cross)
{
    for(row = 0; row < SIZE; row++)
    {
        if(get_square(board1, row, column) == CROSS)
        {
            result_cross += 1;
        }
        else if(get_square(board1, row, column) == ROUND)
        {
            result_round += 1;
        }

        column += 1;
    }

    if(result_cross == SIZE)
    {
        return CROSS;
    }
    else if(result_round == SIZE)
    {
        return ROUND;
    }
    else
    {
        return EMPTY;
    }
}

/*
 * Recherche si on a la diagonale droite gagnante
 */
uint8_t search_diagonal_right(board *board1, uint8_t row, uint8_t column, uint8_t result_round, uint8_t result_cross)
{
    column = (uint8_t) (SIZE - 1);

    for(row = 0; row < SIZE; row++)
    {
        if(get_square(board1, row, column) == CROSS)
        {
            result_cross += 1;
        }
        else if(get_square(board1, row, column) == ROUND)
        {
            result_round += 1;
        }

        column -= 1;
    }

    if(result_cross == SIZE)
    {
        return CROSS;
    }
    else if(result_round == SIZE)
    {
        return ROUND;
    }
    else
    {
        return EMPTY;
    }
}

/*
 * Recherche si on a une ligne gagnante
 */
uint8_t search_row(board *board1, uint8_t row, uint8_t column, uint8_t result_round, uint8_t result_cross)
{
    for(row = 0; row < SIZE; row++)
    {
        for(column = 0; column < SIZE; column++)
        {
            if(get_square(board1, row, column) == CROSS)
            {
                result_cross += 1;
            }
            else if(get_square(board1, row, column) == ROUND)
            {
                result_round += 1;
            }
        }

        if(result_cross == SIZE)
        {
            return CROSS;
        }
        else if(result_round == SIZE)
        {
            return ROUND;
        }

        result_cross = 0;
        result_round = 0;
    }

    return EMPTY;
}

/*
 * Recherche si on a une colonne gagnante
 */
uint8_t search_column(board *board1, uint8_t row, uint8_t column, uint8_t result_round, uint8_t result_cross)
{
    uint8_t result_empty = 0;

    for(row = 0; row < SIZE; row++)
    {
        for(column = 0; column < SIZE; column++)
        {
            if(get_square(board1, column, row) == CROSS)
            {
                result_cross += 1;
            }
            else if(get_square(board1, column, row) == ROUND)
            {
                result_round += 1;
            }
            else
            {
                result_empty += 1;
            }
        }

        if(result_cross == SIZE)
        {
            return CROSS;
        }
        else if(result_round == SIZE)
        {
            return ROUND;
        }

        result_cross = 0;
        result_round = 0;
    }

    if(result_empty > 0)
    {
        return EMPTY;
    }
    else
    {
        return DRAW;
    }
}

/*
 * Recupere la saisie utilisateur
 */
void capture_player(uint8_t *choice)
{
    printf("\n\tVotre choix : ");
    scanf("%hhu", &(*choice)); // Recupere le choix de l'utilisateur
}

/*
 * Tour du joueur courant
 */
void turn_current_player(game *game1)
{
    uint8_t row, column;

    if(game1->current_player->shape == ROUND)
        printf("\n\tJoueur Courant : ROND\n");
    else
        printf("\n\tJoueur Courant : CROIX\n");

    do{ // Verifie si la case est vide
        do
        { // Recupere le numero de ligne
            printf("\n\tChoix de la ligne :");
            capture_player(&row);
        }while(index_valid(game1->tray, row) == false);


        do
        { // Recupere le numero de colonne
            printf("\n\tChoix de la colonne :");
            capture_player(&column);
        }while(index_valid(game1->tray, column) == false);
    }while(empty_square(game1->tray, row, column) == false);

    set_square(game1->tray, row, column, game1->current_player->shape); // Place le joueur dans une case
}

/*
 * Partie en cours
 */
void play(game *game1, hash_table *hash_table1, stack *stack1)
{
    uint8_t state;

    do
    { // Tant que le jeu n'est pas termine. Il faut que les cases ne soient pas vides.
        if(game1->current_player->shape == game1->player2->shape)
        { // Si c'est au tour de l'utilisateur
            print_board(game1->tray); // Affichage du plateau
            turn_current_player(game1); // Tour du joueur courant
            change_current_player(game1); // Changement de joueur
            play_ai(hash_table1, stack1, game1->tray, game1->current_player->shape); // Tour de l'intelligence artificielle
        }
        else
        { // Si c'est au tour de l'ordinateur
            play_ai(hash_table1, stack1, game1->tray, game1->current_player->shape); // Tour de l'intelligence artificielle
            change_current_player(game1); // Changement de joueur
            print_board(game1->tray); // Affichage du plateau
            turn_current_player(game1); // Tour du joueur courant
        }

        change_current_player(game1); // Changement de joueur
        state = state_game(game1->tray); // Etat du jeu
        printf("\e[1;1H\e[2J"); // Nettoyage ecran
    }while(state == EMPTY);

    switch (state)
    { // Resultat du jeu
        case ROUND : // Rond gagne
            if(game1->player1->shape == ROUND)
            { // Si l'intelligence artificielle gagne
                printf("\n\tGagnant : %s", game1->player1->name);
                result_game(hash_table1, stack1, WIN);
            }
            else
            { // Si l'intelligence artificielle perd
                printf("\n\tGagnant : %s", game1->player2->name);
                result_game(hash_table1, stack1, LOSE);
            }
            break;
        case CROSS : // Croix gagne
            if(game1->player1->shape == CROSS)
            { // Si l'intelligence artificielle gagne
                printf("\n\tGagnant : %s", game1->player1->name);
                result_game(hash_table1, stack1, WIN);
            }
            else
            { // Si l'intelligence artificielle perd
                printf("\n\tGagnant : %s", game1->player2->name);
                result_game(hash_table1, stack1, LOSE);
            }
            break;
        case DRAW :
            printf("\n\tEgalite");
            result_game(hash_table1, stack1, DRAW);
            break;
        default :
            break;
    }

    print_board(game1->tray);
}

void auto_play_ai(game *game1, hash_table *hash_table1, stack *stack1, stack *stack2)
{
    uint8_t state; // Etat du jeu

    printf("Begin\n");
    do
    {
        if(game1->current_player->shape == game1->player1->shape)
        { // Jeu actuel : intelligence artificielle ROND
            play_ai(hash_table1, stack1, game1->tray, game1->current_player->shape);
        }
        else
        { // Jeu actuel : intelligence artificielle CROIX
            play_ai(hash_table1, stack2, game1->tray, game1->current_player->shape);
        }
        change_current_player(game1); // Changement de joueur
        state = state_game(game1->tray); // Etat du jeu
    }while(state == EMPTY); // On continue si la partie n'a pas trouve de gagnant ou d'egalite

    printf("End\n");
    print_board(game1->tray);

    switch (state)
    { // Resultat du jeu
        case ROUND : // Rond gagne
            result_game(hash_table1, stack1, WIN); // stack1 = ROND
            result_game(hash_table1, stack2, LOSE); // stack2 = CROIX
            break;
        case CROSS : // Croix gagne
            result_game(hash_table1, stack2, WIN);
            result_game(hash_table1, stack1, LOSE);
            break;
        case DRAW : // Egalite
            result_game(hash_table1, stack1, DRAW);
            result_game(hash_table1, stack2, DRAW);
            break;
        default :
            break;
    }
}

/*
 * Affichage du plateau
 */
void print_board(board *board1)
{
    uint8_t row, column, shape;

    printf("\n\n\t");
    for(column = 0; column < SIZE; column++)
    {
        printf("\t%hhu\t", column);
    }

    printf("\n");
    for(row = 0; row < SIZE; row++)
    {
        printf("%hhu\t", row);
        for(column = 0; column < SIZE; column++)
        {
            shape = get_square(board1, row, column);

            switch(shape)
            {
                case CROSS :
                    color_printf(BLACK, WHITE,"\tX\t");
                    break;
                case ROUND :
                    color_printf(BLACK, YELLOW,"\tO\t");
                    break;
                case EMPTY :
                    color_printf(CYAN, CYAN,"\t \t");
                    break;
                default :
                    break;
            }
        }
        printf("\n");
    }
}

/*
 * Menu
 */
void menu(game *game1)
{
    uint8_t choice;
    uint32_t number, step; // Servent pour jouer CPU contre CPU
    hash_table *hash_table1 = NULL; // Intelligence artificielle inexistant
    stack *stack1 = new_stack(); // Pile de la partie

    do
    {
        printf("\n\t\t\t*** Menu Principal ***\n\n"
                       "\tSaisir :\n\n"
                       "\t\t* 0 : Quitter le jeu\n"
                       "\t\t* 1 : Commencer une nouvelle partie avec le CPU\n"
                       "\t\t* 2 : Generer un certain nombre de partie avec le CPU\n"
                       "\t\t* 3 : Sauvegarder les fichiers\n"
                       "\t\t* 4 : Charger les fichiers\n");

        capture_player(&choice); // Choix de l'utilisateur

        switch (choice)
        { // Choix du joeur
            case CREATE : // Creation d'une partie
                if(game1 == NULL)
                { // Si le jeu n'existe pas
                    game1 = create_game(); // Creation du jeu
                    change_player_name(game1->player1, CPU); // Change le nom du joueur
                    game1->player1->shape = ROUND;
                    game1->player2->shape = CROSS;
                }
                else
                { // Si le jeu existe, inutile de recreer par dessus un autre jeu
                    reset_board(game1->tray); // Remet seulement le plateau à 0
                }

                change_player_name(game1->player2, PLAYER); // Change le nom du joueur
                player_random(game1); // Choisit au hasard le joueur

                if(hash_table1 == NULL)
                { // Si l'intelligence artificielle n'existe pas
                    hash_table1 = create_ai(); // Creation de l'intelligence artificielle
                }

                printf("\e[1;1H\e[2J"); // Nettoyage ecran
                play(game1, hash_table1, stack1); // Jouer

                break;
            case CPU_ONLY : // Joue seulement contre le CPU
                if(game1 == NULL)
                { // Si le jeu n'existe pas, on cree le jeu
                    game1 = create_game(); // Creation du jeu
                    change_player_name(game1->player1, CPU); // Change le nom du premier joueur
                    game1->player1->shape = ROUND;
                    game1->player2->shape = CROSS;
                }
                else
                { // Si le jeu existe deja
                    reset_board(game1->tray); // On remet seulement le plateau de jeu a 0
                }

                stack *stack2 = new_stack(); // Pile de la partie CPU contre CPU

                change_player_name(game1->player2, CPU); // Change le nom du deuxieme joueur

                if(hash_table1 == NULL)
                { // Si l'intelligence artificielle n'existe pas
                    hash_table1 = create_ai(); // Creation de l'intelligence artificielle
                }

                printf("Nombre de fois que l'ordinateur doit jouer contre lui-meme : ");
                scanf("%u", &number); // Recupere le nombre de fois que l'ordinateur doit joueur

                for(step = 0; step < number; step++)
                { // Pour chaque nombre de fois que l'intelligence artficielle joue
                    player_random(game1); // Choisit au hasard le joueur
                    auto_play_ai(game1, hash_table1, stack1, stack2);
                    reset_board(game1->tray); // On remet seulement le plateau de jeu a 0
                }

                destroy_stack(stack2); // Destruction du stack 2
                break;
            case SAVE :
                save_file(game1, hash_table1, stack1);
                break; 
            default : // Autre
                break;
        }
    }while(choice != QUIT); // Tant que l'on ne quitte pas

    if(hash_table1 != NULL)
    {
        clear_hash_table(hash_table1); // Destruction de l'intelligence artificielle
    }
    destroy_stack(stack1); // Destruction de la pile de la partie
    destroy_game(game1); // Detruit la partie
}

/*
 * Allouer un maillon
 */
chain *new_chain(uint8_t size)
{
    chain *chain1 = malloc(sizeof(chain)); // Allouer une place pour un maillon
    if(chain1 == NULL)
    { // Erreur lors de l'allocation d'un maillon
        printf("Erreur : Allocation impossible d'un maillon.\n");
        exit(EXIT_FAILURE); // Sorti du programme
    }

    // Tableau qui contiendra toutes les rotations de la configuration
    chain1->table_configuration = malloc(size * sizeof(uint32_t));
    if(chain1->table_configuration == NULL)
    { // Erreur lors de l'allocation d'un tableau de configuration
        printf("Erreur : Allocation impossible pour le tableau de configuration.\n");
        exit(EXIT_FAILURE);
    }

    chain1->table_ball = malloc(size * sizeof(board *));
    if(chain1->table_ball == NULL)
    { // Erreur lors de l'allocation d'un tableau de billes
        printf("Erreur : Allocation impossible pour le tableau de billes.\n");
        exit(EXIT_FAILURE); // Sorti du programme
    }

    chain1->size = size; // Nombre de rotation
    chain1->state = EMPTY; // Aucune

    return chain1; // Retour de la chaine
}

/*
 * Allouer une nouvelle liste
 */
list *new_list()
{
    list *list1 = malloc(sizeof(list)); // Allouer une place pour une liste
    if(list1 == NULL)
    { // Erreur d'allocation pour une liste
        printf("Erreur : Probleme d'allocation de la liste.\n");
        exit(EXIT_FAILURE); // Sorti d'un programme
    }

    list1->size = 0; // Taille initiale à 0
    list1->head = NULL; // Rien en tete de liste
    list1->tail = NULL; // Rien en queue de liste
    return list1; // Retour d'une liste
}

/*
 * Fonction qui test si la liste est vide
 */
_Bool is_empty(list *list1)
{
    return list1->size == 0; // Retourne vrai si la taille de la liste est nulle
}

/*
 * Ajouter un maillon en tete de liste
 */
void add_head(list *list1, chain *chain1)
{
    chain1->next = list1->head; // Le nouveau maillon qui suit est celui de la tete de liste
    list1->head = chain1; // La tete de liste est le nouveau maillon
    if(list1->size == 0)
    { // Si la liste est vide
        list1->tail = chain1; // La queue de liste est identique a la tete de liste
    }
    list1->size += 1; // Ajoute 1 àa la taille de la liste
}

/*
 * Supprimer la tete de liste
 */
void remove_head(list *list1)
{
    uint8_t index;
    chain *head_chain = list1->head; // Pointeur pointe sur le maillon de la tete de liste

    list1->head = list1->head->next; // La nouvelle tete de liste sera le maillon suivant de la tete de liste

    for(index = 0; index < head_chain->size; index++)
    { // Parcours chaque indice du tableau de billes
        destroy_board(head_chain->table_ball[index]); // Detruit le plateau de billes
    }
    free(head_chain->table_ball); // Liberer la memoire du tableau de billes
    free(head_chain->table_configuration); // Liberer la memoire de la grille de morpion
    free(head_chain); // Liberer le maillon

    list1->size -= 1; // Taille de la liste moins 1

    if(list1->size == 0)
    { // Si taille est nulle
        list1->tail = NULL; // La queue de liste est mise a nulle
    }
}

/*
 * Trouver un maillon a une position particuliere
 */
chain *find_chain(list *list1, uint32_t position)
{
    chain *chain1 = list1->head; // Recuperer le maillon de la tete de liste
    uint32_t index; // Indice du maillon

    if(position >= list1->size)
    { // Erreur, on demande une position en dehors de la liste
        return NULL;
    }

    for(index = 0; index < position; index++)
    { // Parcours jusqu'a la position du maillon
        chain1 = chain1->next; // Recuperation du maillon
    }
    return chain1; // Retourne le maillon
}

/*
 * Nettoyer la liste
 */
void clear_list(list *list1)
{
    while(is_empty(list1) == false)
    { // Tant que la liste est remplie
        remove_head(list1); // Retirer en tete
    }
}

/*
 * Creation d'un nouveau maillon de pile
 */
chain_stack *new_chain_stack(uint8_t index_hash_table, uint32_t index_list, uint8_t index_table_ball, uint8_t ball)
{
    chain_stack *chain_stack1 = malloc(sizeof(chain_stack)); // Reserver une place pour le maillon de pile
    if(chain_stack1 == NULL)
    { // Erreur allocation d'un maillon de la pile
        printf("Probleme d'allocation d'un maillon de la pile.\n");
        exit(EXIT_FAILURE);
    }

    chain_stack1->current.index_hash_table = index_hash_table;
    chain_stack1->current.index_list = index_list;
    chain_stack1->current.index_table_ball = index_table_ball;
    chain_stack1->current.ball = ball;

    return chain_stack1;
}

stack *new_stack()
{
    stack *stack1 = malloc(sizeof(stack));
    if(stack1 == NULL)
    {
        printf("Erreur lors de la creation de la pile de jeu.\n");
        exit(EXIT_FAILURE);
    }

    stack1->head = NULL;
    stack1->size = 0;

    return stack1;
}

/*
 * Mettre un maillon dans la pile
 */
void push(stack *stack1, uint8_t index_hash_table, uint32_t index_list, uint8_t index_table_ball, uint8_t ball)
{
    chain_stack *chain1 = new_chain_stack(index_hash_table, index_list, index_table_ball, ball); // Nouveau maillon
    chain1->next = stack1->head; // Le suivant du nouveau maillon est la tete de pile
    stack1->head = chain1; // La nouvelle tete de liste est le nouveau maillon
    stack1->size += 1; // Taille de la pile augmente de 1
}

/*
 * Retirer un maillon dans la pile
 */
current_play pop(stack *stack1)
{
    chain_stack *chain1 = stack1->head; // Recuperer le maillon de la tete de pile
    current_play current_play1 = chain1->current; // Recuperer la donnee du maillon

    stack1->head = stack1->head->next; // La nouvelle tete de la pile est le maillon suivant
    free(chain1); // Liberer le maillon
    stack1->size -= 1; // Diminuer la pile de 1

    return current_play1; // Retourner la donnee
}

/*
 * Libere la pile
 */
void destroy_stack(stack *stack1)
{
    current_play current;

    while(stack1->size != 0)
    {
        current = pop(stack1);
    }

    free(stack1);
}

/*
 * Creation de la table de hashage
 */
hash_table *new_hash_table()
{
    uint32_t index; // Indice du tableau

    hash_table *hash_table1 = malloc(sizeof(hash_table)); // Reserver une place memoire pour la table de hashage
    if(hash_table1 == NULL)
    { // Erreur lors de l'allocation
        printf("Erreur : Allocation impossible de la table de hashage.\n");
        exit(EXIT_FAILURE); // Sortie du programme
    }

    hash_table1->table = malloc((SIZE*SIZE+1) * sizeof(list*)); // Reserver une place pour le tableau contenant des listes
    if(hash_table1->table == NULL)
    { // Probleme lors de l'allocation
        printf("Erreur : Allocation impossible du tableau de liste.\n");
        exit(EXIT_FAILURE); // Sortie du programme
    }

    for(index = 0; index < (SIZE*SIZE+1); index++)
    { // Parcours du tableau de la table de hashage
        hash_table1->table[index] = new_list(); // Nouvelle liste
    }

    hash_table1->size = (SIZE*SIZE+1); // Taille de la table de hashage
    return hash_table1; // Retourne la table de hashage
}

/*
 * Nettoyage de la table de hashage
 */
void clear_hash_table(hash_table *hash_table1)
{
    uint32_t index; // Indice du tableau

    for(index = 0; index < hash_table1->size; index++)
    { // Parcours du tableau de la table de hashage
        clear_list(hash_table1->table[index]); // Nettoyage de la liste
    }

    free(hash_table1->table); // Liberer le tableau de la table de hashage
    free(hash_table1); // Liberer la table de hashage
}

/*
 * Fonction qui convertie le plateau en base 10
 */
uint32_t convert_board(board *board1)
{
    uint8_t index; // Indice de la grille de morpion
    uint8_t power = (uint8_t) (board1->size);
    uint32_t configuration = 0; // Configuration du plateau

    for(index = 0; index < board1->size; index++)
    { // Calcul de la configuration en passant de la base 3 a la base 10
        configuration = (uint32_t) (board1->table[index] * pow(3, power) + configuration);
        power -= 1;
    }

    return configuration; // Retourne la configuration
}

/*
 * Fonction qui convertie le plateau en base 3
 */
void convert_base_three(board *board1, uint32_t number)
{
    int8_t index;

    for(index = (int8_t) (board1->size - 1); index >= 0; index--)
    {
        board1->table[index] = (uint8_t) (number%3);
        number = (uint32_t) number/3;
    }
}

/*
 * On ajoute la donnee dans la liste indiquee par la fonction de hachage
 */
void add_head_hash_table(hash_table *hash_table1, uint8_t number_ball_remained, chain *chain1)
{
    add_head(hash_table1->table[number_ball_remained], chain1); // Place le maillon en tete de liste
}

/*
 * Recherche d'un maillon dans la table de hashage
 */
chain *search_chain(hash_table *hash_table1, uint8_t number_ball_remained, uint32_t configuration, uint32_t *index_list)
{
    uint8_t index;
    chain *chain1 = hash_table1->table[number_ball_remained]->head; // Recupere le maillon de tete

    while(chain1 != NULL)
    { // Tant que le maillon n'est pas nulle
        for(index = 0; index < chain1->size; index++)
        { // Parcours de la table de configuration
            if(chain1->table_configuration[index] == configuration)
            { // Si on trouve la configuration, on retourne la chaine
                return chain1;
            }
        }
        chain1 = chain1->next; // Maillon suivant
        (*index_list) += 1;
    }
    return NULL; // Retourne NULL en cas d'aucun maillon trouve
}

/*
 * Nombre de boules ou la case est egale 0
 */
uint8_t number_ball(board *board1)
{
    uint8_t number = 0, row, column;

    for(row = 0; row < SIZE; row++)
    { // Parcours de la grille de morpion
        for(column = 0; column < SIZE; column++)
        {
            if(empty_square(board1, row, column))
            { // Incremente 1 lorsque la case est vide
                number += 1;
            }
        }
    }

    return number; // Retourne le nombre de boules
}

/*
 * Remplit un tableau contenant les billes de couleurs
 */
void fill_ball_board(board *board1, board *board_ball)
{
    uint8_t index, index1 = 0;

    for(index = 0; index < board1->size; index++)
    { // Parcours de la grille de morpion a la recherche de billes
        if(board1->table[index] == EMPTY)
        { // Si la case correspond a une bille de couleur
            board_ball->table[index1] = index; // Recupere l'indice de la bille
            index1 += 1; // Incremente le nombre de bille
        }
    }
}

/*
 * Fonction prennant une boule au hasard
 */
uint8_t random_ball(board *board_ball)
{
    uint8_t index = (uint8_t) rand()%(board_ball->size);
    return board_ball->table[index];
}

/*
 * Rotation de la configuration de 90 degres
 */
void rotation_configuration(board *board1)
{
    uint8_t index_row = 0, index_column, index_column_new = SIZE - 1;
    uint8_t *row = malloc(SIZE * sizeof(uint8_t));

    board *board2 = create_board(board1->size);

    for(index_column = 0; index_column < SIZE; index_column++)
    {
        copy_row(board1, row, index_column, index_row);
        set_square_rotation(board2, row, index_row, index_column_new);
        index_column_new -= 1;
    }

    free(row);

    for(index_row = 0; index_row < board1->size; index_row++)
    {
        board1->table[index_row] = board2->table[index_row];
    }

    destroy_board(board2);
}

/*
 * Symetrie d'une configuration
 */
void symetry_configuration(board *board1)
{
    int8_t column;
    uint8_t row, index = 0;
    uint8_t *table = malloc(board1->size * sizeof(uint8_t));

    for(column = (SIZE - 1); column > -1; column--)
    {
        for(row = 0; row < SIZE; row++)
        {
            table[index] = get_square(board1, (uint8_t) column, row);
            index += 1;
        }
    }

    for(index = 0; index < board1->size; index++)
    {
        board1->table[index] = table[index];
    }

    free(table);
}

/*
 * Copie la ligne du plateau dans un tableau
 */
void copy_row(board *board1, uint8_t *row, uint8_t index_row, uint8_t column)
{
    for(column = 0; column < SIZE; column++)
    {
        row[column] = get_square(board1, index_row, column);
    }
}

/*
 * Prend les valeurs du tableau et les places dans une colonne du plateau
 */
void set_square_rotation(board *board1, uint8_t *row, uint8_t index_row, uint8_t column)
{
    for(index_row = 0; index_row < SIZE; index_row++)
    {
        set_square(board1, index_row, column, row[index_row]);
    }
}

/*
 * Cree une Intelligence artificielle
 */
hash_table *create_ai()
{
    return new_hash_table(); // Retourne une table de hashage
}

/*
 * Jeu de l'Intelligence Artificielle
 */
void play_ai(hash_table *hash_table1, stack *stack1, board *board1, uint8_t value)
{
    board *board_ball;
    uint8_t number_ball_remained = number_ball(board1),
            index = 0,
            index_table_ball = 0,
            ball;
    uint32_t configuration = convert_board(board1),
            index_list = 0;

    // Cherche le maillon correspondant a la configuration
    chain *chain1 = search_chain(hash_table1, number_ball_remained, configuration, &index_list);

    if(chain1 != NULL)
    { // Si le maillon existe
        for (index = 0; index < chain1->size; index++)
        { // On parcourt le tableau de configuration qui contient toutes les rotations
            if (chain1->table_configuration[index] == configuration)
            { // Si on atteind l'indice qui contient la configuration
                break; // On sort de la boucle for
            }
        }

        if (chain1->table_ball[index]->size == 0)
        {
            chain1->table_ball[index]->size = number_ball_remained;
            fill_ball_board(board1, chain1->table_ball[index]); // Rempli le tableau de billes
            ball = random_ball(chain1->table_ball[index]); // Prend au hasard une bille de couleur
            // Place en tete de pile la configuration et la bille utilise
            push(stack1, number_ball_remained, index_list, index_table_ball, ball);
            board1->table[ball] = value; // Place le pion sur la grille de morpion
            return;
        }
        index_table_ball = index;
        ball = random_ball(chain1->table_ball[index]); // Prend au hasard une bille de couleur
    }

    if (chain1 == NULL)
    { // Si le maillon n'existe pas
        if(number_ball_remained == board1->size)
        {
            ball = random_ball(board1);
            board1->table[ball] = value; // Place le pion sur la grille de morpion
            return;
        }

        chain1 = new_chain(ROTATION); // On cree un nouveau maillon

        for (index = 0; index < ROTATION; index++)
        {
            board_ball = create_board(number_ball_remained); // Creation d'un tableau contenant les billes
            fill_ball_board(board1, board_ball); // Rempli le tableau de billes
            chain1->table_ball[index] = board_ball; // On affecte le tableau de billes dans le tableau
            chain1->table_configuration[index] = configuration; // On affecte la configuration dans le tableau
            rotation_configuration(board1); // On tourne a 90 degres la configuration
            configuration = convert_board(board1); // On convertit le plateau en base 3
            if(chain1->table_configuration[index] == configuration)
            {
                break;
            }
        }

        fill_ball_board(board1, board_ball); // Rempli le tableau de billes

        // Ajoute en tete de liste dans la table de hashage
        add_head_hash_table(hash_table1, number_ball_remained, chain1);
        ball = random_ball(board_ball); // Prend au hasard une bille de couleur
    }

    // Place en tete de pile la configuration et la bille utilise
    push(stack1, number_ball_remained, index_list, index_table_ball, ball);
    board1->table[ball] = value; // Place le pion sur la grille de morpion

}

/*
 * Supprimer un element d'un tableau
 */
void delete_element(board *board_ball, uint8_t ball)
{
    if(board_ball->size != 0 )
    { // On retire que lorsque c'est possible de retirer
        uint8_t counter = 0, index1 = 0, index2 = 0;
        uint8_t *table = malloc((board_ball->size-1) * sizeof(uint8_t)); // Alloue un tableau d'un element en moins
        if(table == NULL)
        { // Erreur allocation
            printf("Erreur : impossible d'allouer une grille plus petite.\n");
            exit(EXIT_FAILURE);
        }

        while(index1 < board_ball->size-1)
        { // Parcours du nouveau tableau
            if(counter == 0 && board_ball->table[index2] == ball)
            { // Si on trouve l'element une fois
                counter += 1; // On incremente le compteur pour passer a l'element suivant
                index2 += 1; // Passe a l'element suivant
            }

            table[index1] = board_ball->table[index2]; // Recopie dans le nouveau tableau
            index2 += 1; // Incremente l'indice de l'ancien tableau
            index1 += 1; // Incremente l'indice du nouveau tableau
        }

        free(board_ball->table); // Liberer memoire de l'ancien tableau

        board_ball->table = table;
        board_ball->size -= 1;
    }
}

/*
 * Ajouter un element dans un tableau
 */
void add_element(board *board_ball, uint8_t ball, uint8_t number)
{
    if(board_ball->size < 255)
    {
        uint8_t index;
        uint8_t *table = malloc((board_ball->size + number) * sizeof(uint8_t));
        if (table == NULL) { // Erreur allocation
            printf("Erreur : impossible d'allouer une grille plus grande.\n");
            exit(EXIT_FAILURE);
        }

        // Copie le contenu du tableau de billes dans un nouveau tableau plus grand
        memcpy(table, board_ball->table, board_ball->size);
        index = board_ball->size; // Indice maximale de l'ancien tableau
        board_ball->size = board_ball->size + number; // Aggrandissement de la taille

        while (index < board_ball->size) { // Parcours du nouveau tableau
            table[index] = ball; // Ajout en fin des balles
            index += 1;
        }

        board_ball->table = table;
    }
}

/*
 * Resultat de l'Intelligence Artificielle
 */
void result_game(hash_table *hash_table1, stack *stack1, uint8_t result)
{
    uint8_t index;
    current_play current_play1;
    chain *chain1;

    while(stack1->size > 0)
    { // Tant que la taille n'a pas atteint 0
        current_play1 = pop(stack1); // Recupere le maillon en tete de pile
        chain1 = find_chain(hash_table1->table[current_play1.index_hash_table], current_play1.index_list);

        switch (result)
        { // Selon l'IA a gagne ou non
            case LOSE : // Supprime element
                delete_element(chain1->table_ball[current_play1.index_table_ball], current_play1.ball);
                chain1->state = LOSE;
                break;
            case WIN : // Ajoute 3 boules de meme couleur
                add_element(chain1->table_ball[current_play1.index_table_ball], current_play1.ball, 3);
                chain1->state = WIN;
                break;
            case DRAW : // Ajoute 1 boule de meme couleur
                add_element(chain1->table_ball[current_play1.index_table_ball], current_play1.ball, 1);
                chain1->state = DRAW;
                break;
            default :
                break;
        }
    }
}

/*
 * Ecrit dans un fichier un plateau
 */
void write_board(FILE *file1, board *board1)
{
    uint8_t index;
    fprintf(file1, "Plateau :\nTaille : %hhu\n", board1->size);
    for(index = 0; index < board1->size; index++)
    { // Parcours le tableau
        fprintf(file1, "[%hhu] ", board1->table[index]);
    }
    fprintf(file1, "\n");
}

/*
 * Lit dans un fichier un plateau
 */
void read_board(FILE *file1, board *board1)
{
    uint8_t index, size;
    fscanf(file1, "Plateau :\nTaille : %hhu\n", &size);
    board1 = create_board(size); // Creation d'un plateau
    for(index = 0; index < size; index++)
    { // Parcours le tableau
        fscanf(file1, "[%hhu] ", &board1->table[index]);
    }
    fscanf(file1, "\n");
}

/*
 * Ecrit dans un fichier une chaine de la liste
 */
void write_chain(FILE *file1, chain *chain1)
{
    uint8_t index;
    fprintf(file1, "Maillon :\nTaille : %hhu\nEtat : %hhu\n", chain1->size, chain1->state);
    for (index = 0; index < chain1->size; index++)
    {
        fprintf(file1, "Configuration : %u\n", chain1->table_configuration[index]);
        write_board(file1, chain1->table_ball[index]);
    }
}

/*
 * Lit dans un fichier une chiane de la liste
 */
void read_chain(FILE *file1, chain *chain1)
{
    uint8_t index, size, state;
    fscanf(file1, "Maillon :\nTaille : %hhu\nEtat : %hhu\n", &size, &state);
    chain1 = new_chain(size);
    chain1->state = state;
    for(index = 0; index < chain1->size; index++)
    {
        fscanf(file1, "Configuration : %u\n", &(chain1->table_configuration[index]));
        read_board(file1, chain1->table_ball[index]);
    }
}

/*
 * Ecrit une pile dans un fichier
 */
void write_stack(FILE *file1, stack *stack1)
{
    uint32_t index;
    chain_stack *chain_stack1 = stack1->head; // On pointe le maillon de tete de pile

    fprintf(file1, "Pile :\nTaille : %u\n", stack1->size);
    while(chain_stack1 != NULL)
    {
        fprintf(file1, "Indice de la table de hashage : %hhu\n", chain_stack1->current.index_hash_table);
        fprintf(file1, "Indice de la liste : %u\n", chain_stack1->current.index_list);
        fprintf(file1, "Indice du tableau de bille : %hhu\n", chain_stack1->current.index_table_ball);
        fprintf(file1, "Bille : %hhu\n", chain_stack1->current.ball);
        chain_stack1 = chain_stack1->next; // On pointe au maillon suivant
    }
}

/*
 * Lit une pile dans un fichier
 */
void read_stack(FILE *file1, stack *stack1)
{
    uint32_t index, size, index_list;
    uint8_t index_hash_table, index_table_ball, ball;
    current_play current_play1;
    stack *stack_temp = new_stack(); // Pile temporaire

    fscanf(file1, "Pile :\nTaille : %u\n", &size);

    for(index = 0; index < size; index++)
    { // Recupere chaque maillon de la pile et push dans la pile
        fscanf(file1, "Indice de la table de hashage : %hhu\n", &index_hash_table);
        fscanf(file1, "Indice de la liste : %u\n", &index_list);
        fscanf(file1, "Indice du tableau de bille : %hhu\n", &index_table_ball);
        fscanf(file1, "Bille : %hhu\n", &ball);
        push(stack_temp, index_hash_table, index_list, index_table_ball, ball);
    } // Lorsque l'on recupere, on recupere le bas de la pile jusqu'en haut

    while(stack_temp->size != 0)
    { // Tant qu'on atteind pas la fin de la pile
        current_play1 = pop(stack_temp); // On recupere le maillon
        // On push le maillon dans la bonne pile
        push(stack1, current_play1.index_hash_table, current_play1.index_list, current_play1.index_table_ball, current_play1.ball);
    } // On remet a l'endroit les elements de la pile

    destroy_stack(stack_temp); // On detruit la pile temporaire
}

/*
 * Ecrit le jeu dans un fichier
 */
void write_game(FILE *file1, game *game1)
{
    fprintf(file1, "Joueur 1 : %s\n", game1->player1->name);
    fprintf(file1, "Forme : %hhu\n", game1->player1->shape);
    fprintf(file1, "Joueur 2 : %s\n", game1->player2->name);
    fprintf(file1, "Forme : %hhu\n", game1->player2->shape);
    fprintf(file1, "Joueur courant : %s\n", game1->current_player->name);
    fprintf(file1, "Forme : %hhu\n", game1->current_player->shape);
    write_board(file1, game1->tray);
}

/*
 * Lit le jeu dans un fichier
 */
void read_game(FILE *file1, game *game1)
{
    fscanf(file1, "Joueur 1 : %s\n", &(*game1->player1->name));
    fscanf(file1, "Forme : %hhu\n", &game1->player1->shape);
    fscanf(file1, "Joueur 2 : %s\n", &(*game1->player2->name));
    fscanf(file1, "Forme : %hhu\n", &game1->player2->shape);
    fscanf(file1, "Joueur courant : %s\n", &(*game1->current_player->name));
    fscanf(file1, "Forme : %hhu\n", &game1->current_player->shape);
    read_board(file1, game1->tray);
}

/*
 * Sauvegarde du fichier
 */
void save_file(game *game2, hash_table *hashtable, stack *stack1)
{
    uint8_t index;
    chain *chain1; // Declare un pointeur de maillon
    FILE *file1 = fopen("game.txt", "wt"); // Ouverture du fichier en ecriture
    if(file1 == NULL)
    { // Si il y a un probleme lors de la creation du fichier
        printf("Erreur lors de l'ouverture du fichier\n");
        exit(EXIT_FAILURE); // Sorti du programme
    }

    write_game(file1, game2);

    for (index = 0 ; index < hashtable->size ; index ++)
    { // Parcours de la table de hashage
        fprintf(file1, "Taille de la liste : %u\n", hashtable->table[index]->size);
        chain1 = hashtable->table[index]->head; // On recupere le premier maillon
        while (chain1 != NULL) // Tant que l'on atteind pas la fin de la liste
        { // Parcours des maillons
            write_chain(file1, chain1); // Ecrit le premier maillon
            chain1 = chain1->next; // On passe au maillon suivant
        }
    }

    write_stack(file1, stack1);

    fclose(file1);
}

void charge_file(game *game2, hash_table *hash_table1, stack *stack1)
{
    uint8_t index, index1;
    chain *chain1; // Declare un pointeur de maillon
    FILE *file1 = fopen("hashage.txt", "rt"); // Ouverture du fichier en lecture
    if(file1 == NULL)
    { // Si le fichier n'existe pas
        printf("Erreur : fichier non existant\n");
        exit(EXIT_FAILURE);
    }

    read_game(file1, game2);

    for(index = 0; index < hash_table1->size; index++)
    { // Parcours de la table de hashage
        fscanf(file1, "Taille de la liste : %u\n", &(hash_table1->table[index]->size));
        for(index1 = 0; index1 < hash_table1->table[index]->size; index1++)
        { // Parcours de la liste
            read_chain(file1, chain1); // Lecture d'une chaine
            add_head(hash_table1->table[index], chain1); // Ajoute une nouvelle chaine
        }
    }

    read_stack(file1, stack1); // Lecture d'une pile

    fclose(file1);
}