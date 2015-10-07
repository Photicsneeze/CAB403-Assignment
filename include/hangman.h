#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_WORD_LENGTH 20

typedef struct{
    int guess_count;
    int number_guesses;
    char word1[MAX_WORD_LENGTH];
    char word2[MAX_WORD_LENGTH];
    int len_word1;
    int len_word2;
    char guessed_word1_potion[MAX_WORD_LENGTH];
    char guessed_word2_potion[MAX_WORD_LENGTH];
    char guesses_made[26];
} Game;

void number_of_guesses(Game *game);

void choose_words(Game *game, int number);

int get_number_words_available(void);

void display_game(Game *game, char *str);

void update_guess(Game *game, char guess);

int check_complete(Game *game);