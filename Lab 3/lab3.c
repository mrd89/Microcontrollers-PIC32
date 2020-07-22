///*
// * File:        TFT, keypad, DAC, LED, PORT EXPANDER test
// *              With serial interface to PuTTY console
// * Author:      Bruce Land
// * For use with Sean Carroll's Big Board
// * http://people.ece.cornell.edu/land/courses/ece4760/PIC32/target_board.html
// * Target PIC:  PIC32MX250F128B
// */
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
//// graphics libraries
//// SPI channel 1 connections to TFT
//#include "tft_master.h"
//#include "tft_gfx.h"
//// need for rand function
//#include <stdlib.h>
//// need for sin function
//#include <math.h>
//////////////////////////////////////
//
//// lock out timer 2 interrupt during spi comm to port expander
//// This is necessary if you use the SPI2 channel in an ISR.
//// The ISR below runs the DAC using SPI2
//#define start_spi2_critical_section INTEnable(INT_T2, 0)
//#define end_spi2_critical_section INTEnable(INT_T2, 1)
//
//// string buffer
//char buffer[60];
//
//////////////////////////////////////
//// DAC ISR
//// A-channel, 1x, active
//#define DAC_config_chan_A 0b0011000000000000
//// B-channel, 1x, active
//#define DAC_config_chan_B 0b1011000000000000
//// DDS constant
////#define two32 4294967296.0 // 2^32 
////#define Fs 100000
//
////for the game button, some useful macros
//#define EnablePullupB(bits) CNPDBCLR = bits; CNPUBSET = bits;
//#define DisablePullupB(bits) CNPDBCLR = bits;
//
////== Timer 2 interrupt handler ===========================================
//volatile unsigned int DAC_data ;// output value
//volatile SpiChannel spiChn = SPI_CHANNEL2 ;	// the SPI channel to use
//volatile int spiClkDiv = 4 ; // 10 MHz max speed for port expander!!
//
//int dutycycle = 0; //from 0 to 39,999
//
////raw value from the adc
//volatile int adcVal=0;
////raw offset from the adc
//int potbias= 0;
//int once =1;
//#define dmaChn 0
//double dutycycle_display;
////----PID parameters-----
//int pid_p=0, pid_d=0, pid_a=360;
//int pid_a_input = -90;
//double pid_i;
//double alpha = .8;
//double beta = .9;
//volatile double angle=0,scaled_angle=0,adc_scale=1.0/1023.0;
//volatile int error=0;
//volatile int prev_error;
//int velocity_array[4];
//volatile int error_deriv = 0;
//volatile int error_integ =0;
//volatile double term_integ=0;
//int i,j; //loop vars if needed  
//double max_integ=65000  ;
//int button_pressed = 0; // button connected to the GPIOB7
//
//
//
//void __ISR(_TIMER_2_VECTOR, ipl2) Timer2Handler(void)
//{
//    int junk;
//    mT2ClearIntFlag();
//    //1KHz interrupt
//    //PID logic and PWM control
//    //read beam angle with the ADC
//    adcVal = ReadADC10(0);
//    if(once){
//        potbias = adcVal; //initial adc reading stored as bias
//        potbias =730;
//        once=0;
//    }
//    
//   //low pass filter
//    angle =angle * alpha + (1-alpha)* (double)(adcVal);
//    
//    //output the beam angle to DACA'
//    start_spi2_critical_section;
//    SPI_Mode16();
//    mPORTBClearBits(BIT_4);
//    WriteSPI2(DAC_config_chan_A|((int)(((int)angle)<<2))&0xfff);
//    while(SPI2STATbits.SPIBUSY){};//wait for that to transmit
//    junk = ReadSPI2();
//    mPORTBSetBits(BIT_4);
//    end_spi2_critical_section;
//    //low pass read beam angle
//    //angle = (double)(alpha*angle+(1.0-alpha)*adcVal)-(double)potbias;
//
//    //scaled_angle = ((alpha*adc_scale)*180)-90;//this double now between -90 and 180
//    //if angle measured is exactly the potbias, angle is zero, and scaled angle is -90
//    //error calc using low passed angle signal, and compares values between 0 and 1023 resolution
//    error=pid_a-(int)angle; //error from desired angle in ADC reads
//    //update the stored error values for diff and int gain calcs
//    for(i=1;i<4;i++){
//        velocity_array[i]=velocity_array[i-1];
//    }
//    velocity_array[0] = error - prev_error;
//    
//    //check if there was a zero crossing
//    if (prev_error>0&&error<0){
//        error_integ = 0.95*error_integ;
//    }
//    
////    if (prev_error<0&&error>0){
////        error_integ = 1.03*error_integ;
////    }
////    
//    
//    prev_error = error;
//    //tft_setCursor(3, 3);
//    //tft_fillRoundRect(3,3, 180, 24, 1, ILI9340_BLACK);// x,y,w,h,radius,color
//    //sprintf(buffer,"Error: %d", pid_a);
//    //sprintf(buffer,"Error: %d", error);
//    //sprintf(buffer,"Error: %d", error);
//    //tft_writeString(buffer);
//    //sprintf(PT_send_buffer,"error: %d", pid_a);
//    //PT_SPAWN(pt, &pt_DMA_output, PT_DMA_PutSerialBuffer(&pt_DMA_output) );
//    //prop control
//    dutycycle = 0; //zero the output to recalculate it
//    if (error<200){
//    dutycycle += pid_p*(error );
//    }
//    else {dutycycle+= pid_p*error;}
//    //diff control
//    error_deriv = velocity_array[0]+velocity_array[1]+velocity_array[2]+velocity_array[3];//approx derivative fct
//    dutycycle+=pid_d*error_deriv;
//    //i control
//    
//    //accumulate integral term
//    
//    error_integ += error;
//    term_integ = (double)pid_i*error_integ; //what the integrator wants to be
//    
//    //check to make sure the integral term isn't too big
//    if(term_integ>max_integ){
//        //set to the maximum instead
//        term_integ = max_integ;
//        error_integ= term_integ / pid_i;
//    }
//    else if (term_integ<(-max_integ)){
//        //set to the minimum instead
//        term_integ = -max_integ;
//        error_integ= -term_integ / pid_i;
//    }
//    dutycycle += (int)term_integ;
//    
//    //safety checks 
//    //error non neg
//    //clamp range of dutycycle
//    if (dutycycle<=0){
//        dutycycle=0;
//    }
//    if (dutycycle>=39999){
//        dutycycle=39999;
//    }
//    //dutycycle = 28000;
//    //set the PWM output
//    SetDCOC3PWM(dutycycle);
//    dutycycle_display = (.97*dutycycle_display+(1-.97)*dutycycle);
//    ///zz = (int)dutycyle_display
//    //interpret motor control signal
//    //output the motor control signal over DACB
//    SPI_Mode16();
//    mPORTBClearBits(BIT_4);
//    WriteSPI2(DAC_config_chan_B|(((int)(dutycycle_display*.1))&0xfff));
//    while(SPI2STATbits.SPIBUSY){};//wait for that to transmit
//    junk = ReadSPI2();
//    mPORTBSetBits(BIT_4);
//    
//}
//
//// === thread structures ============================================
//// thread control structs
//// note that UART input and output are threads
//static struct pt pt_timer, pt_screen, pt_serial, pt_button;
//// The following threads are necessary for UART control
//static struct pt pt_input, pt_output, pt_DMA_output ;
//
//// system 1 second interval tick
//int sys_time_seconds ;
//
//
//// === Timer Thread =================================================
//// update a 1 second tick counter
//static PT_THREAD (protothread_timer(struct pt *pt))
//{
//    PT_BEGIN(pt);
//     // set up LED to blink
//     mPORTASetBits(BIT_0 );	//Clear bits to ensure light is off.
//     mPORTASetPinsDigitalOut(BIT_0 );    //Set port as output
//      while(1) {
//        // yield time 1 second
//        PT_YIELD_TIME_msec(1000) ;
//        sys_time_seconds++ ;
//        // toggle the LED on the big board
//        mPORTAToggleBits(BIT_0);
//        // draw sys_time
//        sprintf(buffer,"Time=%d", sys_time_seconds);       
//        // NEVER exit while
//        
//      } // END WHILE(1)
//  PT_END(pt);
//} // timer thread
//
//// === Screen Update Thread =============================================
//static int xc=10, yc=150, vxc=2, vyc=0;
//static PT_THREAD (protothread_screen(struct pt *pt))
//{
//    PT_BEGIN(pt);
//      while(1) {
//        // yield time 100 millisecond
//        PT_YIELD_TIME_msec(100);
//        //update the screen at 10 times a second
//      } // END WHILE(1)
//  PT_END(pt);
//} // screen update thread
//
////=== Serial terminal thread =================================================
//
//static PT_THREAD (protothread_serial(struct pt *pt))
//{
//    PT_BEGIN(pt);
//      static char cmd[30];
//      static float value;
//      while(1) {
//          
//            // send the prompt via DMA to serial
//            cursor_pos(4,1);
//            red_text ;
//            sprintf(PT_send_buffer,"Enter p ##, i ##, d ##, a ##:>");
//            // by spawning a print thread
//            PT_SPAWN(pt, &pt_DMA_output, PT_DMA_PutSerialBuffer(&pt_DMA_output) );
//            clr_right ;
//            normal_text ;
//            
//            //spawn a thread to handle terminal input
//            // the input thread waits for input
//            // -- BUT does NOT block other threads
//            // string is returned in "PT_term_buffer"
//            PT_SPAWN(pt, &pt_input, PT_GetSerialBuffer(&pt_input) );
//            PT_terminate_char = 0 ;//hit enter to end command 
//            PT_terminate_count = 5 ; 
//            PT_terminate_time = 5000 ;
//            // note that there will NO visual feedback using the following function
//            //PT_SPAWN(pt, &pt_input, PT_GetMachineBuffer(&pt_input) );
//            
//            // returns when the thead dies on the termination condition
//            // IF using PT_GetSerialBuffer, when <enter> is pushed
//            // IF using PT_GetMachineBuffer, could be on timeout
//             if(PT_timeout==0) {
//                 sscanf(PT_term_buffer, "%s %f", cmd, &value);
//             }
//            // no actual string
//             else {
//                 // uncomment to prove time out works
//                 //mPORTAToggleBits(BIT_0);
//                 // disable the command parser below
//                 cmd[0] = 0 ;
//             }
//
//             switch(cmd[0]){
//                 case 'p': 
//                     
//                    pid_p = (int)value;
//                    cursor_pos(6,1);
//                    sprintf(PT_send_buffer,"Prop Gain = %6d", pid_p);
//                    // by spawning a print thread
//                    PT_SPAWN(pt, &pt_DMA_output, PT_DMA_PutSerialBuffer(&pt_DMA_output) );
//                    clr_right ;
//                     break;
//                 
//                 case 'i': 
//                     
//                    pid_i = (double)value;
//                    cursor_pos(6,1);
//                    sprintf(PT_send_buffer,"Int Gain = %f", pid_i);
//                    // by spawning a print thread
//                    PT_SPAWN(pt, &pt_DMA_output, PT_DMA_PutSerialBuffer(&pt_DMA_output) );
//                    clr_right ;
//                    break;
//                 case 'd': 
//                     // enter diff gain
//                    pid_d = (int)value;
//                    cursor_pos(6,1);
//                    sprintf(PT_send_buffer,"Diff Gain = %d", pid_d);
//                    // by spawning a print thread
//                    PT_SPAWN(pt, &pt_DMA_output, PT_DMA_PutSerialBuffer(&pt_DMA_output) );
//                    clr_right ;
//                     break;
//                 case 'a': 
//                     // enter angle desired in degrees, 
//                     //and scale the value to a desired angle between 0 and 1023
//                    pid_a_input=(int) value;
//                    cursor_pos(6,1);
//                    sprintf(PT_send_buffer,"Angle = %d ", value);
//                    cursor_pos(7,1);
//                    //pid_a=(int) ((value+90)/180.0)*1023+potbias;//pid_a is 0-1023
//                    pid_a = (int)(pid_a_input*3.95+(double)potbias);//3.8 ADC reads per degree, so this is a desired angle in ADC reads
//                    sprintf(PT_send_buffer,"Angle scaled = %d ", pid_a);
//                    // by spawning a print thread
//                    PT_SPAWN(pt, &pt_DMA_output, PT_DMA_PutSerialBuffer(&pt_DMA_output) );
//                    clr_right ;
//                    //reset the motor arm to try the PID controller again
//                    
//                     break;
//                 case 'l': 
//                     // enter frequecy in HZ
//                    cursor_pos(6,1);
//                    sprintf(PT_send_buffer,"P, I, D, A = %d, %f, %d, %d", pid_p, pid_i, pid_d, pid_a);
//                    // by spawning a print thread
//                    PT_SPAWN(pt, &pt_DMA_output, PT_DMA_PutSerialBuffer(&pt_DMA_output) );
//                    clr_right ;
//                    //reset the motor arm to try the PID controller again
//                    
//                     break;
//             }
//             
//                  tft_fillScreen(ILI9340_BLACK);
//                  tft_setRotation(1);
//                  tft_setCursor(2, 2);
//                  tft_setTextColor(ILI9340_WHITE);  tft_setTextSize(2);
//                  sprintf(buffer," P = %d \n I = %f \n D = %d \n Angle = %d", pid_p, pid_i, pid_d, pid_a_input);
//                  tft_writeString(buffer);
//             
//            // never exit while
//      } // END WHILE(1)
//  PT_END(pt);
//} // thread 3
////=== Button push thread =================================================
//static PT_THREAD (protothread_button(struct pt *pt))
//{
//    PT_BEGIN(pt);
//    
//      while(1) {
//          
//          //pid_d = 100;
//        start_spi2_critical_section;
//        button_pressed= mPORTBReadBits(BIT_7);
//        end_spi2_critical_section;
//        if (button_pressed == 0) {
//            //set angle to -90 degree now
//            pid_a = 360;
//            while(button_pressed == 0){
//                //read the value of B7 now
//
//                start_spi2_critical_section;
//                button_pressed= mPORTBReadBits(BIT_7);
//                end_spi2_critical_section;
//                PT_YIELD_TIME_msec(150);
//            }
//            pid_a = potbias;
//            pid_a_input = 0;
//            
//            // update screen
//            tft_fillScreen(ILI9340_BLACK);
//            tft_setRotation(1);
//            tft_setCursor(2, 2);
//            tft_setTextColor(ILI9340_WHITE);  tft_setTextSize(2);
//            sprintf(buffer," P = %d \n I = %f \n D = %d \n Angle = %d", pid_p, pid_i, pid_d, pid_a_input);
//            tft_writeString(buffer);
//            
//            PT_YIELD_TIME_msec(5000);
//            pid_a = (int)(30 * 3.95 + (double)potbias);
//            pid_a_input = 30;
//            
//            // update screen
//            tft_fillScreen(ILI9340_BLACK);
//            tft_setRotation(1);
//            tft_setCursor(2, 2);
//            tft_setTextColor(ILI9340_WHITE);  tft_setTextSize(2);
//            sprintf(buffer," P = %d \n I = %f \n D = %d \n Angle = %d", pid_p, pid_i, pid_d, pid_a_input);
//            tft_writeString(buffer);
//            
//            PT_YIELD_TIME_msec(5000);
//            pid_a = (int)(-30 * 3.95 + (double)potbias);
//            pid_a_input = -30;
//             // update screen
//            tft_fillScreen(ILI9340_BLACK);
//            tft_setRotation(1);
//            tft_setCursor(2, 2);
//            tft_setTextColor(ILI9340_WHITE);  tft_setTextSize(2);
//            sprintf(buffer," P = %d \n I = %f \n D = %d \n Angle = %d", pid_p, pid_i, pid_d, pid_a_input);
//            tft_writeString(buffer);
//            
//            PT_YIELD_TIME_msec(5000);
//            
//            // set angle =0 
//            pid_a = potbias;
//            pid_a_input = 0;
//            
//            // update screen
//            tft_fillScreen(ILI9340_BLACK);
//            tft_setRotation(1);
//            tft_setCursor(2, 2);
//            tft_setTextColor(ILI9340_WHITE);  tft_setTextSize(2);
//            sprintf(buffer," P = %d \n I = %f \n D = %d \n Angle = %d", pid_p, pid_i, pid_d, pid_a_input);
//            tft_writeString(buffer);
//        }
//        
//        else{
//            PT_YIELD_TIME_msec(150);
//        }
//      } // END WHILE(1)
//  PT_END(pt);
//} 
//
//// === Main  ======================================================
//void main(void) {
// //SYSTEMConfigPerformance(PBCLK);
//  
//  ANSELA = 0; ANSELB = 0; 
//
//  // set up DAC on big board
//  // timer interrupt //////////////////////////
//    // Set up timer2 on,  interrupts, internal clock, prescalar 1, toggle rate
//    // at 30 MHz PB clock 60 counts is two microsec
//    // 400 is 100 ksamples/sec
//    // 2000 is 20 ksamp/sec
//
//    // control CS for DAC
//    mPORTBSetPinsDigitalOut(BIT_4);
//    mPORTBSetBits(BIT_4);
//    // SCK2 is pin 26 
//    // SDO2 (MOSI) is in PPS output group 2, could be connected to RB5 which is pin 14
//    PPSOutput(2, RPB5, SDO2);
//    // 16 bit transfer CKP=1 CKE=1
//    // possibles SPI_OPEN_CKP_HIGH;   SPI_OPEN_SMP_END;  SPI_OPEN_CKE_REV
//    // For any given peripherial, you will need to match these
//    // NOTE!! IF you are using the port expander THEN
//    // -- clk divider must be set to 4 for 10 MHz
//    SpiChnOpen(SPI_CHANNEL2, SPI_OPEN_ON | SPI_OPEN_MODE16 | SPI_OPEN_MSTEN | SPI_OPEN_CKE_REV , 4);
//  // end DAC setup
//    
//  // === config threads ==========
//  // turns OFF UART support and debugger pin, unless defines are set
//  PT_setup();
//
//  // === setup system wide interrupts  ========
//  INTEnableSystemMultiVectoredInt();
//
//  // init the threads
//  PT_INIT(&pt_timer);
//  PT_INIT(&pt_screen);
//  PT_INIT(&pt_serial);
//  PT_INIT(&pt_button);
//
//  // init the display
//  // NOTE that this init assumes SPI channel 1 connections
//  tft_init_hw();
//  tft_begin();
//  tft_fillScreen(ILI9340_BLACK);
//  //240x320 vertical display
//  tft_setRotation(0); // Use tft_setRotation(1) for 320x240
//
//  // seed random color
//  //srand(1);
//  //--------output compare unit setup-----------
//  // === Config timer and output compare to make PWM ========
//    // set up timer2 to generate the wave period -- SET this to 1 mSec!
//    OpenTimer2(T2_ON | T2_SOURCE_INT | T2_PS_1_1, 40000);
//    // Need ISR to compute PID controller
//    ConfigIntTimer2(T2_INT_ON | T2_INT_PRIOR_2);
//    mT2ClearIntFlag(); // and clear the interrupt flag
//    // set up compare3 for PWM mode
//    OpenOC3(OC_ON | OC_TIMER2_SRC | OC_PWM_FAULT_PIN_DISABLE , 0,0); //
//    // OC3 is PPS group 1, map to RPB7
//    PPSOutput(4, RPB9, OC3);
//    //SetDCOC3PWM(10000); for testing b4 interrupt added PID
//
//  //----------------------------------------------
//    
//    //----------------ADC setup--------------
//    // configure and enable the ADC
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
//    //--------------------------------
//    tft_setTextColor(ILI9340_WHITE);  tft_setTextSize(1);
//    tft_fillScreen(ILI9340_BLACK);
//    //potbias = 715; //the pendulum is hanging at zero, so this bias corresponds to -90
//    
//    
//    // setup pin 7 as input
//        mPORTBSetPinsDigitalIn(BIT_7);
//        EnablePullupB(BIT_7);
//        
//        // update screen
//        tft_fillScreen(ILI9340_BLACK);
//        tft_setRotation(1);
//        tft_setCursor(2, 2);
//        tft_setTextColor(ILI9340_WHITE);  tft_setTextSize(2);
//        sprintf(buffer," P = %d \n I = %f \n D = %d \n Angle = %d", pid_p, pid_i, pid_d, pid_a_input);
//        tft_writeString(buffer);
//
//  // round-robin scheduler for threads    
//  while (1){
//      PT_SCHEDULE(protothread_timer(&pt_timer));
//      PT_SCHEDULE(protothread_screen(&pt_screen));
//      PT_SCHEDULE(protothread_serial(&pt_serial));
//      PT_SCHEDULE(protothread_button(&pt_button));
//      }
//  } // main
//
//// === end  ======================================================
//
