/*
   LCD16x2 8 bit AVR ATmega16 interface
   http://www.electronicwings.com
*/



#include <avr/io.h>			    /* Include AVR std. library file */
#include <util/delay.h>			/* Include inbuilt defined Delay header file */
#include "defs.h"               /* Include #define */
#include "h_bridge.h"           /* Include h_bridge header */
#include "servo.h"              /* Include servo header */



//===========================
//Ledjes
//===========================
//Stoplicht
void GroenLichtVerkeerAAN(/*Groenlicht*/)
{
    PORTC |= (1<<3);

}
void GroenVerkeerUIT()
{
    PORTC &= ~(1<<3);
}
void GeelLichtVerkeerAAN(/*Geellicht*/)
{
    PORTG |= (1<<1);
}
void GeelLichtVerkeerUIT()
{
    PORTG &= ~(1<<1);
}
void RoodLichtVerkeerAAN(/*Roodlicht*/)
{
    PORTG |= (1<<0);
}
void RoodLichtVerkeerUIT()
{
    PORTG &= ~(1<<0);
}
//BOOTLICHTEN
void RoodKant1aan(/*LedRood1*/)
{
    PORTA |= (1<<0);
}
void RoodKant1UIT()
{
    PORTA &= ~(1<<0);
}
void RoodKant2AAN(/*LedRood2*/)
{
    PORTA |= (1<<1);
}
void RoodKant2UIT()
{
    PORTA &= ~(1<<1);
}
void GroenKant1AAN(/*LedGroen1*/)
{
    PORTA |= (1<<2);
}
void GroenKant1UIT()
{
    PORTA &= ~(1<<2);
}

void GroenKant2AAN(/*LedGroen2*/)
{
    PORTA |= (1<<3);
}
void GroenKant2UIT()
{
    PORTA &= ~(1<<3);
}
void GeelZijkantBrugAAN(/*LedGeel*/)
{
    PORTA |= (1<<4);
}
void GeelZijkantBrugUIT()
{
    PORTA &= ~(1<<4);
}
void BuitenGebruikLedAAN(/*LedRoodBG*/)
{
    PORTA |= (1<<5);
}
void BuitenGebruikUIT()
{
    PORTA &= ~(1<<5);
}

//==================================
// LCD functies
//===================================
void buzzerBrug(int buzzer_lengte_ms, int puls_lengte_ms, int buzzer_frequentie_ms)
{

    int verstreken_tijd_puls_ms = 0;
    int verstreken_tijd_totaal_ms = 0;
    //buzzer_lengte_ms = hoe lang de buzzer blijft buzzen voor hij helemaal stopt (in ms).
    //puls_lengte_ms = lengte van een puls van de buzzer (in ms).
    //buzzer_frequentie_ms = frequentie van de buzzer(in ms).
    while(verstreken_tijd_puls_ms < puls_lengte_ms)
    {
        PORTB |= (1<<0);
        _delay_ms(buzzer_frequentie_ms);
        PORTB &= ~(1<<0);
        _delay_ms(buzzer_frequentie_ms);

        //puls timer
        verstreken_tijd_puls_ms = verstreken_tijd_puls_ms + (2 * buzzer_frequentie_ms);

        //wacht een pulslengte en zet daarna de timer weer op 0
        if(verstreken_tijd_puls_ms >= puls_lengte_ms)
        {
            _delay_ms(puls_lengte_ms);
            verstreken_tijd_puls_ms = 0;
        }

        //houd bij hoe lang de buzzer al aan staat
        verstreken_tijd_totaal_ms = verstreken_tijd_totaal_ms + (4 * buzzer_frequentie_ms);

        //zet de buzzer uit als de tijd voorbij is
        if(verstreken_tijd_totaal_ms >= buzzer_lengte_ms)
        {
            return;
        }
    }
}
void initBuzz()
{
    DDRB |= _BV(AudioPort);
}


void LCD_Command(unsigned char cmnd)
{
    LCD_Data_Port= cmnd;
    LCD_Command_Port &= ~(1<<RS);	/* RS=0 command reg. */
    LCD_Command_Port &= ~(1<<RW);	/* RW=0 Write operation */
    LCD_Command_Port |= (1<<EN);	/* Enable pulse */
    _delay_us(1);
    LCD_Command_Port &= ~(1<<EN);
    _delay_ms(3);
}

void LCD_Char (unsigned char char_data)	/* LCD data write function */
{
    LCD_Data_Port= char_data;
    LCD_Command_Port |= (1<<RS);	/* RS=1 Data reg. */
    LCD_Command_Port &= ~(1<<RW);	/* RW=0 write operation */
    LCD_Command_Port |= (1<<EN);	/* Enable Pulse */
    _delay_us(1);
    LCD_Command_Port &= ~(1<<EN);
    _delay_ms(1);
}

