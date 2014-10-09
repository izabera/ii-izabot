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


FILE * logfile;
int L_flag;
#define OUTPUT (L_flag == 1 ? logfile : stderr)
#define OUTERR (L_flag == 1 ? logfile : stderr)


#define MAXLVL 9


void printmsg (char * msg, int urgency);
int explore (int row, int col, int board[8][8], int turn, int direction);
int islegal (int row, int col, int board[8][8], int turn);
void domove (int row, int col, int board[8][8], int turn);
void showscore (int board[8][8]);
int * score (int board[8][8]);
int nextturn (int board[8][8], int turn);
void draw (int board[8][8], int turn, int u_flag, int d_flag, int r_flag, int l_flag, int c_flag, int H_flag);
void printturn (int turn);
void usage ();
int won (int board[8][8]);
void copyboard (int source[8][8], int dest[8][8]);
int diffscoressmart (int board[8][8]);
void bestmove (int board[8][8], int turn, int * row, char * col, int * diffscore, int lookahead);
void getscores (int board[8][8], int *s0, int *s1);
void quit (int signal, int L_flag);


void printmsg (char * msg, int urgency) {
  switch (urgency) {
    case 0: /* default */
      break;
    case 1: /* things are good */
      fprintf (OUTERR, CYAN BOLD   "SUCCESS " RESET); break;
    case 2: /* alert */
      fprintf (OUTERR, YELLOW BOLD "WARNING " RESET); break;
    case 3: /* things are horribly wrong and armageddon is coming */
      fprintf (OUTERR, RED BOLD    "ERROR   " RESET); break;
    case 4: /* for testing purposes, to be disabled before actual usage */
      fprintf (OUTERR, BLUE BOLD   "TESTING " RESET); break;
    case 5: /* for debug use */
      fprintf (OUTERR, MAGENTA     "DEBUG   " RESET); break;
    }
  fprintf (OUTERR, "%s\n", msg);
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
  for (i = 2; i < 8 && tempvector[i] < 2; i++) if (tempvector[i] == turn) return 1;
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
      if (islegal (i, j, board, 1 - turn)) count ++;
  if (count) return 1 - turn;
  else return turn;
}


#define PRINT_X (c_flag == 1 ? RED BOLD "X" RESET : "X")
#define PRINT_O (c_flag == 1 ? GREEN BOLD "O" RESET : "O")
#define PRINT_DOT ((H_flag == 1 && islegal (i, j, board, turn)) ? \
                   (c_flag == 1 ? CYAN "x" RESET : "x") : "." )

void draw (int board[8][8], int turn, int u_flag, int d_flag, int r_flag, int l_flag, int c_flag, int H_flag) {
  int i, j;
  if (l_flag) fprintf (OUTPUT, "  ");
  if (u_flag) fprintf (OUTPUT, "ABCDEFGH");
  if (l_flag || u_flag) fprintf (OUTPUT, "\n");
  for (i = 0; i < 8; i++) {
    if (l_flag) fprintf (OUTPUT, "%d ", (~i & 7) + 1);
    for (j = 0; j < 8; j++)
      switch (board[i][j]) {
        case 0:
          fprintf (OUTPUT, PRINT_O); break;
        case 1:
          fprintf (OUTPUT, PRINT_X); break;
        default:
          fprintf (OUTPUT, PRINT_DOT); break;
      }
    if (r_flag) fprintf (OUTPUT, " %d", 8 - i);
    fprintf (OUTPUT, "\n");
  }
  if (l_flag) fprintf (OUTPUT, "  ");
  if (d_flag) fprintf (OUTPUT, "ABCDEFGH");
  if (l_flag || d_flag) fprintf (OUTPUT, "\n");
}


void printturn (int turn) {
  char msg[80];
  sprintf(msg, "It's %s's turn", turn ? RED BOLD "red" RESET : GREEN BOLD "green" RESET);
  printmsg (msg, PRINT_DEFAULT);
}

