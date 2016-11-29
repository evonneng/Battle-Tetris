; Print.s
; Student names: Calvin Ly, Evonne Ng
; Last modification date: 10/26/16
; Runs on LM4F120 or TM4C123
; EE319K lab 7 device driver for any LCD
;
; As part of Lab 7, students need to implement these LCD_OutDec and LCD_OutFix
; This driver assumes two low-level LCD functions
; ST7735_OutChar   outputs a single 8-bit ASCII character
; ST7735_OutString outputs a null-terminated string 
INPUT EQU 0

    IMPORT   ST7735_OutChar
    IMPORT   ST7735_OutString
    EXPORT   LCD_OutDec
    EXPORT   LCD_OutFix

    AREA    |.text|, CODE, READONLY, ALIGN=2
    THUMB

;---- MOD ----
; R0 is dividend
; R1 is divisor
; value returned in R0
Mod
	UDIV R2, R0, R1
	MUL R2, R2, R1
	SUB R0, R0, R2
	BX LR

;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutDec
	CMP R0, #0
	BNE LCD_OutDecRecursion
	PUSH {LR, R0}		
	ADD R0, R0, #48
	BL ST7735_OutChar	
	POP {LR, R0}
	BX LR
LCD_OutDecRecursion		;recursive function to print out dec
	PUSH {LR, R0}
	CMP R0, #0		;base case = 0
	BEQ done
	MOV R1, #10		;the division factor
	UDIV R0, R0, R1
	BL LCD_OutDecRecursion	;continue to recurse until base case
	LDR R0, [SP, #INPUT]
	MOV R1, #10
	BL Mod
	ADD R0, R0, #48
	BL ST7735_OutChar
done
	POP {LR, R0}
    BX  LR
;* * * * * * * * End of LCD_OutDec * * * * * * * *

; -----------------------LCD _OutFix----------------------
; Output characters to LCD display in fixed-point format
; unsigned decimal, resolution 0.001, range 0.000 to 9.999
; Inputs:  R0 is an unsigned 32-bit number
; Outputs: none
; E.g., R0=0,    then output "0.000 "
;       R0=3,    then output "0.003 "
;       R0=89,   then output "0.089 "
;       R0=123,  then output "0.123 "
;       R0=9999, then output "9.999 "
;       R0>9999, then output "*.*** "
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutFix
	PUSH {LR, R0}
	MOV R1, #9999
	CMP R0, R1
	BHI too_big
	LDR R0, [SP, #INPUT]
	MOV R1, #1000
	UDIV R0, R0, R1
	ADD R0, R0, #48
	BL ST7735_OutChar ;first digit
	MOV R0, #46 ;.
	BL ST7735_OutChar
	LDR R0, [SP, #INPUT]
	MOV R1, #100
	UDIV R0, R0, R1
	MOV R1, #10
	BL Mod
	ADD R0, R0, #48
	BL ST7735_OutChar ;second digit
	LDR R0, [SP, #INPUT]
	MOV R1, #10
	UDIV R0, R0, R1
	MOV R1, #10
	BL Mod
	ADD R0, R0, #48
	BL ST7735_OutChar ;third digit
	LDR R0, [SP, #INPUT]
	MOV R1, #10
	BL Mod
	ADD R0, R0, #48
	BL ST7735_OutChar ;fourth digit
	POP {LR, R0}
	BX LR
too_big				;case check that the number is too big
	MOV R0, #42 ;*
	BL ST7735_OutChar
	MOV R0, #46 ;.
	BL ST7735_OutChar
	MOV R0, #42 ;*
	BL ST7735_OutChar
	MOV R0, #42 ;*
	BL ST7735_OutChar
	MOV R0, #42 ;*
	BL ST7735_OutChar
	POP {LR, R0}
    BX   LR
 
    ALIGN
;* * * * * * * * End of LCD_OutFix * * * * * * * *

     ALIGN                           ; make sure the end of this section is aligned
     END                             ; end of file
