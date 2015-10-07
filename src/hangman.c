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
 *          Name of function would be similar to "printf" (i.e "cprintf")
 *          For easy adapting.
 */

#include "hangman.h"


/*
 * Summary: Calculate the number of guesses allowed for given word
 *
 * Parameters:
 *        Game *game : game struct pointer
 *
 */
void number_of_guesses(Game *game)
{
    game->guess_count = 0;    /*Initialse guess_count [didnt like it in struct] */
    for (int i = 0; i < 26; i++){
        game->guesses_made[i] = '\0';
    }

    int formula = (strlen(game->word1) + strlen(game->word2) + 10);

    game->number_guesses = (formula < 26) ? formula : 26;
}



/*
 * Summary: Randomly choose given word
 *          Also swap words around for proper displayment
 *
 * Parameters:
 *        Game *game : game struct pointer
 *        int number : Number of words in list to search through
 *
 */
void choose_words(Game *game, int number)
{
    char *word1 = game->word1;
    char *word2= game->word2;
    FILE *fr;
    fr = fopen ("bin/hangman_text.txt", "rt");

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


/*
 * Summary: Search through file to get number of words available.
 *          Allowing the addition and subtraction of more words in the future
 *
 * Return: number of words available to use in game
 *
 */
int get_number_words_available(void)
{
    FILE *fr;
    fr = fopen ("bin/hangman_text.txt", "rt");
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


/*
 * Summary: Send/Display game on client end
 *
 * Parameters:
 *        Game *game : game struct pointer
 *        char *str : the string that will be sent to client to be displayed
 *
 */
void display_game(Game *game, char *str)
{
    char temp[30] = {0};

    strcat(str, "\n");
    for (int i = 0; i < 30; i++) {
        strcat(str, "-");
    }

    strcat(str, "\nGuessed letters: ");
    for (int i = 0; i < game->number_guesses; i++) {
        if (game->guesses_made[i] != '\0') {
            sprintf(temp, "%c", game->guesses_made[i]);
            strcat(str, temp);
        } else {
            break;
        }
    }

    sprintf(temp, "\n\nNumber of guesses left: %d", ((game->number_guesses)-(game->guess_count)));
    strcat(str, temp);

    strcat(str, "\n\nWord: ");
    for(int i = 0; i < game->len_word1; i++) {
        sprintf(temp, "%c ", game->guessed_word1_potion[i]);
        strcat(str, temp);
    }

    strcat(str, "    ");
    for(int i = 0; i < game->len_word2; i++) {
        sprintf(temp, "%c ", game->guessed_word2_potion[i]);
        strcat(str, temp);
    }
    strcat(str, "\n\nEnter your guess - ");
}


/*
 * Summary: Update game with guess
 *
 * Parameters:
 *        Game *game : game struct pointer
 *        char guess : letter guessed in hangman game
 *
 */
void update_guess(Game *game, char guess)
{
    for(int i = 0; i < game->len_word1; i++) {
        if (game->word1[i] == guess)
            game->guessed_word1_potion[i] = guess;
    }

    for(int i = 0; i < (game->len_word2); i++) {
        if (game->word2[i] == guess)
            game->guessed_word2_potion[i] = guess;
    }
}

/*
 * Summary: Simple check if game has been won.
 *
 * TODO:
 *    Could merge check for game loss in as well.
 *
 * Return:
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
// int main(int argc, char *argv[])
// {
//     Game game;
//     char guess;

//     /* flip words around, word 1 is word 2 and vice versa */
//     choose_words(&game, get_number_words_available());

//     number_of_guesses(&game);

//     for(;;){
//       display_game(&game);

//       if(check_complete(&game)==1){
//         //Win
//         printf("\nGame Over\n\n\n");
//         printf("Well done *user*! You won this round of Hangman!\n");
//         break;
//       }

//       if(game.guess_count>=game.number_guesses){
//         //Game over
//         printf("\nGame Over\n\n\n");
//         printf("Bad luck *user*! You have run out of guesses. The Hangman got you!\n");
//         break;
//       }

//       scanf(" %c",&guess);

//       update_guess(&game,guess);

//       game.guesses_made[game.guess_count] = guess;
//       game.guess_count++;
//     }

//     //printf("Word 1:%s, Word2:%s\n",game.word1,game.word2);
// }
