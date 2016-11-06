#include <Serial_service.h>
#include <LiquidCrystal.h>
#include <LCD_service.h>
#include <Xapi.h>
#include <Subscriptions.h>
#include <Universal.h>
#include <Util.h>
#include <Single_buff.h>

//0013a200
//40a1446d

//***********************************************
//***********************************************
Xapi xapi = Xapi(Serial);
Serial_service serial_service = Serial_service(Serial1, xapi);
LCD_service lcd_service(xapi);

uint8_t msg1[] =   "I FEEL GREAT";
uint8_t msg2[] =   "COMMODORE 64";
uint8_t _clear[] = "                ";


//***********************************************
//***********************************************
void setup()
{
 Serial.begin(MISC_SERIAL_SPEED);
 Serial1.begin(MISC_SERIAL_SPEED);
  
  
  
}

//***********************************************
//***********************************************
void loop()
{
  system_active();
  process_buttons();
  xapi.xapi_latch();
  lcd_service.lcd_service_latch();
  serial_service.serial_service_latch();
  //delay(4000);
}

//***********************************************
//***********************************************

void process_buttons()
{
  // storage for the button
  int button;

  button = lcd_service.get_lcd_key();

  // process packet
  if(button == LCD_btnSELECT)
  {
  
    lcd_service.lcd_snd_local_serial_debug((const uint8_t*)"SELECT");
    //lcd_service.lcd_snd_LOCAL_message(0,0,(const uint8_t*)"VICTORY(C)     ");
    lcd_service.lcd_snd_EXTERNAL_message(ADDR_MSB, 0x40a1446d, ADDR16_BROADCAST, 
                                        0,0,(const uint8_t*)"VICTORY(EN)    ");
  
  
    /*
    lcd_service.lcd_snd_EXTERNAL_message(  DEBUG_MSB_ADDR,
					  DEBUG_LSB_ADDR,
				          DEBUG_ADDR16,
					  0,
					  0,
					  _clear);
    
    
    
   lcd_service.lcd_snd_EXTERNAL_message(  DEBUG_MSB_ADDR,
					  DEBUG_LSB_ADDR,
				          DEBUG_ADDR16,
					  0,
					  0,
					  msg1);
  
  lcd_service.lcd_print(0, 0, (const char*)"first message");
  lcd_service.lcd_print(0,1, (const char*)"                ");
  
*/
  }

  if(button == LCD_btnLEFT )
  {
    
    lcd_service.lcd_snd_LOCAL_message(0,0,(const uint8_t*)"GOOD JERB!!!!!!");
    
    /*
    lcd_service.lcd_snd_EXTERNAL_message(  DEBUG_MSB_ADDR,
					  DEBUG_LSB_ADDR,
				          DEBUG_ADDR16,
					  0,
					  1,
					  _clear);
    
    
    lcd_service.lcd_snd_EXTERNAL_message(  DEBUG_MSB_ADDR,
					  DEBUG_LSB_ADDR,
				          DEBUG_ADDR16,
					  0,
					  1,
					  msg2);

  lcd_service.lcd_print(0, 1, (const char*)"second message");
  lcd_service.lcd_print(0,0, (const char*)"                ");
  
  */
  }
}

//***********************************************
//***********************************************
void system_active()
{
    static uint16_t cnt = 0;
    static uint8_t row = 0;
    
    cnt++;
    
    if ( (cnt%2500) == 0)
   {
     // turn off both stars
     lcd_service.lcd_print(15, 0, (const char*)" ");
     lcd_service.lcd_print(15, 1, (const char*)" ");
     
     row++;
     // turn on new row
     lcd_service.lcd_print(15, row%2, (const char*)"*");
    
   } 
  
}





