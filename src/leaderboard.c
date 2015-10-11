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

void sort_leaderboard(Leaderboard *leaderboard)
{
	/* The statistics for the clients should be displayed in ascending order of games won.
    If two or more players have the same number of games won then the player with the
    highest percentage of games won should be displayed last (percentage is the number
    of games won divided by the number of games played). If two or more players have the
    same number of games won and the same percentage of games won then display those
    players in alphabetical order.
    */


}

Score* get_score_at_index(Leaderboard *leaderboard, int atIndex){
	if(atIndex<0){
		return NULL;
	}
	Score *moving;
	moving = leaderboard->first;
	if(atIndex != 0){
		for(int i=0;i<atIndex;i++){
			moving = moving->next;
		}
	}
	return moving;
}


/* If index is 0 then there is nothing before it and therefore returns null*/
Score* get_score_before_index(Leaderboard *leaderboard, int atIndex){
	if(atIndex<=0){
		return NULL;
	}
	Score *moving;
	moving = leaderboard->first;
	if(atIndex != 0){
		for(int i=1;i<atIndex;i++){
			moving = moving->next;
		}
	}
	return moving;
}

int move_score(Leaderboard *leaderboard, int atIndex, int toIndex){
	if(atIndex == toIndex){
		return -1;
	}
	if((atIndex < 0)||(toIndex < 0)){
		return -1;
	}
	Score *current = get_score_at_index(leaderboard, atIndex);


	pop(leaderboard,atIndex);

	if(toIndex<atIndex){
		push(leaderboard,current,toIndex);
	}else{
		push(leaderboard,current,toIndex-1);
	}

}

void pop(Leaderboard *leaderboard, int index){
	Score *before =  get_score_before_index(leaderboard, index);
	Score *current = get_score_at_index(leaderboard, index);
	if(index == 0){
		/* Pop first */
		leaderboard->first = current->next;
		current->next = NULL;

	}else if(index == (leaderboard->size-1)){
		/* Pop last */
		before->next = NULL;
		leaderboard->last = before;
		current->next = NULL;

	}else{
		/* Normal Pop */
		before->next = current->next;
		current->next = NULL;
	}
	leaderboard->size--;
}

void push(Leaderboard *leaderboard, Score *score, int index){
	/* If pop is done first the list for push will be one less*/
	Score *before =  get_score_before_index(leaderboard, index);
	Score *current = get_score_at_index(leaderboard, index);
	if(index == 0){
		/* Push first */
		score->next = leaderboard->first;
		leaderboard->first = score;

	}else if(index == (leaderboard->size-1)){
		/* Push last */
		current->next = score;
		leaderboard->last = score;
		score->next = NULL;
	}else{
		/* Normal Push */
		score->next = current;
		before->next = score;
	}
	leaderboard->size++;
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
