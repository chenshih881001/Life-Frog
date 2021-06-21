#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <float.h>
#include "robot_Lifefrog_E.h"

#define max_row 8
#define max_col 20
#define MAX_STEPS 200

#define N 1
#define E 2
#define S 3
#define W 4
#define MODE_CHANGE 5
#define DES_N 6
#define DES_E 7
#define DES_S 8
#define DES_W 9

#defind land 0
#defind water 1

// IMPLEMENT THIS FUNCTION
// ALLOWED RETURN VALUES:
// 1: North, 2: East, 3: South, 4: West, 5: Toggle watern/land mode


//Path modes:
//1. Only moves on O
//2. Moves on O and ~
//3. Moves on O and *
//4. Moves on O, ~ and *
//Energy are calculated later to find the shortest path with lowest energy

char map[max_row][max_col];

int current_id = -1;
int c = 0;

int origin_row, origin_col, target_row, target_col, path_mode = 1, cnt_pth1, cnt_pth2, cnt_pth3, cnt_pth4, cnt_path, energy1, energy2, energy3, energy4;

int path_1[MAX_STEPS] = {0}, path_2[MAX_STEPS] = {0}, path_3[MAX_STEPS] = {0}, path_4[MAX_STEPS] = {0}, path_f[MAX_STEPS];  //Initiallizing diferrent paths and path_f is the final path

bool p1, p2, p3, p4, target_reached, returned, map_init, path_init; //Bunch of flags. The p's are for checking whether there is a path found for that path mode

int drive_mode = land, *mvDir;

struct pair //creating a struct for pair of coordinates for ease of usage
{
  int row;
  int col;
};

struct pPair  //creating a struct for a pair of total cost coordinates for the open list
{
  double f;
  struct pair coord;
};

struct nodes			//Creating struct for the nodes;
{
	int parent_row, parent_col;	//Row and column position of the parents
	double f, g, h;			//f = sum of cost, g = movement cost from starting point to given cell, h = estimated movement cost from current cell to destination
  char travel_dir;
};

struct pair origin, target, coord1[MAX_STEPS], coord2[MAX_STEPS], coord3[MAX_STEPS], coord4[MAX_STEPS]; //Origin - Robot coordinates | Target - target coordinates | Coords - to use to compare energy later

double heuristic_calc(int row, int col, struct pair target)		//Calculate the approximate Euclidean Distance
{
  double h = sqrt(pow((row - target.row), 2.0) + pow((col - target.col), 2.0)); //Euclidean Distance formula
  return h;
}

bool isValid(int row, int col)  //Check to see if cell position is valid
{
  if (row >= 0 && row < max_row && col >= 0 && row < max_col)
    return true;
  else
    return false;
}

bool isDestination(int row, int col, struct pair target)  //check to see if it is the destination
{
  if (row == target.row && col == target.col)
    return true;
  else
    return false;
}

bool isOpen(char map[][max_col], int row, int col)  //check to see if the node is an obstacle or not
{
  if(path_mode == 1)  //No obstacles or water
  {
    if(map[row][col]=='O' || map[row][col]=='T' || map[row][col]=='t' || map[row][col]=='X')
      return true;
    else
      return false;
  }

  else if(path_mode == 2) //No obstacles
  {
    if(map[row][col]=='O' || map[row][col]=='T' || map[row][col]=='t' || map[row][col]=='X' || map[row][col]=='~')
      return true;
    else
      return false;
  }

  else if(path_mode == 3) //No water
  {
    if(map[row][col]=='O' || map[row][col]=='T' || map[row][col]=='t' || map[row][col]=='X' || map[row][col]=='*')
      return true;
    else
      return false;
  }

  else if(path_mode == 4) //All a go
  {
    if(map[row][col]=='O' || map[row][col]=='T' || map[row][col]=='t' || map[row][col]=='X' || map[row][col]=='~' || map[row][col]=='*')
      return true;
    else
      return false;
  }
}

