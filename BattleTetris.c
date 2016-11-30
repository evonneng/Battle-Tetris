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
#define ROTATE 4

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
	uint16_t piece_number;
	uint16_t rotation_number;
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
Piece next_piece;

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
	//t piece, rotation 0
	static_pieces[0][0].point1.x = 0;
	static_pieces[0][0].point1.y = 1;
	static_pieces[0][0].point2.x = 1;
	static_pieces[0][0].point2.y = 1;
	static_pieces[0][0].point3.x = 1;
	static_pieces[0][0].point3.y = 2;
	static_pieces[0][0].point4.x = 2;
	static_pieces[0][0].point4.y = 1;
	static_pieces[0][0].origin.x = 0;
	static_pieces[0][0].origin.y = 0;
	static_pieces[0][0].piece_number = 0;
	static_pieces[0][0].rotation_number = 0;
	static_pieces[0][0].color = 0x780F; //purple
	//t piece, rotation 1
	static_pieces[0][1].point1.x = 1;
	static_pieces[0][1].point1.y = 0;
	static_pieces[0][1].point2.x = 1;
	static_pieces[0][1].point2.y = 1;
	static_pieces[0][1].point3.x = 1;
	static_pieces[0][1].point3.y = 2;
	static_pieces[0][1].point4.x = 2;
	static_pieces[0][1].point4.y = 1;
	static_pieces[0][1].origin.x = 0;
	static_pieces[0][1].origin.y = 0;
	static_pieces[0][1].piece_number = 0;
	static_pieces[0][1].rotation_number = 1;
	static_pieces[0][1].color = 0x780F; //purple
	//t piece, rotation 2
	static_pieces[0][2].point1.x = 0;
	static_pieces[0][2].point1.y = 1;
	static_pieces[0][2].point2.x = 1;
	static_pieces[0][2].point2.y = 0;
	static_pieces[0][2].point3.x = 1;
	static_pieces[0][2].point3.y = 1;
	static_pieces[0][2].point4.x = 2;
	static_pieces[0][2].point4.y = 1;
	static_pieces[0][2].origin.x = 0;
	static_pieces[0][2].origin.y = 0;
	static_pieces[0][2].piece_number = 0;
	static_pieces[0][2].rotation_number = 2;
	static_pieces[0][2].color = 0x780F; //purple
	//t piece, rotation 3
	static_pieces[0][3].point1.x = 0;
	static_pieces[0][3].point1.y = 1;
	static_pieces[0][3].point2.x = 1;
	static_pieces[0][3].point2.y = 0;
	static_pieces[0][3].point3.x = 1;
	static_pieces[0][3].point3.y = 1;
	static_pieces[0][3].point4.x = 1;
	static_pieces[0][3].point4.y = 2;
	static_pieces[0][3].origin.x = 0;
	static_pieces[0][3].origin.y = 0;
	static_pieces[0][3].piece_number = 0;
	static_pieces[0][3].rotation_number = 3;
	static_pieces[0][3].color = 0x780F; //purple
	//i piece, rotation 0
	static_pieces[1][0].point1.x = 0;
	static_pieces[1][0].point1.y = 2;
	static_pieces[1][0].point2.x = 1;
	static_pieces[1][0].point2.y = 2;
	static_pieces[1][0].point3.x = 2;
	static_pieces[1][0].point3.y = 2;
	static_pieces[1][0].point4.x = 3;
	static_pieces[1][0].point4.y = 2;
	static_pieces[1][0].origin.x = 0;
	static_pieces[1][0].origin.y = 0;
	static_pieces[1][0].piece_number = 1;
	static_pieces[1][0].rotation_number = 0;
	static_pieces[1][0].color = 0x07FF; //cyan
	//i piece, rotation 1
	static_pieces[1][1].point1.x = 2;
	static_pieces[1][1].point1.y = 0;
	static_pieces[1][1].point2.x = 2;
	static_pieces[1][1].point2.y = 1;
	static_pieces[1][1].point3.x = 2;
	static_pieces[1][1].point3.y = 2;
	static_pieces[1][1].point4.x = 2;
	static_pieces[1][1].point4.y = 3;
	static_pieces[1][1].origin.x = 0;
	static_pieces[1][1].origin.y = 0;
	static_pieces[1][1].piece_number = 1;
	static_pieces[1][1].rotation_number = 1;
	static_pieces[1][1].color = 0x07FF; //cyan
	//i piece, rotation 2
	static_pieces[1][2].point1.x = 0;
	static_pieces[1][2].point1.y = 1;
	static_pieces[1][2].point2.x = 1;
	static_pieces[1][2].point2.y = 1;
	static_pieces[1][2].point3.x = 2;
	static_pieces[1][2].point3.y = 1;
	static_pieces[1][2].point4.x = 3;
	static_pieces[1][2].point4.y = 1;
	static_pieces[1][2].origin.x = 0;
	static_pieces[1][2].origin.y = 0;
	static_pieces[1][2].piece_number = 1;
	static_pieces[1][2].rotation_number = 2;
	static_pieces[1][2].color = 0x07FF; //cyan
	//i piece, rotation 3
	static_pieces[1][3].point1.x = 1;
	static_pieces[1][3].point1.y = 0;
	static_pieces[1][3].point2.x = 1;
	static_pieces[1][3].point2.y = 1;
	static_pieces[1][3].point3.x = 1;
	static_pieces[1][3].point3.y = 2;
	static_pieces[1][3].point4.x = 1;
	static_pieces[1][3].point4.y = 3;
	static_pieces[1][3].origin.x = 0;
	static_pieces[1][3].origin.y = 0;
	static_pieces[1][3].piece_number = 1;
	static_pieces[1][3].rotation_number = 3;
	static_pieces[1][3].color = 0x07FF; //cyan
	//o piece, rotation 0
	static_pieces[2][0].point1.x = 1;
	static_pieces[2][0].point1.y = 1;
	static_pieces[2][0].point2.x = 1;
	static_pieces[2][0].point2.y = 2;
	static_pieces[2][0].point3.x = 2;
	static_pieces[2][0].point3.y = 1;
	static_pieces[2][0].point4.x = 2;
	static_pieces[2][0].point4.y = 2;
	static_pieces[2][0].origin.x = 0;
	static_pieces[2][0].origin.y = 0;
	static_pieces[2][0].piece_number = 2;
	static_pieces[2][0].rotation_number = 0;
	static_pieces[2][0].color = 0xFFE0; //yellow
	//o piece, rotation 1
	static_pieces[2][1].point1.x = 1;
	static_pieces[2][1].point1.y = 1;
	static_pieces[2][1].point2.x = 1;
	static_pieces[2][1].point2.y = 2;
	static_pieces[2][1].point3.x = 2;
	static_pieces[2][1].point3.y = 1;
	static_pieces[2][1].point4.x = 2;
	static_pieces[2][1].point4.y = 2;
	static_pieces[2][1].origin.x = 0;
	static_pieces[2][1].origin.y = 0;
	static_pieces[2][1].piece_number = 2;
	static_pieces[2][1].rotation_number = 1;
	static_pieces[2][1].color = 0xFFE0; //yellow
	//o piece, rotation 2
	static_pieces[2][2].point1.x = 1;
	static_pieces[2][2].point1.y = 1;
	static_pieces[2][2].point2.x = 1;
	static_pieces[2][2].point2.y = 2;
	static_pieces[2][2].point3.x = 2;
	static_pieces[2][2].point3.y = 1;
	static_pieces[2][2].point4.x = 2;
	static_pieces[2][2].point4.y = 2;
	static_pieces[2][2].origin.x = 0;
	static_pieces[2][2].origin.y = 0;
	static_pieces[2][2].piece_number = 2;
	static_pieces[2][2].rotation_number = 2;
	static_pieces[2][2].color = 0xFFE0; //yellow
	//o piece, rotation 3
	static_pieces[2][3].point1.x = 1;
	static_pieces[2][3].point1.y = 1;
	static_pieces[2][3].point2.x = 1;
	static_pieces[2][3].point2.y = 2;
	static_pieces[2][3].point3.x = 2;
	static_pieces[2][3].point3.y = 1;
	static_pieces[2][3].point4.x = 2;
	static_pieces[2][3].point4.y = 2;
	static_pieces[2][3].origin.x = 0;
	static_pieces[2][3].origin.y = 0;
	static_pieces[2][3].piece_number = 2;
	static_pieces[2][3].rotation_number = 3;
	static_pieces[2][3].color = 0xFFE0; //yellow
	//s piece, rotation 0
	static_pieces[3][0].point1.x = 0;
	static_pieces[3][0].point1.y = 1;
	static_pieces[3][0].point2.x = 1;
	static_pieces[3][0].point2.y = 1;
	static_pieces[3][0].point3.x = 1;
	static_pieces[3][0].point3.y = 2;
	static_pieces[3][0].point4.x = 2;
	static_pieces[3][0].point4.y = 2;
	static_pieces[3][0].origin.x = 0;
	static_pieces[3][0].origin.y = 0;
	static_pieces[3][0].piece_number = 3;
	static_pieces[3][0].rotation_number = 0;
	static_pieces[3][0].color = 0x07E0; //green
	//s piece, rotation 1
	static_pieces[3][1].point1.x = 1;
	static_pieces[3][1].point1.y = 1;
	static_pieces[3][1].point2.x = 1;
	static_pieces[3][1].point2.y = 2;
	static_pieces[3][1].point3.x = 2;
	static_pieces[3][1].point3.y = 0;
	static_pieces[3][1].point4.x = 2;
	static_pieces[3][1].point4.y = 1;
	static_pieces[3][1].origin.x = 0;
	static_pieces[3][1].origin.y = 0;
	static_pieces[3][1].piece_number = 3;
	static_pieces[3][1].rotation_number = 1;
	static_pieces[3][1].color = 0x07E0; //green
	//s piece, rotation 2
	static_pieces[3][2].point1.x = 0;
	static_pieces[3][2].point1.y = 0;
	static_pieces[3][2].point2.x = 1;
	static_pieces[3][2].point2.y = 0;
	static_pieces[3][2].point3.x = 1;
	static_pieces[3][2].point3.y = 1;
	static_pieces[3][2].point4.x = 2;
	static_pieces[3][2].point4.y = 1;
	static_pieces[3][2].origin.x = 0;
	static_pieces[3][2].origin.y = 0;
	static_pieces[3][2].piece_number = 3;
	static_pieces[3][2].rotation_number = 2;
	static_pieces[3][2].color = 0x07E0; //green
	//s piece, rotation 3
	static_pieces[3][3].point1.x = 0;
	static_pieces[3][3].point1.y = 1;
	static_pieces[3][3].point2.x = 0;
	static_pieces[3][3].point2.y = 2;
	static_pieces[3][3].point3.x = 1;
	static_pieces[3][3].point3.y = 0;
	static_pieces[3][3].point4.x = 1;
	static_pieces[3][3].point4.y = 1;
	static_pieces[3][3].origin.x = 0;
	static_pieces[3][3].origin.y = 0;
	static_pieces[3][3].piece_number = 3;
	static_pieces[3][3].rotation_number = 3;
	static_pieces[3][3].color = 0x07E0; //green
	//z piece, rotation 0
	static_pieces[4][0].point1.x = 0;
	static_pieces[4][0].point1.y = 2;
	static_pieces[4][0].point2.x = 1;
	static_pieces[4][0].point2.y = 1;
	static_pieces[4][0].point3.x = 1;
	static_pieces[4][0].point3.y = 2;
	static_pieces[4][0].point4.x = 2;
	static_pieces[4][0].point4.y = 1;
	static_pieces[4][0].origin.x = 0;
	static_pieces[4][0].origin.y = 0;
	static_pieces[4][0].piece_number = 4;
	static_pieces[4][0].rotation_number = 0;
	static_pieces[4][0].color = 0xF800; //red
	//z piece, rotation 1
	static_pieces[4][1].point1.x = 1;
	static_pieces[4][1].point1.y = 0;
	static_pieces[4][1].point2.x = 1;
	static_pieces[4][1].point2.y = 1;
	static_pieces[4][1].point3.x = 2;
	static_pieces[4][1].point3.y = 1;
	static_pieces[4][1].point4.x = 2;
	static_pieces[4][1].point4.y = 2;
	static_pieces[4][1].origin.x = 0;
	static_pieces[4][1].origin.y = 0;
	static_pieces[4][1].piece_number = 4;
	static_pieces[4][1].rotation_number = 1;
	static_pieces[4][1].color = 0xF800; //red
	//z piece, rotation 2
	static_pieces[4][2].point1.x = 0;
	static_pieces[4][2].point1.y = 1;
	static_pieces[4][2].point2.x = 1;
	static_pieces[4][2].point2.y = 0;
	static_pieces[4][2].point3.x = 1;
	static_pieces[4][2].point3.y = 1;
	static_pieces[4][2].point4.x = 2;
	static_pieces[4][2].point4.y = 0;
	static_pieces[4][2].origin.x = 0;
	static_pieces[4][2].origin.y = 0;
	static_pieces[4][2].piece_number = 4;
	static_pieces[4][2].rotation_number = 2;
	static_pieces[4][2].color = 0xF800; //red
	//z piece, rotation 3
	static_pieces[4][3].point1.x = 0;
	static_pieces[4][3].point1.y = 0;
	static_pieces[4][3].point2.x = 0;
	static_pieces[4][3].point2.y = 1;
	static_pieces[4][3].point3.x = 1;
	static_pieces[4][3].point3.y = 1;
	static_pieces[4][3].point4.x = 1;
	static_pieces[4][3].point4.y = 2;
	static_pieces[4][3].origin.x = 0;
	static_pieces[4][3].origin.y = 0;
	static_pieces[4][3].piece_number = 4;
	static_pieces[4][3].rotation_number = 3;
	static_pieces[4][3].color = 0xF800; //red
	//j piece, rotation 0
	static_pieces[5][0].point1.x = 0;
	static_pieces[5][0].point1.y = 1;
	static_pieces[5][0].point2.x = 0;
	static_pieces[5][0].point2.y = 2;
	static_pieces[5][0].point3.x = 1;
	static_pieces[5][0].point3.y = 1;
	static_pieces[5][0].point4.x = 2;
	static_pieces[5][0].point4.y = 1;
	static_pieces[5][0].origin.x = 0;
	static_pieces[5][0].origin.y = 0;
	static_pieces[5][0].piece_number = 5;
	static_pieces[5][0].rotation_number = 0;
	static_pieces[5][0].color = 0x001F; //blue
	//j piece, rotation 1
	static_pieces[5][1].point1.x = 1;
	static_pieces[5][1].point1.y = 0;
	static_pieces[5][1].point2.x = 1;
	static_pieces[5][1].point2.y = 1;
	static_pieces[5][1].point3.x = 1;
	static_pieces[5][1].point3.y = 2;
	static_pieces[5][1].point4.x = 2;
	static_pieces[5][1].point4.y = 2;
	static_pieces[5][1].origin.x = 0;
	static_pieces[5][1].origin.y = 0;
	static_pieces[5][1].piece_number = 5;
	static_pieces[5][1].rotation_number = 1;
	static_pieces[5][1].color = 0x001F; //blue
	//j piece, rotation 2
	static_pieces[5][2].point1.x = 0;
	static_pieces[5][2].point1.y = 1;
	static_pieces[5][2].point2.x = 1;
	static_pieces[5][2].point2.y = 1;
	static_pieces[5][2].point3.x = 2;
	static_pieces[5][2].point3.y = 0;
	static_pieces[5][2].point4.x = 2;
	static_pieces[5][2].point4.y = 1;
	static_pieces[5][2].origin.x = 0;
	static_pieces[5][2].origin.y = 0;
	static_pieces[5][2].piece_number = 5;
	static_pieces[5][2].rotation_number = 2;
	static_pieces[5][2].color = 0x001F; //blue
	//j piece, rotation 3
	static_pieces[5][3].point1.x = 0;
	static_pieces[5][3].point1.y = 0;
	static_pieces[5][3].point2.x = 1;
	static_pieces[5][3].point2.y = 0;
	static_pieces[5][3].point3.x = 1;
	static_pieces[5][3].point3.y = 1;
	static_pieces[5][3].point4.x = 1;
	static_pieces[5][3].point4.y = 2;
	static_pieces[5][3].origin.x = 0;
	static_pieces[5][3].origin.y = 0;
	static_pieces[5][3].piece_number = 5;
	static_pieces[5][3].rotation_number = 3;
	static_pieces[5][3].color = 0x001F; //blue
	//l piece, rotation 0
	static_pieces[6][0].point1.x = 0;
	static_pieces[6][0].point1.y = 1;
	static_pieces[6][0].point2.x = 1;
	static_pieces[6][0].point2.y = 1;
	static_pieces[6][0].point3.x = 2;
	static_pieces[6][0].point3.y = 1;
	static_pieces[6][0].point4.x = 2;
	static_pieces[6][0].point4.y = 2;
	static_pieces[6][0].origin.x = 0;
	static_pieces[6][0].origin.y = 0;
	static_pieces[6][0].piece_number = 6;
	static_pieces[6][0].rotation_number = 0;
	static_pieces[6][0].color = 0xFD20; //orange
	//l piece, rotation 1
	static_pieces[6][1].point1.x = 1;
	static_pieces[6][1].point1.y = 0;
	static_pieces[6][1].point2.x = 1;
	static_pieces[6][1].point2.y = 1;
	static_pieces[6][1].point3.x = 1;
	static_pieces[6][1].point3.y = 2;
	static_pieces[6][1].point4.x = 2;
	static_pieces[6][1].point4.y = 0;
	static_pieces[6][1].origin.x = 0;
	static_pieces[6][1].origin.y = 0;
	static_pieces[6][1].piece_number = 6;
	static_pieces[6][1].rotation_number = 1;
	static_pieces[6][1].color = 0xFD20; //orange
	//l piece, rotation 2
	static_pieces[6][2].point1.x = 0;
	static_pieces[6][2].point1.y = 0;
	static_pieces[6][2].point2.x = 0;
	static_pieces[6][2].point2.y = 1;
	static_pieces[6][2].point3.x = 1;
	static_pieces[6][2].point3.y = 1;
	static_pieces[6][2].point4.x = 2;
	static_pieces[6][2].point4.y = 1;
	static_pieces[6][2].origin.x = 0;
	static_pieces[6][2].origin.y = 0;
	static_pieces[6][2].piece_number = 6;
	static_pieces[6][2].rotation_number = 2;
	static_pieces[6][2].color = 0xFD20; //orange
	//l piece, rotation 3
	static_pieces[6][3].point1.x = 0;
	static_pieces[6][3].point1.y = 2;
	static_pieces[6][3].point2.x = 1;
	static_pieces[6][3].point2.y = 0;
	static_pieces[6][3].point3.x = 1;
	static_pieces[6][3].point3.y = 1;
	static_pieces[6][3].point4.x = 1;
	static_pieces[6][3].point4.y = 2;
	static_pieces[6][3].origin.x = 0;
	static_pieces[6][3].origin.y = 0;
	static_pieces[6][3].piece_number = 6;
	static_pieces[6][3].rotation_number = 3;
	static_pieces[6][3].color = 0xFD20; //orange
}

