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
#include <stdbool.h>

char world2[21][9];
bool worldiscopied=false, stuck=false;
int tx, ty, rx, ry, state, xmax=0, ymax=0, moveto, movetoold, rxold = 0, ryold = 0,lastopposidemoveto=0, failcounter=0;

// THIS IS THE FUNCTION YOU IMPLEMENT
int move(char *world) {
    bool go=false;
    if(!worldiscopied)
    {
        int counter = 0, x = 0, y = 0;
        do
        {
            if(world[counter]!='\n')
            {
                world2[x][y]=world[counter];
                if(x>xmax){xmax=x;}
                x++;
            }
            else
            {
                x=0;
                if(y>ymax){ymax=y;}
                y++;
            }
            counter++;
        }while(counter<200);
        //printf("copy world with dimensions of x=%i and y=%i\n",xmax,ymax);
        /*for(int i = 0; i<=8; i++)
        {
            for(int j = 0; j<=20; j++)
            {
                printf("%c", world2[j][i]);
            }
            printf("\n");
        }*/
        printf("\n");
        worldiscopied=true;
        for(int i = 0; i<=ymax; i++)
        {
            for(int j = 0; j<=xmax; j++)
            {
                if(world2[j][i]=='T')
                {
                    tx=j;
                    ty=i;
                }
                if(world2[j][i]=='R')
                {
                    rx=j;
                    ry=i;
                }
            }
        }
        printf("Robot: x=%i, y=%i;      Target: x=%i, y=%i;\n",rx,ry,tx,ty);
    }
    
    
    rxold=rx;
    ryold=ry;
    do{
    switch(state)
    {
        case 0:
            printf("State 0\n");
             if(abs(tx-rx)>=abs(ty-ry))
            {
                if(tx-rx>0)
                {
                    moveto=2;
                    rx++;
                }
                else
                {
                    moveto=4;
                    rx--;
                }
            }
            else
            {
                if(ty-ry>0)
                {
                    moveto=3;
                    ry++;
                }
                else
                {
                    moveto=1;
                    ry--;
                }
            }
            if(world2[rx][ry]=='#'||world2[rx][ry]=='F')
            {
                state=1;
                rx=rxold;
                ry=ryold;
            }
            else
            {
                world2[rxold][ryold]='F';
                go=true;
                stuck=false;
            }
            break;
            
        case 1:
            printf("State 1\n");
            if(moveto==2||moveto==4) // Try other direction that leads to T
            {
                if(ty-ry>0)
                {
                    moveto=3;
                    ry++;
                }
                else
                {
                    moveto=1;
                    ry--;
                }
            }
            else
            {
                if(tx-rx>0)
                {
                    moveto=2;
                    rx++;
                }
                else
                {
                    moveto=4;
                    rx--;
                }
            }
            if(world2[rx][ry]=='#'||world2[rx][ry]=='F')
            {
                state=2;
                rx=rxold;
                ry=ryold;
            }
            else
            {
                world2[rxold][ryold]='F';
                state=0;
                go=true;
                stuck=false;
            }
            break;
            
        case 2:
            printf("State 2\n");
            int round=0, bestdirection[2][4], initialmoveto=moveto;
            do{
                rx=rxold;
                ry=ryold;
                if(moveto==2||moveto==4) // Try other direction that doesn't leads to T
                {
                    if(ty-ry<=0)
                    {
                        moveto=3;
                        ry++;
                    }
                    else
                    {
                        moveto=1;
                        ry--;
                    }
                }
                else
                {
                    if(tx-rx<=0)
                    {
                        moveto=2;
                        rx++;
                    }
                    else
                    {
                        moveto=4;
                        rx--;
                    }
                }
                if(round<2)
                {
                    bestdirection[round][1]=moveto;
                    bestdirection[round][2]=rx;
                    bestdirection[round][3]=ry;
                    if((world2[rx][ry]!='#'&&world2[rx][ry]!='F')&&(world2[rx][ry--]=='#'||world2[rx--][ry]=='#'||world2[rx][ry++]=='#'||world2[rx++][ry]=='#'))
                    {
                        bestdirection[round][0]=2;
                    }
                    else if(world2[rx][ry]!='#'&&world2[rx][ry]!='F')
                    {
                        bestdirection[round][0]=1;
                    }
                    else
                    {
                        bestdirection[round][0]=0;
                    }
                }
                round++;
            }while(round<3);
            if(bestdirection[0][0]==0&&bestdirection[1][0]==0)
            {
                state=3;
                moveto=initialmoveto;
            }
            else
            {
                if(bestdirection[0][0]>bestdirection[1][0])
                {
                    moveto=bestdirection[0][1];
                    rx=bestdirection[0][2];
                    ry=bestdirection[0][3];
                }
                else
                {
                    moveto=bestdirection[1][1];
                    rx=bestdirection[1][2];
                    ry=bestdirection[1][3];
                }
                world2[rxold][ryold]='F';
                state=0;
                go=true;
                stuck=false;
            }
            break;
            
        case 3:
            printf("State 3\n");
            if(!stuck)
            {
                printf("Stuck %c\n", world2[rx][ry]);
                stuck=true;
                if(world2[rx][ry]=='#')
                {
                    if(moveto==1)
                    {
                        movetoold=4;
                    }
                    else
                    {
                        movetoold=moveto-1;
                    }
                }
                if(world2[rx][ry]=='F')
                {
                    movetoold=moveto;
                }
            }
            rx=rxold;
            ry=ryold;
            
            switch(movetoold){
                case 1:
                    ry--;
                    break;
                case 2:
                    rx++;
                    break;
                case 3:
                    ry++;
                    break;
                case 4:
                    rx--;
                    break;
            }
            //printf("lastopposidemoveto %i, movetoold %i, place %c\n", lastopposidemoveto, movetoold, world2[rx][ry]);
            if(world2[rx][ry]=='#'||(lastopposidemoveto==movetoold&&failcounter<4))
            {
                rx=rxold;
                ry=ryold;
                if(movetoold==1)
                {
                    movetoold=4;
                }
                else
                {
                    movetoold--;
                }
                failcounter++;
            }
            else
            {
                switch(movetoold){
                case 1:
                    lastopposidemoveto=3;
                    break;
                case 2:
                    lastopposidemoveto=4;
                    break;
                case 3:
                    lastopposidemoveto=1;
                    break;
                case 4:
                    lastopposidemoveto=2;
                    break;
                }
            
                world2[rxold][ryold]='F';
                state=0;
                go=true;
                moveto=movetoold;
                failcounter=0;
            }
            break;
    }
    }while(!go);
    go=false;
    printf("Direction %i\n", moveto);
    return moveto; // REPLACE THE RETURN VALUE WITH YOUR CALCULATED RETURN VALUE
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
        '#','T','O','O','O','O','O','O','O','O','O','#','O','O','O','O','O','O','O','#','\n',
        '#','O','O','#','#','#','#','#','#','#','O','#','O','O','O','O','O','O','O','#','\n',
        '#','#','#','#','O','O','O','O','O','O','O','#','O','O','O','O','O','O','O','#','\n',
        '#','O','O','O','O','#','O','#','#','#','#','#','O','O','O','O','O','O','O','#','\n',
        '#','#','#','#','#','#','O','#','O','O','O','O','O','O','O','O','O','O','O','#','\n',
        '#','O','R','O','O','O','O','#','O','O','O','O','O','O','O','O','O','O','O','#','\n',
        '#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','\n',
    };

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

        movement = move(world);
        robot_index = update_world(movement, world, robot_index, width);
        printf("%s", world);
        printf("%c", '\n');
        step = step+1;
    }
    if(step >= MAX_STEPS) {
        printf("FAILURE, maximum number of steps exceeded.");
    }
    
}
