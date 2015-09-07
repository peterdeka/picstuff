/*
 * File:   main.c
 * Author: deka
 *
 * Created on September 7, 2015, 12:49 AM
 */


#include <xc.h>
#include <string.h>
// CONFIG
#pragma config FOSC = INTOSCIO  // Oscillator Selection bits (INTOSC oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RA5/MCLR/VPP Pin Function Select bit (RA5/MCLR/VPP pin function is digital input, MCLR internally tied to VDD)
#pragma config BOREN = OFF      // Brown-out Detect Enable bit (BOD disabled)
#pragma config LVP = ON         // Low-Voltage Programming Enable bit (RB4/PGM pin has PGM function, low-voltage programming enabled)
#pragma config CPD = OFF        // Data EE Memory Code Protection bit (Data memory code protection off)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)
#define _XTAL_FREQ 4000000 // Freq XTAL 4Mhz



char caracter;
bit flag_interrupcao = 0;
bit flag_sensor = 0;
///////////////////////////////////////////////////interrupção//////////////////////////////////////////////////////////////
void interrupt RS232(void)//vetor de interrupção
 {
    if (RCIF == 1){ 
    caracter = RCREG;
     flag_interrupcao = 1;
     RCIF = 0;//  limpa flag de interrupção de recepção
    }
    if(INTF == 1){
        flag_sensor = 1;
        INTF = 0; 
    }
 }

/////////////////////////////////funçoes usadas pela uart //////////////////////////////////////////////////////
void inicializa_RS232(long velocidade,int modo)
{   /*  Por padrão é usado o modo 8 bits e sem paridade, mas se necessario ajuste
     *  aqui a configuração desejada.
     *  verifique datasheet para ver a porcentagem de erro e se a velocidade é
     *  possivel para o cristal utilizado.
    */
    RCSTA = 0x90;  //habilita porta serial (RCSTA.SPEN=1),recepção de 8 bit em
                   // modo continuo (RCSTA.CREN = 1), asincronico.

    int valor;
        if(modo == 1)
        {//modo = 1 ,modo alta velocidade
         TXSTA = 0x24;//modo assincrono,trasmissao 8 bits.
         valor =(int)(((_XTAL_FREQ/velocidade)-16)/16);//calculo do valor do gerador de baud rate
        }
        else
        {//modo = 0 ,modo baixa velocidade
         TXSTA = 0x20;//modo assincrono,trasmissao 8 bits.
         valor =(int)(((_XTAL_FREQ/velocidade)-64)/64);//calculo do valor do gerador de baud rate
        }
    SPBRG = valor;
    RCIE = 1; //habilita interrupção de recepção
    TXIE = 0; //deixa interrupção de transmissão desligado(pois corre se o risco de ter
              //uma interrupção escrita e leitura ao mesmo tempo)
}
void escreve(char valor)
{
    TXIF = 0;//limpa flag que sinaliza envio completo.
    TXREG = valor;
    while(TXIF ==0);//espera enviar caracter
}
void imprime(const char frase[])
{
     char indice = 0;
     char tamanho = strlen(frase);
      while(indice < tamanho ) 
       {
           escreve(frase[indice]);
           __delay_ms(1);   // @retux: Agregado sin este delay se envia demasiado rapido.
           indice++;
       }
}

///////// Funcion main ///////////////////////



void main(void)
{
    TRISB = 0X03;//configura portB  B0,B1 (pino RX) como entrada
    PORTB = 0;  // limpar as portas que estão configuradas como saidas
    inicializa_RS232(9600,1);//modo de alta velocidade
    PEIE = 1;//habilita interrupção de perifericos do pic
    GIE = 1; //GIE: Global Interrupt Enable bit
    __delay_ms(8000);
    INTCONbits.INTE = 1;
    //imprime("Usando serial MPlab xc8 \n\r");
    //imprime("En Minicom habilite newline con <ctrl-a> a. \n\r");
    //imprime("Digite, yo le voy a dar eco. \n\r");
    for(;;)
    {
       /* if(flag_interrupcao ==  1)
        {   //tem dados para ler
            //imprime("\n\r Digitaste:");
            escreve(caracter);
            flag_interrupcao = 0;
        }
        */
        if(flag_sensor == 1){
            imprime("HIGH\n\r");
            flag_sensor = 0;
        }
        __delay_ms(2000);
    }//loop infinito
}
