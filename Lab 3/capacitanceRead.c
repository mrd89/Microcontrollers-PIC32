///**
// * This program measures the time to charge a capacitor
// * to 1.2 V and converts it into capacitance. These 
// * values are then displayed on screen.  A circle 
// * heartbeat occurs at the bottom of the screen 
// * concurrently.
// *
// * There is a display thread which updates the LCD screen
// * There is a one second timer tick thread
// * There is a blink thread that manages the heartbeat
// * There is a measure thread that resets the capacitor and 
// * takes the measurements
// * 
// * -- Pin RB5 and RB9 are output compare outputs
// * -- Pin RB13 is input capture input -- connect this to one of the output compares
// *
// * -- TFT LCD connections explained elsewhere
// * Modified by Bruce Land 
// * Jan 2015
// * 
// * Modified by Jared Gallina and Michael Xiao
// * September 2018
// */
//
//////////////////////////////////////
//// clock AND protoThreads configure!
//// threading library
//
//////////////////////////////////////
//// clock AND protoThreads configure!
//// You MUST check this file!
////  TEST OLD CODE WITH NEW THREADS
////#include "config_1_2_3.h"
//#include "config_1_3_2.h"
//// threading library
////#include "pt_cornell_1_2_3.h"
//#include "pt_cornell_1_3_2.h"
//// yup, the expander
//#include "port_expander_brl4.h"
//
//////////////////////////////////////
//////////////////////////////////////
//// graphics libraries
//#include "tft_master.h"
//#include "tft_gfx.h"
//// need for rand function
//#include <stdlib.h>
//
//////////////////////////////////////
///*
// Linear Regression Measurements
// 0.976, 402.000
//2.142, 860.000
//15.5, 6360.000
//46.64, 1.816e4
// */
//// === thread structures ============================================
//// thread control structs
//
//// note that UART input and output are threads
//static struct pt pt_print, pt_measure, pt_time, pt_blink ; //, pt_input, pt_output, pt_DMA_output ;
//
//// system 1 second interval tick
//int sys_time_seconds ;
//
////measured capacitance
//float capacitance;
////adjusted time in uS
//int t;
////HEARTBEAT CIRCLE VISIBLE OR NOT
//int circle ;
//
////values based on regressions
//float offset = 104.865;
//float slope = 388.65;
//
////adjusted period of the wave
//int capture_period=0 ;
////measured period of the wave
//float capture1;
//
//// == Capture 1 ISR ====================================================
//// check every cpature for consistency
//void __ISR(_INPUT_CAPTURE_1_VECTOR, ipl3) C1Handler(void)
//{                
//    // read the capture register 
//    capture1 = mIC1ReadCapture();  
//    capture_period = capture1 - offset;  //adjust with offset from linear regression
//    t = capture_period/40; //adjust by dividing by the 40 MHz 
//    capacitance = capture_period/slope; //adjust with offset from linear regression
//    mIC1ClearIntFlag();
//}
//
//// === Period print Thread ======================================================
//// prints the captured period of the generated wave
//static PT_THREAD (protothread_print(struct pt *pt))
//{
//    PT_BEGIN(pt);
//      // string buffer
//      static char buffer[128];
//      
//      tft_setCursor(0, 0);
//      tft_setTextColor(ILI9340_WHITE);  tft_setTextSize(2);
//      tft_writeString("Connect RB9 \n");
//      tft_setCursor(0, 20);
//      tft_writeString("OR RB5 \n");
//      tft_setCursor(0, 40);
//      tft_writeString("to RB13 thru 300ohm\n");
//      while(1) {
//            // print every 200 mSec
//            PT_YIELD_TIME_msec(200) ;
//            // erase
//            tft_setCursor(0, 80);
//            tft_fillRoundRect(0,80, 240, 20, 1, ILI9340_BLACK);// x,y,w,h,radius,color
//            // print the periods
//            if(capacitance > 0)
//                sprintf(buffer,"Time=%d uS", t);
//             else
//                 sprintf(buffer, "%s", "Time= N/A");
//                
//             tft_writeString(buffer);
//             
//             tft_fillRoundRect(0,100, 240, 20, 1, ILI9340_BLACK);// x,y,w,h,radius,color
//             tft_setCursor(0, 100);
//             if(capacitance > 0)
//                sprintf(buffer,"Capacitance=%5.1f nF", capacitance);
//             else
//                 sprintf(buffer, "%s", "No Capacitor");
//            tft_writeString(buffer);
//      } // END WHILE(1)
//  PT_END(pt);
//} // thread 4
//
//// === One second Thread ======================================================
//// update a 1 second tick counter
//static PT_THREAD (protothread_time(struct pt *pt))
//{
//    PT_BEGIN(pt);
//      static char buffer[128];
//      
//      while(1) {
//            // yield time 1 second
//            PT_YIELD_TIME_msec(1000) ;
//            sys_time_seconds++ ;
//            tft_fillRoundRect(0,150, 200, 20, 1, ILI9340_BLACK);// x,y,w,h,radius,color
//             tft_setCursor(0, 150);
//             sprintf(buffer,"sys_time=%d  ", sys_time_seconds);
//            tft_writeString(buffer);
//            // NEVER exit while
//      } // END WHILE(1)
//
//  PT_END(pt);
//} // 
//
//// === Blink Thread ======================================================
//// update a 1 second "heartbeat" circle
//static PT_THREAD (protothread_blink(struct pt *pt))
//{
//    PT_BEGIN(pt);
//      static char buffer[128];
//	//a variable that indicates if the circle is displayed ( =   //1) or not ( = 0)
//      circle = 0;
//      while(1) {
//            // yield time 1 second
//            PT_YIELD_TIME_msec(1000);
//            if(!circle){ //check to see if circle is visible with circle variable
//                tft_fillCircle(120,280, 30,  ILI9340_WHITE);// x,y,w,h,radius,color
//                circle = 1;
//            }
//            else{ //circle is on screen, needs to be cleared
//                tft_fillCircle(120,280, 30,  ILI9340_BLACK);
//                circle = 0;
//            }
//        } // END WHILE(1)
//
//  PT_END(pt);
//} // 
//
//// === Measure Thread ======================================================
//// update a 1 second "heartbeat" circle
//static PT_THREAD (protothread_measure(struct pt *pt))
//{
//    PT_BEGIN(pt);
//      static char buffer[128];
//      
//      //setting up the comparator
//      CMP1Open(CMP_ENABLE | CMP_OUTPUT_ENABLE | CMP1_NEG_INPUT_IVREF);
//      PPSOutput(4, RPB9, C1OUT);
//        
//      while(1) {
//            
//            //step one of the lab
//            mPORTBSetPinsDigitalOut(BIT_3); //Set port as input (pin 7 is RB3)
//            mPORTBClearBits(BIT_3); //set RB3 to low by clearing bits
//            
//            // step 2 of measure thread, yield for enough time to allow a full charge of the capacitor
//            PT_YIELD_TIME_msec(50);
//            
//            //step 3 of measure thread
//            mPORTBSetPinsDigitalIn(BIT_3); //set RB3 to an input
//            WriteTimer3(0); //reset timer to 0 so overflow can be ignored altogether, the delay in doing this will be reduced away with the offset used in calculation
//            
//            // step 6 of measure thread
//            PT_YIELD_TIME_msec(200); //yield the measure thread for 200 milliseconds
//            
//            
//            
//        } // END WHILE(1)
//
//  PT_END(pt);
//} // 
//
//// === Main  ======================================================
//
//int main(void)
//{
// 
//  // === Config timer3 free running ==========================
//  // set up timer3 as a souce for input capture
//  // and let it overflow for contunuous readings
//  OpenTimer3(T3_ON | T3_SOURCE_INT | T3_PS_1_1, 0xffff);
//
//  // === set up input capture ================================
//  OpenCapture1(  IC_EVERY_RISE_EDGE | IC_INT_1CAPTURE | IC_TIMER3_SRC | IC_ON );
//  // turn on the interrupt so that every capture can be recorded
//  ConfigIntCapture1(IC_INT_ON | IC_INT_PRIOR_3 | IC_INT_SUB_PRIOR_3 );
//  INTClearFlag(INT_IC1);
//  // connect PIN 24 to IC1 capture unit
//  PPSInput(3, IC1, RPB13);
//
//  // init the display
//  tft_init_hw();
//  tft_begin();
//  tft_fillScreen(ILI9340_BLACK);
//  //240x320 vertical display
//  tft_setRotation(0); // Use tft_setRotation(1) for 320x240
//  tft_setCursor(0, 0);
//  
//  // === config the uart, DMA, vref, timer5 ISR ===========
//  PT_setup();
//
//  // === setup system wide interrupts  ====================
//  INTEnableSystemMultiVectoredInt();
//  
//  // === now the threads ===================================
//  // init the threads
////provided threads
//  PT_INIT(&pt_print);
//  PT_INIT(&pt_time);
////user created threads
//  PT_INIT(&pt_blink);
//  PT_INIT(&pt_measure);
//
//  // schedule the threads
//  while(1) {
////provided
//    PT_SCHEDULE(protothread_print(&pt_print));
//    //PT_SCHEDULE(protothread_blink(&pt_blink));
////user created
//    PT_SCHEDULE(protothread_time(&pt_time));
//    PT_SCHEDULE(protothread_measure(&pt_measure));
//  }
//} // main
