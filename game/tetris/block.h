#ifndef _BLOCK_H_
#    define _BLOCK_H_

#    define  p_x 40                //init postion row;
#    define  p_y 5                //init postion col;
#    define  X 20                // game_window_size
#    define  Y 20
#    define  LEVEL_SCORE 500    // need scores to upgrade

jmp_buf env;
static int x, y;                //   current diamonds postion
static int flag_erase;            //   erase flag
static int flag_pause;            //   pause flag
static int sig_winch;            //    SIGWINCH
static int num, mode, next_num, next_mode;    //   current and next diamonds
static int save_row, save_col, save_x, save_y, save_mode;    //   save coordinate, save graph
static int color, save_color, flag_color;    //   save the color of the next diamonds
static int matirx[Y][X] = { 0 };    //   save diamonds' matrix

static int level = 1;            //   game levels
static int score = 0;            //   game scores
/*
struct itimerval {
struct timeval it_interval; // next value
struct timeval it_value;    // current value
};
struct timeval {
long tv_sec;                // seconds
long tv_usec;               // microseconds
};
*/
typedef struct itimerval LEVEL;
static LEVEL level_00 = { {0, 0}, {0, 0} };
static LEVEL level_01 = { {0, 800000}, {1, 0} };
static LEVEL level_02 = { {0, 500000}, {0, 500000} };
static LEVEL level_03 = { {0, 400000}, {0, 300000} };
static LEVEL level_04 = { {0, 300000}, {0, 300000} };
static LEVEL level_05 = { {0, 200000}, {0, 300000} };
static LEVEL level_06 = { {0, 150000}, {0, 300000} };
static LEVEL level_07 = { {0, 100000}, {0, 300000} };
static LEVEL level_08 = { {0, 80000}, {0, 300000} };
static LEVEL level_09 = { {0, 60000}, {0, 300000} };

//three-dimensional for saving  diamonds and diamonds' shape :
//first-dimensional for kind of diamonds-shape
//second-dimensional for alterable's mode
//third-dimensional for reality value of row and col
static const int shape[7][4][18] = {
    {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 2, 1},    //
     {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1, 2},    //   []   []    [][][]     []
     {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 2, 1},    // [][][] [][]    []     [][]
     {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 2}},    //         []               []

    {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 2, 1},    //
     {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 2},    //          []            [][]
     {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 2, 1},    //      [] []    [][][]    []
     {0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 2}},    // [][][] [][]  []        []

    {{0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 1, 2},    //
     {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 2, 1},    //          [][]            []
     {0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 2},    // []       []    [][][]    []
     {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 2, 1}},    // [][][]   []        []  [][]

    {{0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 2},    //
     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 2, 1},    //    []
     {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 2},    //    [][]     [][]
     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 2, 1}},    //       []   [][]

    {{0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 2},    //
     {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 2, 1},    //      []
     {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 2},    //    [][]   [][]
     {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 2, 1}},    //    []       [][]

    {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 2, 2},    //
     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 2, 2},    //
     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 2, 2},    //     [][]
     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 2, 2}},    //     [][]

    {{0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 3},    //     []
     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 3, 0},    //     []     [][][][]
     {0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 3},    //     []
     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 3, 0}}    //     []
};

void init_for_globle(void);
void print_start_interface(void);
void print_mode_shape(void);
void print_save_matrix(void);
void change_shape(void);
void move_left(void);
void move_right(void);
void move_down();
void fall_down();
void store_flag_color(void);
void key_control(void);
void erase_last(void);
void destroy_line(void);
void print_next(void);
void erase_next(void);
void change_level(void);
void print_score(void);
void print_level(void);
int judge_by_color(int x, int mode);
void game_over(void);

#endif
