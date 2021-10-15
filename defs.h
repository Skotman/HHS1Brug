#ifndef DEFS_H_INCLUDED
#define DEFS_H_INCLUDED

// LCD
#define LCD_Data_Dir DDRK		/* Define LCD data port direction */
#define LCD_Command_Dir DDRC		/* Define LCD command port direction register */
#define LCD_Data_Port PORTK		/* Define LCD data port */
#define LCD_Command_Port PORTC		/* Define LCD data port */
#define RS PC0				/* Define Register Select (data/command reg.)pin */
#define RW PC1				/* Define Read/Write signal pin */
#define EN PC2				/* Define Enable signal pin */
#define NOODSTOP PINB & (1<<5) //11

//Motor
#define SENSOR_ZIJKANT_1   PINH & (1<<4)
#define SENSOR_ZIJKANT_2   PINH & (1<<5)
#define SENSOR_ONDER_BRUG  PINH & (1<<3)
#define ENDSTOPTOP PINL & (1<<2)
#define ENDSTOPBOT PINL & (1<<3)

//Slagboom
//---

//AnemoMeter
#define MaxWindSnelheid 69

//Ledjes
// led zijkant brug
#define LedRood1 PINA & (1<<0)
#define LedRood2 PINA & (1<<1)
#define LedGroen1 PINA & (1<<2)
#define LedGroen2 PINA & (1<<3)
#define LedGeel PINA & (1<<4)
#define LedRoodBG PINA & (1<<5)

//stoplicht
#define Groenlicht PING & (1<<2)
#define Geellicht PING & (1<<0)
#define Roodlicht PING & (1<<1)

//Geluid
#define AudioPort PB0

#endif // DEFS_H_INCLUDED
