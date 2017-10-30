#include "test.h"
#include "bsp_esp8266.h"
#include "bsp_SysTick.h"
#include <stdio.h>  
#include <string.h>  
#include <stdbool.h>
#include "bsp_dht11.h"
#include "./ds18b20/bsp_ds18b20.h"
#include "bsp_led.h" 
#include "./Beep/bsp_beep.h"  

volatile uint8_t ucTcpClosedFlag = 0;
uint8_t uc, ucDs18b20Id [ 8 ];

/**
  * @brief  ESP8266 （Sta Tcp Client）透传
  * @param  无
  * @retval 无
  */
void ESP8266_StaTcpClient_UnvarnishTest ( void )
{
	uint8_t ucStatus;
	
	char cStr [ 100 ] = { 0 };
	char cStr1 [ 200 ] = { 0 };
	char cStr2 [ 200 ] = { 0 };
	
	printf("\r\n this is a ds18b20 test demo \r\n");
	
	while( DS18B20_Init() )	
		printf("\r\n no ds18b20 exit \r\n");
	
	printf("\r\n ds18b20 exit \r\n");
	
	DS18B20_ReadId ( ucDs18b20Id  );           // 读取 DS18B20 的序列号
	
	printf("\r\nDS18B20的序列号是： 0x");

	for ( uc = 0; uc < 8; uc ++ )             // 打印 DS18B20 的序列号
	 printf ( "%.2x", ucDs18b20Id [ uc ] );
	
  printf ( "\r\n正在配置 ESP8266 ......\r\n" );

	macESP8266_CH_ENABLE();
	
	ESP8266_AT_Test ();
	
	ESP8266_Net_Mode_Choose ( STA );

  while ( ! ESP8266_JoinAP ( macUser_ESP8266_ApSsid, macUser_ESP8266_ApPwd ) );	
	
	ESP8266_Enable_MultipleId ( DISABLE );
	
	while ( !	ESP8266_Link_Server ( enumTCP, macUser_ESP8266_TcpServer_IP, macUser_ESP8266_TcpServer_Port, Single_ID_0 ) );
	
	while ( ! ESP8266_UnvarnishSend () );
	
	printf ( "\r\n配置 ESP8266 完毕\r\n" );
	
	
	while ( 1 )
	{		
		
		if(DS18B20_GetTemp_MatchRom ( ucDs18b20Id ))
		{
			sprintf ( cStr, "%.1f", DS18B20_GetTemp_MatchRom ( ucDs18b20Id ));
		}
		else
			sprintf ( cStr, "Read DHT11 ERROR!\r\n" );

		printf ( "\r\n获取该序列号器件的温度： " );  
		printf ( "%s\n", cStr );                                             //打印读取 DS18B20 温湿度信息

		sprintf ( cStr1, "{\"method\": \"update\",\"gatewayNo\": \"01\",\"userkey\": \"47f49923403b4b18aaf8836d5d54dc65\"}&^!" );
		sprintf ( cStr2, "{\"method\":\"upload\",\"data\":[{\"Name\":\"temperature\",\"Value\":\"%s\"}]}&^!", cStr );

		ESP8266_SendString ( ENABLE, cStr1, 84, Single_ID_0 );               //发送 DS18B20 温湿度信息到网络调试助手
		Delay_ms ( 2000 );
		ESP8266_SendString ( ENABLE, cStr2, 70, Single_ID_0 );               //发送 DS18B20 温湿度信息到网络调试助手
		Delay_ms ( 2000 );
		
		if ( ucTcpClosedFlag )                                             //检测是否失去连接
		{
			ESP8266_ExitUnvarnishSend ();                                    //退出透传模式
			
			do ucStatus = ESP8266_Get_LinkStatus ();                         //获取连接状态
			while ( ! ucStatus );
			
			if ( ucStatus == 4 )                                             //确认失去连接后重连
			{
				printf ( "\r\n正在重连热点和服务器 ......\r\n" );
				
				while ( ! ESP8266_JoinAP ( macUser_ESP8266_ApSsid, macUser_ESP8266_ApPwd ) );
				
				while ( !	ESP8266_Link_Server ( enumTCP, macUser_ESP8266_TcpServer_IP, macUser_ESP8266_TcpServer_Port, Single_ID_0 ) );
				
				printf ( "\r\n重连热点和服务器成功\r\n" );

			}
			
			while ( ! ESP8266_UnvarnishSend () );		
			
		}

	}
	
		
}


