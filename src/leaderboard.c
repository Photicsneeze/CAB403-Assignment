#include "leaderboard.h"

Leaderboard* create_leaderboard(void)
{
	Leaderboard *leaderboard = malloc(sizeof(Leaderboard));
	if (leaderboard == NULL) {
		printf("Failed to allocate memory for leaderboard.\n");
		exit(EXIT_FAILURE);
	}
	
	leaderboard->first = NULL;
	leaderboard->last = NULL;
	leaderboard->size = 0;
    
    return leaderboard;
}

void delete_leaderboard(Leaderboard *leaderboard)
{
	Score *current_score = leaderboard->first;
	Score *previous_score;

	while (current_score != NULL) {
		previous_score = current_score;
		current_score = previous_score->next;

		free(previous_score);
	}

	free(leaderboard);
}

Leaderboard* sort_leaderboard(Leaderboard *leaderboard)
{
	/* The statistics for the clients should be displayed in ascending order of games won.
    If two or more players have the same number of games won then the player with the
    highest percentage of games won should be displayed last (percentage is the number
    of games won divided by the number of games played). If two or more players have the
    same number of games won and the same percentage of games won then display those
    players in alphabetical order.
    */

    /* IDEA - Copy leaderboard into array, sort array, new leaderboard from array, delete old leaderboard.*/
    Leaderboard *new_leaderboard;
    int num_scores = leaderboard->size;
    Score score_array[num_scores];
    Score *curr;

    curr = leaderboard->first;

    for (int i = 0; i < num_scores; i++) {
    	score_array[i] = *curr;
    	curr = curr->next;
    }

    // sort array

    new_leaderboard = create_leaderboard();

    for (int i = 0; i < num_scores; i++) {
    	add_score(new_leaderboard, score_array[i]);
    }

    delete_leaderboard(leaderboard);

    return new_leaderboard;
}

void update_score(Leaderboard *leaderboard, char *username, bool win)
{
	Score *score;

	if (contains_user(leaderboard, username)) {
		score = get_score(leaderboard, username);
	} else {
		score = add_user(leaderboard, username);
	}

	score->games_played++;
	score->games_won += win;
	leaderboard = sort_leaderboard(leaderboard);
}

void add_score(Leaderboard *leaderboard, Score score)
{
	Score *new_score;

	new_score = malloc(sizeof(Score));
	if (new_score == NULL) {
		printf("Failed to allocate memory for new leaderboard entry.\n");
		exit(EXIT_FAILURE);
	}

	memcpy(new_score, &score, sizeof(Score));

	if (leaderboard->first == NULL) { 		/* Leaderboard is empty. */
		leaderboard->first = new_score;
		leaderboard->last = new_score;
		new_score->next = NULL;
	} else {								/* Add new user to start of list */
		new_score->next = leaderboard->first;
		leaderboard->first = new_score;
	}

    leaderboard->size++;
}

Score* add_user(Leaderboard *leaderboard, char *username)
{
	Score *new_score;

	new_score = malloc(sizeof(Score));
	if (new_score == NULL) {
		printf("Failed to allocate memory for new leaderboard entry.\n");
		exit(EXIT_FAILURE);
	}

	strcpy(new_score->username, username);
	new_score->games_played = 0;
	new_score->games_won = 0;

	if (leaderboard->first == NULL) { 		/* Leaderboard is empty. */
		leaderboard->first = new_score;
		leaderboard->last = new_score;
		new_score->next = NULL;
	} else {								/* Add new user to start of list */
		new_score->next = leaderboard->first;
		leaderboard->first = new_score;
	}

    leaderboard->size++;
	return new_score;
}

Score* get_score(Leaderboard *leaderboard, char *username)
{
	Score *current_score = leaderboard->first;

	while (current_score != NULL) {
		if (strcmp(current_score->username, username) == 0) {
			return current_score;
		}

		current_score = current_score->next;
	}

	return NULL;
}

bool contains_user(Leaderboard *leaderboard, char* username)
{
	Score *current_score = leaderboard->first;

	while (current_score != NULL) {
		if (strcmp(current_score->username, username) == 0) {
			return true;
		}

		current_score = current_score->next;
	}

	return false;
}

void print_leaderboard(Leaderboard *leaderboard)
{
	char score_string[256];
	Score *current_score = leaderboard->first;

	while (current_score != NULL) {
		memset(score_string, 0, 256);
		score_to_string(score_string, current_score);
		printf("%s", score_string);

		current_score = current_score->next;
	}

	printf("\n");
}

void score_to_string(char *str, Score *score)
{
	char temp[128] = {0};
    
    strcat(str, "\n==================================================\n\n");
    sprintf(temp, "Player  - %s\nNumber of games won  - %d\nNumber of games played  - %d\n",
            score->username, score->games_won, score->games_played);
    strcat(str, temp);
    strcat(str, "\n==================================================\n");
}
