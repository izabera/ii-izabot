/*
 * reversi by iza
 * most of ai by mindflyer
 * mit license
 *
 * reads moves from stdin
 * configurable
 */ 

/* 0 = black 1 = white 2 = empty */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "reversi.h"


int main (int argc, char ** argv) {

  /* get options */
  char msg[80];
  int v_flag = 0,        /* _V_erbose, show board after each move (default = no) */
      c_flag = 0,        /* _C_olor, print colors (default = no) */ 
      u_flag = 0,        /* _U_p, show letters above board (default = no) */
      d_flag = 0,        /* _D_own, show letters below (default = no) */
      r_flag = 0,        /* _R_ight, show numbers to the right (default = no) */
      l_flag = 0,        /* _L_eft, show numbers to the left (default = no) */
      s_flag = 0,        /* _S_tarting position, provided or not (default = no) */
      L_flag = 0,        /* _L_og everything to /tmp/ */
      H_flag = 0;        /* _H_int valid moves */
  char * s_tart = NULL;  /* _S_tarting position */

  int player[2] = {0,0}; /* 0: human, 1: computer */
  int c, /* compturn, */ complvl = 2, turn = 0;

  opterr = 0;
  while ((c = getopt (argc, argv, "hs:t:vcLHudrlA::B::")) != -1)
    switch (c) {
      case 'h':
        usage ();
        quit (PRINT_SUCCESS, L_flag); /* which is actually 1 */
      case 'v':
        v_flag = 1; break;
      case 'c':
        c_flag = 1; break;
      case 'u':
        u_flag = 1; break;
      case 'd':
        d_flag = 1; break;
      case 'r':
        r_flag = 1; break;
      case 'l':
        l_flag = 1; break;
      case 'L':
        L_flag = 1; break;
      case 'H':
        H_flag = 1; break;
      case 's':
        s_tart = optarg; s_flag = 1; break;
      case 'A':
        if (optarg) complvl = atoi (optarg);
        player[0] = 1;
      /*  compturn = 0; */
        if (complvl < 1 || complvl > MAXLVL) {
          sprintf(msg, "Invalid level: %s. Level must be between 1 and %d (inclusive).", optarg, MAXLVL);
          printmsg (msg, PRINT_DEBUG);
          usage ();
          quit (PRINT_DEBUG, L_flag);
        }
        break;
      case 'B':
        if (optarg) complvl = atoi (optarg);
        player[1] = 1;
      /*  compturn = 1; */
        if (complvl < 1 || complvl > MAXLVL) {
          sprintf(msg, "Invalid level: %s. Level must be between 1 and %d (inclusive).", optarg, MAXLVL);
          printmsg (msg, PRINT_DEBUG);
          usage ();
          quit (PRINT_DEBUG, L_flag);
        }
        break;
      case '?':
        if (optopt == 's' || optopt == 't' || optopt == 'A' || optopt == 'B')
          printmsg ("Option -s requires an argument.", PRINT_DEBUG);
        else {
          sprintf(msg, "Unknown option -%c", optopt);
          printmsg (msg, PRINT_DEBUG);
          usage ();
          return 1;
        }
        break;
      default:
        printmsg ("Unknown commmand line option caused an unknown error", PRINT_DEBUG);
        usage ();
        return 1;
    }
  if (optind < argc) {
    sprintf (msg, "Unknow additional parameter: %s", argv[optind]);
    printmsg (msg, PRINT_DEBUG);
    usage ();
    return PRINT_DEBUG;
  }
  /* get options */


  /* prepare board */
  int board[8][8];
  int i, j;
  if (s_flag) {
    FILE * start = fopen (s_tart, "r");
    if (start == NULL) {
      sprintf (msg, "Can't read file: %s", s_tart);
      printmsg (msg, PRINT_ERROR);
      return PRINT_ERROR;
    }
    char *temp[8];
    i = 0;
    char line[10];
    char * eof;
    while ((eof = fgets (line, 10, start)) != NULL) {
      strtok (line, "\n");
      temp[i] = strdup (line);
      i++;
    }
    if (i != 8) {
      printmsg ("Invalid input file", PRINT_ERROR);
      return PRINT_ERROR;
    }
    for (i = 0; i < 8; i++)
      for (j = 0; j < 8; j++)
        switch (temp[i][j]) {
          case 'O':
            board[i][j] = 0;
            break;
          case 'X':
            board[i][j] = 1;
            break;
          case '.':
            board[i][j] = 2;
            break;
          default:
            /*printmsg ("Invalid input file", PRINT_ERROR);*/
            sprintf (msg, "Invalid input file, invalid char at position i=%d j=%d which was %cEND", i, j, temp[i][j]);
            printmsg(msg, PRINT_ERROR);
            return PRINT_ERROR;
        }
  }
  else {
    for (i = 0; i < 8; i++)
      for (j = 0; j < 8; j++)
        board[i][j] = 2;
    board[3][3] = 0;
    board[4][3] = 1;
    board[4][4] = 0;
    board[3][4] = 1;
  }
  /* board is now ready */


  char col;
  int row;
  int dummyscore;
  draw (board, turn, u_flag, d_flag, r_flag, l_flag, c_flag, H_flag);
  printturn (turn);
  srand (time (NULL));
  while (won (board) == 2) {
    if (player[turn] == 0) {
      if (scanf (" %c %d", &col, &row) == EOF) break;
      if ((!(97 <= col && col <= 104)
            && !(65 <= col && col <=  72))
          || !( 1 <= row && row <=   8))
      {
        sprintf (msg, "Out of bounds: row=%d column=%c.", 8 - row, col);
        printmsg (msg, PRINT_ERROR);
        continue;
        /* quit (PRINT_ERROR, L_flag); */
      }
      col %= 65;
      col %= 32;
      row = 8 - row;
    }
    else {
      if (turn == 0) bestmove (board, turn, &row, &col, &dummyscore, complvl);
      else bestmove (board, turn, &row, &col, &dummyscore, complvl);
      printf ("Computer's move: %c%d\n", col + 65, 8 - row);
    }
    if (islegal (row, col, board, turn)) {
      domove (row, col, board, turn);
      turn = nextturn (board, turn);
      if (v_flag) {
        draw (board, turn, u_flag, d_flag, r_flag, l_flag, c_flag, H_flag);
        printturn (turn);
      }
      showscore (board);
    }
    else {
      sprintf (msg, "Invalid move: row=%d column=%c", 8 - row, col + 65);
      printmsg (msg, PRINT_WARNING);
    }
  }
  sprintf (msg, "%s won", won (board) == 0 ? "Green" : won (board) == 1 ? "Red" : "Nobody");
  printmsg (msg, PRINT_SUCCESS);
  return PRINT_DEFAULT;
}

