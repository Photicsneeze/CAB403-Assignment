#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "leaderboard.h"

int main(void)
{


	Leaderboard *leaderboard;

	leaderboard = create_leaderboard();

	update_score(leaderboard, "Four", 0);
	update_score(leaderboard, "Three", 1);
	update_score(leaderboard, "Two", 0);
	update_score(leaderboard, "Two", 1);
	update_score(leaderboard, "One", 1);
	update_score(leaderboard, "Six", 1);
	update_score(leaderboard, "Six", 1);
	update_score(leaderboard, "Six", 1);
	update_score(leaderboard, "One", 1);
	update_score(leaderboard, "Five", 0);
	update_score(leaderboard, "One", 1);
	update_score(leaderboard, "Zero", 1);
	update_score(leaderboard, "Seven", 1);
	update_score(leaderboard, "Seven", 1);
	update_score(leaderboard, "Seven", 1);
	update_score(leaderboard, "Eight", 1);
	update_score(leaderboard, "Eight", 1);
	update_score(leaderboard, "Eight", 1);
	update_score(leaderboard, "Eight", 1);
	update_score(leaderboard, "Nine", 1);
	update_score(leaderboard, "Nine", 1);
	update_score(leaderboard, "Nine", 1);
	update_score(leaderboard, "Nine", 1);
	update_score(leaderboard, "Nine", 0);
	update_score(leaderboard, "Nine", 0);

	sort_leaderboard(leaderboard);
	//printf("\nDone\n\n");
	print_leaderboard(leaderboard);

	free_leaderboard(leaderboard);

	printf("Program finished successfully.\n");

	exit(EXIT_SUCCESS);

}
