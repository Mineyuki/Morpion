#ifndef MORPION_H
#define MORPION_H
#define SIZE 3
#define ROTATION 4
#define CPU "CPU"
#define PLAYER "PLAYER"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <math.h>

/*
 * Structure du plateau
 */

typedef struct
{
    uint8_t *table; // Stocke le plateau de jeu
    uint8_t size; //  Stocke la taille du plateau
}board;


typedef struct
{
    char *name;
    uint8_t shape;
}player;

/*
 * Structure d'une partie
 */
typedef struct
{
    player *player1;
    player *player2;
    player *current_player; // Stocke le joueur courant
    board *tray; // Stocke le plateau
}game;

/*
 * Maillon classique de liste
 */
typedef struct _chain
{
    uint32_t *table_configuration; // Tableau de configuration
    board **table_ball; // Tableau de billes
    uint8_t state; // Tableau de d'etat
    uint8_t size; // Taille des tableaux
    struct _chain *next; // Maillon suivant
}chain;

/*
 * On déclare avant tout une structure classique de liste
 */
typedef struct
{
    uint32_t size;
    chain *head;
    chain *tail;
}list;

/*
 * Puis un tableau de listes
 */
typedef struct
{
    list **table;
    uint32_t size;
}hash_table;

typedef struct
{
    uint8_t index_hash_table;
    uint32_t index_list;
    uint8_t index_table_ball;
    uint8_t ball;
}current_play;

typedef struct _chain_stack
{
    current_play current;
    struct _chain_stack *next;
}chain_stack;

typedef struct
{
    uint32_t size;
    chain_stack *head;
}stack;

typedef enum
{ // Differents etat de la partie
    EMPTY, ROUND, CROSS, DRAW, WIN, LOSE
}STATE_GAME;

typedef enum
{ // Differents etats du menu
    QUIT, CREATE, CPU_ONLY, CHARGE, SAVE, RESUME
}STATE_MENU;

typedef enum
{
    BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE
}COLOR_TERMINAL;

void color_printf(COLOR_TERMINAL, COLOR_TERMINAL, const char *, ...);

// Plateau
board *create_board(uint8_t);
void destroy_board(board *);
void reset_board(board *);
uint8_t get_square(board *, uint8_t, uint8_t);
void set_square(board *, uint8_t, uint8_t, uint8_t);
_Bool index_valid(board *, uint8_t);
_Bool empty_square(board *, uint8_t, uint8_t);

// Joueur
player *create_player();
void change_player_name(player *, char *);

// Jeu
game *create_game();
void destroy_game(game *);
void player_random(game *);
void change_current_player(game *);

// Etat du jeu
uint8_t state_game(board *);
uint8_t search_diagonal_left(board *, uint8_t, uint8_t, uint8_t, uint8_t);
uint8_t search_diagonal_right(board *, uint8_t, uint8_t, uint8_t, uint8_t);
uint8_t search_row(board *, uint8_t, uint8_t, uint8_t, uint8_t);
uint8_t search_column(board *, uint8_t, uint8_t, uint8_t, uint8_t);

// Cours de la partie
void capture_player(uint8_t *);
void turn_current_player(game *, uint8_t *);
void play(game *, hash_table *, stack *);
void auto_play_ai(game *, hash_table *, stack *, stack *);

// Systemes
void print_board(board *);
void menu(game *);

// Listes
chain *new_chain(uint8_t);
list *new_list();
_Bool is_empty(list *);
void add_head(list *, chain *);
void remove_head(list *);
chain *find_chain(list *, uint32_t);
void clear_list(list *);

// Piles
chain_stack *new_chain_stack(uint8_t, uint32_t, uint8_t, uint8_t);
stack *new_stack();
void push(stack *, uint8_t, uint32_t, uint8_t, uint8_t);
current_play pop(stack *);
void destroy_stack(stack *);

// Table de hashage
hash_table *new_hash_table();
void clear_hash_table(hash_table *);
uint32_t convert_board(board *);
void convert_base_three(board *, uint32_t);
void add_head_hash_table(hash_table *, uint8_t, chain *);
chain *search_chain(hash_table *, uint8_t, uint32_t, uint32_t *);

// Billes
uint8_t number_ball(board *);
void fill_ball_board(board *, board *);
uint8_t random_ball(board *);

// Rotation et symetrie
void rotation_configuration(board *);
void copy_row(board *, uint8_t *, uint8_t, uint8_t);
void set_square_rotation(board *, uint8_t *, uint8_t, uint8_t);
void symetry_configuration(board *);
uint8_t rotation_ball(uint8_t);

// Intelligence artificielle
hash_table *create_ai();
void play_ai(hash_table *, stack *, board *, uint8_t);
void reset_ball_chain(board *, chain *, uint8_t, uint8_t);
void print_ball(board *);
void delete_element(board *, uint8_t);
void add_element(board *, uint8_t, uint8_t);
void result_game(hash_table *, stack *, uint8_t);

// Fichiers
void write_board(FILE *, board *);
board *read_board(FILE *, board *);
void write_chain(FILE *, chain *);
chain *read_chain(FILE *, chain *);
void write_stack(FILE *, stack *);
void read_stack(FILE *, stack *);
void write_game(FILE *, game *);
void read_game(FILE *, game *);
void save_file(FILE *, game *, hash_table *, stack *);
void charge_file(FILE *,game *, hash_table *, stack *);

#endif