Piece* gen_piece(void) {
	uint32_t index = Random()%7;
	return &static_pieces[index][0];
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
	dest->piece_number = src->piece_number;
	dest->rotation_number = src->rotation_number;
	dest->color = src->color;
}

void display_start_menu(void) {
	//TODO: display start screen
}

// 0 = do nothing, 1 = left, 2 = right, rest is down
uint8_t get_buttons(void) {
	//TODO: get the input from
	return 0;
}

void draw_game_start(void) {
	ST7735_FillRect(0, 0, 80, 160, 0);
	ST7735_DrawFastVLine(80, 0, 160, 0xFFFF);
	ST7735_FillRect(81, 0, 47, 160, 0x7BE0);
	ST7735_SetCursor(85, 10);
	ST7735_OutString("Next Piece");
	ST7735_SetCursor(85, 80);
	ST7735_OutString("Score");
}

void draw_piece(Piece* p, uint16_t color) {
	ST7735_FillRect(p->point1.x * 80, p->point1.y * 160, 8, 8, color);
	ST7735_FillRect(p->point2.x * 80, p->point2.y * 160, 8, 8, color);
	ST7735_FillRect(p->point3.x * 80, p->point3.y * 160, 8, 8, color);
	ST7735_FillRect(p->point4.x * 80, p->point4.y * 160, 8, 8, color);
}

