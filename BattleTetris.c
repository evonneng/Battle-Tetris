#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "ST7735.h"
#include "Random.h"
#include "TExaS.h"
#include "ADC.h"
#include "UART.h"

#define START_MENU 0
#define ONE_PLAYER 1
#define TWO_PLAYER 2
#define FINISHED 3

#define DO_NOTHING 0
#define MOVE_LEFT 1
#define MOVE_RIGHT 2
#define MOVE_DOWN 3

typedef struct Point {
	uint16_t x;
	uint16_t y;
} Point;

typedef struct Piece {
	Point point1;
	Point point2;
	Point point3;
	Point point4;
	Point origin;
	struct Piece* next_rotation;
	uint16_t color;
} Piece;

// 0 = t piece
// 1 = i piece
// 2 = o piece
// 3 = s piece
// 4 = z piece
// 5 = j piece
// 6 = l piece
Piece static_pieces[7][4];

Piece current_piece;

uint16_t board [20][10];
uint8_t mode = 0;
uint8_t play_state;
uint32_t score;

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

void board_init(void) {
	for(int i = 0; i < 20; i++) {
		for(int j = 0; j < 10; j++) {
			board[i][j] = 0;
		}
	}
}

void pieces_init(void) {
	// TODO: initialize pieces
}

Piece* gen_piece(void) {
	//TODO: generate random piece
	return 0;
}

void copy_piece(Piece* dest, Piece* src, Point origin) {
	dest->point1.x = src->point1.x + origin.x;
	dest->point1.y = src->point1.y + origin.y;
	dest->point2.x = src->point2.x + origin.x;
	dest->point2.y = src->point2.y + origin.y;
	dest->point3.x = src->point3.x + origin.x;
	dest->point3.y = src->point3.y + origin.y;
	dest->point4.x = src->point4.x + origin.x;
	dest->point4.y = src->point4.y + origin.y;
	dest->origin.x = origin.x;
	dest->origin.y = origin.y;
	dest->next_rotation = src->next_rotation;
	dest->color = src->color;
}

void display_start_menu(void) {
	//TODO: display start screen
}

// 0 = do nothing, 1 = left, 2 = right, rest is down
uint8_t get_input(void) {
	//TODO: get the input from
	return 0;
}

void draw_piece(Piece* p, uint16_t color) {
	//TODO: draw piece
}

void draw_score() {
	//TODO: display the updated score
}

//-1 if game over, 0 if can't move(left/right collisions), 1 if can move, 2 if moving down and will place
int can_move(void) {
	//TODO: check if can move
	return 0;
}

void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;                   // disable SysTick during setup
  NVIC_ST_RELOAD_R = 2000000-1;  // maximum reload value
  NVIC_ST_CURRENT_R = 0;                // any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x20000000; // priority 1
  NVIC_ST_CTRL_R = 0x0007;
}

uint32_t move_down_timer = 0;
#define MOVE_DOWN_MAX 30

void SysTick_Handler(void) {
	if(mode == START_MENU || mode == FINISHED)
		return;
	move_down_timer++;
	if(move_down_timer == MOVE_DOWN_MAX) {
		play_state = MOVE_DOWN;
		move_down_timer = 0;
	} else {
		uint8_t input = get_input();
		if(input == 0)
			play_state = DO_NOTHING;
		else if(input == 1)
			play_state = MOVE_LEFT;
		else if(input == 2)
			play_state = MOVE_RIGHT;
		else
			play_state = MOVE_DOWN;
	}
}

void game_one(void) {
	Point origin;
	origin.x = 4;
	origin.y = 16;
	copy_piece(&current_piece, gen_piece(), origin);
	draw_piece(&current_piece, current_piece.color);
	while(mode == ONE_PLAYER) {
		int state = can_move();
		if(state == 1) {
			draw_piece(&current_piece, 0);
			//TODO: move piece
			draw_piece(&current_piece, current_piece.color);
		} else if(state == 2) {
			//TODO: check for clear row, increment score if so
			//TODO: generate new piece, generate next piece
		} else if(state == -1) {
			mode = FINISHED;
		}
	}
	//TODO: display score screen
}

void game_two(void) {
	while(mode == TWO_PLAYER) {
		
	}
}

void left(void) {
	int x1 = current_piece.point1.x-1;
	int x2 = current_piece.point2.x-1;
	int x3 = current_piece.point3.x-1;
	int x4 = current_piece.point4.x-1;
	int y1 = current_piece.point1.y;
	int y2 = current_piece.point2.y;
	int y3 = current_piece.point3.y;
	int y4 = current_piece.point4.y;
	// if out of bounds
	if (x1 < 0 || x2 < 0 || x3 < 0 || x4 < 0) return;
	// if a piece is blocking it from moving
	if (board[y1][x1] == 1 ||
		board[y2][x2] == 1 ||
		board[y3][x3] == 1 ||
		board[y4][x4] == 1) {
		return;
	}
	current_piece.point1.x = x1;
	current_piece.point2.x = x2;
	current_piece.point3.x = x3;
	current_piece.point4.x = x4;
	return;
}

void right(void) {
	int x1 = current_piece.point1.x+1;
	int x2 = current_piece.point2.x+1;
	int x3 = current_piece.point3.x+1;
	int x4 = current_piece.point4.x+1;
	int y1 = current_piece.point1.y;
	int y2 = current_piece.point2.y;
	int y3 = current_piece.point3.y;
	int y4 = current_piece.point4.y;
	// if out of bounds
	if (x1 >=10 || x2 >= 10 || x3 >= 10 || x4 >= 10) return;
	// if a piece is blocking it from moving
	if (board[y1][x1] == 1 ||
		board[y2][x2] == 1 ||
		board[y3][x3] == 1 ||
		board[y4][x4] == 1) {
		return;
	}
	current_piece.point1.x = x1;
	current_piece.point2.x = x2;
	current_piece.point3.x = x3;
	current_piece.point4.x = x4;
}

void down(void) {
	int x1 = current_piece.point1.x;
	int x2 = current_piece.point2.x;
	int x3 = current_piece.point3.x;
	int x4 = current_piece.point4.x;
	int y1 = current_piece.point1.y+1;
	int y2 = current_piece.point2.y+1;
	int y3 = current_piece.point3.y+1;
	int y4 = current_piece.point4.y+1;
	// if out of bounds
	if (y1 >= 20 || y2 >= 20 || y3 >= 20 || y4 >= 20) return;
	// if a piece is blocking it from moving
	if (board[y1][x1] == 1 || 
		board[y2][x2] == 1 ||
		board[y3][x3] == 1 ||
		board[y4][x4] == 1) {
		return;
	}
	current_piece.point1.y = y1;
	current_piece.point2.y = y2;
	current_piece.point3.y = y3;
	current_piece.point4.y = y4;
}

void place(void) {
	int x1 = current_piece.point1.x; 
	int x2 = current_piece.point2.x;

}

int main(void) {
	TExaS_Init();  // set system clock to 80 MHz
 	Random_Init(1);

 	ST7735_InitR(INITR_REDTAB);
 	ADC_Init();    // initialize to sample ADC1
 	UART_Init();       // initialize UART
	SysTick_Init();
	board_init();
	pieces_init();
	//TODO: initialize buttons
	while(1) {
		display_start_menu();
		while(0) { //TODO: waiting for menu selection
		}
		score = 0;
		if(mode == ONE_PLAYER)
			game_one();
		else if(mode == TWO_PLAYER)
			game_two();
		//TODO: click a button to go back to start menu from finished
	}
}
