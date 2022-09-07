#include <16F688.h>
#device ADC=10

#FUSES PUT                      //Power Up Timer
#FUSES PROTECT                  //Code protected from reads
#FUSES NOBROWNOUT               //No brownout reset

#use delay(internal=8000000)
#use rs232(baud=19200,parity=N,bits=8,UART1,ERRORS)

#define Led  Pin_C0
#define Rele1  Pin_C2
#define  BtnTest pin_c1
#define  BtnTest2 pin_c3
#define  ID_UDP '0'
#define  ID_TCP '1'
int8  cnt_rx; // posicion del vector, Max
char  vrx [50];
char  vname [10];
char  vpass [10];
char  vIP [4];

void Crear_red_pordefecto();
void Wait_ClearBuffer_uart( int16 time_ms);
void Udp_paso1();
void Conecta_Wifi();
unsigned int Rx_Udp_IP();
void Crea_udpsocket();
int8  VcompareNB (char *v1[],char *v2[],int8 nbytes);
void Conectar_Como_Cliente();
void Close_connection(char Id);
VOID Crear_new_red();
void Activar_servidor();
void Rx_new_wifi();
char IPD[]="+IPD,0,";
char TCPClosed[] ="1,CLOSED"; 
char OK[]="OK";


//----------------------------------
char get_cmd(){
   char cmd1;
   int8 jk;
   cmd1=0;   
   for(jk=7;jk<cnt_rx;jk++){
      if(vrx[jk]==':'){
        cmd1 = vrx[jk+1];
        return cmd1;
      }
   }
   
   return cmd1;
}
//----------------------------------
//+IPD,0,21:C,WIFITEST,CLAVE
int8 get_entrecoma(int8 posini ,char *v1[] ){
   int8 jk,k1;    
   for(jk=posini;jk<cnt_rx;jk++){
      if(vrx[jk]==','){
         
        //----------------
        for(k1=jk+1;k1<cnt_rx;k1++){            
            if(vrx[k1]==','){  
              *v1 =0;
              return k1 ;
            }
            *v1 = vrx[k1];
            v1=v1+1 ; // incremento puntero
         }     
          *v1 =0;
         
        //-------------------
      }
   }
      
}

