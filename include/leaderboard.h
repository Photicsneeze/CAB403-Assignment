#include <stdlib.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define USERNAME_LENGTH	8

typedef struct Score Score;

struct Score {
	char username[USERNAME_LENGTH];
	int games_played;
	int games_won;
	Score *next;
};

typedef struct {
	Score *first;
	Score *last;
	int size;
} Leaderboard;

Leaderboard* create_leaderboard();

void delete_leaderboard(Leaderboard *leaderboard);

void update_score(Leaderboard *leaderboard, char *username, bool win);

Score* add_user(Leaderboard *leaderboard, char *username);

Score* get_score(Leaderboard *leaderboard, char *username);

bool contains_user(Leaderboard *leaderboard, char *username);
