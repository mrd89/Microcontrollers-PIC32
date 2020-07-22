///*
// * File:        Test of compiler fixed point
// * Author:      Bruce Land
// * Adapted from:
// *              main.c by
// * Author:      Syed Tahmid Mahbub
// * Target PIC:  PIC32MX250F128B
// */
//
//////////////////////////////////////
//// clock AND protoThreads configure!
//// You MUST check this file!
//#include "config_1_3_2.h"
//// threading library
//#include "pt_cornell_1_3_2.h"
//
//////////////////////////////////////
//// graphics libraries
//#include "tft_master.h"
//#include "tft_gfx.h"
//// need for rand function
//#include <stdlib.h>
//// fixed point types
//#include <stdfix.h>
//// need for sine function
//#include <math.h>
//////////////////////////////////////
//
//// lock out timer interrupt during spi comm to port expander
//// This is necessary if you use the SPI2 channel in an ISR
//#define start_spi2_critical_section INTEnable(INT_T2, 0);
//#define end_spi2_critical_section INTEnable(INT_T2, 1);
//
//
//
//
//////////////////////////////////////
//// Audio DAC ISR
//// A-channel, 1x, active
//#define DAC_config_chan_A 0b0011000000000000
//// B-channel, 1x, active
//#define DAC_config_chan_B 0b1011000000000000
//
//
//#define NOP asm("nop");
//// 20 cycles
//#define wait20 NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
//// 50 cycles
//#define wait200 wait20;wait20;wait20;wait20;wait20;wait20;wait20;wait20;wait20;wait20;
//
//
//
///* Demo code for interfacing TFT (ILI9340 controller) to PIC32
// * The library has been modified from a similar Adafruit library
// */
//// Adafruit data:
///***************************************************
//  This is an example sketch for the Adafruit 2.2" SPI display.
//  This library works with the Adafruit 2.2" TFT Breakout w/SD card
//  ----> http://www.adafruit.com/products/1480
//
//  Check out the links above for our tutorials and wiring diagrams
//  These displays use SPI to communicate, 4 or 5 pins are required to
//  interface (RST is optional)
//  Adafruit invests time and resources providing this open source code,
//  please support Adafruit and open-source hardware by purchasing
//  products from Adafruit!
//
//  Written by Limor Fried/Ladyada for Adafruit Industries.
//  MIT license, all text above must be included in any redistribution
// ****************************************************/
//
////*****************************//
//// VARIABLES DEFS  BELOW HERE
////*****************************//
//#define dmaChn 0
//#define sine_table_size 1000
//
////for drawing balls a bit more low res but efficiently
//#define drawBall(x,y,color) tft_drawPixel(x,y,color); tft_drawPixel(x-1,y,color); tft_drawPixel(x+1,y,color); tft_drawPixel(x,y-1,color); tft_drawPixel(x,y+1,color);
//
////gor the game button, some useful macros
//#define EnablePullupB(bits) CNPDBCLR = bits; CNPUBSET = bits;
//#define DisablePullupB(bits) CNPDBCLR = bits;
//
//int raw_sin[sine_table_size];
//unsigned short DAC_data1[sine_table_size];
//
//// string buffer
//char buffer[60];
//
//// === thread structures ============================================
//// thread control structs
//// note that UART input and output are threads
//static struct pt pt_timer, pt_color, pt_anim ;
//
//// system 1 second interval tick
//int sys_time_seconds ;
//
////just for the frame rate thread
//int begin_time;
////overall game time
//int game_over_time = 120; // set the maximum game time to be 2 minutes.
////paddle position updated from the potentiometer
//unsigned short paddle_pos = 0;
////paddle size
//unsigned short paddle_width = 30;
////y position of the paddle
//unsigned short paddle_pos_y = 30;
//
////raw value from the adc
//static int adc_9;
//
////score value
//int score = 0;
//
////the max number of balls we are going to simulate
//#define ball_num 200
//// the current number of balls
//volatile int nballs =0;
//
////frame rate calculated
//unsigned int frame_rate;
//
////game button value
//int game_reset;
//
////index to use in all loops
//int i = 0;
//int j = 0;
//
//// === Timer Thread =================================================
//// update a 1 second tick counter
//static PT_THREAD (protothread_timer(struct pt *pt))
//{
//    PT_BEGIN(pt);
//        sys_time_seconds = 0; // reset everything 
//        score = 0;
//        nballs = 0;
//      while((sys_time_seconds <= game_over_time) || !game_reset) { // end thread when it exceeds game time or game over
//        // yield time 1 second
//        PT_YIELD_TIME_msec(1000) ;
//        sys_time_seconds++ ;
//        
//        // draw score
//        tft_setCursor(3, 3);
//        tft_setTextColor(ILI9340_WHITE);  tft_setTextSize(1);
//        tft_fillRoundRect(3,3, 180, 24, 1, ILI9340_BLACK);// x,y,w,h,radius,color
//        //tft_fillRoundRect(0,24, 140, 14, 1, ILI9340_BLACK);// x,y,w,h,radius,color
//        sprintf(buffer,"Score: %d GameTime:%d \n Balls:%d FrameRate: %d", score,sys_time_seconds,nballs,frame_rate);
//        tft_writeString(buffer);
//        // NEVER exit while
//      } // END WHILE(1)
//  PT_END(pt);
//} // timer thread
//
//// === Animation Thread =============================================
//// update a 1 second tick counter
//
//#define float2Accum(a) ((_Accum)(a))
//#define Accum2float(a) ((float)(a))
//#define int2Accum(a) ((_Accum)(a))
//#define Accum2int(a) ((int)(a))
//
// 
//
//typedef struct
//{
//   // each ball should have a position and velocity.
//   volatile _Accum xxPosition; //= int2Accum(120);
//   volatile _Accum yyPosition; //= int2Accum(309);
//   volatile _Accum xxVelocity; //= int2Accum(-10); // injected from right side
//   volatile _Accum yyVelocity; //= int2Accum(-10); 
//   volatile _Accum xxFPosition; //= int2Accum(120);
//   volatile _Accum yyFPosition; //= int2Accum(309);
//   unsigned short color; //ILI9340_GREEN
//   unsigned short hit_count; //number of frames to ignore collisions
//   //unsigned int just_collided;
//}Ball;
//Ball balls[ball_num];
////This method generates a new ball
//unsigned short colors[5] = {ILI9340_GREEN,ILI9340_RED,ILI9340_WHITE,ILI9340_BLUE,ILI9340_CYAN};
//int color_cycle =0;
//void gen_ball(){
//    balls[nballs].xxPosition = int2Accum(120);
//    balls[nballs].yyPosition = int2Accum(309);
//    balls[nballs].xxVelocity = ((_Accum)(rand() & 0xffff)>>14) - 2; // injected from right side
//    balls[nballs].yyVelocity = int2Accum(-3); 
//    balls[nballs].color = colors[color_cycle];
//    balls[nballs].hit_count = 10;
//    //balls[0].just_collided = 1;
//  //where will this ball be next?
//    balls[nballs].xxFPosition = balls[nballs].xxPosition+balls[nballs].xxVelocity;
//    balls[nballs].yyFPosition = balls[nballs].yyPosition+balls[nballs].yyVelocity;
//    nballs++;
//    color_cycle++;
//    color_cycle = color_cycle%5;
//}
//
//
//_Accum deltax=0;
//_Accum deltay =0;
//_Accum deltavx =0;
//_Accum deltavy =0;
//_Accum rij2 = 0;
//_Accum vij = 0;
////these are interim values for doing faster velocity exchange calculations
////vijx and vijy are the components of vij
//_Accum deltavij = 0;
//_Accum deltavijx = 0;
//_Accum deltavijy = 0;
////the time left after every frame draw
//unsigned int time_left;
////the delay between ball spawns
//unsigned int spawn_delay =0;
//_Accum temp_exchange = 0;
//_Accum drag = .0003;
////to tell us to wait to spawn an old ball before generating a new one.
////unsigned int old_ball_spawning;
//
////a tolerance for velocity differences for overlapping trajectory
//_Accum tol = .4;
//
//static PT_THREAD (protothread_anim(struct pt *pt))
//{
//    PT_BEGIN(pt);
//    mPORTBSetPinsDigitalIn(BIT_7);
//    EnablePullupB(BIT_7);
//    
//      while((sys_time_seconds <= game_over_time) || !game_reset) {
//          
//         
//        begin_time = PT_GET_TIME();
//        
//         for (i=0;i<nballs;i++){
//             //if the button is pressed ever, reset the game time and start over
//            start_spi2_critical_section;
//            game_reset = mPORTBReadBits(BIT_7);
//            end_spi2_critical_section;
////            if(!game_reset){
////                //clear the game and reset the time
////                sys_time_seconds =0;
////                i=nballs+1;
////                nballs =0; //effectively clears the balls array
////                
////                 tft_fillScreen(ILI9340_BLACK);
////                //240x320 vertical display
////                //tft_setRotation(0); // Use tft_setRotation(1) for 320x240
////
////                //print the border of the game
////                  tft_drawLine(0, 0, 0, 320, ILI9340_GREEN);//horizontal
////                  tft_drawLine(0, 0, 240, 0, ILI9340_GREEN);//vertical
////                  tft_drawLine(0, 318, 238, 318, ILI9340_GREEN);//vertical right
////                  tft_drawLine(238, 318, 238, 0, ILI9340_GREEN);//horizontal top
////                  //print those weird bars
////                  tft_drawLine(0, 80, 60, 80, ILI9340_RED);
////                  tft_drawLine(240, 80, 180, 80, ILI9340_RED);
////            }
//            
//                //where will the ball be next frame?
//             balls[i].xxFPosition = balls[i].xxPosition+balls[i].xxVelocity;
//             balls[i].yyFPosition = balls[i].yyPosition+balls[i].yyVelocity;
//             
//             
//             //now check for ball to ball collisions
//             for (j=i+1; j<nballs; j++){
//                 if(1==1||balls[i].hit_count==0&&balls[j].hit_count==0){
//                    //check that delta x and delta y are less than four BEFORE doing collision calculations
//                    deltax = balls[i].xxFPosition - balls[j].xxFPosition;
//                    deltay = balls[i].yyFPosition - balls[j].yyFPosition;
//                    if (abs(deltax)<=4&&abs(deltay)<=4){
//                        //balls are close enough for collision calculation
//                        rij2 = deltax*deltax + deltay*deltay;
//                        //if (rij2<=16){
//                            //balls collide compute new velocities and assign them
//                            if (rij2<2){
//                                deltavx = balls[i].xxVelocity - balls[j].xxVelocity; //
//                                deltavy = balls[i].yyVelocity - balls[j].yyVelocity; //
//                                //if the velocities are really similar, keep them from traveling together
//                                if(deltavx<tol&&deltavy<tol){
//                                    balls[i].xxVelocity = -balls[i].xxVelocity;
//                                    balls[i].hit_count = 10;
//                                }
//                                //close enough, just exchange velocities
//                               temp_exchange = balls[j].xxVelocity;
//                               balls[j].xxVelocity = balls[i].xxVelocity;
//                               balls[i].xxVelocity = temp_exchange;
//                               temp_exchange = balls[j].yyVelocity;
//                               balls[j].yyVelocity = balls[i].yyVelocity;
//                               balls[i].yyVelocity = temp_exchange;
//                               
//                            }
//                            else {
//                               deltavx = balls[i].xxVelocity - balls[j].xxVelocity; //
//                               deltavy = balls[i].yyVelocity - balls[j].yyVelocity; //
//                               deltavij = (deltax*deltavx+deltay*deltavy)/rij2;
//                               deltavijx = -deltax*deltavij;
//                               deltavijy = -deltay*deltavij;
//                               balls[i].xxVelocity += deltavijx;
//                               balls[i].yyVelocity += deltavijy;
//                               balls[j].xxVelocity -= deltavijx;
//                               balls[j].yyVelocity -= deltavijy;
//                            }
//                        //}//end if
//                        balls[i].hit_count = 20;
//                        balls[j].hit_count = 20;
//                    }//end <4 if
//
//                }
//                else{
//                     balls[i].hit_count--;
//                     balls[j].hit_count--;
//                 }
//             }//end inner for loop for j balls
//             //where will the ball be next frame?
//             balls[i].xxFPosition = balls[i].xxPosition+balls[i].xxVelocity;
//             balls[i].yyFPosition = balls[i].yyPosition+balls[i].yyVelocity;
//             //first check wall collision for each ball
//             if(balls[i].xxFPosition<int2Accum(10)||balls[i].xxFPosition>int2Accum(230)){
//                 //side bounce
//                 balls[i].xxVelocity = -balls[i].xxVelocity;
//                 //play bounce sound
//                 
//             }
//             if (balls[i].yyFPosition>int2Accum(310)){
//                 //side bounce
//                 balls[i].yyVelocity = -balls[i].yyVelocity;
//                 //play bounce sound
//                 
//             }
//             if (balls[i].yyFPosition<int2Accum(15)){
//                 //ball falls out of the back screen, reset it
//                 //erase the ball
//                tft_fillCircle(Accum2int(balls[i].xxPosition), Accum2int(balls[i].yyPosition), 2, ILI9340_BLACK); //x, y, radius, color
//                balls[i].xxPosition = int2Accum(120);
//                balls[i].yyPosition = int2Accum(305);
//                balls[i].xxVelocity = (_Accum)0xffff&&rand()>>16;
//                score--;
//                //old_ball_spawning = 1;
//                //play a negative sound from the DAC
//             
//            }
//             //check the mid walls collision
//            if(balls[i].yyPosition>int2Accum(83)&&(balls[i].yyFPosition <= int2Accum(83)) && ((balls[i].xxPosition <= int2Accum(63))||(balls[i].xxPosition>=int2Accum(177)))){
//               balls[i].yyVelocity = -balls[i].yyVelocity;
//               //play a bounce sound
////               DmaChnSetTxfer(dmaChn,DAC_data1,(void*)&SPI2BUF,sine_table_size*2,2,2);
////               DmaChnEnable(dmaChn);
//            }//end if
//            if(balls[i].yyPosition<int2Accum(77)&&(balls[i].yyFPosition >= int2Accum(77)) && ((balls[i].xxPosition <= int2Accum(63))||(balls[i].xxPosition>=int2Accum(177)))){
//               balls[i].yyVelocity = -balls[i].yyVelocity;
//               //play a bounce sound
////               DmaChnSetTxfer(dmaChn,DAC_data1,(void*)&SPI2BUF,sine_table_size*2,2,2);
////               DmaChnEnable(dmaChn);
//            }//end if
//             //check paddle collision
//             if((balls[i].yyPosition>int2Accum(paddle_pos_y+2))&&(balls[i].yyFPosition<=int2Accum(paddle_pos_y+2))&&((balls[i].xxPosition >= int2Accum(paddle_pos))&&(balls[i].xxPosition <= int2Accum(paddle_pos+30)))) 
//             {  
//                //erase the ball
//                tft_fillCircle(Accum2int(balls[i].xxPosition), Accum2int(balls[i].yyPosition), 2, ILI9340_BLACK); //x, y, radius, color
//                 //balls[i].yyVelocity = -balls[i].yyVelocity;
//                score++;
//                balls[i].xxPosition = int2Accum(120);
//                balls[i].yyPosition = int2Accum(310);
//                balls[i].xxVelocity = ((_Accum)(rand() & 0xffff)>>14) - 2; //from -2 to 2
//                //old_ball_spawning = 1;
//                //play a nice sound from the DAC vis DMA SPI
//                DmaChnSetTxfer(dmaChn,DAC_data1,(void*)&SPI2BUF,sine_table_size*2,2,2);
//                DmaChnEnable(dmaChn);
//            }//end if
//         
//         }//end for loop (each ball)
//         
//         //loop through erasing and drawing each ball
//         for (i=0;i<ball_num;i++){
//             //erase the ball
//             drawBall(Accum2int(balls[i].xxPosition), Accum2int(balls[i].yyPosition), ILI9340_BLACK); //x, y, radius, color
//             //account for drag
//             balls[i].xxVelocity -= balls[i].xxVelocity*drag;
//             balls[i].yyVelocity -= balls[i].yyVelocity*drag;
//             //set new ball positions based on new information
//            balls[i].xxPosition = balls[i].xxPosition + balls[i].xxVelocity;
//            balls[i].yyPosition = balls[i].yyPosition + balls[i].yyVelocity;
//            //redraw the ball
//            drawBall(Accum2int(balls[i].xxPosition), Accum2int(balls[i].yyPosition), balls[i].color); 
//         }
//        
//         //erase paddle
//         tft_drawLine(paddle_pos, paddle_pos_y, paddle_pos+paddle_width, paddle_pos_y, ILI9340_BLACK);
//         //read the adc to update the paddle position
//         adc_9 = ReadADC10(0);
//         //shift it so that the value is between valid pixel position range
//         paddle_pos = paddle_pos*.7 + .3*( adc_9>>2);
//         //draw paddle
//         tft_drawLine(paddle_pos, paddle_pos_y, paddle_pos+paddle_width, paddle_pos_y, ILI9340_GREEN);
//         time_left = (PT_GET_TIME() - begin_time);
//         frame_rate = (int) 1000/time_left;
//         if(frame_rate>=15&&spawn_delay==0&&nballs<300){
//             //add new ball to the play field
//             gen_ball();
//             spawn_delay =3;
//         }
//         else {spawn_delay--;}
//        // NEVER exit while
//         
//         PT_YIELD(pt) ;
//        //end if(win)
//          
//          //game is over and the screen should show a win
//      } // END WHILE(1)
//      
//  PT_END(pt);
//} // animation thread
//
//// === Main  ======================================================
//void main(void) {
// //SYSTEMConfigPerformance(PBCLK);
//  
//  ANSELA = 0; ANSELB = 0; 
//
//  // === config threads ==========
//  // turns OFF UART support and debugger pin, unless defines are set
//  PT_setup();
//
//  // === setup system wide interrupts  ========
//  INTEnableSystemMultiVectoredInt();
//  
//  // the ADC ///////////////////////////////////////
//        // configure and enable the ADC
//	CloseADC10();	// ensure the ADC is off before setting the configuration
//
//	// define setup parameters for OpenADC10
//	// Turn module on | ouput in integer | trigger mode auto | enable autosample
//        // ADC_CLK_AUTO -- Internal counter ends sampling and starts conversion (Auto convert)
//        // ADC_AUTO_SAMPLING_ON -- Sampling begins immediately after last conversion completes; SAMP bit is automatically set
//        // ADC_AUTO_SAMPLING_OFF -- Sampling begins with AcquireADC10();
//        #define PARAM1  ADC_FORMAT_INTG16 | ADC_CLK_AUTO | ADC_AUTO_SAMPLING_ON //
//
//	// define setup parameters for OpenADC10
//	// ADC ref external  | disable offset test | disable scan mode | do 1 sample | use single buf | alternate mode off
//	#define PARAM2  ADC_VREF_AVDD_AVSS | ADC_OFFSET_CAL_DISABLE | ADC_SCAN_OFF | ADC_SAMPLES_PER_INT_1 | ADC_ALT_BUF_OFF | ADC_ALT_INPUT_OFF
//        //
//	// Define setup parameters for OpenADC10
//        // use peripherial bus clock | set sample time | set ADC clock divider
//        // ADC_CONV_CLK_Tcy2 means divide CLK_PB by 2 (max speed)
//        // ADC_SAMPLE_TIME_5 seems to work with a source resistance < 1kohm
//        #define PARAM3 ADC_CONV_CLK_PB | ADC_SAMPLE_TIME_5 | ADC_CONV_CLK_Tcy2 //ADC_SAMPLE_TIME_15| ADC_CONV_CLK_Tcy2
//
//	// define setup parameters for OpenADC10
//	// set AN11 and  as analog inputs
//	#define PARAM4	ENABLE_AN11_ANA // pin 24
//
//	// define setup parameters for OpenADC10
//	// do not assign channels to scan
//	#define PARAM5	SKIP_SCAN_ALL
//
//	// use ground as neg ref for A | use AN11 for input A     
//	// configure to sample AN11 
//	SetChanADC10( ADC_CH0_NEG_SAMPLEA_NVREF | ADC_CH0_POS_SAMPLEA_AN11 ); // configure to sample AN11 
//	OpenADC10( PARAM1, PARAM2, PARAM3, PARAM4, PARAM5 ); // configure ADC using the parameters defined above
//
//	EnableADC10(); // Enable the ADC
//  ///////////////////////////////////////////////////////
//    
//  ////////DMA SETUP//////////////
//    
//    // build the sounds
//
////sound data 1  
//    int i, current_amplitude, attack_time, decay_time;
//    int attack_inc, decay_inc, sustain_time;
//    attack_time = (int)(sine_table_size /10);
//    decay_time = (int)(sine_table_size *3/5);
//    attack_inc = (int)(2047 / attack_time);
//    decay_inc = (int)(2047 / decay_time);
//    sustain_time = (sine_table_size - attack_time - decay_time);
//   
//   
//           
//for(i=0;i < sine_table_size;i++){
//    if (i < (attack_time + decay_time + sustain_time)){
//        current_amplitude = (i <= attack_time)?
//            current_amplitude + attack_inc :
//            (i <= attack_time + sustain_time)? current_amplitude:
//                current_amplitude - decay_inc ;
//    }
//    else {
//        current_amplitude = 2047 ;
//    }
//   
//    raw_sin[i] = (int)(current_amplitude * sin((float)i*6.2831/(float)sine_table_size*10)+2047); //12 bit
//    DAC_data1[i] = DAC_config_chan_A | (raw_sin[i] & 0xfff);
//}
//    // ==== SPI channel for DAC ===
//// divide Fpb by 2, configure the I/O ports. not using the SS in this example
//// 16 bit transfer CKP =1 , CKE = 1
//// possibilites SPI_OPEN_CKP_HICK;  SPI_OPEN_END; SPI_OPEN_CKE_REV
//// for any given peripherial, you will need to match these
////clk divider set to 2 for 20MHz
//// FRAMED SPI mode, so SS2 is produced by the SPI hardware.
//OpenTimer2(T2_ON| T2_SOURCE_INT| T2_PS_1_1,200);
//
//// ss1 to RPB10 for FRAMED SPI
//SpiChnOpen(SPI_CHANNEL2,SPI_OPEN_ON| SPI_OPEN_MODE16 |SPI_OPEN_MSTEN|
//                SPI_OPEN_CKE_REV| SPICON_FRMEN | SPICON_FRMPOL,2);
//// >>> SCK 2 is RB15 <<< not on PPS
//// SDO2 (MOSI) is in PPS output group 2, could be connected to RB% which is pin 14
//PPSOutput(4,RPB10,SS2);
//PPSOutput(2,RPB5,SDO2);
//
//
//// ==== DMA setup for the DAC ====
//// trasnfer 2 bytes from voltage table to dac spi controled by timer event
//// and repeating indefinitely
//// open desired dma channel
//// priority zero
//// enable the AUTO option, to keep repeating the same trasnfer over and over
//
//// either DMA_OPEN_DEFAULT  or DMA_OPEN_AUTO
//DmaChnOpen(dmaChn,0,DMA_OPEN_DEFAULT); // or DMA_OPEN_NORM (one shot)
//
//// set the transfer paramters: source & destination address and size,
//// number of bytes per event
//// setting the last parameter to 2 makes the DMA output two bytes / timer event
//// sine_table_size mult by 2 bc the table is SHORT ints
//DmaChnSetTxfer(dmaChn,DAC_data1,(void*)&SPI2BUF,sine_table_size*2,2,2);
//
//// set the transfer event control: what event is to start the DMA transfer
//// in this case, timer 2
//
//DmaChnSetEventControl(dmaChn, DMA_EV_START_IRQ(_TIMER_2_IRQ));
//
//// once we configured the DMA channel, we can enable it
//// now it is ready and waiting for an event to occurr..
//
//
//DmaChnEnable(dmaChn);
////while(1){
////    DmaChnEnable(dmaChn);
////    for (i=0;i<100;i++){
////    wait20000
////    }
//    
//  //////////////////////////////  
//
//
////----------------------------------------------------------------
//    //hardcoded unit testing below
//    
//    //initialize the balls to spawn
////    for (i = 0;i<ball_num; i++){
////        balls[i].xxPosition = int2Accum(120);
////        balls[i].yyPosition = int2Accum(305);
////        balls[i].xxVelocity = 0xffff&&rand()-1; // injected from right side
////        balls[i].yyVelocity = int2Accum(-3); 
////        balls[i].xxFPosition = balls[i].xxPosition+balls[i].xxVelocity;
////        balls[i].yyFPosition = balls[i].yyPosition+balls[i].yyVelocity;
////        balls[i].color = ILI9340_GREEN;
////    }
////    //2 hardcoded balls
////    balls[0].xxPosition = int2Accum(119);
////    balls[0].yyPosition = int2Accum(309);
////    balls[0].xxVelocity = int2Accum(0); // injected from right side
////    balls[0].yyVelocity = int2Accum(-3); 
////    balls[0].color = ILI9340_GREEN;
////    balls[0].hit_count = 0;
////    //balls[0].just_collided = 1;
////  //where will this ball be next?
////    balls[0].xxFPosition = balls[0].xxPosition+balls[0].xxVelocity;
////    balls[0].yyFPosition = balls[0].yyPosition+balls[0].yyVelocity;
////    
////    balls[1].xxPosition = int2Accum(120);
////    balls[1].yyPosition = int2Accum(100);
////    balls[1].xxVelocity = int2Accum(0); // injected from left side
////    balls[1].yyVelocity = int2Accum(3); 
////    balls[1].color = ILI9340_RED;
////    balls[1].hit_count =0;
////    //balls[1].just_collided = 1;
////    //where will this ball be next?
////    balls[1].xxFPosition = balls[1].xxPosition+balls[1].xxVelocity;
////    balls[1].yyFPosition = balls[1].yyPosition+balls[1].yyVelocity;
//    
//  // seed random color
//  srand(1);
//
//  // round-robin scheduler for threads
//  while (1){
//       // init the threads
//    PT_INIT(&pt_timer);
//    PT_INIT(&pt_color);
//    PT_INIT(&pt_anim);
//
//      // redraw everything in another game
//      // init the display
//    tft_init_hw();
//    tft_begin();
//    tft_fillScreen(ILI9340_BLACK);
//  //240x320 vertical display
//    tft_setRotation(0); // Use tft_setRotation(1) for 320x240
//  
//  //print the border of the game
//    tft_drawLine(0, 0, 0, 320, ILI9340_GREEN);//horizontal
//    tft_drawLine(0, 0, 240, 0, ILI9340_GREEN);//vertical
//    tft_drawLine(0, 318, 238, 318, ILI9340_GREEN);//vertical right
//    tft_drawLine(238, 318, 238, 0, ILI9340_GREEN);//horizontal top
//    //print those weird bars
//    tft_drawLine(0, 80, 60, 80, ILI9340_RED);
//    tft_drawLine(240, 80, 180, 80, ILI9340_RED);
//    PT_SCHEDULE(protothread_timer(&pt_timer));
//    PT_SCHEDULE(protothread_anim(&pt_anim));
//    PT_WAIT_THREAD(&pt_timer, protothread_timer(struct pt *pt)); // wait for these two thread to end
//    PT_WAIT_THREAD(&pt_anim, protothread_anim(struct pt *pt)); // after they end, game restart.
//    tft_fillScreen(ILI9340_BLACK);
//    tft_setCursor(3, 3);
//    tft_setTextColor(ILI9340_WHITE);  tft_setTextSize(4);
//    tft_fillRoundRect(3,3, 180, 180, 1, ILI9340_BLACK);// x,y,w,h,radius,color
//        //tft_fillRoundRect(0,24, 140, 14, 1, ILI9340_BLACK);// x,y,w,h,radius,color
//    if(score > 100){
//        sprintf("WINNER WINNER !!");
//    }
//    else{
//        sprintf("YOU LOSE");
//    }
//
//    }
//  } // main
//
//// === end  ======================================================