//---------------------------
int8 Get_wifi_name (){
   //+IPD,0,21:C,WIFITEST,CLAVE
   // Vrx[], cnt_rx
int8 jk,k1,cnx;    
   for(jk=7;jk<cnt_rx;jk++){
      if(vrx[jk]==','){ 
        cnx=0;
        //----------------
        for(k1=jk+1;k1<cnt_rx;k1++){            
            if(vrx[k1]==','){  
              vname[cnx] = 0;
              return k1 ;
            }
            vname[cnx] = vrx[k1];
            cnx=cnx+1 ; // incremento puntero
         }    
         //------------
           vname[cnx] = 0;
           return cnt_rx ;
        //-------------------
      }
   }
}
//---------------------------
void Get_wifi_clave (int8 posi){
   //+IPD,0,21:C,WIFITEST,CLAVE
   // Vrx[], cnt_rx
int8 jk,k1,cnx;    
   for(jk=posi;jk<cnt_rx;jk++){
      if(vrx[jk]==','){ 
        cnx=0;
        //----------------
        for(k1=jk+1;k1<cnt_rx;k1++){            
            if(vrx[k1]==','){  
              vpass[cnx] = 0;
              return  ;
            }
            vpass[cnx] = vrx[k1];
            cnx=cnx+1 ; // incremento puntero
         }    
         //------------
           vpass[cnx] = 0;
           return  ;
        //-------------------
      }
   }
}
//-------------------------------------
#INT_RDA
void  RDA_isr(void) 
{  
   
   char Dato,cmd;
   Dato = getc();  
  
   //- procesar con enter=13
   //  +IPD,0,1:A (ENTER)+CHR(10)
   if (Dato==13){
      // vrx [cnt_rx]=0;     
       
    //  if (cnt_rx < 5){ cnt_rx=0; return ;} // valido que esten los datos
      
      if (VcompareNB(&IPD,&vrx,7)==1){
        // +IPD,0,1:C,Wifiname,passwrd
          
          if (Rx_Udp_IP()==1){           
           Close_connection(ID_UDP);
           Conectar_Como_Cliente();
          cnt_rx=0; return ;}
          
          cmd = get_cmd(); 
          
          if (cmd=='A'){output_high( Rele1);}
          else if (cmd=='B'){output_low( Rele1);}
          else if (cmd=='C'){Rx_new_wifi();}       
          //----------------- cambio de clave
           // +IPD,0,23:C,Nombre red,clave chr(13)+chr(10) 
      
        cnt_rx=0; // PARA nuevos datos
      }else if (vrx[0]=='O' && vrx[1]=='K'){
       cnt_rx=0; return;
      }else if (VcompareNB(&TCPClosed,&vrx,8)==1){
        // reactivo escuha UDP
         Udp_paso1();
         Conecta_Wifi();   
         Crea_udpsocket();
      }
      
      
      cnt_rx=0; 
      
   //--------------------------------------- fin enter   
   }else if(Dato!=10){      
           vrx [cnt_rx]=Dato;
           cnt_rx++;
          // if (cnt_rx>=79){cnt_rx=0;}
   } 

 
}
//------------------------------------------------
unsigned int Rx_Udp_IP(){
 int8 jk,cntbyte;
 char cd1;
 // cnt_rx = 26;
  //trcpy (vrx, "+IPD,0,16:IP192.168.43.126");
  //+IPD,0,
 //  if (VcompareNB(&IPD,&vrx,7)==0){  return ; }
   // printf("Rx *IPD");
    if (vrx[10]!='I' && vrx[11]!='P' ){ return  0; }
   //  printf("Rx *IP");
    // Leo IP
   
    Vip[0]=0;
    Vip[1]=0;
    Vip[2]=0;
    Vip[3]=0;
    cntbyte=0;
   for(jk=12;jk<cnt_rx;jk++){
     cd1 = vrx[jk];
     if (cd1 !='.'){
         cd1 = cd1 -'0';
         Vip[cntbyte] = Vip[cntbyte]*10;
         Vip[cntbyte] += cd1;
     }else{
        cntbyte ++;
     }     
   }
  //-----------------------------------------
  // printf("\r\t RxIP:%u.%u.%u.%u",Vip[0],Vip[1],Vip[2],Vip[3]);
 //----------------------------------------------------  
 return 1;
}


//-----------------------------------------

#INT_TIMER1
void  TIMER1_isr(void) 
{
   output_toggle(Led);
   if (!input(BtnTest)){
   //  Crear_red_pordefecto();
   // Activar_servidor();
     Udp_paso1();
     Conecta_Wifi();   
     Crea_udpsocket();
   }
  //--------------------------
  if (!input(BtnTest2)){
     //  Rx_Udp_IP();
   }
  
  //---------------------------
   
}

//------------------------
int8  VcompareNB(char *v1[],char *v2[],int8 nbytes){
    int8 j;
    
    for(j=0;j<nbytes;j++){
      if (*v1!=*v2){return 0;} // hay un Byte diferente
      v1=v1+1;
      v2=v2+1;
    }
   return 1; // son iguales
}
//----------------------
void Wait_ClearBuffer_uart( int16 time_ms){
   int16 i;
  for (i=0;i<time_ms;i++){
     if ( kbhit()){getc();} // Pregunto si hay datos en uart para simplemente leerlo
     delay_us(500);
     if ( kbhit()){getc();} // Pregunto si hay datos en uart para simplemente leerlo
     delay_us(500);
  }
  cnt_rx=0;
}
//-------------------------------
void Activar_servidor(){
   //----------------------- Habilito el servidor
   puts("AT+CIPMUX=1");
   Wait_ClearBuffer_uart(200);   
   puts("AT+CIPSERVER=1,80");
   Wait_ClearBuffer_uart(200); 
   cnt_rx =0;
}
//---------------------------
void Crear_red_pordefecto(){

   puts("AT");   // 13-10  
   Wait_ClearBuffer_uart(200);
   //DELAY_MS(100);
   puts("AT+CWMODE=3");
   Wait_ClearBuffer_uart(200);
   puts("AT+CIPMUX=1");
   Wait_ClearBuffer_uart(200);
   puts("AT+CWSAP=\"Sonoffxxz\",\"password\",3,4");
   Wait_ClearBuffer_uart(200);
   puts("AT+RST");
   Wait_ClearBuffer_uart(3000); 
 
}
 //---------------------------------
