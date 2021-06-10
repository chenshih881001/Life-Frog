#include "robot_example.h"

#define N 1
#define E 2
#define S 3
#define W 4

int lastDirection[4] = {0,0,0,0}; //Store last 4 moves
int colCount, buffer1, buffer2;
int driveMode = 0;

// IMPLEMET THIS FUNCTION
// ALLOWED RETURN VALUES:
// 1: North, 2: East, 3: South, 4: West, 5: Toggle watern/land mode
int move(char *world) {
  // YOUR CODE HERE
  int Tindex = 0;
  int Rindex = 0;
  int Tx, Ty, Rx, Ry, mvDirection;
// ITERATION 1 - Simple no obstacles - This is all that is needed for the task
  if(!colCount) //Determine the length of the map if not already, only once
  {
    for(int i = 0; i < strlen(world); i++) //Work even if the size of map changes (character arrays only)
    {
      if(world[i] != '\n')
      {
        colCount++;
      }
      else if(world[i] == '\n') //Break this loop if there is a new line
      {
        colCount++;
        break;
      }
    }
    printf("There are %d columns in the map, including newlines.\n", colCount);
  }

  while(world[Tindex] != 'T') //Find the target's index
  {
    Tindex++;
  }

  Ty = Tindex / colCount; // Find the target's coordinates
  Tx = Tindex % colCount;

  while(world[Rindex] != 'R') //Find the robot's index
  {
    Rindex++;
  }

  Ry = Rindex / colCount; // Find the robot's coordinates
  Rx = Rindex % colCount;

  printf("Robot position is (%d, %d) and Target position is (%d, %d)\n", Rx,Ry,Tx,Ty);

  if(Ry != Ty) //If vertical coordinates are not the same
  {
    if (Ry > Ty) //If target is above robot
      mvDirection = N; //Move North
    else if (Ry < Ty) //If target is below Robot
      mvDirection = S; //Move South
  }

  else if(Rx != Tx) //If horizontal coordinates are not the same
  {
    if (Rx < Tx) //If target is to the right of robot
      mvDirection = E; //Move East
    else if (Rx > Tx) //If target is to the left of robot
      mvDirection = W; //Move West
  }
  printf("The initial evaluation of the direction is %d\n", mvDirection);

  //ITERATION 2: With mazes bypass (should work with MOST mazes, especially hallway type mazes, and should be able to bypass traps)
  //it can be stuck in a more open mazes with 2 or more spaces gap, fix by having saving more than 1 last moves. Too complicated to implement for now.
  //ITERATION 3: Trying to fix the problem mentioned above (Should work for most circle traps and 3 spaces T traps,
  //if there is a more complicated T traps then need to rethink how the step saving should be done.)

  switch(mvDirection)
  {
    case N:
      if(lastDirection[0] == E && lastDirection[1] == S && lastDirection[2] == W) //Loop breaker 1 for circle trap (Iteration 3 code)
      {
        if(world[Rindex+colCount] != '#')  //Try moving down instead of up
          mvDirection = S;
        else if (world[Rindex+1] != '#') // If doesn't work try moving east again
          mvDirection = E;
        else if (world[Rindex-1] != '#') // If doesn't work try moving west
          mvDirection = W;
      }

      else if(lastDirection[0] == W && lastDirection[1] == S && lastDirection[2] == E) //Loop breaker 2 for circle trap (Iteration 3 code)
      {
        if(world[Rindex+colCount] != '#')  //Try moving down instead of up
          mvDirection = S;
        else if (world[Rindex-1] != '#') // If doesn't work try moving west again
          mvDirection = W;
        else if (world[Rindex+1] != '#') // If doesn't work try moving east
          mvDirection = E;
      }

      else if(lastDirection[0] == E && lastDirection[1] == W && lastDirection[2] == W && lastDirection[3] == E) //Loop breaker 5 for 3 spaces T traps, won't work if the trap is bigger (Iteration 3 code)
      {
        if(world[Rindex+colCount] != '#')  //Try moving down to break the loop
          mvDirection = S;
      }

      else if(lastDirection[0] == W && lastDirection[1] == E && lastDirection[2] == E && lastDirection[3] == W) //Loop breaker 5 for 3 spaces T traps, won't work if the trap is bigger (Iteration 3 code)
      {
        if(world[Rindex+colCount] != '#')  //Try moving down to break the loop
          mvDirection = S;
      }

      else if(lastDirection[1] == S) // Loop breaker number 4 in case if the direction supposed to be reversed instead (Iteration 3 code)
      {
        if (lastDirection[0] == E || lastDirection [0] == W) // Basically a copy of loop breaker 3 but always test the 2nd last move first
        {
            if(world[Rindex+colCount] != '#')
              mvDirection = lastDirection[1];
            else if(world[Rindex-1] != '#' && lastDirection[0] == W) // Try wiggling left and right. The comparison try the last direction first
              mvDirection = W;
            else if(world[Rindex+1] != '#')
              mvDirection = E;
            else
              mvDirection = N;
        }
        else
        {
          if(world[Rindex+colCount] != '#') //If not work try moving down again
            mvDirection = lastDirection[1];
          else if(Rx >= Tx && world[Rindex-1] != '#') // Try wiggling left and right. The comparison decides the closer direction to try first.
            mvDirection = W;
          else if(world[Rindex+1] != '#')
            mvDirection = E;
          else if(world[Rindex-1] != '#')
            mvDirection = W;
          else
            mvDirection = N;
        }
      }

      else if(lastDirection[0] == S) // If it was going south as the last move
      {
        if (lastDirection[1] == E || lastDirection [1] == W) //Loop breaker 3 for circle loops (Iteration 3 code) basically try to move down before wiggling
        {
            if(world[Rindex+colCount] != '#')
              mvDirection = lastDirection[0];
            else if(world[Rindex-1] != '#' && lastDirection[1] == W) // Try wiggling left and right. The comparison try the 2nd last direction first
              mvDirection = W;
            else if(world[Rindex+1] != '#')
              mvDirection = E;
            else
              mvDirection = N;
        }
        else
        {
          if(Rx >= Tx && world[Rindex-1] != '#') // Try wiggling left and right. The comparison decides the closer direction to try first.
            mvDirection = W;
          else if(world[Rindex+1] != '#')
            mvDirection = E;
          else if(world[Rindex-1] != '#')
            mvDirection = W;
          else if(world[Rindex+colCount] != '#') //If not work try moving down again
            mvDirection = lastDirection[0];
          else
            mvDirection = N;
        }
      }

      else if(world[Rindex-colCount] == '#') // If it was not going south and there is a wall up north
      {
        if(lastDirection[0] == W) // If last move was west
        {
          if(world[Rindex-1] != '#') //Try moving west again
            mvDirection = lastDirection[0];
          else if(world[Rindex+colCount] != '#') //If doesn't work, try moving down
            mvDirection = S;
          else //If doesn't work go back to where it came from
            mvDirection = E;
        }
        else if(lastDirection[0] == E) // Opposite of W basically
        {
          if(world[Rindex+1] != '#')
            mvDirection = lastDirection[0];
          else if(world[Rindex+colCount] != '#')
            mvDirection = S;
          else
            mvDirection = W;
        }
        else //If it was going north as last move
        {
          if(Rx >= Tx && world[Rindex-1] != '#') //try moving left if the horizontal is to the right
            mvDirection = W;
          else if(world[Rindex+1] != '#') // else try moving right if horizontal is closer to the left
            mvDirection = E;
          else if(world[Rindex-1] != '#') //try moving left if both above is false
            mvDirection = W;
          else //Go back down
            mvDirection = S;
        }
      }
      break;
      // The rest of the code is basically the same as case N but with their respective direction

      case S:

      if(lastDirection[0] == E && lastDirection[1] == N && lastDirection[2] == W) //Loop breaker 1 for circle trap (Iteration 3 code)
      {
        if(world[Rindex+colCount] != '#')  //Try moving up instead of down
          mvDirection = N;
        else if (world[Rindex+1] != '#') // If doesn't work try moving east again
          mvDirection = E;
        else if (world[Rindex-1] != '#') // If doesn't work try moving west
          mvDirection = W;
      }

      else if(lastDirection[0] == W && lastDirection[1] == N && lastDirection[2] == E) //Loop breaker 2 for circle trap (Iteration 3 code)
      {
        if(world[Rindex+colCount] != '#')  //Try moving up instead of down
          mvDirection = N;
        else if (world[Rindex-1] != '#') // If doesn't work try moving west again
          mvDirection = W;
        else if (world[Rindex+1] != '#') // If doesn't work try moving east
          mvDirection = E;
      }

      else if(lastDirection[0] == E && lastDirection[1] == W && lastDirection[2] == W && lastDirection[3] == E) //Loop breaker 5 for 3 spaces T traps, won't work if the trap is bigger (Iteration 3 code)
      {
        if(world[Rindex-colCount] != '#')  //Try moving up to break the loop
          mvDirection = N;
      }

      else if(lastDirection[0] == W && lastDirection[1] == E && lastDirection[2] == E && lastDirection[3] == W) //Loop breaker 5 for 3 spaces T traps, won't work if the trap is bigger (Iteration 3 code)
      {
        if(world[Rindex-colCount] != '#')  //Try moving up to break the loop
          mvDirection = N;
      }

      else if(lastDirection[1] == N) // Loop breaker number 4 in case if the direction supposed to be reversed instead (Iteration 3 code)
      {
        if (lastDirection[0] == E || lastDirection [0] == W) // Basically a copy of loop breaker 3 but always test the 2nd last move first
        {
            if(world[Rindex-colCount] != '#')
              mvDirection = lastDirection[1];
            else if(world[Rindex-1] != '#' && lastDirection[0] == W) // Try wiggling left and right. The comparison try the last direction first
              mvDirection = W;
            else if(world[Rindex+1] != '#')
              mvDirection = E;
            else
              mvDirection = S;
        }
        else
        {
          if(world[Rindex-colCount] != '#') //If not work try moving down again
            mvDirection = lastDirection[1];
          else if(Rx >= Tx && world[Rindex-1] != '#') // Try wiggling left and right. The comparison decides the closer direction to try first.
            mvDirection = W;
          else if(world[Rindex+1] != '#')
            mvDirection = E;
          else if(world[Rindex-1] != '#')
            mvDirection = W;
          else
            mvDirection = S;
        }
      }

      else if(lastDirection[0] == N) // If it was going North as the last move
        {
          if (lastDirection[1] == E || lastDirection [1] == W) //Loop breaker 3 for circle loops (Iteration 3 code) basically try to move up before wiggling
          {
              if(world[Rindex-colCount] != '#')
                mvDirection = lastDirection[0];
              else if(world[Rindex-1] != '#' && lastDirection[1] == W) // Try wiggling left and right. The comparison try the 2nd last direction first
                mvDirection = W;
              else if(world[Rindex+1] != '#')
                mvDirection = E;
              else
                mvDirection = S;
          }
          else
          {
            if(Rx >= Tx && world[Rindex-1] != '#') // Try wiggling left and right. The comparison decides the closer direction to try first.
              mvDirection = W;
            else if(world[Rindex+1] != '#')
              mvDirection = E;
            else if(world[Rindex-1] != '#')
              mvDirection = W;
            else if(world[Rindex-colCount] != '#') //If not work try moving up again
              mvDirection = lastDirection[0];
            else
              mvDirection = S;
          }
        }

        else if(world[Rindex+colCount] == '#') // If it was not going north and there is a wall down south
        {
          if(lastDirection[0] == W) // If last move was west
          {
            if(world[Rindex-1] != '#') //Try moving west again
              mvDirection = lastDirection[0];
            else if(world[Rindex-colCount] != '#') //If doesn't work, try moving down
              mvDirection = N;
            else //If doesn't work go back to where it came from
              mvDirection = E;
          }
          else if(lastDirection[0] == E) // Opposite of W basically
          {
            if(world[Rindex+1] != '#')
              mvDirection = lastDirection[0];
            else if(world[Rindex-colCount] != '#')
              mvDirection = N;
            else
              mvDirection = W;
          }
          else //If it was going south as last move
          {
            if(Rx >= Tx && world[Rindex-1] != '#') //try moving left if the horizontal is to the right
              mvDirection = W;
            else if(world[Rindex+1] != '#') // else try moving right if horizontal is closer to the left
              mvDirection = E;
            else if(world[Rindex-1] != '#') //try moving left if both above is false
              mvDirection = W;
            else //Go back down
              mvDirection = N;
          }
        }
        break;

        case E:

        if(lastDirection[0] == N && lastDirection[1] == W && lastDirection[2] == S) //Loop breaker 1 for circle trap (Iteration 3 code)
        {
          if(world[Rindex-1] != '#')  //Try moving west instead of east
            mvDirection = W;
          else if (world[Rindex-colCount] != '#') // If doesn't work try moving North again
            mvDirection = N;
          else if (world[Rindex+colCount] != '#') // If doesn't work try moving South
            mvDirection = S;
        }

        else if(lastDirection[0] == S && lastDirection[1] == W && lastDirection[2] == N) //Loop breaker 2 for circle trap (Iteration 3 code)
        {
          if(world[Rindex-1] != '#')  //Try moving west instead of east
            mvDirection = W;
          else if (world[Rindex+colCount] != '#') // If doesn't work try moving South again
            mvDirection = S;
          else if (world[Rindex-colCount] != '#') // If doesn't work try moving North
            mvDirection = N;
        }
        else if(lastDirection[0] == N && lastDirection[1] == S && lastDirection[2] == S && lastDirection[3] == N) //Loop breaker 5 for 3 spaces T traps, won't work if the trap is bigger (Iteration 3 code)
        {
          if(world[Rindex-1] != '#')  //Try moving west to break the loop
            mvDirection = W;
        }

        else if(lastDirection[0] == S && lastDirection[1] == N && lastDirection[2] == N && lastDirection[3] == S) //Loop breaker 5 for 3 spaces T traps, won't work if the trap is bigger (Iteration 3 code)
        {
          if(world[Rindex-1] != '#')  //Try moving up to break the loop
            mvDirection = W;
        }
        else if(lastDirection[1] == W) // Loop breaker number 4 in case if the direction supposed to be reversed instead
        {
          if (lastDirection[0] == N || lastDirection [0] == S) //Loop breaker 3 for circle loops (Iteration 3 code) basically try to go west before wiggling
          {
              if(world[Rindex-1] != '#')
                mvDirection = lastDirection[1];
              else if(world[Rindex-colCount] != '#' && lastDirection[0] == N) // Try wiggling Up and Down. The comparison try the last direction first
                mvDirection = N;
              else if(world[Rindex+colCount] != '#')
                mvDirection = S;
              else
                mvDirection = E;
          }
          else
          {
              if(world[Rindex-1] != '#')
                mvDirection = lastDirection[1];
              else if(Ry >= Ty && world[Rindex-colCount] != '#') // Try wiggling Up and Down. The comparison decides the closer direction to try first.
                mvDirection = N;
              else if(world[Rindex+colCount] != '#')
                mvDirection = S;
              else if(world[Rindex-colCount] != '#')
                mvDirection = N;
              else
                mvDirection = E;
          }
        }

        else if(lastDirection[0] == W) // If it was going west as the last move
          {
            if (lastDirection[1] == N || lastDirection [1] == S) //Loop breaker 3 for circle loops (Iteration 3 code) basically try to go west before wiggling
            {
                if(world[Rindex-1] != '#')
                  mvDirection = lastDirection[0];
                else if(world[Rindex-colCount] != '#' && lastDirection[1] == N) // Try wiggling Up and Down. The comparison try the 2nd last direction first
                  mvDirection = N;
                else if(world[Rindex+colCount] != '#')
                  mvDirection = S;
                else
                  mvDirection = E;
            }
            else
            {
                if(Ry >= Ty && world[Rindex-colCount] != '#') // Try wiggling Up and Down. The comparison decides the closer direction to try first.
                  mvDirection = N;
                else if(world[Rindex+colCount] != '#')
                  mvDirection = S;
                else if(world[Rindex-colCount] != '#')
                  mvDirection = N;
                else if(world[Rindex-1] != '#') //If not work try moving west again
                  mvDirection = lastDirection[0];
                else
                  mvDirection = E;
            }
          }

          else if(world[Rindex+1] == '#') // If it was not going west and there is a wall east
          {
            if(lastDirection[0] == N) // If last move was north
            {
              if(world[Rindex-colCount] != '#') //Try moving north again
                mvDirection = lastDirection[0];
              else if(world[Rindex-1] != '#') //If doesn't work, try moving west
                mvDirection = W;
              else //If doesn't work go back to where it came from
                mvDirection = S;
            }
            if(lastDirection[0] == S) // Opposite of North
            {
              if(world[Rindex+colCount] != '#')
                mvDirection = lastDirection[0];
              else if(world[Rindex-1] != '#') //If doesn't work, try moving west
                mvDirection = W;
              else //If doesn't work go back to where it came from
                mvDirection = N;
            }
            else //If it was going east as last move
            {
              if(Ry >= Ty && world[Rindex-colCount] != '#') //try moving north first if robot is below
                mvDirection = N;
              else if(world[Rindex+colCount] != '#') //try moving south
                mvDirection = S;
              else if(world[Rindex-colCount] != '#') //try moving north again if both fail
                mvDirection = N;
              else //Go back west
                mvDirection = W;
            }
          }
          break;

          case W:

          if(lastDirection[0] == N && lastDirection[1] == E && lastDirection[2] == S) //Loop breaker 1 for circle trap (Iteration 3 code)
          {
            if(world[Rindex+1] != '#')  //Try moving east instead of west
              mvDirection = E;
            else if (world[Rindex-colCount] != '#') // If doesn't work try moving North again
              mvDirection = N;
            else if (world[Rindex+colCount] != '#') // If doesn't work try moving South
              mvDirection = S;
          }

          else if(lastDirection[0] == S && lastDirection[1] == E && lastDirection[2] == N) //Loop breaker 2 for circle trap (Iteration 3 code)
          {
            if(world[Rindex-1] != '#')  //Try moving east instead of west
              mvDirection = E;
            else if (world[Rindex+colCount] != '#') // If doesn't work try moving South again
              mvDirection = S;
            else if (world[Rindex-colCount] != '#') // If doesn't work try moving North
              mvDirection = N;
          }

          else if(lastDirection[0] == N && lastDirection[1] == S && lastDirection[2] == S && lastDirection[3] == N) //Loop breaker 5 for 3 spaces T traps, won't work if the trap is bigger (Iteration 3 code)
          {
            if(world[Rindex+1] != '#')  //Try moving west to break the loop
              mvDirection = E;
          }

          else if(lastDirection[0] == S && lastDirection[1] == N && lastDirection[2] == N && lastDirection[3] == S) //Loop breaker 5 for 3 spaces T traps, won't work if the trap is bigger (Iteration 3 code)
          {
            if(world[Rindex+1] != '#')  //Try moving up to break the loop
              mvDirection = E;
          }

          else if(lastDirection[1] == E) // Loop breaker number 4 in case if the direction supposed to be reversed instead
          {
            if (lastDirection[0] == N || lastDirection [0] == S) //Loop breaker 3 for circle loops (Iteration 3 code) basically try to go east before wiggling
            {
                if(world[Rindex+1] != '#')
                  mvDirection = lastDirection[1];
                else if(world[Rindex-colCount] != '#' && lastDirection[0] == N) // Try wiggling Up and Down. The comparison try the last direction first
                  mvDirection = N;
                else if(world[Rindex+colCount] != '#')
                  mvDirection = S;
                else
                  mvDirection = W;
            }
            else
            {
                if(world[Rindex+1] != '#')
                  mvDirection = lastDirection[1];
                else if(Ry >= Ty && world[Rindex-colCount] != '#') // Try wiggling Up and Down. The comparison decides the closer direction to try first.
                  mvDirection = N;
                else if(world[Rindex+colCount] != '#')
                  mvDirection = S;
                else if(world[Rindex-colCount] != '#')
                  mvDirection = N;
                else
                  mvDirection = W;
            }
          }

          else if(lastDirection[0] == E) // If it was going east as the last move
            {
              if (lastDirection[1] == N || lastDirection [1] == S) //Loop breaker 3 for circle loops (Iteration 3 code) basically try to go east before wiggling
              {
                  if(world[Rindex+1] != '#')
                    mvDirection = lastDirection[0];
                  else if(world[Rindex-colCount] != '#' && lastDirection[1] == N) // Try wiggling Up and Down. The comparison try the 2nd last direction first
                    mvDirection = N;
                  else if(world[Rindex+colCount] != '#')
                    mvDirection = S;
                  else
                    mvDirection = W;
              }
              else
              {
                if(Ry >= Ty && world[Rindex-colCount] != '#') // Try wiggling Up and Down. The comparison decides the closer direction to try first.
                  mvDirection = N;
                else if(world[Rindex+colCount] != '#')
                  mvDirection = S;
                else if(world[Rindex-colCount] != '#')
                  mvDirection = N;
                else if(world[Rindex+1] != '#') //If not work try moving east again
                  mvDirection = lastDirection[0];
                else
                  mvDirection = W;
              }

            }

            else if(world[Rindex-1] == '#') // If it was not going east and there is a wall west
            {
              if(lastDirection[0] == N) // If last move was north
              {
                if(world[Rindex-colCount] != '#') //Try moving north again
                  mvDirection = lastDirection[0];
                else if(world[Rindex+1] != '#') //If doesn't work, try moving east
                  mvDirection = E;
                else //If doesn't work go back to where it came from
                  mvDirection = S;
              }
              if(lastDirection[0] == S) // Opposite of North
              {
                if(world[Rindex+colCount] != '#')
                  mvDirection = lastDirection[0];
                else if(world[Rindex+1] != '#') //If doesn't work, try moving east
                  mvDirection = E;
                else //If doesn't work go back to where it came from
                  mvDirection = N;
              }
              else //If it was going west as last move
              {
                if(Ry >= Ty && world[Rindex-colCount] != '#') //try moving north first if robot is below
                  mvDirection = N;
                else if(world[Rindex+colCount] != '#') //try moving south
                  mvDirection = S;
                else if(world[Rindex-colCount] != '#') //try moving north again if both fail
                  mvDirection = N;
                else //Go back west
                  mvDirection = E;
              }
            }
            break;
  }

  //Toggle for land and water
    if(mvDirection == N)
    {
      if (world[Rindex-colCount] == '~' && !driveMode)
      {
        printf("Water detected!\n");
        mvDirection = 5;
        driveMode = !driveMode;
      }
      else if (world[Rindex-colCount] == 'O' && driveMode)
      {
        printf("Land detected!\n");
        mvDirection = 5;
        driveMode = !driveMode;
      }
    }

    else if(mvDirection == S)
    {
      if (world[Rindex+colCount] == '~' && !driveMode)
      {
        printf("Water detected!\n");
        mvDirection = 5;
        driveMode = !driveMode;
      }
      else if (world[Rindex+colCount] == 'O' && driveMode)
      {
        printf("Land detected!\n");
        mvDirection = 5;
        driveMode = !driveMode;
      }
    }

    else if(mvDirection == E)
    {
      if (world[Rindex+1] == '~' && !driveMode)
      {
        printf("Water detected!\n");
        mvDirection = 5;
        driveMode = !driveMode;
      }
      else if (world[Rindex+1] == 'O' && driveMode)
      {
        printf("Land detected!\n");
        mvDirection = 5;
        driveMode = !driveMode;
      }
    }

    else if(mvDirection == W)
    {
      if (world[Rindex-1] == '~' && !driveMode)
      {
        printf("Water detected!\n");
        mvDirection = 5;
        driveMode = !driveMode;
      }
      else if (world[Rindex-1] == 'O' && driveMode)
      {
        printf("Land detected!\n");
        mvDirection = 5;
        driveMode = !driveMode;
      }
    }


  printf("The final direction is %d\n", mvDirection);

  for(int i = 0; i < 4; i++) //For loop to cycle and save the last 4 moves
  {
    if(mvDirection == 5)
      break;
    else if(i == 0)
    {
      buffer1 = lastDirection[i];
      lastDirection[i] = mvDirection;
    }
    else if(i != 3)
    {
      buffer2 = lastDirection[i];
      lastDirection[i] = buffer1;
      buffer1 = buffer2;
    }
    else
      lastDirection[i] = buffer1;
  }
  printf("The last 4 steps was: %d, %d, %d, %d\n", lastDirection[0], lastDirection[1], lastDirection[2], lastDirection[3]);
  return mvDirection; // REPLACE THE RETURN VALUE WITH YOUR CALCULATED RETURN VALUE
}
