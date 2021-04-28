/*
 * main.c
 *
 *  Created on: Jul 1, 2020
 *      Author  : Heba Ramadan Taha
 */
#include "../Lib/Std_types.h"
#include "../Lib/Bit_math.h"
#undef F_CPU
#define F_CPU 8000000
#include <util/delay.h>
#include "Uart_int.h"
#include "Eeprom_int.h"


#define PASS_FLAG_LOCATION       10      //Location To save Password Flag in EEPROM
#define BLOCK_STATUS_LOCATION    11      //Location To save Block Status Flag in EEPROM

#define ENTER      0x0D      /* ASSCII Of ENTER_KEY IN HEX */

/* At First Clear EEPROM and Push Zero values */


int main(void)
{

	/* Declare Variable To Save The Value Received From UART                */
	u8 App_u8ReceivedData = 0;

	/* Declare Variable To Save The Value Of Password Flag From EEPROM      */
	u8 App_u8PassFlag  = 0;

	/* Declare Variable To Save The Value Of Block Status Flag From EEPROM  */
	u8 App_u8BlockStatusFlag = 0;

	/* Declare Variable To Save The Value Of Create New Password Flag       */
	u8 App_u8CreatePassFlag  = 0;

	/* Declare Variable To Save The Value Of Confirm Password Flag           */
	u8 App_u8ConfirmPassFlag  = 0;

	/* Declare Variable to Save Error In Confirm Password                    */
	u8 App_u8ErrorConfirm = 0;

	/* Declare Variable to Save The Value Of Modifying Password Flag         */
	u8 App_u8ModifyPassFlag = 0;

	/* Declare Variable to Break When End Job Crate Or Modify Password        */
	u8 App_u8BreakFlag = 0;

	/* Declare Variable To Location In EEPRoM                                 */
	u8 App_u8Location  = 0;

	/* Declare Variable to Save Data From EEPROM                              */
	u8 App_u8ReadData  = 0;

	/* Declare Variable to Indicate That Start To Check Block Status Var      */
	u8 App_u8CheckStatus = 0;

	/* Declare Variable to Indicate That Start To Check Block Status Var      */
	u8 App_u8ReleaseSystem = 0;

	/* Declare Variable to Indicate That Change Old Password                   */
	u8 App_u8ChangePass = 0;

	/* Declare Variable to Indicate That Change Old Password and After Release Block */
	u8 App_u8ChangeBlock = 0;

	/* Declare Variable To Save The Value To save The Value of Password Flag before Start Code To Know The First State */
	u8 Pass_Var   = 1;

	/* Declare Variable To Save The Value To save The Value of Block Status Flag before Start Code To Know The First State */
	u8 Block_Var  = 1;


	/* Initialize UART   */
	Uart_vidInit();

	/* Initialize EEPROM */
	Eeprom_vidInit();

	/* Send Data To UART  */
	Uart_vidSendString( "*********Welcome To Password Updater Application**********" );
	_delay_ms(100);

	Uart_vidSendString( "       ____________________________________________      " );
	_delay_ms(100);

	 /* Start New Line */
	 Uart_vidSendByte('\r');
	 Uart_vidSendByte('\n');


    /* Start To Read Flags From EEPROM To Know The First State For Application   */
	Eeprom_u8ReadByte( Device_0 , PASS_FLAG_LOCATION , &App_u8PassFlag);
	_delay_ms(10);

	Eeprom_u8ReadByte( Device_0 , BLOCK_STATUS_LOCATION , &App_u8BlockStatusFlag);
	_delay_ms(10);

	/* Check First State For Flags In EEPROM*/
	if(App_u8BlockStatusFlag == 0)
	{
		/*  */
		Block_Var = 1;
	}

	if(App_u8PassFlag == 1)
	{
		Pass_Var = 1;
	}


	while(1)
	{
		/* Receive Data from UART  */
		App_u8ReceivedData = Uart_u8ReceiveByte();


		/* Start To Read Flags From EEPROM To Know States For Application During Run Time   */

		/* Read The Value Of Password Flag From EEPROM */
		Eeprom_u8ReadByte( Device_0 , PASS_FLAG_LOCATION , &App_u8PassFlag);
		_delay_ms(10);


		/* Read The Value Of Block Status Flag From EEPROM */
		Eeprom_u8ReadByte( Device_0 , BLOCK_STATUS_LOCATION , &App_u8BlockStatusFlag);
		_delay_ms(10);


		/**********************************************************************************/
		/*                     First  Mode To Create New Password                         */
		/**********************************************************************************/

		/* Check Password Flag If It's The First Time To Write Password */
		if( (App_u8PassFlag == 0) && (App_u8CreatePassFlag == 0)  )
		{
			Pass_Var++;
			/* Send Data To UART  */
			Uart_vidSendString(">>>>>>Create New Password Mode");  //Create New Password
			_delay_ms(100);

			 /* Start New Line */
			 Uart_vidSendByte('\r');
			 Uart_vidSendByte('\n');

			/* Raise Flag of Start Creating Password*/
			App_u8CreatePassFlag = 1;

			/* Send Data To UART  */
			Uart_vidSendString("1- Enter Your Password :");        //Enter Your Password
			_delay_ms(100);
		}


		/* Check Start Creating Password Flag */
		if((App_u8CreatePassFlag == 1) && (App_u8ReceivedData != UART_u8NO_RECEIVE))
		{
			/* Send Data Which Received To Display It For User         */
			 Uart_vidSendByte(App_u8ReceivedData);


			 /* Save Received Data In EEPROM Location In Device_0      */
			 Eeprom_u8WiteByte( Device_0 , App_u8Location , App_u8ReceivedData);
			 _delay_ms(10);

			 App_u8Location++;

			 /* Check Location 0 , 1 , 2 , 3 */

			 if((App_u8ReceivedData == ENTER))   //That Mean We Receive Password From 4 Digit
			 {

				 /*  Raise Password Flag  By Writing 1 In EEPROM */
				 Eeprom_u8WiteByte( Device_0 , PASS_FLAG_LOCATION , 1);
				 _delay_ms(10);


				 /* Start New Line */
				 Uart_vidSendByte('\r');
				 Uart_vidSendByte('\n');

				 /* Start New Line */
				 Uart_vidSendByte('\r');
				 Uart_vidSendByte('\n');

				 /* Send Data To UART             */
				 Uart_vidSendString( "2- Confirm Your Password :" );
				 _delay_ms(100);

				 /*  Raise Confirm Password Flag   */
				 App_u8ConfirmPassFlag = 1;

				 App_u8CreatePassFlag  = 0;

				 /* Clear Location Variable         */
				 App_u8Location = 0;
			 }
		}

		/* Confirm Password */
		else if((App_u8PassFlag == 1 ) && (App_u8ConfirmPassFlag == 1) && (App_u8ReceivedData != UART_u8NO_RECEIVE))
		{

			/* Send Data Which Received To Display It For User      */
			 Uart_vidSendByte(App_u8ReceivedData);


			 /* Save Read Data From EEPROM Location In Device_0      */
			 Eeprom_u8ReadByte( Device_0 , App_u8Location , &App_u8ReadData);
			 _delay_ms(10);

			 /* Compare Received Data With EEPROM  Data */
			 if(App_u8ReceivedData != App_u8ReadData)
			 {
				 App_u8ErrorConfirm = 1;
			 }
			 App_u8Location++;



			 /* Check Received Data Is Enter */
			 if(App_u8ReceivedData == ENTER)
			 {
				 /* Start New Line */
				 Uart_vidSendByte('\r');
				 Uart_vidSendByte('\n');

				 /* Start New Line */
				 Uart_vidSendByte('\r');
				 Uart_vidSendByte('\n');

				 /* CHeck Error Confirm Flag */
				 if(App_u8ErrorConfirm == 1)
				 {
					 /* Send Data To UART */
					 Uart_vidSendString( "xxxxxxNot Same Password -_- -_-" );
				     _delay_ms(100);

					 /* Start New Line */
					 Uart_vidSendByte('\r');
					 Uart_vidSendByte('\n');

					 /* Send Data To UART */
					 Uart_vidSendString( ">>>Confirm Password Again :" );
					 _delay_ms(100);

					 /* Clear Error Confirm Flag */
					 App_u8ErrorConfirm = 0;

					 App_u8Location = 0;

				 }
				 else
				 {
					 if( (Pass_Var == 0) || (Block_Var == 0))
					 {
						 Uart_vidSendString( ">>>Password Modified.........." );
						 _delay_ms(100);

						 Uart_vidSendString( "     /*************************************************/     " );
						 _delay_ms(100);
					 }
					 else
					 {

						 Uart_vidSendString( ">>>Password Created.........." );
						 _delay_ms(100);

						 Uart_vidSendString( "     /*************************************************/     " );
						 _delay_ms(100);
					 }

					 /* Start New Line */
					 Uart_vidSendByte('\r');
					 Uart_vidSendByte('\n');


					 App_u8Location = 0;

					 /* Clear Confirm Password Flag     */
					 App_u8ConfirmPassFlag = 0;

					 App_u8ErrorConfirm = 0;

					 /* Now End Job Of Create Password So raise Break Flag */
					 App_u8BreakFlag = 1;
				 }
			 }

		}

		/*  Check Break Flag    */
		if( App_u8BreakFlag == 1 )
		{
			break;
		}




		/**********************************************************************************/
		/*          Second Mode Modify Password OR Release The Blocked System             */
		/**********************************************************************************/

		/* 	check Password Flag  */
		if( (Pass_Var == 1) && (App_u8ModifyPassFlag == 0))
		{
			Uart_vidSendString(">>>>>>>>> Modify Password Mode");
			_delay_ms(100);

			 /* Start New Line */
			 Uart_vidSendByte('\r');
			 Uart_vidSendByte('\n');

			Uart_vidSendString("1- Enter Valid Password In System :");
			_delay_ms(100);

			/* Raise Modifying Flag */
			App_u8ModifyPassFlag = 1;
			App_u8Location =0;
		}

		/* Check Password Which Entered From User and Compare It With EEPROM Data */
		if( (Pass_Var == 1) && (App_u8ModifyPassFlag == 1) && (App_u8ReceivedData != UART_u8NO_RECEIVE))
		{

			/* Send Data Which Received To Display It For User      */
			 Uart_vidSendByte(App_u8ReceivedData);


			 /* Save Read Data From EEPROM Location In Device_0      */
			 Eeprom_u8ReadByte( Device_0 , App_u8Location , &App_u8ReadData);
			 _delay_ms(10);

			 /* Compare Received Data With EEPROM  Data */
			 if(App_u8ReceivedData != App_u8ReadData)
			 {
				 App_u8ErrorConfirm = 1;
			 }

			 App_u8Location++;

			 /* Check Received Data Is Enter */
			 if(App_u8ReceivedData == ENTER)
			 {
				 /* Start New Line */
				 Uart_vidSendByte('\r');
				 Uart_vidSendByte('\n');

				 /* Start New Line */
				 Uart_vidSendByte('\r');
				 Uart_vidSendByte('\n');

				 /* CHeck Error Confirm Flag */
				 if(App_u8ErrorConfirm == 1)
				 {
					 /* Send Data To UART */
					 Uart_vidSendString( "xxxxxxNOT Valid Password -_- -_- " );
					 _delay_ms(100);

					 /* Start New Line */
					 Uart_vidSendByte('\r');
					 Uart_vidSendByte('\n');

					 /* Send Data To UART */
					 Uart_vidSendString( ">>>Please Try Valid Password Again :" );
					 _delay_ms(100);

					 /* Clear Error Confirm Flag */
					 App_u8ErrorConfirm = 0;

					 App_u8Location = 0;

				 }
				 /* Now You Enter Correct Valid Password */
				 else
				 {
					 Uart_vidSendString( ">>>You Enter Valid Password" );
					 _delay_ms(100);

					 /* */
					 App_u8ModifyPassFlag  = 0;
					 App_u8ErrorConfirm    = 0;
					 Pass_Var = 0;
					 App_u8CheckStatus     = 1;
					 App_u8ChangePass      = 1;


				 }
			 }
		}


		 /* Check Block Status */
		 if( (App_u8BlockStatusFlag == 1) && (App_u8CheckStatus == 1))  //System Blocked
		 {
			 /* Release System For user So Write 0 In Block Location */
			  Eeprom_u8WiteByte( Device_0 , BLOCK_STATUS_LOCATION , 0);

			 /* Start New Line */
			 Uart_vidSendByte('\r');
			 Uart_vidSendByte('\n');
			 /* Send Massage For User   */
			 Uart_vidSendString( ">>>Now System Not Blocked" );
			 _delay_ms(100);

			 /* Start New Line */
			 Uart_vidSendByte('\r');
			 Uart_vidSendByte('\n');

			 /* Send Data To UART */
			 Uart_vidSendString( ">>>Do YOU Want To Modify Password OR Same Password ? [M/S]" );
			 _delay_ms(100);

			 /* Raise Release Flag */
			 App_u8ReleaseSystem = 1;
			 Block_Var++;

		 }

		 /*Ask User To Use The Same Password OR Change   */
		 if( (App_u8BlockStatusFlag == 0) && (App_u8ReleaseSystem == 1) && (App_u8ReceivedData) != (UART_u8NO_RECEIVE))
		 {

			 /* Check Received Data From User */
			 if( (App_u8ReceivedData == 'S'))
			 {
				 /* Send Data Which Received To Display It For User      */
				 Uart_vidSendByte(App_u8ReceivedData);

				 /* Start New Line */
				 Uart_vidSendByte('\r');
				 Uart_vidSendByte('\n');

				 /* Start New Line */
				 Uart_vidSendByte('\r');
				 Uart_vidSendByte('\n');

				 /* Send Massage For User   */
				 Uart_vidSendString( ">>>Now System Released With Same Password............." );
				 _delay_ms(100);

				 Uart_vidSendString( "     /*************************************************/     " );
				 _delay_ms(100);

				 /* Raise Break Flag*/
				 App_u8BreakFlag  = 1;

				 /* Clear Change password Flag*/
				 App_u8ChangePass = 0;

			 }
			 else if( App_u8ReceivedData == 'M')
			 {
				 /* Send Data Which Received To Display It For User      */
				 Uart_vidSendByte(App_u8ReceivedData);

				 /* Start New Line */
				 Uart_vidSendByte('\r');
				 Uart_vidSendByte('\n');

				 /* Start New Line */
				 Uart_vidSendByte('\r');
				 Uart_vidSendByte('\n');

				 App_u8ChangeBlock = 1;

			 }

			 /* Clear Release Flag */
			 App_u8ReleaseSystem = 0;
		 }


		 /* Check To  Change Password  */
		 if( ((Block_Var == 1) && (App_u8ChangePass == 1)) || (App_u8ChangeBlock == 1) )
		 {

				/* Send Data To UART  */
				Uart_vidSendString(">>>Modify Password");  //Enter Your Password
				_delay_ms(100);

				 /* Start New Line */
				 Uart_vidSendByte('\r');
				 Uart_vidSendByte('\n');

				/* Send Data To UART  */
				Uart_vidSendString("1- Enter Your Password :");  //Enter Your Password
				_delay_ms(100);


				Eeprom_u8ClearDevice(Device_0 , 7);
				/* Raise Flag Of Change Old Password*/
				App_u8CreatePassFlag = 1;
				App_u8ChangeBlock    = 0;
				App_u8ChangePass     = 0;
				App_u8Location = 0;
		 }

		/*  Check Break Flag    */
		if( App_u8BreakFlag == 1 )
		{
			break;
		}

		_delay_ms(200);
	}
	return 0;
}