void LCD_Init (void)			/* LCD Initialize function */
{
    LCD_Command_Dir = 0xFF;		/* Make LCD command port direction as o/p */
    LCD_Data_Dir = 0xFF;		/* Make LCD data port direction as o/p */
    _delay_ms(20);			/* LCD Power ON delay always >15ms */

    LCD_Command (0x38);		/* Initialization of 16X2 LCD in 8bit mode */
    LCD_Command (0x0C);		/* Display ON Cursor off */
    LCD_Command (0x06);		/* Auto Increment cursor */
    LCD_Command (0x01);		/* Clear display */
    LCD_Command (0x80);		/* Cursor at home position */
}

void LCD_String (char *str)		/* Send string to LCD function */
{
    int i;
    for(i=0; str[i]!=0; i++)		/* Send each char of string till the NULL */
    {
        LCD_Char (str[i]);
    }
}

void LCD_Clear()
{
    LCD_Command (0x01);		/* clear display */
    LCD_Command (0x80);		/* cursor at home position */
}

int menuScroler(char Menu[][40], int sizeText)
{
    int cycle_knop;
    int select_knop;
    int counter = 0;
    int secLineCounter = counter + 1;
    LCD_Clear();
    _delay_ms(200);
    LCD_String(Menu[0]);
    LCD_Command(0x80);                      //begin bij de eerst rij
    LCD_String(">");                        //zet pijltje neer
    LCD_Command(0xc0);                      //begin bij de tweede rij
    LCD_String(Menu[secLineCounter]);
    while(NOODSTOP)
    {
        cycle_knop = PINB & (1<<7);                 //test of de knop ingedurkt is
        select_knop = PINB & (1<<6);                //test of de knop ingedurkt is
        if(cycle_knop)
        {

            if(counter>=(sizeText-1))
            {
                counter = 0;
            }
            else
            {
                counter++;
            }
            LCD_Clear();                            //haal alle text van de lcd af

            LCD_Command(0x80);                      //begin bij de eerst rij
            _delay_ms(200);                         //wacht een halve sec
            LCD_String(Menu[counter]);
            LCD_Command(0x80);                      //begin bij de eerst rij
            LCD_String(">");                        //zet pijtje neer
            LCD_Command(0xc0);                      //begin bij de tweede rij
            if(secLineCounter>=(sizeText-1))
            {
                secLineCounter = 0;
            }
            else
            {
                secLineCounter++;
            }
            LCD_String(Menu[secLineCounter]);
        }
        if(select_knop)
        {
            return counter;
        }
        if(NOODSTOP == 0)
        {
            return 1000;
        }
    }
    LCD_Clear();
    return 1000;
}

