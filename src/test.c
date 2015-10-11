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
    //print_leaderboard(leaderboard);
    
	update_score(leaderboard, "Shaun", 0);
    print_leaderboard(leaderboard);
	
    update_score(leaderboard, "Shaun", 1);
    print_leaderboard(leaderboard);
    
    update_score(leaderboard, "Tom", 1);
    print_leaderboard(leaderboard);
    
    update_score(leaderboard, "Lachlan", 1);
    print_leaderboard(leaderboard);
    
    update_score(leaderboard, "Shaun", 1);
    print_leaderboard(leaderboard);
    
    update_score(leaderboard, "Maolin", 1);
	print_leaderboard(leaderboard);

	free_leaderboard(leaderboard);

	printf("Program finished successfully.\n");

	exit(EXIT_SUCCESS);
}
