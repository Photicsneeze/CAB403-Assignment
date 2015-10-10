#include "leaderboard.h"

Leaderboard* create_leaderboard()
{
	Leaderboard *leaderboard = malloc(sizeof(Leaderboard));
	if (leaderboard == NULL) {
		printf("Failed to allocate memory for leaderboard.\n");
		exit(EXIT_FAILURE);
	}
	
	leaderboard->size = 0;
    
    return leaderboard;
}

void delete_leaderboard(Leaderboard *leaderboard)
{

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
}

Score* add_user(Leaderboard *leaderboard, char *username)
{
	Score *new_score;
	Score current_score;
	Score previous_score;

	new_score = malloc(sizeof(Score));
	if (new_score == NULL) {
		printf("Failed to allocate memory for new leaderboard entry.\n");
		exit(EXIT_FAILURE);
	}

	strcpy(new_score->username, username);
	new_score->games_played = 0;
	new_score->games_won = 0;

	if (leaderboard->first == NULL) {
		leaderboard->first = new_score;
		leaderboard->last = new_score;
	} else {
		leaderboard->last->next = new_score;
		leaderboard->last = new_score;
	}

	leaderboard->size++;
    
    return new_score;

	// previous_score = leaderboard->first;
	// current_score = previous_score->next;
	// while (current_score != NULL) {

	// }
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
