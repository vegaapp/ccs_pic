#include <16F88.h>
#device ADC=10

#FUSES PUT                      //Power Up Timer
#FUSES NOBROWNOUT               //No brownout reset
#FUSES NOLVP,INTRC_IO                    //No low voltage prgming, B3(PIC16) or B5(PIC18) used for I/O

#use delay(internal=8000000)
#USE RS232 (BAUD=9600,UART1,ERRORS)

int16  Datoadc;
Float  Voltaje;

void main()
{
 //  setup_ccp1(CCP_OFF);
     SETUP_ADC(ADC_CLOCK_DIV_64);
     setup_adc_ports(sAN0 | sAN1,VSS_VDD); 
     set_tris_b(0xF0);
    PORT_b_PULLUPS(TRUE);
   //0x70 
   while(TRUE)
   {
      output_High(pin_B3);
      output_b(0xff);
      //TODO: User Code
      set_adc_channel(0);        // the next read_adc call will read channel 0

      delay_us(10);           // a small delay is required after setting the channel
                                                                             
   
      Datoadc=read_adc();  
      Voltaje= Datoadc*(5.0/1023.0);
      printf("Dato adc= %LU V=%0.2f \n\r",Datoadc,Voltaje);
      delay_ms(400);

   }

}
