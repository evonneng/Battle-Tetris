#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "ST7735.h"
#include "Random.h"
#include "TExaS.h"
#include "ADC.h"
#include "UART.h"
#include "Sound.h"
#include "UART.h"
#include "FiFo.h"

#define START_MENU 0
#define ONE_PLAYER 1
#define TWO_PLAYER 2
#define FINISHED 3

#define DO_NOTHING 0
#define MOVE_LEFT 1
#define MOVE_RIGHT 2
#define MOVE_DOWN 3
#define ROTATE 4

#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))

#define RIGHT_SIDE_COLOR 0x7BE0 //olive

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
uint32_t key_pressed = 0;
uint32_t heartbeat = 0;

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

void buttons_init(void) { //PE 0, 1, 3, 4
	SYSCTL_RCGCGPIO_R |= 0x00000010; // 1) activate clock for Port E
	int delay;
	delay = SYSCTL_RCGCGPIO_R; // allow time for clock to stabilize
	GPIO_PORTE_DIR_R &= ~0x1B; // 2) make PE ports input
	GPIO_PORTE_AFSEL_R &= ~0x1B; // 3) disable alternate function on PE
	GPIO_PORTE_DEN_R |= 0x1B; // 4) enable digital I/O on PE
	//GPIO_PORTE_AMSEL_R |= 0x1B; // 5) enable analog function on PE
}

void PortF_Init(void){
  // Intialize PortF for hearbeat
	SYSCTL_RCGCGPIO_R |= 0x00000020;
	int delay;
	delay = SYSCTL_RCGCGPIO_R;
	GPIO_PORTF_DIR_R |= 0x0E;
	GPIO_PORTF_AFSEL_R &= ~0x0E;
	GPIO_PORTF_DEN_R |= 0x0E;
}

