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
 * Summary: Read hangman words from text file and load them into memory
 *
 * Parameters:
 *        Word *word_list : pointer to array struct of words
 *
 */
void create_word_list(Word *word_list)
{
    int num_words_available = get_number_words_available();


    FILE *fr;
    fr = fopen ("bin/hangman_text.txt", "rt");

    char line[80];

    int i = 0;
    /* Read file iterating over each line */
    /* saving the results in each line to unique variables */
    while(fgets(line, 80, fr) != NULL)
     {

         sscanf (line, "%[^,],%s", word_list[i].word,word_list[i].adjective);
         i++;

     }
     fclose(fr);  /* close the file prior to exiting the routine */

}


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
void choose_words(Game *game, Word *word_list, int number)
{

    srand(time(NULL));
    int random_word = rand() % number;

    strcpy(game->word1,word_list[random_word].word);
    strcpy(game->word2,word_list[random_word].adjective);



    game->len_word1 = strlen(game->word1);
    game->len_word2 = strlen(game->word2);

    for(int i=0;i<strlen(game->word1);i++){
        game->guessed_word1_portion[i] = '_';
    }

    for(int i=0;i<strlen(game->word2);i++){
        game->guessed_word2_portion[i] = '_';
    }

    //printf("\nWord1:%s,Word2:%s,len1:%d,len2:%d",game->word1,game->word2,game->len_word1,game->len_word2);
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
    char temp[50] = {0};

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
        sprintf(temp, "%c ", game->guessed_word1_portion[i]);
        strcat(str, temp);
    }

    strcat(str, "    ");
    for(int i = 0; i < game->len_word2; i++) {
        sprintf(temp, "%c ", game->guessed_word2_portion[i]);
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
            game->guessed_word1_portion[i] = guess;
    }

    for(int i = 0; i < (game->len_word2); i++) {
        if (game->word2[i] == guess)
            game->guessed_word2_portion[i] = guess;
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
        if(game->guessed_word1_portion[i] == '_')
                return 0;
    }

    for(int i=0;i<(game->len_word2);i++){
        if(game->guessed_word2_portion[i] == '_')
                return 0;
    }
    return 1;
}
