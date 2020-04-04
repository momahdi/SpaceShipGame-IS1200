/* mipslabfunc.c
   This file written 2015 by F Lundevall
   Some parts are original code written by Axel Isaksson

   For copyright and licensing, see file COPYING */

#include <stdio.h>
#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */

/* Declare a helper function which is local to this file */
static void num32asc( char * s, int );

#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)

#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)

float yLShip = 0;
float yRShip = 7;
int shipHeight = 7;
float shipVelocity = 2.75;

int health = 3;

int yLStar = 0;
int yRStar = 2;
float xBackStar = 128;
float xFrontStar = 126;
float starVelocity = 0.25;

int yLStar2 = 16;
int yRStar2 = 18;
float xBackStar2 = 128;
float xFrontStar2 = 126;
float starVelocity2 = 0.25;

int yLStar3 = 29;
int yRStar3 = 31;
float xBackStar3 = 128;
float xFrontStar3 = 126;
float starVelocity3 = 0.25;

int star1Created = 0;
int star2Created = 0;
int star3Created = 0;

int createStar2 = 0;
int createStar3 = 0;

/* quicksleep:
   A simple function to create a small delay.
   Very inefficient use of computing resources,
   but very handy in some special cases. */
void quicksleep(int cyc) {
	int i;
	for(i = cyc; i > 0; i--);
}

uint8_t spi_send_recv(uint8_t data) {
    while(!(SPI2STAT & 0x08));
    SPI2BUF = data;
    while(!(SPI2STAT & 1));
    return SPI2BUF;
}

void display_init(void) {
        DISPLAY_CHANGE_TO_COMMAND_MODE;
	quicksleep(10);
	DISPLAY_ACTIVATE_VDD;
	quicksleep(1000000);
	
	spi_send_recv(0xAE);
	DISPLAY_ACTIVATE_RESET;
	quicksleep(10);
	DISPLAY_DO_NOT_RESET;
	quicksleep(10);
	
	spi_send_recv(0x8D);
	spi_send_recv(0x14);
	
	spi_send_recv(0xD9);
	spi_send_recv(0xF1);
	
	DISPLAY_ACTIVATE_VBAT;
	quicksleep(10000000);
	
	spi_send_recv(0xA1);
	spi_send_recv(0xC8);
	
	spi_send_recv(0xDA);
	spi_send_recv(0x20);
	
	spi_send_recv(0xAF);
}

void displayPx(int x, int y){
    screen[x][y/8] |= (1 << (y % 8));
}

void hidePx(int x, int y){
    screen[x][y/8] &= ~(1 << (y % 8));
}

void hideScreen(){
    int x;
    int y;
    for (x = 0; x < 128; x++) {
        for (y = 0; y < 32; y++) {
            screen[x][y] = 0;
        }
    }
}

void setDifficulty(int velocity){
    switch (velocity) {
        case 1:
            starVelocity = 1;
            break;
        case 2:
            starVelocity = 1.75;
            break;
        case 3:
            starVelocity = 2.5;
            break;
    }
}

void displayShip(){
    int x;
    int y;
    for (x = 0; x <= shipHeight; x++) {
        for (y = yLShip; y <= yRShip; y++) {
            displayPx(x, y);
        }
    }
    hidePx(0,yLShip + 2);
    hidePx(0,yLShip + 3);
    hidePx(0,yLShip + 4);
    hidePx(0,yLShip + 5);
    hidePx(1,yLShip + 3);
    hidePx(1,yLShip + 4);
    hidePx(4,yLShip + 0);
    hidePx(4,yLShip + 7);
    hidePx(5,yLShip + 0);
    hidePx(5,yLShip + 1);
    hidePx(5,yLShip + 6);
    hidePx(5,yLShip + 7);
    hidePx(6,yLShip + 0);
    hidePx(6,yLShip + 1);
    hidePx(6,yLShip + 2);
    hidePx(6,yLShip + 5);
    hidePx(6,yLShip + 6);
    hidePx(6,yLShip + 7);
    hidePx(7,yLShip + 0);
    hidePx(7,yLShip + 1);
    hidePx(7,yLShip + 2);
    hidePx(7,yLShip + 5);
    hidePx(7,yLShip + 6);
    hidePx(7,yLShip + 7);
}