void buitenBedrijf()
{
    char Menu[][40] =
    {
        " brug",        //0
        " slagbomen",   //1
        " lichten",     //2
        " geluid",       //3
        " Feest"
    };
    char menuBrug[][40] =
    {
        " Brug open",        //0
        " Brug dicht",       //1
        " Terug"             //2
    };
    char menuSlagbomen[][40] =
    {
        " Slagbomen open",        //0
        " Slagbomen dicht",       //1
        " het is feest!",         //2
        " Terug"                  //3
    };
    char menuLichten[][40] =
    {
        " wegverkeer",        //0
        " Bootverkeer",       //1
        " Terug"              //2
    };
    char menuWegverkeer[][40] =
    {
        " Rood",        //0
        " Oranje",      //1
        " Groen",       //2
        " terug"        //3
    };
    char menuBootverkeer[][40]=
    {
        " Buitenbedrijf",//0
        " Kant1Rood", //1
        " Kant1Groen", //2
        " Kant2Rood", //3
        " Kant2Groen", //4
        " VaarLichtOnder", //5
        " terug" //6
    };
    char menuGeluid[][40] =
    {
        " Aan",                      //0
        " Uit",                      //1
        " Warn. 3 sec",       //2
        " terug"                     //3
    };
    char MenuFeest[][40] =
    {
        " Doe eens Dansje",        //0
        " Lampen Uit",   //1
        " terug"
    };


    LCD_Command(0x80);                          //begin bij de eerste rij
    int sizeText = sizeof(Menu)/sizeof(Menu[0]);    //weet hoelang de array is
    int selectMenu;
    while(NOODSTOP)
    {
        int i;
        switch(menuScroler(Menu, sizeText))
        {
        case 0:
            //brug
            sizeText = sizeof(menuBrug)/sizeof(menuBrug[0]);        //hoelang is de array
            //selectMenu;
            switch(menuScroler(menuBrug, sizeText))                                      //wat moet er worden uitgevoerd
            {
            case 0:
                motorNOOD(1);
                break;                                               //brug open

            case 1:
                motorNOOD(0);
                break;                                              //brug dicht

            case 2:
                break;
            }
            break;
        case 1:
            sizeText = sizeof(menuSlagbomen)/sizeof(menuSlagbomen[0]);        //hoelang is de array
            switch(menuScroler(menuSlagbomen, sizeText))                                      //wat moet er worden uitgevoerd
            {
            case 0:
                servo1_set_percentage(3);                                                 //slagbomen open
                break;

            case 1:
                servo1_set_percentage(-98);                                                 //slagbomen dicht
                break;

            case 2:
                //int i=0;
                for(i=0; i<25; i++)         //Disco
                {
                    if(i%2==0)
                    {
                        servo1_set_percentage(-98);
                        _delay_ms(250);
                    }
                    else
                    {
                        servo1_set_percentage(100);
                        _delay_ms(250);
                    }
                }
                break;
            case 3:
                break;
            }
            break;
        case 2:
            sizeText = sizeof(menuLichten)/sizeof(menuLichten[0]);      //hoelang is de array
            switch(menuScroler(menuLichten, sizeText))                  //wat moet er worden uitgevoerd
            {
            case 0:                                                           //Wegverkeer
                sizeText = sizeof(menuWegverkeer)/sizeof(menuWegverkeer[0]);  //hoelang is de array
                switch(menuScroler(menuWegverkeer, sizeText))
                {
                case 0:
                    RoodLichtVerkeerAAN();            //rood
                    break;
                case 1:
                    GeelLichtVerkeerAAN();           //oranje
                    break;
                case 2:
                    GroenLichtVerkeerAAN();             //groen
                    break;
                case 3:             //terug
                    break;
                }

                break;

            case 1:
                sizeText = sizeof(menuBootverkeer)/sizeof(menuBootverkeer[0]);
                switch(menuScroler(menuBootverkeer, sizeText))
                {
                case 0:
                    BuitenGebruikLedAAN();
                    break;
                case 1:
                    RoodKant1aan();
                    break;
                case 2:
                    GroenKant1AAN();
                    break;
                case 3:
                    RoodKant2AAN();
                    break;
                case 4:
                    GroenKant2AAN();
                    break;
                case 5:
                    GeelZijkantBrugAAN();
                    break;
                case 6:
                    break;
                }


                /* case 1:                                                     //bootverkeer
                    break;

                case 2:
                    sizeText = sizeof(menuBootverkeer)/sizeof(menuBootverkeer[0]);
                    selectMenu = menuScroler(menuBootverkeer, sizeText);
                    break; */
            }
            break;
        case 3:
            sizeText = sizeof(menuGeluid)/sizeof(menuGeluid[0]);        //hoelang is de array
            switch(menuScroler(menuGeluid, sizeText))                   //wat moet er worden uitgevoerd
            {
            case 0:
                LCD_Buzzer_test_aan();                                                //geluid aan
                break;

            case 1:                                                 //geluid uit
                LCD_Buzzer_test_uit();
                break;

            case 2:
                LCD_Buzzer_Feest_aan();                                                 //waarschuwing pulse van 3 sec
                break;
            case 3:                                                 //terug
                break;
            }
            break;
        }

    }
    return;

}


void status(int actie)
{
    switch(actie)
    {
    case 0:
        LCD_Clear();
        LCD_Command(0X80);
        LCD_String("   brug staat");
        LCD_Command(0xc0);
        LCD_String("     omlaag");
        break;
    case 1:
        LCD_Clear();
        LCD_Command(0X80);
        LCD_String("   brug gaat");
        LCD_Command(0xc0);
        LCD_String("     omhoog");
        break;
    case 2:
        LCD_Clear();
        LCD_Command(0X80);
        LCD_String("   brug staat");
        LCD_Command(0xc0);
        LCD_String("     omhoog");
        break;
    case 3:
        LCD_Clear();
        LCD_Command(0X80);
        LCD_String("   brug gaat");
        LCD_Command(0xc0);
        LCD_String("     omlaag");
        break;
    case 10:
        LCD_Clear();
        LCD_Command(0X80);
        LCD_String("het waait te hard");
        LCD_Command(0xc0);
        LCD_String("brug buiten bedrijf");
        break;

    }
}

//==================================
// Motor & Detectie Functies
//=================================


