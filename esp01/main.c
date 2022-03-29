#include <16F688.h>
#device ADC=10
#use delay(crystal=20000000)
#use rs232(baud=57600,parity=N,xmit=PIN_C4,rcv=PIN_C5,bits=8,stream=PORT1,errors)


#define  Led  pin_C0
Void Read_buffer_100ms();
VOID Crear_red();


#INT_TIMER1
void  TIMER1_isr(void) 
{
   output_toggle(Led);
     Crear_red(); 
}

#INT_RDA
void  Uart_isr(void) 
{  char Dato;
   Dato = getc();
  // putc(Dato);
    
}


Void Read_buffer_100ms(){
  int16 xx;
  for (xx=0;xx<600;xx++){
    if ( kbhit()) { getc();} else{delay_us(500);}
  }
  
}

VOID Crear_red(){
   puts("AT+CWMODE=3"); // envia enter al final
   Read_buffer_100ms();
   puts("AT+CIPMUX=1");
   Read_buffer_100ms();
   printf("AT+CWSAP=\"Sonoff++\",\"password\",3,4\n\r");
   Read_buffer_100ms();
   Read_buffer_100ms();
   puts("AT+RST");
   Read_buffer_100ms();
}



void main()
{
   setup_timer_1(T1_INTERNAL|T1_DIV_BY_8);      //104 ms overflow

   enable_interrupts(INT_TIMER1);
    enable_interrupts(INT_RDA);
   enable_interrupts(GLOBAL);

   while(TRUE)
   {


      //TODO: User Code
   }

}