void displayStar(){
    int x;
    int y;
    for (x = xFrontStar; x <= xBackStar; x++) {
        for (y = yLStar; y <= yRStar; y++) {
            displayPx(x, y);
        }
    }

    hidePx(xBackStar, yLStar + 1);
    hidePx(xBackStar - 1, yLStar);
    hidePx(xBackStar - 1, yLStar + 2);
    hidePx(xFrontStar, yLStar + 1);
    star1Created = 1;
}

void displayStar2(){
    if (createStar2 == 1) {
        int x;
        int y;
        for (x = xFrontStar2; x <= xBackStar2; x++) {
            for (y = yLStar2; y <= yRStar2; y++) {
                displayPx(x, y);
            }
        }

        hidePx(xBackStar2, yLStar2 + 1);
        hidePx(xBackStar2 - 1, yLStar2);
        hidePx(xBackStar2 - 1, yLStar2 + 2);
        hidePx(xFrontStar2, yLStar2 + 1);
        star2Created = 1;
    }
}

void displayStar3(){
    if (createStar3 == 1) {
        int x;
        int y;
        for (x = xFrontStar3; x <= xBackStar3; x++) {
            for (y = yLStar3; y <= yRStar3; y++) {
                displayPx(x, y);
            }
        }

        hidePx(xBackStar3, yLStar3 + 1);
        hidePx(xBackStar3 - 1, yLStar3);
        hidePx(xBackStar3 - 1, yLStar3 + 2);
        hidePx(xFrontStar3, yLStar3 + 1);
        star3Created = 1;
    }
}

void displayObjects(){
    displayShip();
    displayStar();
    displayStar2();
    displayStar3();
}

int rand = 1;
int random(){
    rand = (1500 * rand + 500) % 29;
    return rand;
}

void updateStar(){
    if(star1Created == 1) {
        if (xFrontStar > starVelocity) {
            xBackStar -= starVelocity;
            xFrontStar -= starVelocity;
            if(xFrontStar <= 75)
                createStar2 = 1;
        } else {
            int x;
            int y;
            for (x = xFrontStar; x <= xBackStar; x++) {
                for (y = yLStar; y <= yRStar; y++) {
                    hidePx(x, y);
                }
            }
            yLStar = random();
            yRStar = yLStar + 2;
            xBackStar = 128;
            xFrontStar = 126;
        }
    }
}

void updateStar2(){
    if(star2Created == 1) {
        if (xFrontStar2 > starVelocity) {
            xBackStar2 -= starVelocity;
            xFrontStar2 -= starVelocity;
            if(xFrontStar2 <= 95)
                createStar3 = 1;
        } else {
            int x;
            int y;
            for (x = xFrontStar2; x <= xBackStar2; x++) {
                for (y = yLStar2; y <= yRStar2; y++) {
                    hidePx(x, y);
                }
            }
            yLStar2 = random();
            yRStar2 = yLStar2 + 2;
            xBackStar2 = 128;
            xFrontStar2 = 126;
        }
    }
}

void updateStar3(){
    if(star3Created == 1) {
        if (xFrontStar3 > starVelocity) {
            xBackStar3 -= starVelocity;
            xFrontStar3 -= starVelocity;
        } else {
            int x;
            int y;
            for (x = xFrontStar3; x <= xBackStar3; x++) {
                for (y = yLStar3; y <= yRStar3; y++) {
                    hidePx(x, y);
                }
            }
            yLStar3 = random();
            yRStar3 = yLStar3 + 2;
            xBackStar3 = 128;
            xFrontStar3 = 126;
        }
    }
}