void motor(int stand)   /* motor functie om aan te geven of de motor aan moet gaan of niet */
{
    while(stand == 1)   /* Wanneer de functie een 1 ontvangt wordt dit geloopt: */
    {
        if(ENDSTOPTOP)  /* Checkt eerst of de microswitch aan de bovenzijde is ingedrukt of niet */
        {
            h_bridge_set_percentage(60);   /* Als hij niet wordt ingedrukt; gaat de brug op volle snelheid open */
            noodstop();
        }
        else            /* Mocht het knopje ingedrukt zijn, dan dit: */
        {
            h_bridge_set_percentage(0);     /* blijft de motor stil staan */
            noodstop();
            _delay_ms(10);
            return;
        }


    }
    while(stand == 0)   /* Wanneer de functie een 0 ontvangt, is dit het signaal om dicht te gaan */
    {
        if(NOODSTOP)
        {
            h_bridge_set_percentage(0);
            return;
        }
        if(ENDSTOPBOT)  /* Checkt of de microswitch aan de onderzijde wordt ingedrukt */
        {
            h_bridge_set_percentage(0); /*Wanneer de switch wordt ingedrukt, gaat hij niet dicht dus motor uit */
            noodstop();
            _delay_ms(10);
            return;
        }
        else            /* Wanneer hij dus niet wordt ingedrukt: */
        {
            h_bridge_set_percentage(-60); /* Dan gaat hij dicht */
            noodstop();
        }
    }
}

void motorNOOD(int stand)
{
    while(stand == 1)   /* Wanneer de functie een 1 ontvangt wordt dit geloopt: */
    {
        if(ENDSTOPTOP)  /* Checkt eerst of de microswitch aan de bovenzijde is ingedrukt of niet */
        {
            h_bridge_set_percentage(60);   /* Als hij niet wordt ingedrukt; gaat de brug op volle snelheid open */
            //noodstop();
        }
        else            /* Mocht het knopje ingedrukt zijn, dan dit: */
        {
            h_bridge_set_percentage(0);     /* blijft de motor stil staan */
            //noodstop();
            _delay_ms(10);
            return;
        }


    }
    while(stand == 0)   /* Wanneer de functie een 0 ontvangt, is dit het signaal om dicht te gaan */
    {
        if(ENDSTOPBOT)  /* Checkt of de microswitch aan de onderzijde wordt ingedrukt */
        {
            h_bridge_set_percentage(0); /*Wanneer de switch wordt ingedrukt, gaat hij niet dicht dus motor uit */
            //noodstop();
            _delay_ms(10);
            return;
        }
        else            /* Wanneer hij dus niet wordt ingedrukt: */
        {
            h_bridge_set_percentage(-60); /* Dan gaat hij dicht */
            //noodstop();
        }
    }
}

int detectie()          /* Checkt de inputs van de sensor en geeft aan welke */
{
    if (!(SENSOR_ZIJKANT_1))
    {
        return 1;
    }
    else if (!(SENSOR_ZIJKANT_2))
    {
        return 2;
    }
    else if (!(SENSOR_ONDER_BRUG))
    {
        return 3;
    }
    else
    {
        return 4;
    }
}

void brugopen(int kant)
{
    if(kant == 10)
    {
        motor(0);
        return;
    }
    else
    {
        status(1);
        GroenVerkeerUIT();
        GeelLichtVerkeerAAN();
        buzzerBrug(5000, 500,10);
        noodstop();
        servo1_set_percentage(-98);
        GeelLichtVerkeerUIT();
        RoodLichtVerkeerAAN();
        _delay_ms(1000);
        int i;
        for(i = 1; i <= 2; i++)
        {
            if(kant == 1)
            {
                GroenKant1AAN();
                GroenKant2UIT();
            }
            else if(kant == 2)
            {
                GroenKant2AAN();
                GroenKant1UIT();
            }
            motor(1);
            status(2);
            if(kant == 1)
            {
                RoodKant1UIT();
                RoodKant2AAN();
                kant = 2;
            }
            else if(kant == 2)
            {
                RoodKant2UIT();
                RoodKant1aan();
                kant = 1;
            }
            _delay_ms(5000);
        }

    }


}


