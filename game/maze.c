#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "maze.h"

FILE *open_file(char filename[], char mode[])
{
    FILE *file = fopen(filename, mode);
    if (file == NULL)
    {
        perror("");
        exit(EXIT_FILE_ERROR);
    }
    return file;
}

// create the 2d array map from maze struct
// allocated appropiate memory based on height and width

int create_maze(maze *this, int height, int width)
{
    this->height = height;
    this->width = width;
    this->map = (char **)malloc((unsigned long)height * sizeof(char *));
    if (this->map == NULL)
    {
        return 1;
    }

    int i;
    for (i = 0; i < height; i++)
    {
        this->map[i] = (char *)malloc((unsigned long)width * sizeof(char));
        if (this->map[i] == NULL)
        {
            return 1;
        }
    }
    return 0;
}
// free the maze function row by row
// to prevent memory leakage

void free_maze(maze *this)
{
    for (int i = 0; i < this->height; i++)
    {
        free(this->map[i]);
    }
    free(this->map);
}

// check if last character is a newline character
// if so removes
// used in get_width function
void remove_newline(char *str)
{
    // if newline character exists replace it with null terminator
    // reducing size of string by 1
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n')
    {
        str[len - 1] = '\0';
    }
}

// reads the length of each width of the file
// to check for discrepancy
// uses remove_newline function
// will return 0 if the dimensions of the maze are not retangular
// as if the width or height are in the wrong place the newline character
// will be at an unexpected place
int get_width(char *filename)
{
    FILE *new_file = fopen(filename, "r");

    int new_buffersize = 256;
    char lines[new_buffersize];
    int row_length = 0;
    int initial_width = 0;

    while (fgets(lines, new_buffersize, new_file) != NULL)
    {
        remove_newline(lines);
        int current_width = (int)strlen(lines);
        if (initial_width != current_width && row_length != 0)
        {
            fclose(new_file);
            return 0;
        }
        else
        {
            row_length++;
            if (row_length == 1)
            {
                initial_width = current_width;
            }
        }
    }
    fclose(new_file);
    printf("intital width is : %d\n", initial_width);

    if (initial_width < MAX_DIM && initial_width > MIN_DIM)
    {
        return initial_width;
    }
    else
    {
        return 0;
    }
}

// returns the height of the file
// parameter filename
// returns type 0

int get_height(char *filename)
{
    FILE *file = fopen(filename, "r");

    int buffersize = 256;
    int height = 0;
    char line[buffersize];

    while (fgets(line, buffersize, file) != NULL)
    {
        height += 1;
    }
    fclose(file);

    if (height < MAX_DIM && height > MIN_DIM)
    {
        return height;
    }
    else
    {
        return 0;
    }
}

// check if the maze is valid
// then reads in file to 2d array this->maze
// set up start and end coordinates
// close file
// returns 0 if successful
// else returns other non zero exit argument
int read_maze(maze *this, char *filename)
{
    int height;
    int width;

    // check if file exists
    FILE *file_check = fopen(filename, "r");
    if (file_check == NULL)
    {
        printf("File '%s' does not exist\n", filename);
        return EXIT_FILE_ERROR;
    }

    fclose(file_check); // Close the file after checking
    height = get_height(filename);
    width = get_width(filename);
    if (width == 0 || height == 0)
    {
        printf("invalid dimensions");
        return 3;
    }
    if (create_maze(this, height, width) == 1)
    {
        printf("Memory allocation failed\n");
        return 1;
    }

    FILE *newer_file = fopen(filename, "r");
    if (newer_file == NULL)
    {
        perror("");
        exit(EXIT_FILE_ERROR);
    }

    int buffersize = 256;
    int i = 0;
    char line[this->width];

    while (fgets(line, buffersize, newer_file) != NULL)
    {
        for (int j = 0; j < this->width; j++)
        {
            printf("%c", line[j]); // Print the character read from the file

            switch (line[j])
            {
            case 'S':
                this->start.x = j;
                this->start.y = i;
                this->map[i][j] = 'S';
                break;
            case 'E':
                this->end.x = j;
                this->end.y = i;
                this->map[i][j] = 'E';
                break;
            case '\n':
                break;
            case ' ':
            case '#':
                this->map[i][j] = line[j];
                break;
            default:
                printf("invalid character in file");
                return (EXIT_MAZE_ERROR);
            }
        }
        i++;
    }

    fclose(newer_file);
    return 0;
}