void draw_score() {
	ST7735_SetCursor(85, 100);
	LCD_OutDec(score);
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
		if(play_state != DO_NOTHING) return;
		uint8_t input = get_buttons();
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
	draw_game_start();
	Point origin;
	origin.x = 3;
	origin.y = 3;
	copy_piece(&current_piece, gen_piece(), origin);
	copy_piece(&next_piece, gen_piece(), origin);
	draw_piece(&current_piece, current_piece.color);
	play_state = DO_NOTHING;
	//TODO: display the next piece
	while(mode == ONE_PLAYER) {
		//TODO: try to move piece
		play_state = DO_NOTHING;
	}
	//TODO: display score screen
}

void game_two(void) {
	while(mode == TWO_PLAYER) {
		
	}
}

void rotate(void) {
	uint16_t next_rot = (current_piece.rotation_number+1)%4;
	Piece *tmp = &static_pieces[current_piece.piece_number][next_rot];
	// calculate location of the next rotated pieces
	int x1 = current_piece.origin.x + tmp->point1.x;
	int x2 = current_piece.origin.x + tmp->point2.x;
	int x3 = current_piece.origin.x + tmp->point3.x;
	int x4 = current_piece.origin.x + tmp->point4.x;
	int y1 = current_piece.origin.y + tmp->point1.y;
	int y2 = current_piece.origin.y + tmp->point2.y;
	int y3 = current_piece.origin.y + tmp->point3.y;
	int y4 = current_piece.origin.y + tmp->point4.y;
	// check if the rotated piece is out of bounds
	if (x1 < 0 || x1 >= 10 || y1 < 0 || y1 >= 20
			|| x2 < 0 || x2 >= 10 || y2 < 0 || y2 >= 20
			|| x3 < 0 || x3 >= 10 || y3 < 0 || y3 >= 20
			|| x4 < 0 || x4 >= 10 || y4 < 0 || y4 >= 20) {
		return;
	}
	// check if something is in the way of the rotation
	if (board[y1][x1] != 0 || board[y2][x2] != 0 
			|| board[y3][x3] != 0 || board[y4][x4] != 0) {
		return;
	}
	draw_piece(&current_piece, 0);
	copy_piece(&current_piece, tmp, current_piece.origin);
	draw_piece(&current_piece, current_piece.color);
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
	if (board[y1][x1] != 0 ||
			board[y2][x2] != 0 ||
			board[y3][x3] != 0 ||
			board[y4][x4] != 0) {
		return;
	}
	draw_piece(&current_piece, 0);
	current_piece.point1.x = x1;
	current_piece.point2.x = x2;
	current_piece.point3.x = x3;
	current_piece.point4.x = x4;
	current_piece.origin.x --;
	draw_piece(&current_piece, current_piece.color);
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
	if (board[y1][x1] != 0 ||
			board[y2][x2] != 0 ||
			board[y3][x3] != 0 ||
			board[y4][x4] != 0) {
		return;
	}
	draw_piece(&current_piece, 0);
	current_piece.point1.x = x1;
	current_piece.point2.x = x2;
	current_piece.point3.x = x3;
	current_piece.point4.x = x4;
	current_piece.origin.x ++;
	draw_piece(&current_piece, current_piece.color);
}