void sort(struct pPair *openList, int list_count) //Sort the open list for the lowest f to be tested first
{
  int index_tmp;
  bool smaller_found = false;
  struct pPair tmp_1, tmp_2;                // creating tmp structs
  for(int x = 0; x < list_count; x++)       // first loop is for index
  {
    tmp_1 = openList[x];                    // assign the tmp variable as the current open list member for comparison later
    for (int y = x; y < list_count; y++)    // 2nd for loop for comparison of all element
    {
      if(openList[x].f > openList[y].f && tmp_1.f > openList[y].f)  // If a smaller f is found
      {
        tmp_1 = openList[y];                                        // assign the tmp as that member
        index_tmp = y;                                              // save the index for later
        smaller_found = true;                                       // trip the flag
      }
    }
    if(smaller_found) //Swap the values if a smaller f found
    {
      tmp_2 = openList[x];    //save the current in tmp_2
      openList[x] = tmp_1;    //assign the current as the smaller value
      openList[index_tmp] = tmp_2;  //assign the other index that the current value
      smaller_found = false;
    }
  }
/*  printf("The values of the open list so far after sorting: ");
  for (int a = 0; a < list_count; a++)
  {
    printf("(f: %.3f, r: %d, c: %d), ", openList[a].f, openList[a].coord.row, openList[a].coord.col);
  }
  printf("\n"); */
}

void shift_forward(struct pPair *openList, int list_count)  //For shifting the stack forward and removing the first part of the stack
{
  for(int x = 0; x < list_count; x++)
  {
    if(x != list_count - 1) //if not the last element
      openList[x] = openList[x+1];  //copy the next value into it
    else  //if last value just assign an invalid value so that it can be ignored
    {
        openList[x].f = FLT_MAX;
        openList[x].coord.row = -1;
        openList[x].coord.col = -1;
    }
  }
  /* printf("The values of the open list so far after shifting forward: ");
  for (int a = 0; a < list_count; a++)
  {
    printf("(f: %.3f, r: %d, c: %d), ", openList[a].f, openList[a].coord.row, openList[a].coord.col);
  }
  printf("\n"); */
  return;
}

void shift_backward(struct pPair *openList, int list_count) //For shifting the whole stack backward and adding a new part to the stack
{
  struct pPair tmp_1, tmp_2;                //creating tmp structs
  tmp_1.f = 0.0;        //assign tmp_1 as a blank canvas
  tmp_1.coord.row = 0;
  tmp_1.coord.col = 0;
  for(int x = 0; x < list_count; x++)
  {
    tmp_2 = openList[x];  //Usual switcharoo
    openList[x] = tmp_1;
    tmp_1 = tmp_2;
  }
  /* printf("The values of the open list so far after shifting backward: ");
  for (int a = 0; a < list_count; a++)
  {
    printf("(f: %.3f, r: %d, c: %d), ", openList[a].f, openList[a].coord.row, openList[a].coord.col);
  }
  printf("\n"); */
  return;
}

