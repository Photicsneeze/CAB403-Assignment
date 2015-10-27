#include "leaderboard.h"

Leaderboard* create_leaderboard()
{
	Leaderboard *leaderboard = malloc(sizeof(Leaderboard));
	if (leaderboard == NULL) {
		printf("Failed to allocate memory for leaderboard.\n");
		exit(EXIT_FAILURE);
	}

	leaderboard->first = NULL;
	leaderboard->last = NULL;
	leaderboard->num_scores = 0;

    return leaderboard;
}

void free_leaderboard(Leaderboard *leaderboard)
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

void sort_leaderboard(Leaderboard *leaderboard)
{
	/* The statistics for the clients should be displayed in ascending order of games won.
    If two or more players have the same number of games won then the player with the
    highest percentage of games won should be displayed last (percentage is the number
    of games won divided by the number of games played). If two or more players have the
    same number of games won and the same percentage of games won then display those
    players in alphabetical order.
    */

	int num_scores = leaderboard->num_scores;
	bool swap;
	for (int i = 0; i < (num_scores - 1); i++) {
		Score *s1 = get_score_at_index(leaderboard,i);
		Score *s2 = get_score_at_index(leaderboard,i+1);
		swap = false;
		if (s1->games_won > s2->games_won) {
			swap = true;
		} else if (s1->games_won == s2->games_won) {
			if ((s1->games_won / s1->games_played) > (s2->games_won / s2->games_played)) {
				swap = true;
			} else if ((s1->games_won / s1->games_played) == (s2->games_won / s2->games_played)) {
				if (!alphabetical_order(s1->username, s2->username)) {
					swap = true;
				}
			}
		}
		if (swap) {
			swap_score(leaderboard,i,i+1);
			i = -1;
		}
	}
}

bool alphabetical_order(char *str1, char *str2)
{
	if (strcasecmp(str1, str2) <= 0) {
		return true;
	} else {
		return false;
	}
}

Score* get_score_at_index(Leaderboard *leaderboard, int fromIndex){
	if(fromIndex<0){
		return NULL;
	}
	Score *moving;
	moving = leaderboard->first;
	if(fromIndex != 0){
		for(int i=0;i<fromIndex;i++){
			moving = moving->next;
		}
	}
	return moving;
}


/* If index is 0 then there is nothing before it and therefore returns null*/
Score* get_score_before_index(Leaderboard *leaderboard, int fromIndex){
	if(fromIndex<=0){
		return NULL;
	}
	Score *moving;
	moving = leaderboard->first;
	if(fromIndex != 0){
		for(int i=1;i<fromIndex;i++){
			moving = moving->next;
		}
	}
	return moving;
}

int move_score(Leaderboard *leaderboard, int fromIndex, int toIndex){
	if(fromIndex == toIndex){
		return -1;
	}
	if((fromIndex < 0)||(toIndex < 0)){
		return -1;
	}
	Score *current = get_score_at_index(leaderboard, fromIndex);


	pop(leaderboard,fromIndex);
	push(leaderboard,current,toIndex);

}

int swap_score(Leaderboard *leaderboard, int fromIndex, int toIndex){
	if(move_score(leaderboard,fromIndex,toIndex)==-1){
		return -1;
	}

	int passed;
	if(fromIndex>toIndex){
		if(toIndex == leaderboard->num_scores-1){
			passed = move_score(leaderboard,toIndex,fromIndex);
		}else{
			passed = move_score(leaderboard,toIndex+1,fromIndex);
		}
	}else{
		if(toIndex == 0){
			passed = move_score(leaderboard,toIndex,fromIndex);
		}else{
			passed = move_score(leaderboard,toIndex-1,fromIndex);
		}
	}
	return passed;
}

void pop(Leaderboard *leaderboard, int index){
	Score *before =  get_score_before_index(leaderboard, index);
	Score *current = get_score_at_index(leaderboard, index);
	if(index == 0){
		/* Pop first */
		leaderboard->first = current->next;
		current->next = NULL;

	}else if(index == (leaderboard->num_scores-1)){
		/* Pop last */
		before->next = NULL;
		leaderboard->last = before;
		current->next = NULL;

	}else{
		/* Normal Pop */
		before->next = current->next;
		current->next = NULL;
	}
	leaderboard->num_scores--;
}

void push(Leaderboard *leaderboard, Score *score, int index){
	/* If pop is done first the list for push will be one less*/
	if(index == (leaderboard->num_scores)){
		/* Push last */
		leaderboard->last->next = score;
		leaderboard->last = score;
		score->next = NULL;
		leaderboard->num_scores++;
		return;
	}
	Score *before =  get_score_before_index(leaderboard, index);
	Score *current = get_score_at_index(leaderboard, index);
	if(index == 0){
		/* Push first */
		score->next = leaderboard->first;
		leaderboard->first = score;

	}else{
		/* Normal Push */
		score->next = current;
		before->next = score;
	}
	leaderboard->num_scores++;
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

	sort_leaderboard(leaderboard);
}

Score* add_user(Leaderboard *leaderboard, char *username)
{
	Score *new_score;
	Score *current_score;
	Score *previous_score;

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

    leaderboard->num_scores++;
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
