#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#define USERNAME_LENGTH	8

typedef struct Score {
	char username[USERNAME_LENGTH];
	int games_played;
	int games_won;
	struct Score *next;
} Score;

typedef struct Leaderboard {
	Score *first;
	Score *last;
	int size;
} Leaderboard;

Leaderboard* create_leaderboard();

int swap_score(Leaderboard *leaderboard, int fromIndex, int toIndex);

void free_leaderboard(Leaderboard *leaderboard);

void sort_leaderboard(Leaderboard *leaderboard);

struct Score* get_score_at_index(Leaderboard *leaderboard, int fromIndex);

bool alphabetical_order(char *str1, char *str2);

void pop(Leaderboard *leaderboard, int index);

int move_score(Leaderboard *leaderboard, int fromIndex, int toIndex);

void push(Leaderboard *leaderboard, Score *score, int index);

void update_score(Leaderboard *leaderboard, char *username, bool win);

struct Score* add_user(Leaderboard *leaderboard, char *username);

struct Score* get_score(Leaderboard *leaderboard, char *username);

bool contains_user(Leaderboard *leaderboard, char *username);

void print_leaderboard(Leaderboard *leaderboard);

void score_to_string(char *str, Score *score);