void draw_path(struct nodes node[][max_col], struct pair target)  //Collecting tha path that was created using A*
{
  printf("\n Path %d is ", path_mode);
  int row = target.row;
  int col = target.col;
  int count = 0;
  struct pair path[MAX_STEPS];
  char moves[MAX_STEPS];
  memset(moves, ' ', MAX_STEPS);

  while(!(node[row][col].parent_row == row && node[row][col].parent_col == col))  //If not traced back to the origin
  {
    path[count].row = row;    //Copy the row and column
    path[count].col = col;
    moves[count] = node[row][col].travel_dir; //Copy the direction
    count++;  //increase the counter
    int temp_row = node[row][col].parent_row; //Copying the parents of this node
    int temp_col = node[row][col].parent_col;
    row = temp_row; //Replace
    col = temp_col;
  }
  path[count].row = row;  //For the origin
  path[count].col = col;
  moves[count] = node[row][col].travel_dir;
  for(int i = count; i >= 0; i--)
    printf("-> (%d,%d, %c) ", path[i].row, path[i].col, moves[i]);
  printf("\n");

  int steps = 0;
  int true_steps = 0;

  for(int i = 0; i <= count; i++)   //count the number of steps taken
  {
    if(moves[i] != ' ')
    {
      steps++;
    }

    if(moves[i] != ' ' && moves[i] != 'O')  // Ignoring the origin
    {
      true_steps++;
    }
  }

  printf("The number of steps is %d, and discounting the origin: %d\n", steps, true_steps);

  struct pair tmp_path[true_steps];

  int j = count - 1;  //the last count is the origin so ignore that
  for(int i = 0; i < true_steps; i++) //loop to copy the flipped coordinates
  {
    if(j >= 0)
    {
      tmp_path[i] = path[j];
  //    printf("%d, %d\n", tmp_path[i].row, tmp_path[i].col);
      j--;
    }
  }

  int tmp_moves[true_steps];
  memset(tmp_moves, 0, sizeof(tmp_moves)/sizeof(int));

  int tmp = 0;  //Extra variable to bypass the 'O'/Origin point

  for(int j = count; j >= 0; j--)  //Convert the char value of the path into int while also flipping the list since it is flipped
  {
    if(moves[j] == 'N')
      {
        tmp_moves[tmp] = N;
        tmp++;
      }
    else if(moves[j] == 'E')
      {
        tmp_moves[tmp] = E;
        tmp++;
      }
    else if(moves[j] == 'S')
      {
        tmp_moves[tmp] = S;
        tmp++;
      }
    else if(moves[j] == 'S')
      {
        tmp_moves[tmp] = W;
        tmp++;
      }
    else
      continue;
  }

  if(path_mode == 1) //If it is 1st path condition
  {
    memcpy(path_1, tmp_moves, sizeof(tmp_moves));       //copy the content of tmp_moves into the path
    memcpy(coord1, tmp_path, sizeof(tmp_path));         //copy the coordinates after each step
    cnt_pth1 = true_steps;    //copy the step counters
    for(int i = 0; i < 200; i++)
/*      printf("%d ,(%d, %d); ", path_1[i], coord1[i].row, coord1[i].col);
    printf("\n"); */
    p1 = true;
  }

  else if(path_mode == 2) //If it is 2nd path condition
  {
    memcpy(path_2, tmp_moves, sizeof(tmp_moves));
    memcpy(coord2, tmp_path, sizeof(tmp_path));
    cnt_pth2 = true_steps;
    p2 = true;
  }

  else if(path_mode == 3) //If it is 3rd path condition
  {
    memcpy(path_3, tmp_moves, sizeof(tmp_moves));
    memcpy(coord3, tmp_path, sizeof(tmp_path));
    cnt_pth3 = true_steps;
    p3 = true;
  }

  else if(path_mode == 4) //If it is 4th path condition
  {
    memcpy(path_3, tmp_moves, sizeof(tmp_moves));
    memcpy(coord4, tmp_path, sizeof(tmp_path));
    cnt_pth3 = true_steps;
    p4 = true;
  }

  return;
}