void Crear_new_red(){
  //printf("%X,%X",vname[8],vpass[6]);

   puts("AT");   // 13-10  
   Wait_ClearBuffer_uart(200);
   puts("AT+CWMODE=3");
   Wait_ClearBuffer_uart(300);  
   puts("AT+CIPMUX=1");
   Wait_ClearBuffer_uart(300);
   printf("AT+CWSAP=\"%s\",\"%s\",3,4\r\n",&vname,&vpass);
  // puts("AT+CWSAP_DEF=\"Wifivega\",\"12345678\",3,4"); 
   // putc(13); // 
   // putc(10);
  //\r\n
 
   Wait_ClearBuffer_uart(800);
   puts("AT+RST");
   Wait_ClearBuffer_uart(3000); 

}

//------------------------------
Void Udp_paso1 (){
   puts("ATE0"); // quita el eco ó echo 
   Wait_ClearBuffer_uart(200); 
   puts("AT+CWMODE=3"); // Modo AP + Station 
   Wait_ClearBuffer_uart(200); 
   puts("AT+CWDHCP_DEF=2,1"); // activa DHCP para get IP
   Wait_ClearBuffer_uart(200); 
   puts("AT+CIPMUX=1"); //activa multiples conexiones
   Wait_ClearBuffer_uart(200);
}
//---------------------------------------------
void Conecta_Wifi (){   
//  printf("AT+CWSAP=\"%s\",\"%s\",3,4\r\n",&vname,&vpass);
   printf("AT+CWJAP_DEF=\"VegaMoto\",\"12345678--&&\"\r\n");   
   Wait_ClearBuffer_uart(7000);
}
//--------------------
void Crea_udpsocket(){
 printf("AT+CIPSTART=0,\"UDP\",\"0.0.0.0\",8080,8080,2\r\n"); 
 Wait_ClearBuffer_uart(2000);

}
//--------------------
void Close_connection(char Id){
 printf("AT+CIPCLOSE=%c\r\n",Id); 
 Wait_ClearBuffer_uart(500); 
}
//-----------------------
void Conectar_Como_Cliente(){
  puts("AT"); // 
  Wait_ClearBuffer_uart(200); 
   puts("AT"); // 
  Wait_ClearBuffer_uart(200); 
  printf("AT+CIPSTART=1,\"TCP\",\"%u.%u.%u.%u\",8081\r\n",Vip[0],Vip[1],Vip[2],Vip[3]);
  Wait_ClearBuffer_uart(2000);  
}


//--------------------------------------
void main()
{
   // configuraciones
   setup_oscillator(OSC_8MHZ);
   setup_timer_1(T1_INTERNAL|T1_DIV_BY_8); //262 ms overflow

   cnt_rx =0;
  
   
   //-------------------
     vrx [0]='+';
     vrx [1]='I';
     vrx [2]='P';
     vrx [3]='D';
     
     delay_ms(500);
     Activar_servidor();
     Wait_ClearBuffer_uart(500);
     puts("ATE0");
     Wait_ClearBuffer_uart(200);
     
     //----------------------------
     enable_interrupts(INT_RDA);
     enable_interrupts(INT_TIMER1);
     enable_interrupts(GLOBAL);
   
  /*  if (VcompareNB(&IPD,&vrx,3)==1){
       printf("Son igulales");
     }else{printf("Error");}
     
      if (VcompareNB(&OK,&vrx,3)==1){
       printf("Son igulales");
     }else{printf("Error");} */
   
   //--------------------

   while(TRUE)
   {

   enable_interrupts(INT_RDA);
   enable_interrupts(INT_TIMER1);
   enable_interrupts(GLOBAL);
      //TODO: User Code
   }
   
   //-----------------------

}


//------------------------------------
void Rx_new_wifi(){
  //+IPD,0,23:C,Nombre red,clave chr(13)+chr(10)
  int8 cnt2;
  int8 posclave = get_entrecoma(7,&vname);
  cnt2=get_entrecoma(posclave,&vpass);
  //  cnt2 = Get_wifi_name();
  // Get_wifi_clave(cnt2);
   Crear_new_red();
   Activar_servidor();
 // puts(vname);
 // puts(vpass);
  
}
