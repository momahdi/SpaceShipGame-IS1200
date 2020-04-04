/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog 

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */
#define TMR2PERIOD ((80000000 / 256) / 10)


int timeoutcount = 0;
int running;

/* Interrupt Service Routine */
void user_isr( void )
{
  return;
}

/* Lab-specific initialization goes here */
void labinit( void )
{
  T2CON &= ~(1 << 15);   // rensar on knappen
  TMR2 = 0;               // den sätter timern till 0
  IFS(0) &= ~(1 << 8);       // rensa flaggan
  T2CON |= (0b111 << 4);          // 1:256
  PR2 = (80000000/256)/(1000/25);  // uppdateras snabbare, mindre period
  T2CON = (T2CON >> 2) << 2;    // inspiration
  T2CON |= (1 << 15);           // sätter på on knappen
}


/* This function is called repetitively from the main program */
void labwork( void )
{
  if (running != 1){
    hideScreen();
    while (1){
      display_string(0,"WELCOME");
      display_string(1,"TO");
      display_string(2,"SPACE SHIP");
      display_string(3,"press BTN1");
      display_text();
      if(getbtns(1))
        break;
      }

    hideScreen();
    while (1){
      display_string(0, "Difficulty");
      display_string(1, "BTN2: Easy");
      display_string(2, "BTN3: Normal");
      display_string(3, "BTN4: Hard");
      display_text();
      if(getbtns(2)){
        setDifficulty(1);
        break;
      }
      if(getbtns(3)){
        setDifficulty(2);
        break;
      }
      if(getbtns(4)){
        setDifficulty(3);
        break;
      }
    }
    running = 1;
  }
  while (!(IFS(0) & (1 << 8)));
  IFS(0) &= ~(1 << 8); // reset ( Binary One's Complement Operator)

  hideScreen();
  displayObjects();
  updateStars();
  collision();
  if(alive() == 1){
    hideScreen();
    while (1){
      display_string(1, "Game Over");
      display_string(2, "");
      display_string(3, "");
      display_text();
    }
  }
  display_update();

  if (getbtns(3)){
    rightBtn();
  }
  if (getbtns(4)){
    leftBtn();
  }

}