// function from skeleton code given to us
// prints it in desired format
// untouched

void print_maze(maze *this, coord *player)
{
    printf("\n");
    printf("maze start = %d,%d\n", this->start.x, this->start.y);
    printf("player start = %d,%d\n", player->x, player->y);
    printf("maze end = %d,%d\n", this->end.x, this->end.y);
    for (int i = 0; i < this->height; i++)
    {
        for (int j = 0; j < this->width; j++)
        {
            if (player->x == j && player->y == i)
            {
                printf("X");
            }
            else
            {
                printf("%c", this->map[i][j]);
            }
        }
        printf("\n");
    }
}

// take player input and move player across maze
// additionally check if move is in bounds and into
// empty space
// does not return anything just updates position and maze
// depending on move choice
void move(maze *this, coord *player, char direction)
{
    int new_x;
    int new_y;

    switch (direction)
    {
    case 'W':
    case 'w':
        new_x = player->x;
        new_y = player->y - 1;
        break;
    case 'A':
    case 'a':
        new_x = player->x - 1;
        new_y = player->y;
        break;
    case 'S':
    case 's':
        new_x = player->x;
        new_y = player->y + 1;
        break;
    case 'D':
    case 'd':
        new_x = player->x + 1;
        new_y = player->y;
        break;
    }
    if (new_x > this->width - 1 || new_x < 0 || new_y > this->height - 1 || new_y < 0)
    {
        printf("invalid move option");
    }
    else if (this->map[new_y][new_x] == '#')
    {
        printf("cannot move there");
    }
    else
    {
        player->x = new_x;
        player->y = new_y;
    }
}

// takes maze and player coordinates as arguments
// returns 0 if player is not at end of maze
// returns 1 if player is at end
int has_won(maze *this, coord *player)
{
    if (this->end.x == player->x && this->end.y == player->y)
    {
        printf("congratulations, you have reached the end of the maze!\n");
        return 1;
    }
    else
    {
        return 0;
    }
}

// main function has paramater argv[1] which is the
// text of the filename to read from
// returns an exit code depending on succcess or failure
// see exit arguments for further details

int main(int argc, char *argv[])
{
    // initisalise struct with memory for maze and player
    coord *player = malloc(sizeof(coord));
    if (player == NULL)
    {
        perror("");
        exit(EXIT_MAZE_ERROR);
    }
    maze *this_maze = malloc(sizeof(maze));
    if (this_maze == NULL)
    {
        perror("");
        exit(EXIT_MAZE_ERROR);
    }

    // take commmand line argument 1 and read it into maze creation function
    if (argc != 2)
    {
        printf("argument error");
        exit(EXIT_ARG_ERROR);
    }
    char *filename = argv[1];
    int result;
    result = read_maze(this_maze, filename);
    if (result == 2)
    {
        printf("Error reading file");
        exit(EXIT_FILE_ERROR);
    }
    else if (result == 3)
    {
        exit(EXIT_MAZE_ERROR);
    }
    else if (result != 0)
    {
        printf("unexpected error");
        return 1;
    }

    // now that maze is valid initialise player position at start
    player->x = this_maze->start.x;
    player->y = this_maze->start.y;
    print_maze(this_maze, player);

    // while player is not at end ask for input
    int won_game = 0;
    while (won_game == 0)
    {
        char choice;
        printf("select what option to choose\n1. press m to see map\n2. press wasd to move\n");
        scanf("%s", &choice);
        switch (choice)
        {
        case 'W':
        case 'w':
        case 'A':
        case 'a':
        case 'S':
        case 's':
        case 'D':
        case 'd':
            move(this_maze, player, choice);
            break;
        case 'M':
        case 'm':
            print_maze(this_maze, player);
            break;
        default:
            printf("invalid selection, please try again\n");
            break;
        }
        won_game = has_won(this_maze, player);
    }

    free_maze(this_maze);
    free(player);

    return EXIT_SUCCESS;
}