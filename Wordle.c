// This program is written for Windows, and will not work for linux out of the box.
// To port to Linux:
// 1. Remove <conio.h> (line 13)
// 2. Replace system("cls") with system("clear") (line 126 & line 181)
// 3. Remove system("color") (line 19), because ANSI colors work natively on Linux terminals

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <ctype.h>

//#include <conio.h>

// Score depends on tries taken to guess the word

int main()
{
    //system("color");

    struct Tile
    {
        int color; // 0 for grey, 1 for yellow, 2 for green
        char letter;
    };

    FILE* fptr;
    // The following line will work on DEV C++. In Visual Studio, please provide the full path of the file (or atleast thats what we experienced)
    fptr = fopen("WordsList.txt", "r");
    if (!fptr)
    {
        printf("Error: Could not open WordsList.txt\n");
        return 1;
    }
    srand(time(0)); //without this, ran() wouldn't be random

    char dictionary[5800][10];
    int totalWords = 0;

    while (fgets(dictionary[totalWords], sizeof(dictionary[totalWords]), fptr) && totalWords < 5800)
    {
        dictionary[totalWords][strcspn(dictionary[totalWords], "\r\n")] = '\0';
        totalWords++;
    }//above while loop is used to input the entire .txt into a array, afterwhich the .txt is closed.
    //why? because it makes the program much faster and more efficient rather than reading the entire .txt repeatedly.
    fclose(fptr); // close file immediately, disk access is done.

    int userCh; //to check if the user wants to replay or not

    printf("\nWelcome to Wordle! You have 6 tries to guess a 5-letter word\n");

    do
    {
        /*
            Generate a random number between 1 and 5757 to select a word from the file by using for loop
            We use same variable (RandomNumber) to generate a random position for hint letter 1-4
            characterFlag is used to separate the condition of letter, +, and - in the display
            correctCount represents the amount of times a correct letter is entered in a single try(present in the file)
            found is used to check whether the entered word is present in the file or not
            guessCount is used to edit the display according to the number of tries
        */
        int correctCount = 0, found = 0, guessCount = 0;
        char secret[100], guess[100];
        struct Tile words[6][6];

        int randomIndex = rand() % totalWords;
        strcpy(secret, dictionary[randomIndex]);

        int RandomNumber = 1 + rand() % (5);//generating random number for the hint letter.
        printf("Hint: Letter '%c' is at the position %d.\n", secret[RandomNumber - 1], RandomNumber);

        do
        {
            found = 0;

            // The following section checks whether the word input is present in our file, to prevent gibberish inputs
            do
            {
                printf("\nEnter a correct five-letter word: ");
                scanf(" %99s", guess); // space clears input buffer, 99s prevents overflow (leaves 1 byte for '\0')
                        //99 is the limit of 99 characters, and s takes input until it encounters space or newline
                // convert input to lowercase to handle uppercase entries

                for (int i = 0; guess[i]; i++)
                    guess[i] = tolower(guess[i]);

                for(int i = 0; i < totalWords; i++)
                {
                    //below is confirming if the input word exists in our dictionary or not, otherwise its gibberish
                    if (strcmp(dictionary[i], guess) == 0)
                    {
                        found = 1;
                        break;
                    }
                }
                if (found == 0)
                {//if its gibberish or invalid word not in the distionary, this block will run
                    printf("\nIncorrect word.\n");
                }

            } while (found == 0);

            //the Logic below is the letter by letter checking system
            correctCount = 0;
            char temporary_secret[5]; //temporary copy of the secret letter for comparision and modification
            for(int i =0 ; i<5 ; i++)
            {
                temporary_secret[i] = secret[i];
                words[guessCount][i].letter = guess[i];
                words[guessCount][i].color = 0; //initially it is assumed to be gray
            }

            for(int i = 0; i <5 ; i++)
            {
                if(guess[i] == temporary_secret[i])
                {
                    words[guessCount][i].color = 2; //here the letter is marked as green if its correct at correct position
                    temporary_secret[i] = '*';
                    ++correctCount;
                }
            }
            for(int i = 0; i< 5 ; i++)
            {
                if(words[guessCount][i].color == 2)
                {
                    continue; //if the letter is green, it is skipped
                }
                for(int j = 0; j < 5 ; j++)
                {
                    if(guess[i] == temporary_secret[j])
                    {
                        words[guessCount][i].color = 1; //the letter is marked as yellow if its at the wrong position
                        temporary_secret[j] = '*';
                        //since we skipped loops with green positions, they won't be highlighted here
                        break; //stop checking this guess letter to avoid duplicates further for the same letter
                    }
                }
            }
            // The following section is for giving feedback to the player about their guesses
            // and printing the entire table
            //system("cls");
            system("clear");

            for (int i = 0; i < guessCount + 1; i++)
            {
                printf("+---+---+---+---+---+\n");
                for (int j = 0; j < 5; j++)
                {
                    if (words[i][j].color == 0)
                    {
                        printf("| \033[1;90m%c\033[0m ", words[i][j].letter);
                    }
                    if (words[i][j].color == 1)
                    {
                        printf("| \033[1;93m%c\033[0m ", words[i][j].letter);
                    }
                    if (words[i][j].color == 2)
                    {
                        printf("| \033[1;92m%c\033[0m ", words[i][j].letter);
                    }
                }
                printf("|");
                printf("\n");
            }
            printf("+---+---+---+---+---+");
            if (5 - guessCount != 0) {
                printf("\nYou have %d remaining tries\n", 5 - guessCount);
            }
            if (correctCount == 5)
            {
                printf("\nYou won!\n");
            }
            guessCount++;

        } while (correctCount != 5 && guessCount != 6); //Game continues until all letters are correct or tries are exhausted

        // Basic score calculation. Minimum score is 40 and maximum is 100
        // Score is 0 if the player fails to guess the word
        printf("\nThe secret word was [%s]", secret);
        if (correctCount != 5)
        {
            printf("\nYou lost!\n");
            printf("\nYour final score is 0");
        }
        else
        {
            int score = 100;
            score -= 10 * (guessCount - 1);
            printf("\nYour final score is %d", score);
        }

        do
        {
            printf("\nDo you want to play again? (1 = yes, 0 = no): ");
            scanf("%d", &userCh);

            //system("cls");
            system("clear");

            if (userCh != 1 && userCh != 0)
            {
                printf("\nInvalid input, please enter 1 or 0.");
            }
        } while (userCh != 1 && userCh != 0);

    } while (userCh != 0);

    return 0;
}
