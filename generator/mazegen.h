#ifndef maze_generator_h
#define maze_generator_h

#define MAX_DIMENSION 101
#define MIN_DIMENSION 4

typedef struct __Coord
{
    int x;
    int y;
} coord;

typedef struct __Maze
{
    char **map;
    int height;
    int width;
    coord start;
    coord end;
} maze;

#endif