void brugdetectie()     /* Detecteert of de brug open moet of dicht moet */
{
    int counter2 = 0;
    if(detectie()==1)   // De sensor aan kant 1 wordt geblokkeerd
    {
        counter2 = 0;
        brugopen(1);
    }
    else if (detectie()==2) // De sensor aan kant 2 wordt geblokkeerd
    {
        counter2 = 0;
        brugopen(2);
        return;
    }
    else if(detectie()==3)// De sensor aan de onderzijde wordt geblokkeerd
    {
        counter2 = 0;
        return;
    }
    while(detectie() == 4)              // Wanneer geen ir-sensor getriggerd wordt
    {
        counter2++;                      // Counter om te kijken of er niet een boot snel er aan komt
        if(counter2 >= 5)               // wanneer het lang genoeg heeft geduurt, dan:
        {
            counter2 = 0;
            status(3);
            motor(0);                   // Brug dicht

            //buzzerBrug(5000, 500, 10);
            GroenKant2UIT();
            GroenKant1UIT();
            RoodKant1aan();
            RoodKant2AAN();
            noodstop();
            _delay_ms(500);
            servo1_set_percentage(3); // Slagboom open
            RoodLichtVerkeerUIT();
            GroenLichtVerkeerAAN();
            return;
        }
        else
        {
            _delay_ms(1000);
            buzzerBrug(0,0,10);
        }

    }
}



//===================================
//Slagboom en Servo Functies zijn verwerkt in brugdetectie();
//==================================


//====================================
//AnemoMeter
//====================================

void initAnemo()
{
    ADMUX |= 0b1100000;         // ADMUX neemt de interne voltage van 2.56V als referentiepunt
    ADCSRA |= 0b10000111;       // ADCSRA is enabled en leest met een scaler van 128
}

int WindSnelheid()
{
    int WindMeter = 0;
    int Herhaling; // Declaratie van variabelen

    for(Herhaling = 0; Herhaling < 20; Herhaling++) // For loop van een detectiegemiddelde van 20 herhalingen
    {

        ADCSRA = ADCSRA |= (1<<ADSC);       // Lezing van ADC beginnen

        while(ADCSRA & (1 << ADSC));        // Wachten zo lang ADC bezig is met verwerken
        _delay_ms(100);
        if(ADCH > MaxWindSnelheid)          // Bekijken of de output van ADC hoger is dan de windsnelheid
        {
            WindMeter ++;                   // Counter tellen in de 20 momenten waar de windsnelheid te hoog is/was
        }
    }
    return WindMeter;   // output geven van hoeveelheid momenten dat het te hoog is
}




//===================================
//Geluid
//===================================

void LCD_Buzzer_test_aan()
{
    PORTB |= (1<<0);
}
void LCD_Buzzer_test_uit()
{
    PORTB &= ~(1<<0);
}
void LCD_Buzzer_Feest_aan()
{
    PORTB |= (1<<0);
    _delay_ms(200);
    PORTB &= ~(1<<0);
    _delay_ms(500);
    PORTB |= (1<<0);
    _delay_ms(100);
    PORTB &= ~(1<<0);
    _delay_ms(500);
    PORTB |= (1<<0);
    _delay_ms(200);
    PORTB &= ~(1<<0);
    _delay_ms(500);
    //  PORTB &= ~(1<<0);
}

void noodstop() /* Functie voor noodstop */
{
    if(NOODSTOP)    /* detecteert of de noodstop wordt ingedrukt */
    {
        BuitenGebruikLedAAN();
        h_bridge_set_percentage(0);
        GroenKant1UIT();
        GroenKant2UIT();
        buitenBedrijf();    /* Start de functie van buiten bedrijf */
        LCD_Clear();        /* Verschoont de LCD weergave */
        main();
    }
    else
    {
        BuitenGebruikUIT();
    }



    return;
}


//=========================
//Main
//=========================




int main()
{
    DDRA = 0b11111111;
    DDRG |= 0b00000111;
    DDRC |= (1<<3);
    LCD_Init();			/* Initialize LCD */
    init_servo();       // servo initialiseren (zie servo.c)
    init_h_bridge();    // h-bridge initialiseren (zie h-bridge.c)
    initAnemo();        // functie van anemometer initialiseren
    initBuzz();
    servo1_set_percentage(-98);
    motor(0);
    status(0);      // Beginstatus van LCD doorgeven (moet dit aangepast worden?)
    GeelZijkantBrugAAN();
    GroenLichtVerkeerAAN();
    RoodKant1aan();
    RoodKant2AAN();
    GroenLichtVerkeerAAN();
    _delay_ms(1000);
    servo1_set_percentage(3);
    while(1)
    {
        while(WindSnelheid() > 5)
        {
            BuitenGebruikLedAAN();
            RoodKant1aan();
            RoodKant2AAN();
            status(10);
        }

        noodstop();
        brugdetectie(); // Kijken of de brug open moet of niet
        if(ENDSTOPBOT)
        {
            status(0);
        }
    }
    return 0;
}