void place(void) {
	//TODO: check for full row, clear stuff if there are rows to clear
	// mark the piece on the board with the color
	board[current_piece.point1.y][current_piece.point1.x] = current_piece.color;
	board[current_piece.point2.y][current_piece.point2.x] = current_piece.color;
	board[current_piece.point3.y][current_piece.point3.x] = current_piece.color;
	board[current_piece.point4.y][current_piece.point4.x] = current_piece.color;
	// check if the placed piece is above the cutoff for the game
	if (current_piece.point1.y == 0 ||
			current_piece.point2.y == 0 ||
			current_piece.point3.y == 0 ||
			current_piece.point4.y == 0) {
		mode = FINISHED;
		return;
	}
	// generate a new piece and copy it over
	Point origin;
	origin.x = 3;
	origin.y = 3;
	copy_piece(&current_piece, &next_piece, origin);
	draw_piece(&current_piece, current_piece.color);
	copy_piece(&next_piece, gen_piece(), origin);
	//TODO: render next piece
	if (board[current_piece.point1.y][current_piece.point1.x] != 0 ||
		 board[current_piece.point2.y][current_piece.point2.x] != 0 ||
		 board[current_piece.point3.y][current_piece.point3.x] != 0 ||
		 board[current_piece.point4.y][current_piece.point4.x] != 0) {
		mode = FINISHED;
		return;
	}
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
	if (y1 >= 20 || y2 >= 20 || y3 >= 20 || y4 >= 20) {
		place();
		return;
	}
	// if a piece is blocking it from moving
	if (board[y1][x1] != 0 || 
			board[y2][x2] != 0 ||
			board[y3][x3] != 0 ||
			board[y4][x4] != 0) {
		place();
		return;
	}
	draw_piece(&current_piece, 0);
	current_piece.point1.y = y1;
	current_piece.point2.y = y2;
	current_piece.point3.y = y3;
	current_piece.point4.y = y4;
	current_piece.origin.y ++;
	draw_piece(&current_piece, current_piece.color);
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