void a_star(char map[][max_col], struct pair origin, struct pair target)  //The A* algo itself
{
  bool closedList[max_row][max_col];  //Creating a closed list for all nodes that has been visited
  memset(closedList, false, sizeof(closedList));  //Initially set all of the nodes as not visited

  struct nodes node[max_row][max_col]; //Initiallize the nodes with in the map

  int turn_count = 1; // Turn counter (For debugging)
  int i, j; //Variable for ease of typing i = row, j = column

  for (i = 0; i<max_row; i++) // Assign all of the node
  {
    for (j = 0; j<max_col; j++) //Basically just to Initiallize them
    {
      node[i][j].f = FLT_MAX; //inf f
      node[i][j].g = FLT_MAX; //inf g
      node[i][j].h = FLT_MAX; //inf h
      node[i][j].parent_row = -1; //invalid parent row
      node[i][j].parent_col = -1; //invalid parent column
      node[i][j].travel_dir = 'U';//unknown travel direction
    }
  }

  i = origin.row, j = origin.col; //Make i and j the origin

  node[i][j].f = 0.0;//reset the origin basically
  node[i][j].g = 0.0;
  node[i][j].h = 0.0;
  node[i][j].parent_row = i;
  node[i][j].parent_col = j;
  node[i][j].travel_dir = 'O';

  int list_count = 0; //Initiallize a counter for the open nodes that need to be tested
  struct pPair openList[max_col * max_row]; //Initiallize an list of open nodes
  //Make the first in the list the origin node
  openList[list_count].f = node[i][j].f;
  openList[list_count].coord.row = origin.row;
  openList[list_count].coord.col = origin.col;
  list_count++;

  bool target_located = false; // variable to check if target is located

  while(list_count) //while there are still nodes that need to be check
  {
    /*printf("___________________________________________________________________________________________________");
    printf("\nTurn %d, nodes to be checked %d: \n", turn_count, list_count);


    printf("The values of the open list so far at start of turn: ");
    for (int a = 0; a < list_count; a++)
    {
      printf("(f: %.3f, r: %d, c: %d), ", openList[a].f, openList[a].coord.row, openList[a].coord.col);
    }
    printf("\n");*/


    struct pPair tmp = openList[0]; //copy the 1st index of the openList to tmp
    shift_forward(openList, list_count);  //use up the first element of the list
    list_count--; //subtract the count by 1
    i = tmp.coord.row;  //assign the i and j as the node that need to be tested
    j = tmp.coord.col;

    //printf("Testing row: %d, col: %d.\n", i,j);

        // To store the 'g', 'h' and 'f' of the 4 neighbours
        double gNew, hNew, fNew;

        //----------- Northern neighbour ------------

        if (isValid(i - 1, j))    //Check the validity of the node
        {
            if (isDestination(i - 1, j, target))  //Check the target node is the destination
            {
                node[i - 1][j].parent_row = i;     // Set the parent of the destination node
                node[i - 1][j].parent_col = j;
                node[i - 1][j].travel_dir = 'N';
                printf("The target is found\n");
                draw_path(node, target);  //Collect the path
                target_located = true;    // Set that a path is found
                return;
            }

            else if (!closedList[i - 1][j] && isOpen(map, i - 1, j))  //If the node is not closed and is on not blocked
            {
                gNew = node[i][j].g + 1.0;                            //Calculate the g (always 1 more than the previous), h and f.
                hNew = heuristic_calc(i - 1, j, target);
                fNew = gNew + hNew;

                if (node[i - 1][j].f == FLT_MAX || node[i - 1][j].f > fNew)     //If it is not on the open list or the f is better in this direction
                {
                //    printf("///Neighbour found in the north direction!(%d, %d)\n", i-1, j);
                    list_count++;
                    shift_backward(openList, list_count);
                    openList[0].f = fNew;
                    openList[0].coord.row = i - 1;
                    openList[0].coord.col = j;

                  /*  printf("The values of the open list so far after adding a new neighbour: ");
                    for (int a = 0; a < list_count; a++)
                    {
                      printf("(f: %.3f, r: %d, c: %d), ", openList[a].f, openList[a].coord.row, openList[a].coord.col);
                    }
                    printf("\n"); */

                    // Update the details of this cell
                    node[i - 1][j].f = fNew;
                    node[i - 1][j].g = gNew;
                    node[i - 1][j].h = hNew;
                    node[i - 1][j].parent_row = i;
                    node[i - 1][j].parent_col = j;
                    node[i - 1][j].travel_dir = 'N';
                }
            }
        }

        //----------- Southern neighbour ------------

        // Only process this cell if this is a valid one
        if (isValid(i + 1, j))
        {
            if (isDestination(i + 1, j, target))
            {
                // Set the Parent of the destination cell
                node[i + 1][j].parent_row = i;
                node[i + 1][j].parent_col = j;
                node[i + 1][j].travel_dir = 'S';
                printf("The target is found\n");
                draw_path(node, target);
                target_located = true;
                return;
            }

            else if (!closedList[i + 1][j] && isOpen(map, i + 1, j))
            {
                gNew = node[i][j].g + 1.0;
                hNew = heuristic_calc(i + 1, j, target);
                fNew = gNew + hNew;

                if (node[i + 1][j].f == FLT_MAX || node[i + 1][j].f > fNew)
                {
                    //printf("///Neighbour found in the south direction!(%d, %d)\n", i+1, j);
                    list_count++;
                    shift_backward(openList, list_count);
                    openList[0].f = fNew;
                    openList[0].coord.row = i + 1;
                    openList[0].coord.col = j;

                    /*printf("The values of the open list so far after adding a new neighbour: ");
                    for (int a = 0; a < list_count; a++)
                    {
                      printf("(f: %.3f, r: %d, c: %d), ", openList[a].f, openList[a].coord.row, openList[a].coord.col);
                    }
                    printf("\n");*/

                    // Update the details of this cell
                    node[i + 1][j].f = fNew;
                    node[i + 1][j].g = gNew;
                    node[i + 1][j].h = hNew;
                    node[i + 1][j].parent_row = i;
                    node[i + 1][j].parent_col = j;
                    node[i + 1][j].travel_dir = 'S';
                }
            }
        }

        //----------- Eastern neighbour ------------
        if (isValid(i, j + 1))
        {
            if (isDestination(i, j + 1, target))
            {
                node[i][j + 1].parent_row = i;
                node[i][j + 1].parent_col = j;
                node[i][j + 1].travel_dir = 'E';
                printf("The target is found\n");
                draw_path(node, target);
                target_located = true;
                return;
            }
            else if (!closedList[i][j + 1] && isOpen(map, i, j + 1))
            {
                gNew = node[i][j].g + 1.0;
                hNew = heuristic_calc(i, j + 1, target);
                fNew = gNew + hNew;

                if (node[i][j + 1].f == FLT_MAX || node[i][j + 1].f > fNew)
                {
              //      printf("///Neighbour found in the east direction!(%d, %d)\n", i, j+1);
                    list_count++;
                    shift_backward(openList, list_count);
                    openList[0].f = fNew;
                    openList[0].coord.row = i;
                    openList[0].coord.col = j + 1;

            /*        printf("The values of the open list so far after adding a new neighbour: ");
                    for (int a = 0; a < list_count; a++)
                    {
                      printf("(f: %.3f, r: %d, c: %d), ", openList[a].f, openList[a].coord.row, openList[a].coord.col);
                    }
                    printf("\n"); */

                    // Update the details of this cell
                    node[i][j + 1].f = fNew;
                    node[i][j + 1].g = gNew;
                    node[i][j + 1].h = hNew;
                    node[i][j + 1].parent_row = i;
                    node[i][j + 1].parent_col = j;
                    node[i][j + 1].travel_dir = 'E';
                }
            }
        }

        //----------- Western neighbour ------------
        if (isValid(i, j - 1))
        {
            if (isDestination(i, j - 1, target))
            {
                node[i][j - 1].parent_row = i;
                node[i][j - 1].parent_col = j;
                node[i][j - 1].travel_dir = 'W';
                printf("The target is found\n");
                draw_path(node, target);
                target_located = true;
                return;
            }
            else if (!closedList[i][j - 1] && isOpen(map, i, j - 1))
            {
                gNew = node[i][j].g + 1.0;
                hNew = heuristic_calc(i, j - 1, target);
                fNew = gNew + hNew;

                if (node[i][j - 1].f == FLT_MAX || node[i][j - 1].f > fNew)
                {
        //            printf("///Neighbour found in the west direction!(%d, %d)\n", i, j-1);
                    list_count++;
                    shift_backward(openList, list_count);
                    openList[0].f = fNew;
                    openList[0].coord.row = i;
                    openList[0].coord.col = j - 1;

        /*            printf("The values of the open list so far after adding a new neighbour: ");
                    for (int a = 0; a < list_count; a++)
                    {
                      printf("(f: %.3f, r: %d, c: %d), ", openList[a].f, openList[a].coord.row, openList[a].coord.col);
                    }
                    printf("\n"); */

                    // Update the details of this cell
                    node[i][j - 1].f = fNew;
                    node[i][j - 1].g = gNew;
                    node[i][j - 1].h = hNew;
                    node[i][j - 1].parent_row = i;
                    node[i][j - 1].parent_col = j;
                    node[i][j - 1].travel_dir = 'W';
                }
            }
        }
    //Sort the open list here
/*    printf("The values of the open list so far before sorting: ");
    for (int a = 0; a < list_count; a++)
    {
      printf("(f: %.3f, r: %d, c: %d), ", openList[a].f, openList[a].coord.row, openList[a].coord.col);
    }
    printf("\n"); */

    sort(openList, list_count);
    turn_count++;
  }
  if (target_located == false)
        printf("Failed to find the target\n");

    return;
}

