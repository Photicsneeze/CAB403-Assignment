/*
 *  CAB403 Assignment - Semester 2 2015
 *
 *  Created by Lachlan Cesca & Shaun Karran
 */

#include "leaderboard.h"

Leaderboard* create_leaderboard()
{
	Leaderboard *leaderboard;

	leaderboard = malloc(sizeof(Leaderboard));
	if (leaderboard == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	leaderboard->entries = malloc(sizeof(Score) * INITIAL_SIZE);
	if (leaderboard->entries == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	leaderboard->current_size = INITIAL_SIZE;
	leaderboard->num_scores = 0;

	return leaderboard;
}

void free_leaderboard(Leaderboard *leaderboard)
{
	free(leaderboard->entries);
	free(leaderboard);
}

void update_score(Leaderboard *leaderboard, char *username, bool win)
{
	int index;

	index = contains_user(leaderboard, username);

	if (index < 0) { /* User not found, add them first. */
		add_user(leaderboard, username);
		index = leaderboard->num_scores - 1;
	}

	leaderboard->entries[index].games_played++;
	leaderboard->entries[index].games_won += win;

	sort_leaderboard(leaderboard);
}

void print_leaderboard(Leaderboard *leaderboard)
{
	char score_string[256];
	int num_scores = leaderboard->num_scores;

	for (int i = 0; i < num_scores; i++) {
		memset(score_string, 0, 256);
		score_to_string(score_string, leaderboard->entries[i]);
		printf("%s", score_string);
	}
	printf("\n");
}

void add_user(Leaderboard *leaderboard, char *username)
{
	Score score;

	strcpy(score.username, username);
	score.games_played = 0;
	score.games_won = 0;

	add_score(leaderboard, score);
}

void add_score(Leaderboard *leaderboard, Score score)
{
	int num_scores = leaderboard->num_scores;
	int current_size = leaderboard->current_size;
	double ratio = (double)num_scores / current_size;

	if (ratio > RESIZE_THRESHOLD) {
		resize_leaderboard(leaderboard);
	}

	leaderboard->entries[leaderboard->num_scores] = score;
	leaderboard->num_scores++;
}

int contains_user(Leaderboard *leaderboard, char *username)
{
	for (int i = 0; i < leaderboard->num_scores; i++) {
		if (strcmp(leaderboard->entries[i].username, username) == 0) {
			return i;
		}
	}

	return -1;
}

void sort_leaderboard(Leaderboard *leaderboard)
{
	/*The statistics for the clients should be displayed in ascending order of games won.
	If two or more players have the same number of games won then the player with the
	highest percentage of games won should be displayed last (percentage is the number
	of games won divided by the number of games played). If two or more players have
	the same number of games won and the same percentage of games won then display those
	players in alphabetical order.
	*/

	int num_scores = leaderboard->num_scores;
	bool swap;

	for (int i = 0; i < (num_scores - 1); i++) {
		Score s1 = leaderboard->entries[i];
		Score s2 = leaderboard->entries[i + 1];
		swap = false;
		
		if (s1.games_won > s2.games_won) {
			swap = true;
		} else if (s1.games_won == s2.games_won) {
			if ((s1.games_won / s1.games_played) > (s2.games_won / s2.games_played)) {
				swap = true;
			} else if ((s1.games_won / s1.games_played) == (s2.games_won / s2.games_played)) {
				if (!alphabetical_order(s1.username, s2.username)) {
					swap = true;
				}
			}
		}

		if (swap) {
			leaderboard->entries[i] = s2;
			leaderboard->entries[i + 1] = s1;
			i = -1; /* Set to -1 so it increments to 0 at the start of next loop. */
		}
	}
}

void resize_leaderboard(Leaderboard *leaderboard)
{
	int new_size = leaderboard->current_size * RESIZE_FACTOR;

	leaderboard->entries = realloc(leaderboard->entries, sizeof(Score) * new_size);
	if (leaderboard->entries == NULL) {
		perror("realloc");
		exit(EXIT_FAILURE);
	}

	leaderboard->current_size = new_size;
}

void score_to_string(char *str, Score score)
{
	char temp[128] = {0};
    
    strcat(str, "\n==================================================\n\n");
    sprintf(temp, "Player  - %s\nNumber of games won  - %d\nNumber of games played  - %d\n",
            score.username, score.games_won, score.games_played);
    strcat(str, temp);
    strcat(str, "\n==================================================\n");
}

bool alphabetical_order(char *str1, char *str2)
{
	if (strcasecmp(str1, str2) <= 0) {
		return true;
	} else {
		return false;
	}
}
