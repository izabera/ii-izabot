#ifndef REVERSI_H
#define REVERSI_H

#define INSIDE(X, Y) (0 <= (X) && (X) < 8 && 0 <= (Y) && (Y) < 8)

#define PRINT_DEFAULT 0
#define PRINT_SUCCESS 1
#define PRINT_WARNING 2
#define PRINT_ERROR   3
#define PRINT_TESTING 4
#define PRINT_DEBUG   5

#define BOLD    "\x1b[1m"
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"


#ifdef TESTENV
#endif


void printmsg (char * msg, int urgency) {
  switch (urgency) {
    case 0: /* default */
      break;
    case 1: /* things are good */
      fprintf(stderr, CYAN BOLD   "SUCCESS " RESET); break;
    case 2: /* alert */
      fprintf(stderr, YELLOW BOLD "WARNING " RESET); break;
    case 3: /* things are horribly wrong and armageddon is coming */
      fprintf(stderr, RED BOLD    "ERROR   " RESET); break;
    case 4: /* for testing purposes, to be disabled before actual usage */
      fprintf(stderr, BLUE BOLD   "TESTING " RESET); break;
    case 5: /* for debug use */
      fprintf(stderr, MAGENTA     "DEBUG   " RESET); break;
    }
  fprintf(stderr, "%s\n", msg);
}


int explore (int row, int col, int board[8][8], int turn, int direction) {
  /* directions like numpad */
  int tempvector[8] = {3, 3, 3, 3, 3, 3, 3, 3}; /* 3 = to be checked */
  int i = 0;
  /* convert it to a vector */
  switch (direction) {
    case 1:
      for (; INSIDE(row, col); row--, col--) tempvector[i++] = board[row][col]; break;
    case 2:
      for (; INSIDE(row, col); row--       ) tempvector[i++] = board[row][col]; break;
    case 3:
      for (; INSIDE(row, col); row--, col++) tempvector[i++] = board[row][col]; break;
    case 4:
      for (; INSIDE(row, col);        col--) tempvector[i++] = board[row][col]; break;
    case 5: return 0;
    case 6:
      for (; INSIDE(row, col);        col++) tempvector[i++] = board[row][col]; break;
    case 7:
      for (; INSIDE(row, col); row++, col--) tempvector[i++] = board[row][col]; break;
    case 8:
      for (; INSIDE(row, col); row++       ) tempvector[i++] = board[row][col]; break;
    case 9:
      for (; INSIDE(row, col); row++, col++) tempvector[i++] = board[row][col]; break;
  }
  /* now tempvector[0] == 2, skip it and start from tempvector[1] 
   * if (tempvector[1] == 2) return 0;
   * if (tempvector[1] == turn) return 0; */
  if (tempvector[1] != (turn + 1) % 2) return 0;
  for (i = 2; i < 8 && tempvector[i] != 3; i++) if (tempvector[i] == turn) return 1;
  return 0;
}


int islegal (int row, int col, int board[8][8], int turn) {
  if (board[row][col] != 2) return 0;
  int i, tempcount = 0;
  for (i = 1; i <= 9; i++) tempcount += explore (row, col, board, turn, i);
  return tempcount; /* if tempcount == 0 no direction was found */
}


void domove (int row, int col, int board[8][8], int turn) {
  /* by now we know we have a valid move,
   * so we can just write on board[row][col] as many times as we wish
   *
   * not very efficient but whatever */
  int i, tempr, tempc;
  for (i = 1; i <= 9; i++) {
    tempr = row;
    tempc = col;
    if (explore (row, col, board, turn, i))
      switch (i) {
        case 1:
          for (; INSIDE(tempr, tempc) && ((col == tempc && row == tempr) || board[tempr][tempc] != turn); tempr--, tempc--) board[tempr][tempc] = turn; break;
        case 2:
          for (; INSIDE(tempr, tempc) && ((col == tempc && row == tempr) || board[tempr][tempc] != turn); tempr--         ) board[tempr][tempc] = turn; break;
        case 3:
          for (; INSIDE(tempr, tempc) && ((col == tempc && row == tempr) || board[tempr][tempc] != turn); tempr--, tempc++) board[tempr][tempc] = turn; break;
        case 4:
          for (; INSIDE(tempr, tempc) && ((col == tempc && row == tempr) || board[tempr][tempc] != turn);          tempc--) board[tempr][tempc] = turn; break;
        case 5: break;
        case 6:
          for (; INSIDE(tempr, tempc) && ((col == tempc && row == tempr) || board[tempr][tempc] != turn);          tempc++) board[tempr][tempc] = turn; break;
        case 7:
          for (; INSIDE(tempr, tempc) && ((col == tempc && row == tempr) || board[tempr][tempc] != turn); tempr++, tempc--) board[tempr][tempc] = turn; break;
        case 8:
          for (; INSIDE(tempr, tempc) && ((col == tempc && row == tempr) || board[tempr][tempc] != turn); tempr++         ) board[tempr][tempc] = turn; break;
        case 9:
          for (; INSIDE(tempr, tempc) && ((col == tempc && row == tempr) || board[tempr][tempc] != turn); tempr++, tempc++) board[tempr][tempc] = turn; break;
      }
  }
}


