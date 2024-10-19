#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "mazegen.h"

// creates a maze
// assigns memory
// and creates start and end coordinates
// even squares a blank odd ones are walls
// creating a 'grid'
void initialise_maze(maze *this)
{
    int height = this->height;
    int width = this->width;

    this->map = (char **)malloc((unsigned long)height * sizeof(char *));
    for (int i = 0; i < height; i++)
    {
        this->map[i] = (char *)malloc((unsigned long)width * sizeof(char));
        for (int j = 0; j < width; j++)
        {

            this->map[i][j] = '#';
        }
    }

    // Set empty cells and find start and end coordinates
    for (int i = 1; i < height; i += 2)
    {
        for (int j = 1; j < width; j += 2)
        {
            this->map[i][j] = ' ';
        }
    }

    // Randomly choose start and end coordinates on empty squares
    // empty squares being even ones
    this->start.x = (rand() % ((width - 1) / 2)) * 2 + 1;
    this->start.y = (rand() % ((height - 1) / 2)) * 2 + 1;
    this->end.x = (rand() % ((width - 1) / 2)) * 2 + 1;
    this->end.y = (rand() % ((height - 1) / 2)) * 2 + 1;

    // Mark start and end coordinates
    this->map[this->start.y][this->start.x] = 'S';
    this->map[this->end.y][this->end.x] = 'E';
}

// pulled straight from the skeelton given in coursework 1
void print_maze(maze *this)
{
    printf("\n");
    printf("maze start = %d,%d\n", this->start.x, this->start.y);
    printf("maze end = %d,%d\n", this->end.x, this->end.y);
    for (int i = 0; i < this->height; i++)
    {
        for (int j = 0; j < this->width; j++)
        {
            printf("%c", this->map[i][j]);
        }
        printf("\n");
    }
}

// as the maze moves from even cell to even cell
// the function removes the connecting odd cell to represent
// a passage
void carve_passage(maze *this, coord old, coord new)
{
    // figure out direction
    if (old.x > new.x)
    {
        // East
        printf("West\n");
        this->map[old.y][old.x - 1] = ' ';
    }
    else if (old.x < new.x)
    {
        // West
        printf("East\n");
        this->map[old.y][old.x + 1] = ' ';
    }
    else if (old.y > new.y)
    {
        // south
        printf("North\n");
        this->map[old.y - 1][old.x] = ' ';
    }
    else
    {
        // North
        printf("South\n");
        this->map[old.y + 1][old.x] = ' ';
    }
}

// uses recursive backtracking to find a route through the maze
// when all the cells are visited it repeats this process with the previous cell
// creating a recursively generated path throught the maze
// idea stolen from https://weblog.jamisbuck.org/2010/12/27/maze-generation-recursive-backtracking
void recursive_backtracking(maze *this, int **visited, coord cell)
{

    // initialise
    int x = cell.x;
    int y = cell.y;
    // shuffle directions
    enum
    {
        North,
        East,
        South,
        West,
    };
    int direction[4] = {North, East, South, West};
    for (int i = 0; i < 4; i++)
    {
        int j = rand() % 4;
        int temp = direction[i];
        direction[i] = j;
        direction[j] = temp;
    }
    // mark cell as visited
    visited[y][x] = 1;
    // initialise coordinates of cell next visited
    coord new;
    new.x = 0;
    new.y = 0;
    // check for each direction for new route
    for (int i = 0; i < 4; i++)
    {
        switch (direction[i])
        {
        case North:
            new.x = x;
            new.y = y - 2;
            break;
        case South:
            new.x = x;
            new.y = y + 2;
            break;
        case East:
            new.x = x + 2;
            new.y = y;
            break;
        case West:
            new.x = x - 2;
            new.y = y;
            break;
        default:
            break;
        }
        // if possible search next cell
        if (new.x >= 0 && new.x < this->width - 1 && new.y >= 0 && new.y < this->height - 1 && visited[new.y][new.x] == 0)
        {
            printf("new coordinates are %d, %d\n", new.x, new.y);
            printf("height and width boudaries are %d,%d\n", this->height, this->height);
            carve_passage(this, cell, new);
            recursive_backtracking(this, visited, new);
        }
    }
};

// creates a new 2d array with nothing but integers
// to represent if that coordinate has been visited by the RB algorithm
void create_maze(maze *this)
{

    // memory allocation
    int **visited = (int **)malloc(this->height * sizeof(int *));
    for (int i = 0; i < this->height; i++)
    {
        visited[i] = (int *)malloc(this->width * sizeof(int));
        for (int j = 0; j < this->width; j++)
        {
            visited[i][j] = 0;
        }
    }
    recursive_backtracking(this, visited, this->start);
    free(visited);
}

int create_file(maze *this, char *filename)
{
    // create a file
    FILE *file;
    if (access(filename, F_OK) == 0)
    {
        return 0;
    }
    else
    {
        file = fopen(filename, "w"); // write file
        if (file == NULL)
        {
            return 1;
        }
        for (int i = 0; i < this->height; i++)
        {
            for (int j = 0; j < this->width; j++)
            {
                fprintf(file, "%c", this->map[i][j]);
            }
            fprintf(file, "\n"); // newline at end
        }
        fclose(file);
        return 1;
    }
}

int main(int argc, char *argv[])
{
    // create maze
    maze *this_maze = malloc(sizeof(maze));

    if (argc != 4)
    {
        printf("argument error");
        return 0;
    }
    // set arguments to what they are meant to be
    char *filename = argv[1];
    // check arguments in bound
    if ((atoi(argv[2]) < MAX_DIMENSION && atoi(argv[2]) > MIN_DIMENSION) || (atoi(argv[3]) < MAX_DIMENSION && atoi(argv[3]) > MIN_DIMENSION))
    {
        printf("invalid dimenstion input");
        return 1;
    }
    this_maze->height = atoi(argv[2]);
    this_maze->width = atoi(argv[3]);

    // generate maze
    initialise_maze(this_maze);

    create_maze(this_maze);

    // write to file
    if (create_file(this_maze, filename) == 0)
    {
        printf("error creating file");
    }

    // deallocate memory and close
    free(this_maze);
    return 0;
}
