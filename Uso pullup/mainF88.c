#include <16F88.h>
#device ADC=10

#FUSES PUT                      //Power Up Timer
#FUSES NOBROWNOUT               //No brownout reset
#FUSES NOLVP,INTRC_IO                    //No low voltage prgming, B3(PIC16) or B5(PIC18) used for I/O

#use delay(internal=8000000)


void main()
{
 //  setup_ccp1(CCP_OFF);
     set_tris_b(0xF0);
    PORT_b_PULLUPS(TRUE);
   //0x70 
   while(TRUE)
   {
      output_High(pin_B0);
      
      //TODO: User Code
   }

}
