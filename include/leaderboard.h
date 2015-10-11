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

Leaderboard* create_leaderboard(void);

void delete_leaderboard(Leaderboard *leaderboard);

Leaderboard* sort_leaderboard(Leaderboard *leaderboard);

void update_score(Leaderboard *leaderboard, char *username, bool win);

void add_score(Leaderboard *leaderboard, Score score);

Score* add_user(Leaderboard *leaderboard, char *username);

Score* get_score(Leaderboard *leaderboard, char *username);

bool contains_user(Leaderboard *leaderboard, char *username);

void print_leaderboard(Leaderboard *leaderboard);

void score_to_string(char *str, Score *score);
