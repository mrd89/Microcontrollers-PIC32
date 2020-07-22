//
//
//////////////////////////////////////
//// clock AND protoThreads configure!
//// You MUST check this file!
//#include "config_1_3_2.h"
//// threading library
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
//// need for sine function
//#include <math.h>
//// The fixed point types
//#include <stdfix.h>
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
//#define wait2000 wait200;wait200;wait200;wait200;wait200;wait200;wait200;wait200;wait200;wait200;
//#define wait20000 wait2000;wait2000;wait2000;wait2000;wait2000;wait2000;wait2000;wait2000;wait2000;wait2000;
//
//
//
//
////*****************************//
//// VARIABLES DEFS  BELOW HERE
////*****************************//
//#define dmaChn 0
//#define sine_table_size 1500
//
//int raw_sin[sine_table_size];
//unsigned short DAC_data1[sine_table_size]; // lose
//unsigned short DAC_data2[sine_table_size]; // win
//
//unsigned short DAC_data3[sine_table_size]; // end 
//
//
//
//
//void main(){
//// build the sounds
//
////sound data 1   
//    int i, current_amplitude, attack_time, decay_time;
//    int attack_inc, decay_inc, sustain_time;
//    attack_time = (int)(sine_table_size /10);
//    decay_time = (int)(sine_table_size /4) ;
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
//    raw_sin[i] = (int)(current_amplitude * sin((float)i*6.2831/(float)sine_table_size*( 8)) +2047); //12 bi
//    //raw_sin[i] = (int)(current_amplitude * sin((float)i*6.2831/(float)sine_table_size* ((i<sine_table_size*3/4)? 15:40 ))+2047); //12 bit
//    DAC_data1[i] = DAC_config_chan_A | (raw_sin[i] & 0xfff);
//}
//    
//
//    
//    
//    
//
//    
//    
//    // ********************************
//    /// WIN SOUND
//    
//        attack_time = (int)(sine_table_size /2);
//    decay_time = (int)(sine_table_size /5) ;
//    attack_inc = (int)(2047 / attack_time);
//    decay_inc = 0;//(int)(2047 / decay_time);
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
//    raw_sin[i] = (int)(current_amplitude * sin((float)i*6.2831/(float)sine_table_size*(50+i/sine_table_size)) +2047); //12 bi
//    //raw_sin[i] = (int)(current_amplitude * sin((float)i*6.2831/(float)sine_table_size* ((i<sine_table_size*3/4)? 15:40 ))+2047); //12 bit
//    DAC_data2[i] = DAC_config_chan_A | (raw_sin[i] & 0xfff);
//}
//    
//    
//    
//    
//    // FINAL SOUND
//    
//    
//   
//    attack_time = (int)(sine_table_size /8);
//    decay_time = (int)(sine_table_size /20) ;
//    attack_inc =0;// (int)(2047 / attack_time);
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
//    raw_sin[i] = (int)(current_amplitude * sin((float)i*6.2831/(float)sine_table_size*30) +2047); //12 bi
//    //raw_sin[i] = (int)(current_amplitude * sin((float)i*6.2831/(float)sine_table_size* ((i<sine_table_size*3/4)? 15:40 ))+2047); //12 bit
//    DAC_data3[i] = DAC_config_chan_A | (raw_sin[i] & 0xfff);
//}
//    
//
//    
//    
//    
//// ==== SPI channel for DAC ===
//// divide Fpb by 2, configure the I/O ports. not using the SS in this example
//// 16 bit transfer CKP =1 , CKE = 1
//// possibilites SPI_OPEN_CKP_HICK;  SPI_OPEN_END; SPI_OPEN_CKE_REV
//// for any given peripherial, you will need to match these
////clk divider set to 2 for 20MHz
//// FRAMED SPI mode, so SS2 is produced by the SPI hardware.
//OpenTimer2(T2_ON| T2_SOURCE_INT| T2_PS_1_1,909);
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
//// either DMA_OPEN_DEFAULT  or DMA_OPEN_AUTO
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
//while(1){
//
//    DmaChnSetTxfer(dmaChn,DAC_data1,(void*)&SPI2BUF,sine_table_size*2,2,2);
//    DmaChnEnable(dmaChn);
//    for (i=0;i<2000000;i++){
//    wait20            
//    }
//    DmaChnSetTxfer(dmaChn,DAC_data2,(void*)&SPI2BUF,sine_table_size*2,2,2);
//    DmaChnEnable(dmaChn);
//    for (i=0;i<2000000;i++){
//    wait20            
//    }
//    DmaChnSetTxfer(dmaChn,DAC_data3,(void*)&SPI2BUF,sine_table_size*2,2,2);
//    DmaChnEnable(dmaChn);
//    for (i=0;i<6000000;i++){
//    wait20            
//    }
//    
//    
//};
//
//
//return;
//}
//
//
//
//// END CODE //