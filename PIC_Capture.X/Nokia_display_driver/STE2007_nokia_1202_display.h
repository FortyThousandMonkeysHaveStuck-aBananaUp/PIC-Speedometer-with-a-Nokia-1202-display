#ifndef STE2007//Защита от повторного включения
#define STE2007

/*
Нужно будет это объявить в файле main.c, сверху:

typedef enum{
display_initialization,
clear_all,
permanent_caption_display,
temperature_displaying
}command_word;

struct display_dc{
command_word word;
char data[2];
unsigned int delay;
};
  
struct display_dc dis;
struct display_dc *st1=&dis;
extern const unsigned char nlcd_Font[256][5];
*/


#ifdef STM_STE2007_nokia_1202_display
//#define STM_STE2007_nokia_1202_display

extern typedef enum{
display_initialization,
clear_all,
permanent_caption_display,
temperature_displaying,
LCD_Full
}command_word;

extern struct display_dc{
command_word word;
char data[2];
unsigned int delay;
};


extern const unsigned char nlcd_Font[256][5];

void ste2007_display_driver(struct display_dc d);
void transmit_command(char command);
void transmit_data(char data);
void Page_and_Column_adress_set(unsigned char Page, unsigned char Column);
void Display_symbol(char char_0);
void output_character_array(char array[]);
void delay_1(void);
void delay_2(void);
#endif


//PIC
#define PIC_STE2007_nokia_1202_display

#ifdef PIC_STE2007_nokia_1202_display
//#define PIC_STE2007_nokia_1202_display

//RB0 Led
//RB1 SCK
//RB2 MOSI
//RB3 CS
//RB4 RESET

typedef enum{
display_initialization,
clear_all,
permanent_caption_display,
temperature_displaying,
LCD_Full,
speed_displaying        
}command_word;

struct display_dc{
command_word word;
char data[2];
unsigned int delay;
int speed;
};

struct display_dc dis;

void ste2007_display_driver(struct display_dc d);
void transmit_command(char command);
void transmit_data(char data);
void Page_and_Column_adress_set(unsigned char Page, unsigned char Column);
void Display_symbol(char char_0);
void output_character_array(char array[]);
void delay_1(void);
void delay_2(void);

#endif




#endif