/*
 *  CAB403 Assignment - Semester 2 2015
 *
 *  Created by Lachlan Cesca & Shaun Karran
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define LEADERBOARD_LENGTH 3
#define MAX_SAVED_USERS 10
#define USERNAME_BUFFER 25

typedef struct {
    char username[USERNAME_BUFFER];
    int games_played;
    int games_won;
} Score;

typedef struct {
    Score data;
    Score *next;
} List;

/* Could use linked list, might be better for re-sorting*/
Score user_scores[LEADERBOARD_LENGTH];

void get_leaderboard(char *str, int index);

void set_leaderboard(char *user,int win);

int find_user(char *user);

int leaderboard_full(void);

int users_full(void);

int next_user_slot(void);
