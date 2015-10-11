#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "leaderboard.h"

int main(void)
{
	Leaderboard *leaderboard;

	leaderboard = create_leaderboard();

	srand(time(NULL));

	update_score(leaderboard, "Maolin", 1);
	//update_score(leaderboard, "Shaun", 0);
	//update_score(leaderboard, "Shaun", 1);

	print_leaderboard(leaderboard);
    
    leaderboard = sort_leaderboard(leaderboard);

    print_leaderboard(leaderboard);

	delete_leaderboard(leaderboard);

	printf("Program finished successfully.\n");

	exit(EXIT_SUCCESS);
}
