#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define SIZE    10
#define MINES   12

int main(void)
{
    srand(time(NULL));
    char pg[SIZE][SIZE];

    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            pg[i][j] = '.';
        }
    }


    int rows, cols, count = 0;
    while (count != MINES){
        rows = rand() % 10;
        cols = rand() % 10;
        if (pg[rows][cols] == '*') {continue;}
        if (rows == 0 && cols > 0 && cols < 9) { // top line except corners
            if (pg [0][cols -1] != '*' && pg [0][cols + 1] != '*' 
                && pg [1][cols - 1] != '*' && pg [1][cols] != '*' 
                && pg [1][cols + 1] != '*') {
                    pg[rows][cols] = '*';
                    count++;
                }
            }
         if (rows == 9 && cols > 0 && cols < 9) { // bottom line except corners
            if (pg [9][cols -1] != '*' && pg [9][cols + 1] != '*' 
                && pg [8][cols - 1] != '*' && pg [8][cols] != '*' 
                && pg [8][cols + 1] != '*') {
                    pg[rows][cols] = '*';
                    count++;
                }    
            }
         if (rows == 9 && cols == 0) { // bottom left corner
            if (pg [9][1] != '*' && pg [8][0] != '*' 
                && pg [8][1] != '*') {
                    pg[9][0] = '*';
                    count++;
                }  
            }
         if (rows == 9 && cols == 9) { // bottom right corner
            if (pg [9][8] != '*' && pg [8][9] != '*' 
                && pg [8][8] != '*') {
                    pg[9][9] = '*';
                    count++;
                }  
            }
         if (rows == 0 && cols == 9) { // top right corner
            if (pg [0][8] != '*' && pg [1][9] != '*' 
                && pg [1][8] != '*') {
                    pg[0][9] = '*';
                    count++;
                }   
            }
         if (rows == 0 && cols == 0) { // top left corner
            if (pg [0][1] != '*' && pg [1][0] != '*' 
                && pg [1][1] != '*') {
                    pg[0][0] = '*';
                    count++;
                }  
            }
        if (cols == 0 && rows > 0 && rows < 9) { // left cols except corners
            if (pg [rows +1][0] != '*' && pg [rows - 1][0] != '*' 
                && pg [rows][1] != '*' && pg [rows - 1][1] != '*' 
                && pg [rows + 1][1] != '*') {
                    pg[rows][0] = '*';
                    count++;
                }
            }
        if (cols == 9 && rows > 0 && rows < 9) { // right cols except corners
            if (pg [rows + 1][9] != '*' && pg [rows - 1][9] != '*' 
                && pg [rows][8] != '*' && pg [rows - 1][8] != '*' 
                && pg [rows + 1][8] != '*') {
                    pg[rows][9] = '*';
                    count++;
                }
            }
        if (cols > 0 && cols < 9 && rows > 0 && rows < 9) { // without borders
            if (pg [rows - 1][cols - 1] != '*' && pg [rows - 1][cols] != '*' 
                && pg [rows - 1][cols + 1] != '*' && pg [rows][cols - 1] != '*' 
                && pg [rows][cols + 1] != '*' && pg [rows + 1][cols + 1] != '*'
                && pg [rows + 1][cols] != '*' && pg [rows + 1][cols - 1] != '*') {
                    pg[rows][cols] = '*';
                    count++;
                }
            }   
    }

    for (int i = 0; i < 10; ++i){
        for (int j = 0; j < 10; ++j){
            printf("%c ", pg [i][j]);
        }
        printf("\n");
    }

    return 0;
}
