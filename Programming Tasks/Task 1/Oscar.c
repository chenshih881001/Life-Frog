#include <stdio.h>
#include <stdlib.h>

// THIS IS THE FUNCTION YOU IMPLEMENT
int move(char *world,char *map,int robot_index,int width)
{
    if(world[robot_index+(width+1)]=='T')  //go south
    {
        map[robot_index] = 'x';
        return 3;
    }
    else if(world[robot_index-1]=='T')  //go west
    {
        map[robot_index] = 'x';
        return 4;
    }
    else if(world[robot_index-(width+1)]=='T') //go north
    {
        map[robot_index] = 'x';
        return 1;
    }
    else if(world[robot_index+1]=='T') //go north
    {
        map[robot_index] = 'x';
        return 2;
    }

    else if(world[robot_index+(width+1)]!='#' && map[robot_index+(width+1)]!='x')  //go south
    {
        map[robot_index] = 'x';
        return 3;
    }
    else if(world[robot_index-1]!='#' && map[robot_index-1]!='x')  //go west
    {
        map[robot_index] = 'x';
        return 4;
    }
    else if(world[robot_index-(width+1)]!='#' && map[robot_index-(width+1)]!='x') //go north
    {
        map[robot_index] = 'x';
        return 1;
    }
    else if(world[robot_index+1]!='#' && map[robot_index+1]!='x') //go north
    {
        map[robot_index] = 'x';
        return 2;
    }
    // YOUR CODE HERE
    //return 1; // REPLACE THE RETURN VALUE WITH YOUR CALCULATED RETURN VALUE
}

// Return target index
int update_world(int movement, char *world, int robot_index, int width) {
    int target_index = 0;
    // NORTH
    if(movement == 1) {
        target_index = robot_index-(width+1); // +1 for the newline
    }
    // SOUTH
    else if(movement == 3) {
        target_index = robot_index+(width+1); // +1 for the newline
    }
    // EAST
    else if(movement == 2) {
        target_index = robot_index+1;
    }
    // WEST
    else if(movement == 4) {
        target_index = robot_index-1;
    }

    // ACTION
    if(world[target_index] == 'O') {
        world[target_index] = 'R';
        world[robot_index] = 'O';
        return target_index;
    }
    else if(world[target_index] == '#') {
        printf("%s", world);
        printf("%c", '\n');
        printf("FAILURE, crashed into a wall!");
        exit(1);
    }
     else if(world[target_index] == 'T') {
        world[target_index] = 'R';
        world[robot_index] = 'O';
        printf("%s", world);
        printf("%c", '\n');
        printf("SUCCESS, target found!");
        exit(0);
    }
}

int main() {
    const int MAX_STEPS = 200;
    int step = 1;

    int movement;
    int width = 20; // excluding newlines

    // The world
    char world[200] = {
        '#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','\n',
        '#','T','O','O','O','O','O','O','O','O','O','O','O','O','O','O','O','O','O','#','\n',
        '#','O','O','O','O','O','O','O','O','O','O','O','O','O','O','O','O','O','O','#','\n',
        '#','O','O','O','O','O','O','O','O','O','O','O','O','O','O','O','O','O','O','#','\n',
        '#','O','O','O','O','O','O','O','O','O','O','O','O','O','O','O','O','O','O','#','\n',
        '#','O','O','O','O','O','O','O','O','O','O','O','O','O','O','O','O','O','O','#','\n',
        '#','O','R','O','O','O','O','O','O','O','O','O','O','O','O','O','O','O','O','#','\n',
        '#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','\n',
    };

    char map[200];
    for(int i=0;i<200;i++)
        map[i]=world[i];

    // Initialize target and robot positions
    // Assumes only one target, one robot
    unsigned int elements = sizeof(world)/sizeof(world[0]);

    // Initialize the index of the robot and the target.
    // Assumes exactly one robot and one target
    int robot_index;
    int target_index;

    for(int i = 0; i < elements; ++i) {
    if (world[i] == 'R') {
            robot_index = i;
            break;
        }
    }
    for(int i = 0; i < elements; ++i) {
    if (world[i] == 'T') {
            target_index = i;
            break;
        }
    }

    // Print the initial world
    printf("Starting position: %c", '\n');
    // Debug output
    // printf("Robot index: %i / target index: %i %c", robot_index, target_index, '\n');
    printf("%s", world);
    printf("%c", '\n');

    while(step <= MAX_STEPS) {
        printf("After step number %i: %c", step, '\n');

        movement = move(world, map, robot_index, width);
        robot_index = update_world(movement, world, robot_index, width);
        printf("%s", world);
        printf("%c", '\n');
        step = step+1;
    }
    if(step >= MAX_STEPS) {
        printf("FAILURE, maximum number of steps exceeded.");
    }

}



















