void board_init(void) {
	for(int i = 0; i < 20; i++) {
		for(int j = 0; j < 10; j++) {
			board[i][j] = 0xFFFF;
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
	static_pieces[0][0].point3.y = 0;
	static_pieces[0][0].point4.x = 2;
	static_pieces[0][0].point4.y = 1;
	static_pieces[0][0].origin.x = 0;
	static_pieces[0][0].origin.y = 0;
	static_pieces[0][0].piece_number = 0;
	static_pieces[0][0].rotation_number = 0;
	static_pieces[0][0].color = 0x780F; //purple
	//t piece, rotation 1
	static_pieces[0][1].point1.x = 1;
	static_pieces[0][1].point1.y = 2;
	static_pieces[0][1].point2.x = 1;
	static_pieces[0][1].point2.y = 1;
	static_pieces[0][1].point3.x = 1;
	static_pieces[0][1].point3.y = 0;
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
	static_pieces[0][2].point2.y = 2;
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
	static_pieces[0][3].point2.y = 2;
	static_pieces[0][3].point3.x = 1;
	static_pieces[0][3].point3.y = 1;
	static_pieces[0][3].point4.x = 1;
	static_pieces[0][3].point4.y = 0;
	static_pieces[0][3].origin.x = 0;
	static_pieces[0][3].origin.y = 0;
	static_pieces[0][3].piece_number = 0;
	static_pieces[0][3].rotation_number = 3;
	static_pieces[0][3].color = 0x780F; //purple
	//i piece, rotation 0
	static_pieces[1][0].point1.x = 0;
	static_pieces[1][0].point1.y = 1;
	static_pieces[1][0].point2.x = 1;
	static_pieces[1][0].point2.y = 1;
	static_pieces[1][0].point3.x = 2;
	static_pieces[1][0].point3.y = 1;
	static_pieces[1][0].point4.x = 3;
	static_pieces[1][0].point4.y = 1;
	static_pieces[1][0].origin.x = 0;
	static_pieces[1][0].origin.y = 0;
	static_pieces[1][0].piece_number = 1;
	static_pieces[1][0].rotation_number = 0;
	static_pieces[1][0].color = 0x07FF; //cyan
	//i piece, rotation 1
	static_pieces[1][1].point1.x = 2;
	static_pieces[1][1].point1.y = 3;
	static_pieces[1][1].point2.x = 2;
	static_pieces[1][1].point2.y = 2;
	static_pieces[1][1].point3.x = 2;
	static_pieces[1][1].point3.y = 1;
	static_pieces[1][1].point4.x = 2;
	static_pieces[1][1].point4.y = 0;
	static_pieces[1][1].origin.x = 0;
	static_pieces[1][1].origin.y = 0;
	static_pieces[1][1].piece_number = 1;
	static_pieces[1][1].rotation_number = 1;
	static_pieces[1][1].color = 0x07FF; //cyan
	//i piece, rotation 2
	static_pieces[1][2].point1.x = 0;
	static_pieces[1][2].point1.y = 2;
	static_pieces[1][2].point2.x = 1;
	static_pieces[1][2].point2.y = 2;
	static_pieces[1][2].point3.x = 2;
	static_pieces[1][2].point3.y = 2;
	static_pieces[1][2].point4.x = 3;
	static_pieces[1][2].point4.y = 2;
	static_pieces[1][2].origin.x = 0;
	static_pieces[1][2].origin.y = 0;
	static_pieces[1][2].piece_number = 1;
	static_pieces[1][2].rotation_number = 2;
	static_pieces[1][2].color = 0x07FF; //cyan
	//i piece, rotation 3
	static_pieces[1][3].point1.x = 1;
	static_pieces[1][3].point1.y = 3;
	static_pieces[1][3].point2.x = 1;
	static_pieces[1][3].point2.y = 2;
	static_pieces[1][3].point3.x = 1;
	static_pieces[1][3].point3.y = 1;
	static_pieces[1][3].point4.x = 1;
	static_pieces[1][3].point4.y = 0;
	static_pieces[1][3].origin.x = 0;
	static_pieces[1][3].origin.y = 0;
	static_pieces[1][3].piece_number = 1;
	static_pieces[1][3].rotation_number = 3;
	static_pieces[1][3].color = 0x07FF; //cyan
	//o piece, rotation 0
	static_pieces[2][0].point1.x = 1;
	static_pieces[2][0].point1.y = 1;
	static_pieces[2][0].point2.x = 1;
	static_pieces[2][0].point2.y = 0;
	static_pieces[2][0].point3.x = 2;
	static_pieces[2][0].point3.y = 1;
	static_pieces[2][0].point4.x = 2;
	static_pieces[2][0].point4.y = 0;
	static_pieces[2][0].origin.x = 0;
	static_pieces[2][0].origin.y = 0;
	static_pieces[2][0].piece_number = 2;
	static_pieces[2][0].rotation_number = 0;
	static_pieces[2][0].color = 0xFFE0; //yellow
	//o piece, rotation 1
	static_pieces[2][1].point1.x = 1;
	static_pieces[2][1].point1.y = 1;
	static_pieces[2][1].point2.x = 1;
	static_pieces[2][1].point2.y = 0;
	static_pieces[2][1].point3.x = 2;
	static_pieces[2][1].point3.y = 1;
	static_pieces[2][1].point4.x = 2;
	static_pieces[2][1].point4.y = 0;
	static_pieces[2][1].origin.x = 0;
	static_pieces[2][1].origin.y = 0;
	static_pieces[2][1].piece_number = 2;
	static_pieces[2][1].rotation_number = 1;
	static_pieces[2][1].color = 0xFFE0; //yellow
	//o piece, rotation 2
	static_pieces[2][2].point1.x = 1;
	static_pieces[2][2].point1.y = 1;
	static_pieces[2][2].point2.x = 1;
	static_pieces[2][2].point2.y = 0;
	static_pieces[2][2].point3.x = 2;
	static_pieces[2][2].point3.y = 1;
	static_pieces[2][2].point4.x = 2;
	static_pieces[2][2].point4.y = 0;
	static_pieces[2][2].origin.x = 0;
	static_pieces[2][2].origin.y = 0;
	static_pieces[2][2].piece_number = 2;
	static_pieces[2][2].rotation_number = 2;
	static_pieces[2][2].color = 0xFFE0; //yellow
	//o piece, rotation 3
	static_pieces[2][3].point1.x = 1;
	static_pieces[2][3].point1.y = 1;
	static_pieces[2][3].point2.x = 1;
	static_pieces[2][3].point2.y = 0;
	static_pieces[2][3].point3.x = 2;
	static_pieces[2][3].point3.y = 1;
	static_pieces[2][3].point4.x = 2;
	static_pieces[2][3].point4.y = 0;
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
	static_pieces[3][0].point3.y = 0;
	static_pieces[3][0].point4.x = 2;
	static_pieces[3][0].point4.y = 0;
	static_pieces[3][0].origin.x = 0;
	static_pieces[3][0].origin.y = 0;
	static_pieces[3][0].piece_number = 3;
	static_pieces[3][0].rotation_number = 0;
	static_pieces[3][0].color = 0x07E0; //green
	//s piece, rotation 1
	static_pieces[3][1].point1.x = 1;
	static_pieces[3][1].point1.y = 1;
	static_pieces[3][1].point2.x = 1;
	static_pieces[3][1].point2.y = 0;
	static_pieces[3][1].point3.x = 2;
	static_pieces[3][1].point3.y = 2;
	static_pieces[3][1].point4.x = 2;
	static_pieces[3][1].point4.y = 1;
	static_pieces[3][1].origin.x = 0;
	static_pieces[3][1].origin.y = 0;
	static_pieces[3][1].piece_number = 3;
	static_pieces[3][1].rotation_number = 1;
	static_pieces[3][1].color = 0x07E0; //green
	//s piece, rotation 2
	static_pieces[3][2].point1.x = 0;
	static_pieces[3][2].point1.y = 2;
	static_pieces[3][2].point2.x = 1;
	static_pieces[3][2].point2.y = 2;
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
	static_pieces[3][3].point2.y = 0;
	static_pieces[3][3].point3.x = 1;
	static_pieces[3][3].point3.y = 2;
	static_pieces[3][3].point4.x = 1;
	static_pieces[3][3].point4.y = 1;
	static_pieces[3][3].origin.x = 0;
	static_pieces[3][3].origin.y = 0;
	static_pieces[3][3].piece_number = 3;
	static_pieces[3][3].rotation_number = 3;
	static_pieces[3][3].color = 0x07E0; //green
	//z piece, rotation 0
	static_pieces[4][0].point1.x = 0;
	static_pieces[4][0].point1.y = 0;
	static_pieces[4][0].point2.x = 1;
	static_pieces[4][0].point2.y = 1;
	static_pieces[4][0].point3.x = 1;
	static_pieces[4][0].point3.y = 0;
	static_pieces[4][0].point4.x = 2;
	static_pieces[4][0].point4.y = 1;
	static_pieces[4][0].origin.x = 0;
	static_pieces[4][0].origin.y = 0;
	static_pieces[4][0].piece_number = 4;
	static_pieces[4][0].rotation_number = 0;
	static_pieces[4][0].color = 0xF800; //red
	//z piece, rotation 1
	static_pieces[4][1].point1.x = 1;
	static_pieces[4][1].point1.y = 2;
	static_pieces[4][1].point2.x = 1;
	static_pieces[4][1].point2.y = 1;
	static_pieces[4][1].point3.x = 2;
	static_pieces[4][1].point3.y = 1;
	static_pieces[4][1].point4.x = 2;
	static_pieces[4][1].point4.y = 0;
	static_pieces[4][1].origin.x = 0;
	static_pieces[4][1].origin.y = 0;
	static_pieces[4][1].piece_number = 4;
	static_pieces[4][1].rotation_number = 1;
	static_pieces[4][1].color = 0xF800; //red
	//z piece, rotation 2
	static_pieces[4][2].point1.x = 0;
	static_pieces[4][2].point1.y = 1;
	static_pieces[4][2].point2.x = 1;
	static_pieces[4][2].point2.y = 2;
	static_pieces[4][2].point3.x = 1;
	static_pieces[4][2].point3.y = 1;
	static_pieces[4][2].point4.x = 2;
	static_pieces[4][2].point4.y = 2;
	static_pieces[4][2].origin.x = 0;
	static_pieces[4][2].origin.y = 0;
	static_pieces[4][2].piece_number = 4;
	static_pieces[4][2].rotation_number = 2;
	static_pieces[4][2].color = 0xF800; //red
	//z piece, rotation 3
	static_pieces[4][3].point1.x = 0;
	static_pieces[4][3].point1.y = 2;
	static_pieces[4][3].point2.x = 0;
	static_pieces[4][3].point2.y = 1;
	static_pieces[4][3].point3.x = 1;
	static_pieces[4][3].point3.y = 1;
	static_pieces[4][3].point4.x = 1;
	static_pieces[4][3].point4.y = 0;
	static_pieces[4][3].origin.x = 0;
	static_pieces[4][3].origin.y = 0;
	static_pieces[4][3].piece_number = 4;
	static_pieces[4][3].rotation_number = 3;
	static_pieces[4][3].color = 0xF800; //red
	//j piece, rotation 0
	static_pieces[5][0].point1.x = 0;
	static_pieces[5][0].point1.y = 1;
	static_pieces[5][0].point2.x = 0;
	static_pieces[5][0].point2.y = 0;
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
	static_pieces[5][1].point1.y = 2;
	static_pieces[5][1].point2.x = 1;
	static_pieces[5][1].point2.y = 1;
	static_pieces[5][1].point3.x = 1;
	static_pieces[5][1].point3.y = 0;
	static_pieces[5][1].point4.x = 2;
	static_pieces[5][1].point4.y = 0;
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
	static_pieces[5][2].point3.y = 2;
	static_pieces[5][2].point4.x = 2;
	static_pieces[5][2].point4.y = 1;
	static_pieces[5][2].origin.x = 0;
	static_pieces[5][2].origin.y = 0;
	static_pieces[5][2].piece_number = 5;
	static_pieces[5][2].rotation_number = 2;
	static_pieces[5][2].color = 0x001F; //blue
	//j piece, rotation 3
	static_pieces[5][3].point1.x = 0;
	static_pieces[5][3].point1.y = 2;
	static_pieces[5][3].point2.x = 1;
	static_pieces[5][3].point2.y = 2;
	static_pieces[5][3].point3.x = 1;
	static_pieces[5][3].point3.y = 1;
	static_pieces[5][3].point4.x = 1;
	static_pieces[5][3].point4.y = 0;
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
	static_pieces[6][0].point4.y = 0;
	static_pieces[6][0].origin.x = 0;
	static_pieces[6][0].origin.y = 0;
	static_pieces[6][0].piece_number = 6;
	static_pieces[6][0].rotation_number = 0;
	static_pieces[6][0].color = 0xFD20; //orange
	//l piece, rotation 1
	static_pieces[6][1].point1.x = 1;
	static_pieces[6][1].point1.y = 2;
	static_pieces[6][1].point2.x = 1;
	static_pieces[6][1].point2.y = 1;
	static_pieces[6][1].point3.x = 1;
	static_pieces[6][1].point3.y = 0;
	static_pieces[6][1].point4.x = 2;
	static_pieces[6][1].point4.y = 2;
	static_pieces[6][1].origin.x = 0;
	static_pieces[6][1].origin.y = 0;
	static_pieces[6][1].piece_number = 6;
	static_pieces[6][1].rotation_number = 1;
	static_pieces[6][1].color = 0xFD20; //orange
	//l piece, rotation 2
	static_pieces[6][2].point1.x = 0;
	static_pieces[6][2].point1.y = 2;
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
	static_pieces[6][3].point1.y = 0;
	static_pieces[6][3].point2.x = 1;
	static_pieces[6][3].point2.y = 2;
	static_pieces[6][3].point3.x = 1;
	static_pieces[6][3].point3.y = 1;
	static_pieces[6][3].point4.x = 1;
	static_pieces[6][3].point4.y = 0;
	static_pieces[6][3].origin.x = 0;
	static_pieces[6][3].origin.y = 0;
	static_pieces[6][3].piece_number = 6;
	static_pieces[6][3].rotation_number = 3;
	static_pieces[6][3].color = 0xFD20; //orange
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

Piece* gen_piece(void) {
	uint32_t index = Random()%7;
	return &static_pieces[index][0];
}

void gen_next_piece(void) {
	ST7735_FillRect(next_piece.point1.x * 8 + 92, next_piece.point1.y * 8 + 30, 8, 8, RIGHT_SIDE_COLOR);
	ST7735_FillRect(next_piece.point2.x * 8 + 92, next_piece.point2.y * 8 + 30, 8, 8, RIGHT_SIDE_COLOR);
	ST7735_FillRect(next_piece.point3.x * 8 + 92, next_piece.point3.y * 8 + 30, 8, 8, RIGHT_SIDE_COLOR);
	ST7735_FillRect(next_piece.point4.x * 8 + 92, next_piece.point4.y * 8 + 30, 8, 8, RIGHT_SIDE_COLOR);	
	Point origin;
	origin.x = 0;
	origin.y = 0;
	copy_piece(&next_piece, gen_piece(), origin);
	ST7735_FillRect(next_piece.point1.x * 8 + 92, next_piece.point1.y * 8 + 30, 8, 8, next_piece.color);
	ST7735_FillRect(next_piece.point2.x * 8 + 92, next_piece.point2.y * 8 + 30, 8, 8, next_piece.color);
	ST7735_FillRect(next_piece.point3.x * 8 + 92, next_piece.point3.y * 8 + 30, 8, 8, next_piece.color);
	ST7735_FillRect(next_piece.point4.x * 8 + 92, next_piece.point4.y * 8 + 30, 8, 8, next_piece.color);
}

// 0 = do nothing, 1 = left, 2 = right, 3 = rotate, 4 = down
uint8_t get_buttons(void) {
	uint32_t input = GPIO_PORTE_DATA_R & 0x1B;
	if(input & 1)
		return 1;
	if(input & 2)
		return 2;
	if(input & 8)
		return 3;
	if(input & 0x10)
		return 4;
	return 0;
}

uint32_t get_slider() {
	uint32_t slider = ADC_In();
	return slider/68;
}

void draw_start_menu(void) {
	ST7735_FillRect(0, 0, 128, 5, 0x7BE0);
	ST7735_FillRect(0, 0, 5, 160, 0x7BE0);
	ST7735_FillRect(124, 0, 5, 160, 0x7BE0);
	ST7735_FillRect(0, 156, 128, 5, 0x7BE0);
	ST7735_FillRect(5, 5, 120, 152, 0xC618);
	ST7735_DrawStringS(2, 2, "TETRIS", 0x001F, 0xC618, 3);
	ST7735_DrawStringS(6, 6, "One Player", 0xF800, 0xC618, 1);
	ST7735_DrawStringS(6, 10, "Two Player", 0x03EF, 0xC618, 1);
}

void draw_score() {
	ST7735_SetCursor(15, 10);
	LCD_OutDec(score);
}

void draw_game_start(void) {
	ST7735_FillRect(0, 0, 80, 160, 0xFFFF);
	ST7735_DrawFastVLine(80, 0, 160, 0);
	ST7735_FillRect(81, 0, 47, 160, RIGHT_SIDE_COLOR);
	ST7735_DrawStringS(15, 0, "Next", 0xFFFF, RIGHT_SIDE_COLOR, 1);
	gen_next_piece();
	ST7735_DrawStringS(15, 8, "Score", 0xFFFF, RIGHT_SIDE_COLOR, 1);
	draw_score();
}

void draw_piece(Piece* p, uint16_t color) {
	ST7735_FillRect(p->point1.x * 8, p->point1.y * 8, 8, 8, color);
	ST7735_FillRect(p->point2.x * 8, p->point2.y * 8, 8, 8, color);
	ST7735_FillRect(p->point3.x * 8, p->point3.y * 8, 8, 8, color);
	ST7735_FillRect(p->point4.x * 8, p->point4.y * 8, 8, 8, color);
}

void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;                   // disable SysTick during setup
  NVIC_ST_RELOAD_R = 2000000-1;  // maximum reload value
  NVIC_ST_CURRENT_R = 0;                // any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x20000000; // priority 1
  NVIC_ST_CTRL_R = 0x0007;
}

uint32_t move_down_timer = 0;
uint8_t past_input = 0;
uint32_t MOVE_DOWN_MAX = 30;
void SysTick_Handler(void) {
	PF2 ^= 0x4;
	if(mode == START_MENU || mode == FINISHED)
		return;
	MOVE_DOWN_MAX = get_slider();
	move_down_timer++;
	if(move_down_timer >= MOVE_DOWN_MAX) {
		play_state = MOVE_DOWN;
		move_down_timer = 0;
	} else {
		if(play_state != DO_NOTHING) return;
		uint8_t input = get_buttons();
		if(input == past_input) return;
		if(input == 0)
			play_state = DO_NOTHING;
		else if(input == 1)
			play_state = MOVE_LEFT;
		else if(input == 2)
			play_state = MOVE_RIGHT;
		else if(input == 3)
			play_state = ROTATE;
		else if(input == 4)
			play_state = MOVE_DOWN;
		else
			play_state = DO_NOTHING;
		past_input = input;
	}
}

void rotate(void) {
	Sound_Rotate();	
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
	if (board[y1][x1] != 0xFFFF || board[y2][x2] != 0xFFFF 
			|| board[y3][x3] != 0xFFFF || board[y4][x4] != 0xFFFF) {
		return;
	}
	draw_piece(&current_piece, 0xFFFF);
	copy_piece(&current_piece, tmp, current_piece.origin);
	draw_piece(&current_piece, current_piece.color);
}

void left(void) {
	Sound_Left();
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
	if (board[y1][x1] != 0xFFFF ||
			board[y2][x2] != 0xFFFF ||
			board[y3][x3] != 0xFFFF ||
			board[y4][x4] != 0xFFFF) {
		return;
	}
	draw_piece(&current_piece, 0xFFFF);
	current_piece.point1.x = x1;
	current_piece.point2.x = x2;
	current_piece.point3.x = x3;
	current_piece.point4.x = x4;
	current_piece.origin.x --;
	draw_piece(&current_piece, current_piece.color);
}

void right(void) {
	Sound_Right();
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
	if (board[y1][x1] != 0xFFFF ||
			board[y2][x2] != 0xFFFF ||
			board[y3][x3] != 0xFFFF ||
			board[y4][x4] != 0xFFFF) {
		return;
	}
	draw_piece(&current_piece, 0xFFFF);
	current_piece.point1.x = x1;
	current_piece.point2.x = x2;
	current_piece.point3.x = x3;
	current_piece.point4.x = x4;
	current_piece.origin.x ++;
	draw_piece(&current_piece, current_piece.color);
}

// naive way of clearing the board one row
void clear_row(int row) {
	// shift everthing from the row above down
	for (int y = row; y > 0; y--) {
		for (int x = 0; x < 10; x++) {
			board[y][x] = board[y-1][x];
			ST7735_FillRect(x * 8, y * 8, 8, 8, board[y][x]);
		}
	}
	// set the top row as a cleared row
	for (int x = 0; x < 10; x++) {
		board[0][x] = 0xFFFF;
		ST7735_FillRect(x * 8, 0, 8, 8, 0xFFFF);
	}
}

// naive way of checking if there is a row to be cleared
void update_score() {
	uint8_t num_cleared;
	int clear;
	for (int y = 0; y < 20; y++) {
		clear = 1; //init to true
		for (int x = 0; x < 10; x++) {
			if (board[y][x] == 0xFFFF) {
				// if there is a blank in the row
				clear = 0;
				break;
			}
		}
		if (clear) {
			clear_row(y);
			score += 10; //increment score
			num_cleared++;
		}
	}
	draw_score();
	if(mode == TWO_PLAYER) {
		//TODO: send x character based on # lines
		UART_OutChar((char)num_cleared);
	}
}

void place(void) {
	// mark the piece on the board with the color
	board[current_piece.point1.y][current_piece.point1.x] = current_piece.color;
	board[current_piece.point2.y][current_piece.point2.x] = current_piece.color;
	board[current_piece.point3.y][current_piece.point3.x] = current_piece.color;
	board[current_piece.point4.y][current_piece.point4.x] = current_piece.color;
	update_score();
	// check if the placed piece is above the cutoff for the game
	if (current_piece.point1.y == 0 ||
			current_piece.point2.y == 0 ||
			current_piece.point3.y == 0 ||
			current_piece.point4.y == 0) {
		mode = FINISHED;
		Sound_Game_Over();
		return;
	}
	// generate a new piece and copy it over
	Point origin;
	origin.x = 3;
	origin.y = 0;
	copy_piece(&current_piece, &next_piece, origin);
	draw_piece(&current_piece, current_piece.color);
	gen_next_piece();
	if (board[current_piece.point1.y][current_piece.point1.x] != 0xFFFF ||
		 board[current_piece.point2.y][current_piece.point2.x] != 0xFFFF ||
		 board[current_piece.point3.y][current_piece.point3.x] != 0xFFFF ||
		 board[current_piece.point4.y][current_piece.point4.x] != 0xFFFF) {
		mode = FINISHED;
		Sound_Game_Over();
		return;
	}
}

void down(void) {
	Sound_Left();
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
	if (board[y1][x1] != 0xFFFF || 
			board[y2][x2] != 0xFFFF ||
			board[y3][x3] != 0xFFFF ||
			board[y4][x4] != 0xFFFF) {
		place();
		return;
	}
	draw_piece(&current_piece, 0xFFFF);
	current_piece.point1.y = y1;
	current_piece.point2.y = y2;
	current_piece.point3.y = y3;
	current_piece.point4.y = y4;
	current_piece.origin.y ++;
	draw_piece(&current_piece, current_piece.color);
}

void game_one(void) {
	draw_game_start();
	Point origin;
	origin.x = 3;
	origin.y = 0;
	copy_piece(&current_piece, gen_piece(), origin);
	draw_piece(&current_piece, current_piece.color);
	mode = ONE_PLAYER;
	play_state = DO_NOTHING;
	while(mode == ONE_PLAYER) {
		if(play_state == MOVE_LEFT) {
			play_state = DO_NOTHING;
			left();
		}
		else if(play_state == MOVE_RIGHT) {
			play_state = DO_NOTHING;
			right();
		}
		else if(play_state == ROTATE) {
			play_state = DO_NOTHING;
			rotate();
		}
		else if(play_state == MOVE_DOWN) {
			play_state = DO_NOTHING;
			down();
		}
	}
	ST7735_DrawStringS(1, 6, " Game Over ", 0xFFFF, 0, 1);
}

//place the piece but do not generate a new piece
void spawn_place(void) {
   // mark the piece on the board with the color
   board[current_piece.point1.y][current_piece.point1.x] = current_piece.color;
   board[current_piece.point2.y][current_piece.point2.x] = current_piece.color;
   board[current_piece.point3.y][current_piece.point3.x] = current_piece.color;
   board[current_piece.point4.y][current_piece.point4.x] = current_piece.color;
   update_score();
   // check if the placed piece is above the cutoff for the game
   if (current_piece.point1.y == 0 ||
         current_piece.point2.y == 0 ||
         current_piece.point3.y == 0 ||
         current_piece.point4.y == 0) {
      mode = FINISHED;
      Sound_Game_Over();
   }
}

// generate a new piece after placing a piece
void spawn_gen(void) {
	// generate a new piece and copy it over
   Point origin;
   origin.x = 3;
   origin.y = 0;
   copy_piece(&current_piece, &next_piece, origin);
   draw_piece(&current_piece, current_piece.color);
   gen_next_piece();
   if (board[current_piece.point1.y][current_piece.point1.x] != 0xFFFF ||
       board[current_piece.point2.y][current_piece.point2.x] != 0xFFFF ||
       board[current_piece.point3.y][current_piece.point3.x] != 0xFFFF ||
       board[current_piece.point4.y][current_piece.point4.x] != 0xFFFF) {
      mode = FINISHED;
      Sound_Game_Over();
   }
}

// create a new line with a single blank in the middle 
void spawn_line(void) {
	bool placed = false;
	// check if current piece will collide with moving up
	if (board[current_piece.point1.y+1][current_piece.point1.x] != 0xFFFF ||
			board[current_piece.point2.y+1][current_piece.point2.x] != 0xFFFF ||
			board[current_piece.point3.y+1][current_piece.point3.x] != 0xFFFF ||
			board[current_piece.point4.y+1][current_piece.point4.x] != 0xFFFF) {
		spawn_place();
		placed = true;
	}
	// copy all of the lines to the line above
	for (int y = 18; y >= 0; y--) {
		for (int x = 0; x < 10; x ++) {
			board[y][x] = board[y+1][x]
		}
	}
	// generate a random index and fill the bottom row
	uint32_t index = Random()%10;
	for (int x = 0; i < 10; x++) {
		if (x == index) {
			// set random index as blank
			board[0][index] = 0xFFFF;
			continue;
		}
		board[0][x] = 0x7BEF;
	}
	// after/if the piece is placed, create a new piece
	if (placed) {
		spawn_gen();
	}
}

void game_two(void) {
	//TODO: waiting for other player (display something?)
	char receive;
	while(FiFo_Get(&receive) == 0) {
		UART_OutChar('R'); 
	}
	draw_game_start();
	Point origin;
	origin.x = 3;
	origin.y = 0;
	copy_piece(&current_piece, gen_piece(), origin);
	draw_piece(&current_piece, current_piece.color);
	mode = TWO_PLAYER;
	play_state = DO_NOTHING;
	while(mode == TWO_PLAYER) {
		if(play_state == MOVE_LEFT) {
			play_state = DO_NOTHING;
			left();
		}
		else if(play_state == MOVE_RIGHT) {
			play_state = DO_NOTHING;
			right();
		}
		else if(play_state == ROTATE) {
			play_state = DO_NOTHING;
			rotate();
		}
		else if(play_state == MOVE_DOWN) {
			play_state = DO_NOTHING;
			down();
		}
		if(FiFo_Get(&receive) != 0) {
			//TODO: spawn a junk line based on character - grey color
			uint8_t num_spawn = (uint8_t)UART_InChar();
			for (int i = 0; i < num_spawn; i++) {
				spawn_line(); 
			}
		}
	}
}

int main(void) {
	DisableInterrupts();
	TExaS_Init();  // set system clock to 80 MHz
	ST7735_InitR(INITR_REDTAB);
 	ADC_Init();    // initialize to sample ADC1 (slider)
 	UART_Init();       // initialize UART
	PortF_Init();
	pieces_init();
	//buttons_init();
	Sound_Init();
	SysTick_Init();
	EnableInterrupts();
	Random_Init(NVIC_ST_CURRENT_R);
	while(1) {
		mode = START_MENU;
		draw_start_menu();
		while(get_buttons() == 0);
		mode = (get_buttons() == 3)? ONE_PLAYER : TWO_PLAYER;
		board_init();
		score = 0;
		if(mode == ONE_PLAYER)
			game_one();
		else if(mode == TWO_PLAYER)
			game_two();
		while(get_buttons() == 0);
		while(get_buttons() != 0);
	}
}
