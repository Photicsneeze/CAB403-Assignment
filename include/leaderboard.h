#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

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

void delete_leaderboard(Leaderboard *leaderboard);

void sort_leaderboard(Leaderboard *leaderboard);

void update_score(Leaderboard *leaderboard, char *username, bool win);

struct Score* add_user(Leaderboard *leaderboard, char *username);

struct Score* get_score(Leaderboard *leaderboard, char *username);

bool contains_user(Leaderboard *leaderboard, char *username);

void print_leaderboard(Leaderboard *leaderboard);

void score_to_string(char *str, Score *score);
