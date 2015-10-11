/*
 *  CAB403 Assignment - Semester 2 2015
 *
 *  Created by Lachlan Cesca & Shaun Karran
 *
 *
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_SIZE		4
#define RESIZE_FACTOR		2
#define RESIZE_THRESHOLD	0.7
#define USERNAME_BUFFER 	10

typedef struct {
    char username[USERNAME_BUFFER];
    int games_played;
    int games_won;
} Score;

typedef struct {
	Score *entries;
	int current_size;
	int num_scores;
} Leaderboard;

/* ---- Public Functions---- */
Leaderboard* create_leaderboard();

void free_leaderboard(Leaderboard *leaderboard);

void update_score(Leaderboard *leaderboard, char *username, bool win);

void print_leaderboard(Leaderboard *leaderboard);

/* ---- Private Functions ---- */
void add_user(Leaderboard *leaderboard, char *username);

void add_score(Leaderboard *leaderboard, Score score);

/* If user found, return index of user. Else return -1. */
int contains_user(Leaderboard *leaderboard, char *username);

void resize_leaderboard(Leaderboard *leaderboard);

void score_to_string(char *str, Score score);
