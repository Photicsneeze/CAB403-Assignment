/*
 *  CAB403 Assignment - Semester 2 2015
 *
 *  Created by Lachlan Cesca & Shaun Karran
 *
 *
 *       Pretty messy at the moment. Need to comment heaps.
 *       But it works...
 *
 *    TODO: Want to add in a server function for printing to client.
 *          Just because in testing i've used printf a lot.
 *         Name of function would be similar to "printf" (i.e "cprintf")
*          For easy adapting.
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_WORD_LENGTH 20

typedef struct{
    //char *user;
    int guess_count;
    int number_guesses;
    char word1[MAX_WORD_LENGTH];
    char word2[MAX_WORD_LENGTH];
    int len_word1;
    int len_word2;
    char guessed_word1_potion[MAX_WORD_LENGTH];
    char guessed_word2_potion[MAX_WORD_LENGTH];
    char guesses_made[26];
}Game;




void number_of_guesses(Game *game)
{
  game->guess_count = 0;    /*Initialse guess_count [didnt like it in struct] */
  for(int i=0;i<26;i++){
    game->guesses_made[i] = '\0';
  }
  int formula = (strlen(game->word1)+strlen(game->word2)+10);
  int n_guesses = (formula<26) ? formula: 26;
  game->number_guesses = n_guesses;
}

void choose_words(Game *game, int number)
{
  char *word1 = game->word1;
  char *word2= game->word2;
  FILE *fr;
  fr = fopen ("hangman_text.txt", "rt");

  srand(time(NULL));
  int words = rand() % number;

    char line[80];

    int count = 0;
    while(fgets(line, 80, fr) != NULL)
     {
       if(count == words){
         sscanf (line, "%[^,],%s", word1,word2);
         break;
       }else{
         count++;
       }
     }
     fclose(fr);  /* close the file prior to exiting the routine */
     game->len_word1 = strlen(word1);
     game->len_word2 = strlen(word2);

     for(int i=0;i<strlen(word1);i++){
       game->guessed_word1_potion[i] = '_';
     }

     for(int i=0;i<strlen(word2);i++){
       game->guessed_word2_potion[i] = '_';
     }
}

int get_number_words_available(void)
{
  FILE *fr;
  fr = fopen ("hangman_text.txt", "rt");
  if(fr==NULL){
    perror("Cannot Open File!\n");
  }
  char line[80];
  int n_words = 0;

  while(fgets(line, 80, fr) != NULL)
   {
     n_words++;
   }
   fclose(fr);  /* close the file prior to exiting the routine */
   return n_words;
}


void display_game(Game *game)
{
  printf("\n");
  for(int i = 0; i<30;i++){
    printf("-");
  }
  printf("\nGuessed letters: ");
  for(int i=0;i<game->number_guesses;i++){
    if(game->guesses_made[i]!='\0')
    {
      printf("%c",game->guesses_made[i]);
    }else{
      break;
    }
  }
  printf("\n\nNumber of guesses left: %d",((game->number_guesses)-(game->guess_count)));
  printf("\n\nWord: ");
  for(int i=0;i<game->len_word1;i++){
    printf("%c ",game->guessed_word1_potion[i]);
  }
  printf("    ");
  for(int i=0;i<game->len_word2;i++){
    printf("%c ",game->guessed_word2_potion[i]);
  }
  printf("\n\nEnter your guess - ");
}

void update_guess(Game *game, char guess)
{
  for(int i=0;i<game->len_word1;i++){
    if(game->word1[i] == guess)
        game->guessed_word1_potion[i] = guess;
  }

  for(int i=0;i<(game->len_word2);i++){
    if(game->word2[i] == guess)
        game->guessed_word2_potion[i] = guess;
  }
}


/* Summary:
 *    Simple check if game has been won.
 *
 * TODO:
 *    Could merge check for game loss in as well.
 *
 * Outputs:
 *    return 0 - if game hasnt ended
 *    return 1 - if game has been won
 */
int check_complete(Game *game)
{
  for(int i=0;i<game->len_word1;i++){
    if(game->guessed_word1_potion[i] == '_')
        return 0;
  }

  for(int i=0;i<(game->len_word2);i++){
    if(game->guessed_word2_potion[i] == '_')
        return 0;
  }
  return 1;
}



/*MAIN LOOP: testing purposes only*/
int main(int argc, char *argv[])
{
    Game game;

    choose_words(&game, get_number_words_available());    /*flip words around, word 1 is word 2 and vice versa*/
    number_of_guesses(&game);


    char guess;

    for(;;){

      display_game(&game);

      if(check_complete(&game)==1){
        //Win
        printf("\nGame Over\n\n\n");
        printf("Well done *user*! You won this round of Hangman!\n");
        break;
      }

      if(game.guess_count>=game.number_guesses){
        //Game over
        printf("\nGame Over\n\n\n");
        printf("Bad luck *user*! You have run out of guesses. The Hangman got you!\n");
        break;
      }

      scanf(" %c",&guess);

      update_guess(&game,guess);

      game.guesses_made[game.guess_count] = guess;
      game.guess_count++;


    }

    //printf("Word 1:%s, Word2:%s\n",game.word1,game.word2);
}
