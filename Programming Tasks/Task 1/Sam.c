/*
A very simple text based simulation of a rescue scenario. Wall, robot and target only.

Copyright 2021 Kristian Rother (kristian.rother@hshl.de)

Redistribution and use in source and binary forms, with or without modification, are permitted provided
that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions
and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions
and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to
endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#include <stdlib.h>

// THIS IS THE FUNCTION YOU IMPLEMENT
//int flag[4] = {0, 0, 0, 0};

int move(char *world)
{
    //find Target and robot coordinates
    int tc = 0, rc = 0;

    while (world[tc] != 'T')
    {
        tc++;
    }

    int tx = tc % 21; //target X coordinate
    int ty = tc / 21; //target Y coordinate
    while (world[rc] != 'R')
    {
        rc++;
    }
    int rx = rc % 21; //robot X coordinate
    int ry = rc / 21; //robot Y coordinate

/*  if (world[rc - 21] == '#')
        flag[0] = 1; //north is obstruccted
    if (world[rc + 1] == '#')
        flag[1] = 1; //east is obstruccted
    if (world[rc + 21] == '#')
        flag[2] = 1; //south is obstruccted
    if (world[rc - 1] == '#')
        flag[3] = 1; //west is obstruccted
*/
    //choose direction
    if (ty < ry) // && !flag[0]) //target is to the north and unobstructed
    {
        printf("1\n");
        //flag[2] = 1;
        return 1;
    }
    else if (tx > rx) // && !flag[1]) //target is to the east and unobstructed
    {
        printf("2\n");
        //flag[3] = 1;
        return 2;
    }
    else if (ty > ry) // && !flag[2]) //target is to the south and unobstructed
    {
        printf("3\n");
        //flag[0] = 1;
        return 3;
    }
    else if (tx < rx) // && !flag[3]) //target is to the west and unobstructed
    {
        printf("4\n");
        //flag[1] = 1;
        return 4;
    }
    /*else
    {
        if (!flag[0])
        {
            printf("1\n");
            flag[2] = 1;
            return 1;
        }
        if (!flag[1])
        {
            printf("2\n");
            flag[3] = 1;
            return 2;
        }
        if (!flag[2])
        {
            printf("3\n");
            flag[0] = 1;
            return 3;
        }
        if (!flag[3])
        {
            printf("4\n");
            flag[1] = 1;
            return 4;
        }
    }*/
}

// Return target index
int update_world(int movement, char *world, int robot_index, int width)
{
    int target_index = 0;
    // NORTH
    if (movement == 1)
    {
        target_index = robot_index - (width + 1); // +1 for the newline
    }
    // SOUTH
    else if (movement == 3)
    {
        target_index = robot_index + (width + 1); // +1 for the newline
    }
    // EAST
    else if (movement == 2)
    {
        target_index = robot_index + 1;
    }
    // WEST
    else if (movement == 4)
    {
        target_index = robot_index - 1;
    }

    // ACTION
    if (world[target_index] == 'O')
    {
        world[target_index] = 'R';
        world[robot_index] = 'O';
        return target_index;
    }
    else if (world[target_index] == '#')
    {
        printf("%s", world);
        printf("%c", '\n');
        printf("FAILURE, crashed into a wall!");
        exit(1);
    }
    else if (world[target_index] == 'T')
    {
        world[target_index] = 'R';
        world[robot_index] = 'O';
        printf("%s", world);
        printf("%c", '\n');
        printf("SUCCESS, target found!");
        exit(0);
    }
}

int main()
{
    const int MAX_STEPS = 200;
    int step = 1;

    int movement;
    int width = 20; // excluding newlines

    // The world
    char world[200] = {
        '#',
        '#',
        '#',
        '#',
        '#',
        '#',
        '#',
        '#',
        '#',
        '#',
        '#',
        '#',
        '#',
        '#',
        '#',
        '#',
        '#',
        '#',
        '#',
        '#',
        '\n',
        '#',
        'T',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        '#',
        '\n',
        '#',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        '#',
        '\n',
        '#',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        '#',
        '\n',
        '#',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        '#',
        '\n',
        '#',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        '#',
        '\n',
        '#',
        'O',
        'R',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        'O',
        '#',
        '\n',
        '#',
        '#',
        '#',
        '#',
        '#',
        '#',
        '#',
        '#',
        '#',
        '#',
        '#',
        '#',
        '#',
        '#',
        '#',
        '#',
        '#',
        '#',
        '#',
        '#',
        '\n',
    };

    // Initialize target and robot positions
    // Assumes only one target, one robot
    unsigned int elements = sizeof(world) / sizeof(world[0]);

    // Initialize the index of the robot and the target.
    // Assumes exactly one robot and one target
    int robot_index;
    int target_index;

    for (int i = 0; i < elements; ++i)
    {
        if (world[i] == 'R')
        {
            robot_index = i;
            break;
        }
    }
    for (int i = 0; i < elements; ++i)
    {
        if (world[i] == 'T')
        {
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

    while (step <= MAX_STEPS)
    {
        printf("After step number %i: %c", step, '\n');

        movement = move(world);
        robot_index = update_world(movement, world, robot_index, width);
        printf("%s", world);
        printf("%c", '\n');
        step = step + 1;
    }
    if (step >= MAX_STEPS)
    {
        printf("FAILURE, maximum number of steps exceeded.");
    }
}