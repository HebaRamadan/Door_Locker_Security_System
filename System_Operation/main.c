/*
 * main.c
 *
 *  Created on: Jul 5, 2020
 *      Author: Author  : Heba Ramadan Taha
 */
#include "../Lib/Std_types.h"
#include "../Lib/Bit_math.h"
#undef F_CPU
#define F_CPU 8000000
#include <util/delay.h>

#include "Keypad_int.h"
#include "Buzzer_int.h"
#include "Eeprom_int.h"
#include "Servo_int.h"
#include "Led_int.h"
#include "Lcd_int.h"

#define BLOCK_STATUS_LOCATION    11      //Location Which Saved The Value Block Status Flag in EEPROM

int main(void)
{
	/* Declare Variable To Save Value From KeyPad                        */
	u8 App_u8Key             = 0 ;

	/* Declare Variable To Save Value Of Block Status Flag From EEPROM   */
	u8 App_u8BlockStatusFlag = 0;

	/*  Declare Variable To Save Data Which Read From EEPROM             */
	u8 App_u8ReadData        = 0;

	/*  Declare Variable To Save EEPROM Location                         */
	u8 App_u8EepromLocation  = 0;

	/* Declare Variable To Indicate Enter Password Mode                  */
	u8 App_u8EnterPassFlag   = 1;

	/* Declare Variable To Indicate Password Compared With EEPROM        */
	u8 App_ErrorData         = 0;

	/* Declare Variable To Count Number Of Tried To Enter Error Password */
	u8 App_u8ErrorPassCounter = 0;

	/* Declare Variable To Indicate Start Enter Passwor                  */
	u8 App_u8StartSystem     = 0;

	/* Declare Variable To Indicate Start Your System                    */
	u8 App_u8SystemFlag      = 0;



	/* Initialize KeyPad       */
	Keypad_vidInit();

	/* Initialize Buzzer       */
	Buzzer_vidInit();

	/* Initialize EEPROM       */
	Eeprom_vidInit();

	/* Initialize Servo_Motor  */
	Servo_vidInit();

	/* Initialize LCD          */
	Lcd_vidInit();

	/* Initialize LED          */
	Led_vidInit( LED_1 );


	while(1)
	{

		/* Read Block Status Flag From EEPROM   */
		Eeprom_u8ReadByte( Device_0 , BLOCK_STATUS_LOCATION , &App_u8BlockStatusFlag);
		_delay_ms(10);


		/* All Time Ask About Block Status Flag And Make It As Condition To Complete System */
		while( App_u8BlockStatusFlag != 1 )
		{
			/* Get Key From KeyPad  */
			App_u8Key = Keypad_u8GetKey();


			if( App_u8StartSystem == 0 )
			{
				Lcd_vidSendCommand(0x80);
				Lcd_vidWriteStr( "Welcome To Home" );

				Lcd_vidSendCommand(0xc0);

				if( App_u8ErrorPassCounter == 0 )
				{
					Lcd_vidWriteStr( "Password : " );
				}
				else
				{
					Lcd_vidWriteStr( "Try Again : " );
				}

				App_u8EnterPassFlag = 1;
				App_u8StartSystem   = 1;
			}

			/* Check Password Flag To Start Receive Password From KeyPad */
			if( App_u8EnterPassFlag == 1)
			{

				/* Start Check Values From Keypad */
				if( App_u8Key != KEYPAD_u8NO_KEY)
				{
					if( (App_u8Key <= 9)  && (App_u8Key >= 0) )
					{
						/* Display KeyPad Input On LCD */
						Lcd_vidWriteChar( App_u8Key + '0' );

						/* Read Data From EEPROM */
						Eeprom_u8ReadByte( Device_0 , App_u8EepromLocation , &App_u8ReadData);
						_delay_ms(10);

						/* Increment Location In EEPROM For Next Location */
						App_u8EepromLocation++;

						/* Compare Input Password With Saved Password In EEPROM */
						if( (App_u8Key +'0' ) != App_u8ReadData)
						{
							/* Raise Flag That You Enter Not Valid Number */
							App_ErrorData = 1;
						}
						_delay_ms(500);
					}
					else if( App_u8Key == '#' )
					{

						/* Check Error Flag  And Minimum 4_digit Password */
						if( (App_ErrorData == 1) || (App_u8EepromLocation < 4 ) )
						{
							/* Increment Error Password Counter */
							App_u8ErrorPassCounter++;

							/* Check How Many Times User Enter Incorrect Password */
							if(App_u8ErrorPassCounter  == 3)
							{
								/* Raise Block Status Flag By Writing 1 In EEPROM */
								Eeprom_u8WiteByte( Device_0 , BLOCK_STATUS_LOCATION , 1);

								/* Break System */
								break;
							}
							else
							{
								/* Clear LCD  */
								Lcd_vidSendCommand(0x01);
								Lcd_vidWriteStr("Invalid Password");
								_delay_ms(1000);
								Lcd_vidSendCommand(0x01);

								/* Close Enter Password Mode*/
								App_u8EnterPassFlag = 0;
								App_u8StartSystem   = 0;
								App_ErrorData       = 0;
							}
							App_u8EepromLocation = 0;
						}
						else   // Correct Password
						{
							/* Clear LCD  */
							Lcd_vidSendCommand(0x01);
							Lcd_vidWriteStr("Welcome Home......");

							App_u8SystemFlag       = 1;
							App_u8EnterPassFlag    = 0;
							App_u8ErrorPassCounter = 0;
						}
					}
				}
			}

			if( App_u8SystemFlag ==1 )
			{
				Servo_vidSetDegree(90);
				_delay_ms(1000);
				Servo_vidSetDegree(0);

				/* Clear LCD  */
				Lcd_vidSendCommand(0x01);
				/* Clear Flags To Star System Again */
				App_u8SystemFlag       = 0;
				App_u8EnterPassFlag    = 0;
				App_u8StartSystem      = 0;
				App_u8EepromLocation   = 0;

			}
		}

		/* That mean System Blocked And Block_Status_Flag Raised */
		Lcd_vidSendCommand(0x01);
		Lcd_vidWriteStr(" System Blocked");
		Buzzer_vidTurnOn();
		Led_vidTurnOn( LED_1 );

		break;
	}
	return 0;
}
