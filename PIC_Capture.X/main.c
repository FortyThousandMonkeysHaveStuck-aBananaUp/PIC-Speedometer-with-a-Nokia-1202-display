//TMR0
//TMR1
//CCP1
//meander generator
//w1

// PRAGMA CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

#include <xc.h>

//For a NOKIA display
extern struct display_dc dis;
//struct display_dc *st1=&dis;
extern const unsigned char nlcd_Font[256][5];

#include "../PIC_Capture.X/Nokia_display_driver/STE2007_nokia_1202_display.h"


struct wheel{
unsigned int N_number;
char is_computing_flag;
unsigned int TMR1_overflow_counter;
unsigned int TMR1_last_value;
};


unsigned long T_computing(struct wheel * w0);


struct wheel w1={0, 1, 0, 0};
unsigned long TTTT=0;
unsigned int TMR1_overflow_counter=0;
float V=0;



int main(void)
{
/*config*/
    //TRISA
    TRISAbits.TRISA0=0;//0-output RA0 Led
    TRISAbits.TRISA1=0;//RA1 SCK
    TRISAbits.TRISA2=0;//RA2 MOSI
    TRISAbits.TRISA3=0;//RA3 CS
    TRISAbits.TRISA4=0;//RA4 RESET
    TRISB=0;
    PORTB=0xff;
    PORTB=0;
    
    //TRISC
    TRISCbits.TRISC2=1; //1-input for a RC2 pin and CCP1 module
    TRISCbits.TRISC3=0; //for TMR0 indicating
    TRISCbits.TRISC4=0; //for capture indicating
    TRISCbits.TRISC5=0; //for meander generator indicating RC2=RC5
    TRISCbits.TRISC7=0; //for TMR1 indicating

    //TMR0
        //PRESCALER
        OPTION_REGbits.PSA=0;
        OPTION_REGbits.PS2=0; //001 for 1:4
        OPTION_REGbits.PS1=0;
        OPTION_REGbits.PS0=1;

        //
        INTCONbits.TMR0IE=0;
        OPTION_REGbits.T0CS=0;
        OPTION_REGbits.T0SE=0;

    //TMR1
        //PRESCALER
        T1CONbits.T1CKPS1=0; //00 for 1:1
        T1CONbits.T1CKPS0=0;

        T1CONbits.T1OSCEN=1;
        T1CONbits.TMR1ON=1;
        T1CONbits.TMR1CS=0;

        PIE1bits.TMR1IE=0;
        
/* The Nokia display*/    
    dis.word=display_initialization;
    ste2007_display_driver(dis);
    
    dis.word=clear_all;
    //ste2007_display_driver(dis);
    
    dis.word=permanent_caption_display;
    ste2007_display_driver(dis);//Отображение постоянных
    
/* The End the Nokia display*/  
        
    //CCP1    
    CCP1CON=0b00000101; //0101 capture mode, every rising edge
    PIE1bits.CCP1IE=1;

    //other thins
    INTCONbits.GIE=1;
    INTCONbits.PEIE=1;
    
/*a meander generator*/
    #define USER_delay 1000
    unsigned long delay=0;

    while(1)
    {
        PORTCbits.RC5=1;
        delay=USER_delay;
        while(--delay);

        PORTCbits.RC5=0;
        delay=USER_delay;
        while(--delay);

/*a period computing if it is needed*/
       
    if(!w1.is_computing_flag)
    {
        w1.is_computing_flag=1;
        TTTT=T_computing(&w1); //us
        
        if(w1.N_number==0)
        {
            w1.N_number=0;
            
            /*speed computing*/
            //V=S/T
            #define WHEEL_CIRCUMFERENCE 1.413 //meters

            V=(WHEEL_CIRCUMFERENCE/TTTT)*1000000; //*1000000 us->seconds //m/s
            V*=3.6; //km/h
            dis.speed=(int)V;
            
            dis.word=speed_displaying;
            ste2007_display_driver(dis);        
            
        }else {w1.N_number++;}
    }

    }

    return 0;
}

void interrupt something(void)
{
/*TMR0*/
if(INTCONbits.TMR0IF && INTCONbits.TMR0IE)
{    
    PORTCbits.RC3=0;
    INTCONbits.TMR0IF=0;
    INTCONbits.TMR0IE=0;
    PIR1bits.CCP1IF=0;
    PIE1bits.CCP1IE=1; //enable CCP1 interrupts
}   

/*CCP1*/
if(PIR1bits.CCP1IF && PIE1bits.CCP1IE)
{    
    //TMR0
    PORTCbits.RC3=1;
    INTCONbits.TMR0IF=0;
    TMR0=12;
    INTCONbits.TMR0IE=1; //enable TMR0 interrupts
    
    //CCP1
    PORTCbits.RC4=!PORTCbits.RC4;
    PIR1bits.CCP1IF=0;
    PIE1bits.CCP1IE=0; //it is a dead zone //disable CCP1 interrupts    
    
    //w1
//####for commenting start 1
    if(((TMR1_overflow_counter==2) && (CCPR1>40000)) // если не двойное срабатывание геркона 
            || (TMR1_overflow_counter>2))           // и если показывает скорость примерно 150 км/ч и выше
        {
//####for commenting end 1
            w1.TMR1_overflow_counter=TMR1_overflow_counter;
            w1.TMR1_last_value=CCPR1;
            w1.is_computing_flag=0;

            //TMR1
            TMR1=0;
            CCPR1=0;
            PIR1bits.TMR1IF=0;
            PIE1bits.TMR1IE=1;
            TMR1_overflow_counter=0;
        
//####for commenting start 1    
        }
    PIE1bits.TMR1IE=1;
//####for commenting end 1
}

/*TMR1*/
if(PIR1bits.TMR1IF && PIE1bits.TMR1IE)
{
    PORTCbits.RC7=!PORTCbits.RC7;
    PIR1bits.TMR1IF=0;
    ++TMR1_overflow_counter;
    
    
//####for commenting start 2
    if(TMR1_overflow_counter>=390 && (w1.is_computing_flag==1))//if speed < 1 km/h
    {
        PIR1bits.TMR1IF=0;
        PIE1bits.TMR1IE=0; //disable the TMR1 interruption
        TMR1=0;
        CCPR1=0;
        TMR1_overflow_counter=0;//
        w1.TMR1_overflow_counter=0; //reset speed on a display
        w1.TMR1_last_value=0;
        w1.is_computing_flag=0;
    }
//####for commenting end 2
}

}

unsigned long T_computing(struct wheel * w0)
{
    unsigned long T=0;
    T=(unsigned long)(w0->TMR1_overflow_counter)*13107+(w0->TMR1_last_value/5);//
    return T;
}










