void showscore (int board[8][8]) {
  int i, j, countgreen = 0, countred = 0;
  for (i = 0; i < 8; i++)
    for (j = 0; j < 8; j++)
      if (board[i][j] == 0) countgreen ++;
      else if (board[i][j] == 1) countred ++;
  char msg[80];
  sprintf (msg, "Green: %d - Red: %d", countgreen, countred);
  printmsg (msg, PRINT_DEFAULT);
}


int nextturn (int board[8][8], int turn) {
  int i, j, count = 0;
  for (i = 0; i < 8; i++)
    for (j = 0; j < 8; j++)
      if (islegal(i, j, board, (turn + 1) % 2)) count ++;
  if (count) return (turn + 1) % 2;
  else return turn;
}


#define PRINT_X (c_flag == 1 ? RED BOLD "X" RESET : "X")
#define PRINT_O (c_flag == 1 ? GREEN BOLD "O" RESET : "O")

void draw (int board[8][8], int u_flag, int d_flag, int r_flag, int l_flag, int c_flag) {
  int i, j;
  if (l_flag) printf ("  ");
  if (u_flag) printf ("ABCDEFGH");
  if (l_flag || u_flag) printf ("\n");
  for (i = 0; i < 8; i++) {
    if (l_flag) printf ("%d ", (~i & 7) + 1);
    for (j = 0; j < 8; j++)
      printf (board[i][j] == 2 ? "." : board[i][j] == 1 ? PRINT_X : PRINT_O);
    if (r_flag) printf (" %d", (~i & 7) + 1);
    printf ("\n");
  }
  if (l_flag) printf ("  ");
  if (d_flag) printf ("ABCDEFGH");
  if (l_flag || d_flag) printf ("\n");
}


void printturn (int turn) {
  char msg[80];
  sprintf(msg, "It's %s's turn", turn ? RED BOLD "red" RESET : GREEN BOLD "green" RESET);
  printmsg (msg, PRINT_DEFAULT);
}

void usage () {
  printmsg ("Usage: reversi [-v] [-c] [-u] [-d] [-r] [-l] [-s FILE]", PRINT_DEFAULT);
  printmsg ("       -" BOLD YELLOW "v " RESET "        verbose output: print board after each move", PRINT_DEFAULT);
  printmsg ("       -" BOLD YELLOW "c " RESET "        color output", PRINT_DEFAULT);
  printmsg ("       -" BOLD YELLOW "u " RESET "        (UP) show letters above board", PRINT_DEFAULT);
  printmsg ("       -" BOLD YELLOW "d " RESET "        (DOWN) show letters below board", PRINT_DEFAULT);
  printmsg ("       -" BOLD YELLOW "r " RESET "        (RIGHT) show numbers to the right of the board", PRINT_DEFAULT);
  printmsg ("       -" BOLD YELLOW "l " RESET "        (LEFT) show numbers to the left of the board", PRINT_DEFAULT);
  printmsg ("       -" BOLD YELLOW "s " RESET "FILE    read starting board from 'FILE' file", PRINT_DEFAULT);
  printmsg ("       -" BOLD YELLOW "h " RESET "        show this help", PRINT_DEFAULT);
}


int won (int board[8][8]) {
  int bla = 0, whi = 0, i, j;
  for (i = 0; i < 8; i++)
    for (j = 0; j < 8; j++)
      if (board[i][j] == 0) bla++;
      else if (board[i][j] == 1) whi++;
  if (whi == 0) return 0;
  if (bla == 0) return 1;
  if (bla + whi == 64) {
    if (bla > whi) return 0;
    else return 1;
  }
  return 2;
}

#endif
