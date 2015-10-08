/*
 *  CAB403 Assignment - Semester 2 2015
 *
 *  Created by Lachlan Cesca & Shaun Karran
 *
 *
 */

#include "leaderboard.h"

/*
 * Summary: Get/display leaderboard
 */
void get_leaderboard(void){
  for(int i=0;i<LEADERBOARD_LENGTH;i++){
    if(user_scores[i].games_played==0){
      /* First element is empty, then no information in leaderboard*/
      if(i==0){
        printf("=============================================================================\n\n");
        printf("There is no information currently stored in the Leader Board. Try again later\n\n");
        printf("=============================================================================\n\n");
      }
      return;
    }else{
      printf("==================================================\n\n");
      printf("Player  - %s\nNumber of games won  - %d\nNumber of games played  - %d\n",user_scores[i].username,user_scores[i].games_won,user_scores[i].games_played);
      printf("\n==================================================\n");
    }
  }
}


/*
 * Summary: Set/update leaderboard and then sort list
 * Parameters:
 *        string user : users login name
 *        int win : whether the user won or not
 */
void set_leaderboard(char *user,int win){
  int user_pos = find_user(user);
  if(user_pos!= -1){ /* If user saved*/
      user_scores[user_pos].games_played += 1;
      if(win==1){
          user_scores[user_pos].games_won++;
      }

  }else{
    if(users_full()==0){
        //Save user in next spot
        int save_pos = next_user_slot();
        if(save_pos!=-1){
          strcpy(user_scores[save_pos].username,user);
          user_scores[save_pos].games_played = 1;
          win==1 ? (user_scores[save_pos].games_won = 1) : (user_scores[save_pos].games_won = 0);
        }
    }else{
      printf("=======================================\n\n");
      printf("Maximum number of saved users exceeded!\n\n");
      printf("=======================================\n\n");
    }
  }
  //TODO Resort List
}


/*
 * Summary: Searches through struct array for username by comparing strings
 *          Search could be optimized to search by score first
 *          and then search through by name
 * Parameters:
 *        string user : users login name
 *
 * Return: index of user
 *         or -1 if user not found
 *
 */
int find_user(char *user){
  for(int i=0;i<MAX_SAVED_USERS;i++){
    if(strcmp(user,user_scores[i].username)==0){
      return i;
    }
  }
  return -1;
}

/*
 * Summary: Checks last index on the leaderboard to see if empty
 *          If last index is empty then leaderboard is not full
 *
 * Return: 0 if not full
 *         or 1 if full
 *
 */
int leaderboard_full(void){
    if(user_scores[LEADERBOARD_LENGTH-1].games_played==0){
      return 0;
    }else{
      return 1;
    }
}


/*
 * Summary: Checks last index on the users array to see if empty
 *          If last index is empty then users array is not full
 *
 * Return: 0 if not full
 *         or 1 if full
 *
 */
int users_full(void){
    if(user_scores[MAX_SAVED_USERS-1].games_played==0){
      return 0;
    }else{
      return 1;
    }
}


/*
 * Summary: Searches through array and returns the next free spot in list
 *          By seeing if user is blank
 *
 * Return: i of next free spot index
 *         or -1 if no free spots
 *
 */
int next_user_slot(void){
  for(int i=0;i<MAX_SAVED_USERS;i++){
    if(strcmp("",user_scores[i].username)==0){
      return i;
    }
  }
  return -1;
}
