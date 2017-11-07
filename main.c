 #include "msp430_version.h"
#include <stdio.h>
#include "display.h"
#include "osc.h"

int line = 0;
int sec = 0;
char new_char1[]={BIT2,BIT3+BIT1,BIT3+BIT2+BIT1,BIT3+BIT1,BIT3+BIT1,BIT4+BIT3+BIT2+BIT1+BIT0,BIT4+BIT2+BIT0,BIT4+BIT3+BIT2+BIT1+BIT0};
char *p;
int posicion;
char cont;
const char* poema[] = {"la cena ya se",
                        "sirvio en una",
                        "mesa largucha:",
                        "en cada plato",
                        "una trucha, pa",
                        "la trucha, un",
                        "botellon pa",
                        "la botella, un",
                        "copon, pa la",
                        "copa,una galleta"};                       

void main(void)
{       
        
        osc_init_xt2();
	WDTCTL = WDTPW + WDTHOLD;
      //  P2DIR = 0xFF;
        P1DIR = 0x00;
        p=new_char1;
	display_init();

//	TACCTL0 = CCIE;              // CCR0 habilita interrupción del timer
	TACCR0 = 32768 >> 2;
	TACTL = TASSEL_1 + MC_1; // reloj auxiliar, modo up to CCR0
        
        TBCCTL0 = CCIE;
        TBCTL = TBSSEL_1 + MC_1;
        TBCCR0 = (32768/4) - 1;
        
        //display_new_character(0x02,p);  
	display_test_Write_CGRAM_MS();
	display_set_pos(0x00);
    //    printf("\n");
  //      printf("\r");
//	printf("%c",0x01);
   //     printf("%c",0x02); //PARTE PREVIA DEL CARACTER
	_BIS_SR(GIE);  // modo bajo consumo

        _BIS_SR(LPM0_bits);
    
    // interrupciones
}


// Timer A0 rutina de servicio de interrupción
#pragma vector = TIMERA0_VECTOR
__interrupt void Timer_A (void)
{
          int i,j;
	  while(1){
          printf("t:%d",j);
          i++;
          if(j<250){
            //P2OUT=0xFF;
            j++;
          }
          if(j>=250){
            //P2OUT=0x00;
            j++;
          }        
          if(j==499){
            j=0;
          }
        }  
} 

#pragma vector = TIMERB0_VECTOR
__interrupt void Timer_B (void)  
{
  static int segundos=0;
  static int min=0;
  static int linea=0;
  static int contador=0;
  static int var=0;
  char pin_state=0;
  int i=0;
  
  contador = (contador == 3)?0:contador + 1;
  segundos = (contador == 0)?segundos + 1: segundos;
  min      = (segundos == 60)?min + 1: min; 
  segundos = (segundos == 60)?0: segundos; 
  linea    = ((contador == 0) & (linea < 9))?linea + 1: linea;
  linea    = (linea == 9)?0:linea;
  linea    = ((P1IN & 0x40)==0x40)?0:linea;
  pin_state= ((P1IN & 0x80)==0x80)?1:0;
  
  if (contador == 0){
    display_set_pos(0x00);
    printf("%s\n%s",poema[linea],poema[linea+1]); 
    display_set_pos(0x16);    
    if (segundos <10){
      printf("0%d:0%d",min,segundos);
    }
    else
    {
      printf("0%d:%d",min,segundos);     
    }
  }
  
  if ((pin_state==1) & (var == 0))
  {
    for(i=0;i<16;i++) 
    {
      display_left_shift();
    }
    var = 1;
  }
  else if ((pin_state ==0) & (var == 1))
  {
    for(i=0;i<16;i++) 
    {
      display_right_shift();
    }
    var = 0;
  }
  
} 