void copy_map(char *world)
{
  int index = 0;
  for(int i = 0; i < max_row; i++)
  {
    for(int j = 0; j < max_col; j++)
    {
      if(world[index] != '\n')
      {
        map[i][j] = world[index];
        index++;
      }
      else
      {
        map[i][j] = world[++index];
        index++;
      }
    }
  }
  return;
}

void print_map()
{
  printf("My map: \n");
  for(int i = 0; i < max_row; i++)
  {
    for(int j = 0; j < max_col; j++)
    {
      printf("%c", map[i][j]);
    }
    printf("\n");
  }
  return;
}

void indexing_interests()
{
  for(int i = 0; i < max_row; i++)
  {
    for(int j = 0; j < max_col; j++)
    {
      if(map[i][j] == 'T' || map[i][j] == 't')
      {
        target_row = i;
        target_col = j;
      }
      if(map[i][j] == 'R')
      {
        origin_row = i;
        origin_col = j;
      }
    }
  }
  printf("Position of Robot is: r-%d, c-%d, and target is: r-%d, c-%d\n", origin_row, origin_col, target_row, target_col);
  return;
}

int estimate_cost(struct pair *coords, int i)
{
  if(map[coords[i].row][coords[i].col] == 'O' || map[coords[i].row][coords[i].col] == 'T' || map[coords[i].row][coords[i].col] == 'R' || map[coords[i].row][coords[i].col] == 'X')
  {
    if(map[coords[i-1].row][coords[i-1].col] == '~' || map[coords[i-1].row][coords[i-1].col] == 't')
      return 40;
    else
      return 10;
  }

  else if(map[coords[i].row][coords[i].col] == '~' || map[coords[i].row][coords[i].col] == 't')
  {
    if(map[coords[i-1].row][coords[i-1].col] == 'O' || map[coords[i-1].row][coords[i-1].col] == 'T' || map[coords[i].row][coords[i].col] == 'R' || map[coords[i].row][coords[i].col] == 'X')
      return 40;
    else
      return 10;
  }

  else if (map[coords[i].row][coords[i].col] == '*')
    return 80;

  else
  {
    printf("ERROR! Unknown action! Cannot estimate_cost\n");
    return -1;
  }

}

int calculate_total_cost(int cnt_pth, struct pair *coords)
{
  int sum = 0;
  for (int i = 0; i < cnt_pth; i++)
    sum = sum + estimate_cost(coords, i);
  return sum;
}

void

int move(char *world, int map_id)
{
  if(!map_init)
  {
    copy_map(world);
    indexing_interests();
    origin.row = origin_row;
    origin.col = origin_col;
    target.row = target_row;
    target.col = target_col;
    map_init = true;
  }

  if(!target_reached && !path_init)
  {
    while(path_mode <= 4)
    {
      a_star(map, origin, target);
      path_mode++;
    }
    path_mode = 1;
  }

}
