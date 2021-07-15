#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <float.h>
#include <limits.h>
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

#define land 0
#define water 1

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

bool p1 = false, p2 = false, p3 = false, p4 = false, target_reached = false, base_returned = false, path_init = false, index_init = false; //Bunch of flags. The p's are for checking whether there is a path found for that path mode

int drive_mode = land, indEx = 0, mvDir;

struct pair //creating a struct for pair of coordinates for ease of usage
{
  int row;
  int col;
};

struct pPair  //creating a struct for a pair of total cost coordinates for the open list
{
  double f;
  int type;
  struct pair coord;
};

struct nodes			//Creating struct for the nodes;
{
	int parent_row, parent_col, type, parent_type;	//Row and column position of the parents
	double f, g, h;			//f = sum of cost, g = movement cost from starting point to given cell, h = estimated movement cost from current cell to destination
  char travel_dir;
};

struct pair origin, target, coord1[MAX_STEPS], coord2[MAX_STEPS], coord3[MAX_STEPS], coord4[MAX_STEPS], coord_f[MAX_STEPS]; //Origin - Robot coordinates | Target - target coordinates | Coords - to use to compare energy later

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
    printf("->(%c|%d, %d) ", moves[i], path[i].row, path[i].col);
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
  printf("---------------------------------------------------------\n\n", steps, true_steps);

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
    else if(moves[j] == 'W')
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
/*    for(int i = 0; i < true_steps; i++)
      printf("%d ,(%d, %d); ", path_1[i], coord1[i].row, coord1[i].col);
    printf("\n");*/
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
    memcpy(path_4, tmp_moves, sizeof(tmp_moves));
    memcpy(coord4, tmp_path, sizeof(tmp_path));
    cnt_pth4 = true_steps;
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
  int i, j, a, b, tmp_type; //Variable for ease of typing i = row, j = column

  for (i = 0; i<max_row; i++) // Assign all of the node
  {
    for (j = 0; j<max_col; j++) //Basically just to Initiallize them
    {
      node[i][j].f = FLT_MAX; //inf f
      node[i][j].g = FLT_MAX; //inf g
      node[i][j].h = FLT_MAX; //inf h
      node[i][j].parent_row = -1; //invalid parent row
      node[i][j].parent_col = -1; //invalid parent column
      node[i][j].type = -1; //invalid type
      node[i][j].travel_dir = 'U';//unknown travel direction
    }
  }

  i = origin.row, j = origin.col, tmp_type = land; //Make i and j the origin

  node[i][j].f = 0.0;//reset the origin basically
  node[i][j].g = 0.0;
  node[i][j].h = 0.0;
  node[i][j].parent_row = i;
  node[i][j].parent_col = j;
  node[i][j].type = tmp_type;
  node[i][j].parent_type = tmp_type;
  node[i][j].travel_dir = 'O';

  int list_count = 0; //Initiallize a counter for the open nodes that need to be tested
  struct pPair openList[max_col * max_row]; //Initiallize an list of open nodes
  //Make the first in the list the origin node
  openList[list_count].f = node[i][j].f;
  openList[list_count].type = tmp_type;
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
    tmp_type = node[i][j].type;

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
                node[i - 1][j].parent_type = tmp_type;
                node[i - 1][j].type = (map[i - 1][j] == '~' || map[i - 1][j] == 't') ? water : land;
                printf("The target is found\n");
                draw_path(node, target);  //Collect the path
                target_located = true;    // Set that a path is found
                return;
            }

            else if (!closedList[i - 1][j] && isOpen(map, i - 1, j))  //If the node is not closed and is on not blocked
            {
              //Calculate the g, h and f.

                if(map[i - 1][j] == '*')
                {
                  gNew = node[i][j].g + 1.7; //If it is a wall, make the local cost higher
                }
                else if(node[i][j].type == land)
                {
                  if(map[i - 1][j] == '~' || map[i - 1][j] == 't')
                  {
                    gNew = node[i][j].g + 1.3; //If different land type
                    //printf("CHANGING MODE!\n" );
                  }
                  else
                  {
                    gNew = node[i][j].g + 1.0;  //If same land type
                  }

                }
                else if(node[i][j].type == water)
                {
                  if(map[i - 1][j] == 'O' || map[i - 1][j] == 'T' || map[i - 1][j] == 'X' || map[i - 1][j] == '*')
                  {
                    gNew = node[i][j].g + 1.3; //If different land type
                    //printf("CHANGING MODE!\n" );
                  }
                  else
                  {
                    gNew = node[i][j].g + 1.0;  //If same land type
                  }
                }

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
                    openList[0].type = node[i][j].type;

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
                    node[i - 1][j].type = (map[i - 1][j] == '~' || map[i - 1][j] == 't') ? water : land;
                    node[i - 1][j].parent_type = node[i][j].type;
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
                node[i + 1][j].parent_type = tmp_type;
                node[i + 1][j].type = (map[i + 1][j] == '~' || map[i + 1][j] == 't') ? water : land;
                printf("The target is found\n");
                draw_path(node, target);
                target_located = true;
                return;
            }

          else if (!closedList[i + 1][j] && isOpen(map, i + 1, j))
          {
            if(map[i + 1][j] == '*')
            {
              gNew = node[i][j].g + 1.7; //If it is a wall, make the local cost higher
            }
            else if(node[i][j].type == land)
            {
              if(map[i + 1][j] == '~' || map[i + 1][j] == 't')
              {
                gNew = node[i][j].g + 1.3; //If different land type
                //printf("CHANGING MODE!\n");
              }
              else
              {
                gNew = node[i][j].g + 1.0;  //If same land type
              }

            }
            else if(node[i][j].type == water)
            {
              if(map[i + 1][j] == 'O' || map[i + 1][j] == 'T' || map[i + 1][j] == 'X' || map[i + 1][j] == '*')
              {
                gNew = node[i][j].g + 1.3; //If different land type
                //printf("CHANGING MODE!\n" );
              }
              else
              {
                gNew = node[i][j].g + 1.0;  //If same land type
              }
            }
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
                    openList[0].type = node[i][j].type;

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
                    node[i + 1][j].type = (map[i + 1][j] == '~' || map[i + 1][j] == 't') ? water : land;
                    node[i + 1][j].parent_type = node[i][j].type;
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
                node[i][j + 1].parent_type = tmp_type;
                node[i][j + 1].type = (map[i - 1][j] == '~' || map[i - 1][j] == 't') ? water : land;
                printf("The target is found\n");
                draw_path(node, target);
                target_located = true;
                return;
            }
            else if (!closedList[i][j + 1] && isOpen(map, i, j + 1))
            {
              if(map[i][j + 1] == '*')
              {
                gNew = node[i][j].g + 1.7; //If it is a wall, make the local cost higher
              }
              else if(node[i][j].type == land)
              {
                if(map[i][j + 1] == '~' || map[i][j + 1] == 't')
                {
                  gNew = node[i][j].g + 1.3; //If different land type
                  //printf("CHANGING MODE!\n");
                }
                else
                {
                  gNew = node[i][j].g + 1.0;  //If same land type
                }

              }
              else if(node[i][j].type == water)
              {
                if(map[i][j + 1] == 'O' || map[i][j + 1] == 'T' || map[i][j + 1] == 'X' || map[i][j + 1] == '*')
                {
                  gNew = node[i][j].g + 1.3; //If different land type
                  //printf("CHANGING MODE!\n");
                }
                else
                {
                  gNew = node[i][j].g + 1.0;  //If same land type
                }
              }
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
                    openList[0].type = node[i][j].type;

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
                    node[i][j + 1].type = (map[i][j + 1] == '~' || map[i][j + 1] == 't') ? water : land;
                    node[i][j + 1].parent_type = node[i][j].type;
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
                node[i][j - 1].parent_type = tmp_type;
                node[i][j - 1].type = (map[i][j - 1] == '~' || map[i][j - 1] == 't') ? water : land;
                printf("The target is found\n");
                draw_path(node, target);
                target_located = true;
                return;
            }
            else if (!closedList[i][j - 1] && isOpen(map, i, j - 1))
            {
              if(map[i][j - 1] == '*')
              {
                gNew = node[i][j].g + 1.7; //If it is a wall, make the local cost higher
              }
              else if(node[i][j].type == land)
              {
                if(map[i][j - 1] == '~' || map[i][j - 1] == 't')
                {
                  gNew = node[i][j].g + 1.3; //If different land type
                  //printf("CHANGING MODE!\n");
                }
                else
                {
                  gNew = node[i][j].g + 1.0;  //If same land type
                }

              }
              else if(node[i][j].type == water)
              {
                if(map[i][j - 1] == 'O' || map[i][j - 1] == 'T' || map[i][j - 1] == 'X' || map[i][j - 1] == '*')
                {
                  gNew = node[i][j].g + 1.3; //If different land type
                  //printf("CHANGING MODE!\n");
                }
                else
                {
                  gNew = node[i][j].g + 1.0;  //If same land type
                }
              }
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
                    openList[0].type = node[i][j].type;

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
                    node[i][j - 1].type = (map[i][j - 1] == '~' || map[i][j - 1] == 't') ? water : land;
                    node[i][j - 1].parent_type = node[i][j].type;
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

void copy_map(char *world)  //Copying the map
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

void print_map()  //Printing the map
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

void indexing_interests() //Making an index for the target and the origin
{
  for(int i = 0; i < max_row; i++)
  {
    for(int j = 0; j < max_col; j++)
    {
      if(map[i][j] == 'T' || map[i][j] == 't' || map[i][j] == 'X')
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

int estimate_cost(struct pair *coords, int i) //Estimate the cost of each steps assuming that the starting point is always land
{
  if(map[coords[i].row][coords[i].col] == 'O' || map[coords[i].row][coords[i].col] == 'T' || map[coords[i].row][coords[i].col] == 'X')
  {
    if(map[coords[i-1].row][coords[i-1].col] == '~' || map[coords[i-1].row][coords[i-1].col] == 't')
      return 40;
    else
      return 10;
  }

  else if(map[coords[i].row][coords[i].col] == '~' || map[coords[i].row][coords[i].col] == 't')
  {
    if(map[coords[i-1].row][coords[i-1].col] == 'O' || map[coords[i-1].row][coords[i-1].col] == 'T' || map[coords[i].row][coords[i].col] == 'X')
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

int calculate_total_cost(int cnt_pth, struct pair *coords)  //Estimate the total costs
{
  int sum = 0;
  for (int i = 0; i < cnt_pth; i++)
    sum = sum + estimate_cost(coords, i);
  return sum;
}

void compare_cost()
{
  if(!p1 && !p2 && !p3 && !p4)
  {
    printf("None of the path go to the target!\n");
    return;
  }

  else
  {
    //If the run is sucessful, calculate the energy cost. If failed, make the energy infinite.
    if(p1)
      energy1 = calculate_total_cost(cnt_pth1, coord1);
    else
      energy1 = INT_MAX;

    if(p2)
      energy2 = calculate_total_cost(cnt_pth2, coord2);
    else
      energy2 = INT_MAX;

    if(p3)
      energy3 = calculate_total_cost(cnt_pth3, coord3);
    else
      energy3 = INT_MAX;

    if(p4)
      energy4 = calculate_total_cost(cnt_pth4, coord4);
    else
      energy4 = INT_MAX;


    printf("\n_________________________________________________________________________\n", energy1, energy2, energy3, energy4);
    printf("\nEnergy of each path respectively: 1 - %d | 2 - %d | 3 - %d | 4 - %d \n", energy1, energy2, energy3, energy4);

    int a, b, final;
    int x, y;

    // compare the energy level of each paths
    if(energy1 <= energy2)
    {
      a = 1;
      x = energy1;
    }
    else
    {
      a = 2;
      x = energy2;
    }

    if(energy3 <= energy4)
    {
      b = 3;
      y = energy3;
    }

    else
    {
      b = 4;
      y = energy4;
    }

    // Pick the most efficient one
    final = (x <= y) ? a : b;

    printf("\nPath number %d won with the lowest energy!\n", final);
    printf("\n_________________________________________________________________________\n", energy1, energy2, energy3, energy4);
    switch(final) //Copy the most efficient path into the final variable to use
    {
      case 1:
        cnt_path = cnt_pth1;
        memcpy(path_f, path_1, sizeof(path_1));
        memcpy(coord_f, coord1, sizeof(coord1));
        break;
      case 2:
        cnt_path = cnt_pth2;
        memcpy(path_f, path_2, sizeof(path_2));
        memcpy(coord_f, coord2, sizeof(coord2));
        break;
      case 3:
        cnt_path = cnt_pth3;
        memcpy(path_f, path_3, sizeof(path_3));
        memcpy(coord_f, coord3, sizeof(coord3));
        break;
      case 4:
        cnt_path = cnt_pth4;
        memcpy(path_f, path_4, sizeof(path_4));
        memcpy(coord_f, coord4, sizeof(coord4));
        break;
    }
    printf("\nFinal step counter: %d\n\n", cnt_path);

    path_init = true;
    for(int i = 0; i < cnt_path; i++)
        printf("->(%d|%d, %d) ", path_f[i], coord_f[i].row, coord_f[i].col);
    printf("\n\n");

    return;
  }
}

int move(char *world, int map_id)
{

  if(map_id != current_id)  // check the map ids
  { //resetting all flags
    target_reached = false;
    base_returned = false;
    path_init = false;
    index_init = false;
    p1 = false;
    p2 = false;
    p3 = false;
    p4 = false;
    indEx = 0;
    drive_mode = land;
    path_mode = 1;
    current_id = map_id;
  }

  mvDir = 0; //Reset move direction each time;

  copy_map(world);

  if(!index_init)
  {
    indexing_interests();
    origin.row = origin_row;
    origin.col = origin_col;
    target.row = target_row;
    target.col = target_col;
    index_init = true;
  }

  if(!target_reached && !path_init)
  {
    printf("Initiallizing the path\n");
    // Resetting all of the path flags
    p1 = false;
    p2 = false;
    p3 = false;
    p4 = false;
    path_mode = 1;

    while(path_mode <= 4) //Cycling through all of the pathing modes
    {
      a_star(map, origin, target);  //Calculate a path from origin to target
      path_mode++;
    }
    compare_cost();
  }

  else if (!base_returned && !path_init && target_reached)
  {
    printf("Initiallizing the return path\n");
    // Resetting all of the path flags
    p1 = false;
    p2 = false;
    p3 = false;
    p4 = false;
    path_mode = 1;

    while(path_mode <= 4) //Cycling through all of the pathing modes
    {
      a_star(map, target, origin);  //Calculate a path from target to origin
      path_mode++;
    }
    compare_cost();
  }

  printf("Move number %d\n", indEx+1);
  printf(">>>> Robot vision of next Node: (%d, %d) - %c \n",coord_f[indEx].row, coord_f[indEx].col, map[coord_f[indEx].row][coord_f[indEx].col]);
  if (!base_returned && path_init)  //If there is a path inititiated and not yet returned
  {
      mvDir = path_f[indEx];
      //printf(">>>>>>>>>>> %c\n", map[coord_f[indEx].row][coord_f[indEx].row]);
      if(map[coord_f[indEx].row][coord_f[indEx].col] == 'O' || map[coord_f[indEx].row][coord_f[indEx].col] == 'T') //if land
      {
        if(drive_mode == water) //and drive mode is water
        {
          mvDir = MODE_CHANGE; //change the mode
          drive_mode = land;
        }
        else
        {
          indEx++;        //increase the index
        }
      }

      else if(map[coord_f[indEx].row][coord_f[indEx].col] == '~' || map[coord_f[indEx].row][coord_f[indEx].col] == 't') //if water
      {
        if(drive_mode == land) //and drive mode is land
        {
          mvDir = MODE_CHANGE; //change the mode
          drive_mode = water;
        }

        else
        {
          indEx++;        //increase the index
        }
      }

      else if(map[coord_f[indEx].row][coord_f[indEx].col] == '*') //if obstacle
      {
        if(mvDir == N)
          mvDir = DES_N;  //Destroy the northern wall if it is at the north

        else if(mvDir == E)
          mvDir = DES_E;  //Destroy the eastern wall if it is at the east

        else if(mvDir == S)  //Destroy the southern wall if it is at the south
          mvDir = DES_S;

        else if(mvDir == W) //Destroy the western wall if it is at the west
          mvDir = DES_W;
      }

      if(mvDir != MODE_CHANGE && mvDir != DES_N && mvDir != DES_E && mvDir != DES_S && mvDir != DES_W)  //If none of these
      {
        if(indEx == cnt_path && !target_reached)    //And at last index but target isn't marked as reached
        {
          printf("Target reached!\n");
          target_reached = true;
          path_init = false;
          //index_init = false;
          indEx = 0;
        }

        else if(indEx == cnt_path && target_reached && !base_returned)    //And at last index but returned isn't marked as true
        {
          printf("Returned to base!\n");
          target_reached = false;
          base_returned = true;
          path_init = false;
          index_init = false;
          indEx = 0;
        }
      }

      printf("I am returning %d\n", mvDir);
      return mvDir;
  }

  else
  {
    printf("ERROR: NO PATH FOUND! OR ROBOT HAS RETURNED EXITING\n");
    return -1;
  }
}