void usage () {
  printmsg ("Usage: reversi [-h] [-s|t FILE] [-v] [-c] [-L] [-H] [-u] [-d] [-r] [-l] [-A|B [1-5]]", PRINT_DEFAULT);
  printmsg ("       -" BOLD        "h " RESET "        show this help", PRINT_DEFAULT);
  printmsg ("       -" BOLD        "s " RESET "FILE    read starting board from 'FILE' file (green plays)", PRINT_DEFAULT);
  printmsg ("       -" BOLD        "t " RESET "FILE    read starting board from 'FILE' file (red plays)", PRINT_DEFAULT);
  printmsg ("      Output options              ", PRINT_DEFAULT);
  printmsg ("       -" BOLD YELLOW "v " RESET "        verbose output: print board after each move", PRINT_DEFAULT);
  printmsg ("       -" BOLD YELLOW "c " RESET "        color output", PRINT_DEFAULT);
  printmsg ("       -" BOLD YELLOW "L " RESET "        log everything to /tmp/reversilog", PRINT_DEFAULT);
  printmsg ("       -" BOLD YELLOW "H " RESET "        hint valid moves", PRINT_DEFAULT);
  printmsg ("      Borders                     ", PRINT_DEFAULT);
  printmsg ("       -" BOLD GREEN  "u " RESET "        (UP) show letters above board", PRINT_DEFAULT);
  printmsg ("       -" BOLD GREEN  "d " RESET "        (DOWN) show letters below board", PRINT_DEFAULT);
  printmsg ("       -" BOLD GREEN  "r " RESET "        (RIGHT) show numbers to the right of the board", PRINT_DEFAULT);
  printmsg ("       -" BOLD GREEN  "l " RESET "        (LEFT) show numbers to the left of the board", PRINT_DEFAULT);
  printmsg ("      Single-Player mode          ", PRINT_DEFAULT);
  printmsg ("       -" BOLD CYAN   "A " RESET "[1-5]   AI level, Computer plays green (default = 2)", PRINT_DEFAULT);
  printmsg ("       -" BOLD CYAN   "B " RESET "[1-5]   AI level, Computer plays red (default = 2)", PRINT_DEFAULT);
}


void getscores (int board[8][8], int *s0, int *s1) {
  int i, j;
  *s0 = 0;
  *s1 = 0;
  for (i = 0; i < 8; i++)
    for (j = 0; j < 8; j++)
      if (board[i][j] == 0) (* s0) ++;
      else if (board[i][j] == 1) (* s1) ++;
}


int won (int board[8][8]) {
  int red, green;
  getscores (board, &red, &green);
  if ((nextturn (board, 0) == 0) && (nextturn (board, 1) == 1)) {
    if (red > green) return 0;
    else return 1;
  }
  return 2;
}


void copyboard (int source[8][8], int dest[8][8]) {
  int i, j;
  for (i = 0; i < 8; i++)
    for (j = 0; j < 8; j++)
      dest[i][j] = source[i][j];
}


int diffscoressmart (int board[8][8]) {
  static int scoretable[8][8] = { /* totally made up */
    {10, 3, 7, 7, 7, 7, 3,10},
    { 3, 2, 4, 4, 4, 4, 2, 3},
    { 7, 4, 5, 5, 5, 5, 4, 7},
    { 7, 4, 5, 5, 5, 5, 4, 7},
    { 7, 4, 5, 5, 5, 5, 4, 7},
    { 7, 4, 5, 5, 5, 5, 4, 7},
    { 3, 2, 4, 4, 4, 4, 2, 3},
    {10, 3, 7, 7, 7, 7, 3,10},
  };
  int i, j, s0, s1;
  s0 = 0;
  s1 = 0;
  for (i = 0; i < 8; i++)
    for (j = 0; j < 8; j++)
      if (board[i][j] == 0) s0 += scoretable[i][j];
      else if (board[i][j] == 1) s1 += scoretable[i][j];
  return s0 - s1;
}


void bestmove (int board[8][8], int turn, int * row, char * col, int * diffscore, int lookahead) {
  int bestscore = -10000, newdiffscore, newturn;
  int i, j;
  int rdummy;
  char cdummy;
  int tmpboard[8][8];
  int coords[64][2];
  int numbest = 0;
  for (i = 0; i < 8; i++)
    for (j = 0; j < 8; j++)
      if(islegal (i, j, board, turn)){
        copyboard (board, tmpboard);
        domove (i, j, tmpboard, turn);
        if (lookahead <= 1) {
          newdiffscore = diffscoressmart (tmpboard);
          if (turn == 1)  newdiffscore = -newdiffscore;
        }
        else {
          newturn = nextturn (tmpboard, turn);
          if (newturn != turn) {
            bestmove (tmpboard, newturn, & rdummy, & cdummy, & newdiffscore, lookahead - 1);
            newdiffscore = -newdiffscore;
          }
          else if (nextturn (tmpboard, 1 - turn) != 1 - turn) {
            bestmove (tmpboard, newturn, & rdummy, & cdummy, & newdiffscore, lookahead - 1);
          }
          else {
            newdiffscore = diffscoressmart (tmpboard);
            if (turn == 1)  newdiffscore = -newdiffscore;
          }
        }
        if (newdiffscore >= bestscore) {
          if (newdiffscore > bestscore) {
            bestscore = newdiffscore;
            numbest = 0;
          }
          coords[numbest][0] = i;
          coords[numbest++][1] = j;
        }
        break;
      }
  *diffscore = bestscore;
  i = rand() % numbest;
  *row = coords[i][0];
  *col = coords[i][1];
}

void quit (int signal, int L_flag) {
  if (L_flag) fclose (logfile);
  exit (signal);
}

#endif