void updateStars(){
    updateStar();
    updateStar2();
    updateStar3();
}

void collision(){
    if (xFrontStar <= shipHeight && yRStar >= yLShip && yLStar <= yLShip|| xFrontStar <= shipHeight && yLStar <= yRShip && yRStar >= yRShip|| xFrontStar <= shipHeight && yLStar >= yLShip && yRStar <= yRShip){
        health--;
        int x;
        int y;
        for (x = xFrontStar; x <= xBackStar; x++) {
            for (y = yLStar; y <= yRStar; y++) {
                hidePx(x, y);
            }
        }
        yLStar = random();
        yRStar = yLStar + 2;
        xBackStar = 128;
        xFrontStar = 126;
    }
    if (xFrontStar2 <= shipHeight && yRStar2 >= yLShip && yLStar2 <= yLShip|| xFrontStar2 <= shipHeight && yLStar2 <= yRShip && yRStar2 >= yRShip|| xFrontStar2 <= shipHeight && yLStar2 >= yLShip && yRStar2 <= yRShip){
        health--;
        int x;
        int y;
        for (x = xFrontStar2; x <= xBackStar2; x++) {
            for (y = yLStar2; y <= yRStar2; y++) {
                hidePx(x, y);
            }
        }
        yLStar2 = random();
        yRStar2 = yLStar2 + 2;
        xBackStar2 = 128;
        xFrontStar2 = 126;
    }
    if (xFrontStar3 <= shipHeight && yRStar3 >= yLShip && yLStar3 <= yLShip|| xFrontStar3 <= shipHeight && yLStar3 <= yRShip && yRStar3 >= yRShip|| xFrontStar3 <= shipHeight && yLStar3 >= yLShip && yRStar3 <= yRShip){
        health--;
        int x;
        int y;
        for (x = xFrontStar3; x <= xBackStar3; x++) {
            for (y = yLStar3; y <= yRStar3; y++) {
                hidePx(x, y);
            }
        }
        yLStar3 = random();
        yRStar3 = yLStar3 + 2;
        xBackStar3 = 128;
        xFrontStar3 = 126;
    }
}

int alive(){
    if (health == 0){
        return 1;
    }
    return 0;
}

void leftBtn(){
    if(yLShip >= shipVelocity) {
        yLShip -= shipVelocity;
        yRShip -= shipVelocity;
    }
}

void rightBtn(){
    if(yRShip < (32 - shipVelocity)) {
        yLShip += shipVelocity;
        yRShip += shipVelocity;
    }
}

void display_string(int line, char *s) {
	int i;
	if(line < 0 || line >= 4)
		return;
	if(!s)
		return;
	
	for(i = 0; i < 16; i++)
		if(*s) {
			textbuffer[line][i] = *s;
			s++;
		} else
			textbuffer[line][i] = ' ';
}

void display_text(void) {
    int i, j, k;
    int c;
    for(i = 0; i < 4; i++) {
        DISPLAY_CHANGE_TO_COMMAND_MODE;
        spi_send_recv(0x22);
        spi_send_recv(i);

        spi_send_recv(0x0);
        spi_send_recv(0x10);

        DISPLAY_CHANGE_TO_DATA_MODE;

        for(j = 0; j < 16; j++) {
            c = textbuffer[i][j];
            if(c & 0x80)
                continue;

            for(k = 0; k < 8; k++)
                spi_send_recv(font[c*8 + k]);
        }
    }
}

void display_update(void) {
	int x, y, k;
	int value;
	for(y = 0; y < 4; y++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;
		spi_send_recv(0x22);
		spi_send_recv(y);

		spi_send_recv(0x0);
		spi_send_recv(0x10);

		DISPLAY_CHANGE_TO_DATA_MODE;

		for(x = 0; x < 128; x++) {
			spi_send_recv(screen[x][y]);
		}
	}
}