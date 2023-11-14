#ifndef SERVERPIC_H
	#define SERVERPIC_H

	//----------------------------------------------
	//Includes
	//----------------------------------------------
	#include <ESP8266WiFi.h>
	#include <WiFiClient.h> 
	#include <ESP8266WebServer.h>
	#include <WiFiUdp.h>
	#include <ArduinoOTA.h>

	
	#include "Global.h"
	#include "Configuracion.h"
	#include "Eprom.h"
	#include "ModoAP.h"
	#include "ModoSTA.h"
	#include "Servidor.h"


    //#include "TouchTermostato.h"



    #define TERMOSTATO_MANUAL_1



	#ifdef TERMOSTATO_MANUAL_1
		#define Placa "TERMOSTATO_DT.0"		//Termostato Display Touch
		#define Modelo "ESP12"
		#define TERMOSTATO_REMOTO  		
		#define Ino "TermostatoDTR1"   
		#define GA940_MPR121     
		#define SHT20	
	#endif




	#ifdef DS18B20 															//Si el sensor es el DS18B20, cargamos su clase
		#include "SensorDS18B20.h"
	#endif

	#ifdef SHT20 															//Si el sensor es el SHT20, cargamos su clase
		#include "SensorSHT20.h"
	#endif


	#ifdef GA940_MPR121
		#include "TouchTermostato.h"
		TOUCHTERMOSTATO Display  ;
		const uint8_t MPR121_ADDR = 0x5A;   // Direccion I2c de MPR121 
	#endif


	//----------------------------
	//CARACTERISTICAS DISPOSITIVO
	//----------------------------
	#define VIno "2.2"						//Version del programa principal
	#define VBuild  "18"
	//---------------------------------
	//CARACTERISTICAS DE LA COMPILACION
	//--------------------------------
	#define Compiler "Stino";				//Compilador
	#define VCore "2.5.2";					//Versión del Core Arduino

	#include "IO.h";


	//----------------------------------------------
	//HOME KIT
	//----------------------------------------------
	#define HomeKit
	//----------------------------------------------
	//Teimpo de Test de conexion
	//----------------------------------------------
	#define TiempoTest	8000												//Tiempo en milisegundos para Test de conexion a servidor
	#define nCiclosArranque 40												//Ciclos de comprobacion de conexion estable para arrancar el dispositivo
	#define nCiclosParada 10												//Ciclos de comprobacion de desconexion estable para parar el dispositivo
	#define nCiclosMedida 5													//Ciclos de Test para realizar una medida

	#ifdef GA940_MPR121
		//---------------------------------------------
		//Tiempo activacion pantalla
		//---------------------------------------------
		#define nTiempoPantallaOn 	25000										//Milisegundo que la pantalla esta encendida cuando esta enb reposo
	#endif

	//----------------------------------------------
	//DEBUG
	//----------------------------------------------
	#define Debug




	//========================
	//Funciones de Dispositivo (UNIVERSALES)
	//========================	
	boolean GetDispositivo (void);
	void DispositivoOn (void);
	void DispositivoOff (void);


    //----------------------------
	//Declaracion de variables UNIVERSALES
	//----------------------------
	ESP8266WebServer server(80);											//Definimos el objeto Servidor para utilizarlo en Modo AP
	WiFiClient Cliente;														//Definimos el objeto Cliente para utilizarlo en Servidor
	Telegrama oMensaje;									 					//Estructura mensaje donde se almacenaran los mensajes recibidos del servidor

	String cSalida;															//Variable donde se deja el estado ( String ) para mandar en mensaje a ServerPic
	boolean lEstado = 0;													//Variable donde se deja el estado del dispositivo para reponer el estado en caso de reconexion
	boolean lConexionPerdida = 0;											//Flag de conexion perdida, se pone a 1 cuando se pierde la conexion. Se utiliza para saber si se ha perdido la conexion para restablecer estado anterior de la perdida

	boolean lHomeKit;													    //Flag para habililtar HomeKit ( 1 Habilitado, 0 deshabilitado )
	boolean lTelegram;														//Flag para habililtar Telegram ( 1 Habilitado, 0 deshabilitado )
	boolean lWebSocket ;  													//Flag para habililtar WebSocket ( 1 Habilitado, 0 deshabilitado )
	boolean lEstadisticas;													//Flag para habilitar Estadisticas ( 1 Habilitado, 0 Deshabilitado )
	boolean lInfoTec;														//Flag para habilitar Informacion tecnica ( 1 Habilitado, 0 Deshabilitado )			
	boolean lPush;																//Flag para habilitar Push ( 1 Habilitado, 0 Deshabilitado )

	long nMiliSegundosTest = 0;												//Variable utilizada para temporizar el Test de conexion del modulo a ServerPic
	int nTemporizadorArranque = 0;											//Variable utilizada para contar los ciclos de TestConexion antes de arrancar la utilización	
	int nTemporizadorParada = 0;											//Variable utilizada para contar los ciclos de TestConexion antes de parar la utilización	
	
	long nMilisegundosRebotes = 0;                							//Variable utilizada para temporizar el tiempo de absorcion de rebotes
	String cDispositivo = String(' ');										//Nombre del dispositivo, utilizado para conocer el nombre de su pareja en homeKit			
	String cPush = String(' ');												//Nombre del cliente push			


	boolean lOnOff = 1;														//Flag que indica si el PIR está habilitado o no												
	boolean lDispositivo = 0;												//Flag que indica que se ha dado la orden al un dispositivo remoto para que actue

	int nTemporizacionMedida = 0;											//Contador de Testconexion para realizar una medida 

    //----------------------------
	//Declaracion de variables Particulares
	//----------------------------
//	#define nMuestrasMedida 3
	#ifdef GA940_MPR121
	    int lFlagInterrupcion = 0;											//Flag que indica si se ha producido interrupcion por teclado puldsado
		double nTiempoPantalla = 0;											//Variable para contorlar el tiempo de pantalla encendida
		boolean lPantalla = 1;												//Pantalla apagada/encendida (0/1)
	 	boolean lPantallaEncendida = 0;										//Flag que indica que se debe reiniciar la pantalla cuando se sale de estado off
	#endif	
	boolean lEstadoCalefaccion = 0;											//Flag que indica si la calefacción esta encendida o apagada ( 1/0 )


	/**
	******************************************************
	* Estructura Datos sensores
	*
	*/
	typedef struct DataSensor {
		boolean TermometroOnOff;										   /**< Sensor habilitado/Deshabillitado */
		float Consigna;													   /**< Consigna del termostato */
    	float UmbralSuperiorTemperatura;   								   /**< Umbral superior temperatura */					
    	float UmbralInferiorTemperatura;   								   /**< Umbral inferior temperatura */	
		boolean lTipoAlarmaTemperatura;									   /**< Tipo de alarma, 1 por Umbral Superior, 0 por Umbral inferior */		
    	int AlarmaUSTemperatura;										   /**< Segundos para activar alarma por debajo de umbral superior */	
    	int AlarmaUITemperatura;										   /**< Segundos para activar alarma por debajo de umbral inferior */	
		float Offset;													   /**< Temperatura offset del sensor */	
		boolean lEnableAlarma;											   /**< Flag de alarma deshabilitada/habilitada (0/1)*/
		boolean lAutomaticoManual;										   /**< Flag Automatico/Manual (0/1) */	
		boolean lOnOff;													   /**< Flag que indica si la calefaccion esta encendida o apagada*/
	};

	DataSensor DatosSensor;													//Definimos la variable DatosSensor 

	//====================================================================
	//Funciones para grabar datos de sensores en EPROM
	//La grabacion en Eprom se ha sustituido por la grabacion  en Servidor
	//por lo que estas funciones han dejado de utilizarse en este modulo
	//====================================================================
   	void GrabaDatosSensores ( DataSensor oDatos);
	DataSensor LeeDatosSensores ( void );

	//===========================================
	//Funciones particulares de display y touch
	//===========================================
    void ICACHE_RAM_ATTR ScanPulsa (void);
 	void DisplayDatosSensor ( float nTemperatura,  DataSensor eDatosSensor );
    void DisplayDatosSensorOff ( DataSensor eDatosSensor );

   	DataSensor ReadDatosSensores ( void );
	String ReadVariable (String cVariable);
	void PrintDatosSensor (DataSensor oDatos );

	void AlarmaOn (void);
	void AlarmaOff (void);
	boolean GetCalefaccion (void);
	void CalefaccionOn (void);
	void CalefaccionOff (void);


   boolean lAlarma = 0;


	//Variables donde se almacenan los datos definidos anteriormente para pasarlos a Serverpic.h
	//para mandar la información del Hardware y Software utilizados
	//En la libreria ServerPic.h estan definidos como datos externos y se utilizan en la funcion
	//Mensaje () para responder al comando generico #Info.
	//Con ese comando, el dispositivo mandara toda esta información al cliente que se lo pida
	// ESTOS DATOS NO SON PARA MODIFICAR POR USUARIO
	//----------------------------
	//Datos del programa principal
	//----------------------------
	String VMain = VIno;
	String Main = Ino; 
	String VModulo = VBuild;
	//----------------------------
	//Datos de compilación
	//----------------------------	
	String Compilador = Compiler;
	String VersionCore = VCore;

	//----------------------------
	//Datos de Hardware
	//----------------------------	
	String ModeloESP = Modelo;
	String Board = Placa;

	//======================================================
	//Funciones de dispositivo
	//======================================================

    /**
    ******************************************************
    * @brief Devuelve el estado del dispositivo
    *
    * @return devuelve <b>1</b> si el dispositivo esta habilitado o <b>0<\b> en caso de que no lo este
    */
    boolean GetDispositivo (void)
    {

    	return ( lOnOff );

    }
    /**
    ******************************************************
    * @brief Habilita el dispositivo
    *
    */
    void DispositivoOn (void)
    {
    	lOnOff=1;
    }
    /**
    ******************************************************
    * @brief Deshabilita el dispositivo
    *
    */
    void DispositivoOff (void)
    {
     	lOnOff=0;
    }



    /**
    ******************************************************
    * @brief  Función que graba en Eprom los datos de los sensores
    *
    * @param oDatos. Estructura de datos DataSensor
    */
    void GrabaDatosSensores ( DataSensor oDatos)
    {
       EEPROM.put(nPosicionDataUsuario, oDatos); //Guardo la config
       EEPROM.commit();
    }
    /**
    ******************************************************
    * @brief  Función que lee de Eprom los datos de los sensores
    *
    * @return Estructura de datos DataSensor
    */
    DataSensor LeeDatosSensores ( void )
    {
       DataSensor oDatos; 
       EEPROM.get(nPosicionDataUsuario, oDatos); //Guardo la config
       return ( oDatos );
    }
    /**
    ******************************************************
    * @brief  Función que lee los datos del sensor de las variables almacenadas en Serverpic
    *
    * @return Estructura de datos DataSensor
    */
    DataSensor ReadDatosSensores ( void )
    {
	   	String cValor;	
       	DataSensor oDatos; 
       	cValor = ReadVariable ("Termometro");
	   	if ( cValor == "1")
	   	{
			oDatos.TermometroOnOff = 1;
	   	}
	   	if ( cValor == "0")
	   	{
			oDatos.TermometroOnOff = 0;
	   	}
       	cValor = ReadVariable ("Consigna");
	   	oDatos.Consigna = cValor.toFloat();
		cValor = ReadVariable ("UmbralSuperiorTemperatura");
		oDatos.UmbralSuperiorTemperatura = cValor.toFloat();
		cValor = ReadVariable ("UmbralInferiorTemperatura");
		oDatos.UmbralInferiorTemperatura = cValor.toFloat();
       	cValor = ReadVariable ("lTipoAlarmaTemperatura");
	   	if ( cValor == "1")
	   	{
			oDatos.lTipoAlarmaTemperatura = 1;
	   	}
	   	if ( cValor == "0")
	   	{
			oDatos.lTipoAlarmaTemperatura = 0;
	   	}	
		cValor = ReadVariable ("AlarmaUSTemperatura");
		oDatos.AlarmaUSTemperatura = cValor.toInt();
		cValor = ReadVariable ("AlarmaUITemperatura");
		oDatos.AlarmaUITemperatura = cValor.toInt();
		cValor = ReadVariable ("Offset");
		oDatos.Offset = cValor.toFloat();
       	cValor = ReadVariable ("lEnableAlarma");
	   	if ( cValor == "On")
	   	{
			oDatos.lEnableAlarma = 1;
	   	}
	   	if ( cValor == "Off")
	   	{
			oDatos.lEnableAlarma = 0;
	   	}
       	cValor = ReadVariable ("lAutomaticoManual");
	   	if ( cValor == "1")
	   	{
			oDatos.lAutomaticoManual = 1;
	   	}
	   	if ( cValor == "0")
	   	{
			oDatos.lAutomaticoManual = 0;
	   	}
       	cValor = ReadVariable ("TermometroOnOff");
	   	if ( cValor == "1")
	   	{
			oDatos.TermometroOnOff = 1;
	   	}
	   	if ( cValor == "0")
	   	{
			oDatos.TermometroOnOff = 0;
	   	}
		cValor = ReadVariable ("lOnOff");
	   	if ( cValor == "1")
	   	{
			oDatos.lOnOff = 1;
	   	}
	   	if ( cValor == "0")
	   	{
			oDatos.lOnOff = 0;
	   	} 
		cValor = ReadVariable ("Alarma");
	   	if ( cValor == "1")
	   	{
			oDatos.lEnableAlarma = 1;
	   	}
	   	if ( cValor == "0")
	   	{
			oDatos.lEnableAlarma = 0;
	   	} 


PrintDatosSensor( oDatos );
       return ( oDatos );
    }	
	String ReadVariable (String cVariable)
	{
		long nMiliSegundos;
    	String cLinea;                                                                              //Variable donde se almacenara informacio temporal
    
		Cliente.print("load-:-");
		Cliente.print(cVariable);
		Cliente.print("\n");  

		delay(200);

		nMiliSegundos = millis ();
		while ( !Cliente.available() && millis() < nMiliSegundos + 5000 ){}                         //Mientras recibamos datos del servidor
		cLinea = Cliente.readStringUntil('\n');

		return(cLinea);

	
	}
	void PrintDatosSensor (DataSensor oDatos )
	{
		Serial.print ("TermometroOnOff: ");		
		Serial.println( oDatos.TermometroOnOff );	 
		Serial.print ("Consigna: ");		
		Serial.println(oDatos.Consigna);
		Serial.print ("UmbralSuperiorTemperatura: ");
		Serial.println(oDatos.UmbralSuperiorTemperatura);
		Serial.print ("UmbralInferiorTemperatura: ");
		Serial.println(oDatos.UmbralInferiorTemperatura);
		Serial.print ("lTipoAlarmaTemperatura: ");
		Serial.println(oDatos.lTipoAlarmaTemperatura);
		Serial.print ("AlarmaUSTemperatura: ");
		Serial.println(oDatos.AlarmaUSTemperatura);
		Serial.print ("AlarmaUITemperatura: ");
		Serial.println(oDatos.AlarmaUITemperatura);
		Serial.print ("Offset: ");
		Serial.println(oDatos.Offset);
		Serial.print ("lEnableAlarma: ");
		Serial.println( oDatos.lEnableAlarma );	 
		Serial.print ("lAutomaticoManual: ");
		Serial.println( oDatos.lAutomaticoManual );	
		Serial.print ("lOnOff: ");		
		Serial.println( oDatos.lOnOff );	 
		Serial.print ("Alarma: ");		
		Serial.println( oDatos.lEnableAlarma );	 
	}
	DataSensor ActualizaDatosSensor (DataSensor oDatos)
	{
		DataSensor oDatosTmp = oDatos;
		float Temp;
		boolean lTemp;
/*
		if ( oDatosTmp.TermometroOnOff !=  )
		{
//PENDIENTE			
		}
*/
		Temp = Display.GetConsigna();
		if ( oDatosTmp.Consigna != Temp)
		{
			oDatosTmp.Consigna = Temp;
			MensajeServidor("save-:-Consigna-:-"+(String)Temp);
		}
		Temp = Display.GetTemperaturaMaximaAlarma();
		if ( oDatosTmp.UmbralSuperiorTemperatura != Temp )
		{
			oDatosTmp.UmbralSuperiorTemperatura = Temp;
			MensajeServidor("save-:-UmbralSuperiorTemperatura-:-"+(String)Temp);
		}
		Temp = Display.GetTemperaturaMinimaAlarma();
		if ( oDatosTmp.UmbralInferiorTemperatura != Temp )
		{
			oDatosTmp.UmbralInferiorTemperatura = Temp;
			MensajeServidor("save-:-UmbralInferiorTemperatura-:-"+(String)Temp);
		}
		Temp = Display.GetTemperaturaMinimaAlarma();
		if ( oDatosTmp.UmbralInferiorTemperatura != Temp )
		{
			oDatosTmp.UmbralInferiorTemperatura = Temp;
			MensajeServidor("save-:-UmbralInferiorTemperatura-:-"+(String)Temp);
		}
		Temp = Display.GetOffset();
		if ( oDatosTmp.Offset != Temp )
		{
			oDatosTmp.Offset = Temp;
			MensajeServidor("save-:-Offset-:-"+(String)Temp);
		}		
		
		lTemp = Display.GetAutomaticoManual();
		if (oDatosTmp.lAutomaticoManual != lTemp )
		{
			oDatosTmp.lAutomaticoManual = lTemp;
			if (lTemp)
			{
				MensajeServidor("save-:-lAutomaticoManual-:-1");
			}else{
				MensajeServidor("save-:-lAutomaticoManual-:-0");
			}	
		}
		lTemp = Display.GetOnOff();
		if (oDatosTmp.lOnOff != lTemp )
		{
			oDatosTmp.lOnOff = lTemp;
			if (lTemp)
			{
				MensajeServidor("save-:-lOnOff-:-1");
			}else{
				MensajeServidor("save-:-lOnOff-:-0");
			}	
		}		
		return(oDatosTmp);
	}


	#ifdef GA940_MPR121
	

		void DisplayDatosSensor ( float nTemperatura,  DataSensor eDatosSensor )
		{

			if ( eDatosSensor.lEnableAlarma )
			{
				Display.DisplayMarcas (nTemperatura, eDatosSensor.Consigna,  eDatosSensor.UmbralSuperiorTemperatura, eDatosSensor.UmbralInferiorTemperatura);
			}else{
				Display.DisplayMarcas (nTemperatura, eDatosSensor.Consigna);
			}
			if ( eDatosSensor.lAutomaticoManual)
			{
				Display.WriteTxtManual();
			}else{
				Display.WriteTxtAutomatico();
			}
			Display.WriteTxtOffset(eDatosSensor.Offset);
			if ( eDatosSensor.lOnOff )
			{
				Display.WriteTxtOff();
			}else{
				Display.WriteTxtOn();
			}	
		}
		void DisplayDatosSensorOff ( DataSensor eDatosSensor )
		{
			Display.DisplayMarcas();
		}
		void ICACHE_RAM_ATTR ScanPulsa (void)
		{
    	  Serial.println("Interrupcion");
    	  detachInterrupt(digitalPinToInterrupt(PinInterrupt)); 
    	  lFlagInterrupcion = 1;
    	  Display.Scan();
    	  //attachInterrupt(digitalPinToInterrupt(PinInterrupt), ScanPulsa, FALLING);
  		}
	#endif

	void CalefaccionOn (void)
	{
Serial.println("=======================");
Serial.println("Calefaccion On");
Serial.println("=======================");

		lEstadoCalefaccion = 1;
//		String cTexto = "telegram-:-Julian-:-Calefaccion On";
//		MensajeServidor(cTexto);
	}
	void CalefaccionOff (void)
	{
Serial.println("=======================");
Serial.println("Calefaccion Off");
Serial.println("=======================");

		lEstadoCalefaccion = 0;
//		String cTexto = "telegram-:-Julian-:-Calefaccion Off";
//		MensajeServidor(cTexto);
	}
	boolean GetCalefaccion (void)
	{
		return (lEstadoCalefaccion);
	}
	void AlarmaOn (void)
	{
		String cTexto = "telegram-:-Julian-:-Alarma On";
		MensajeServidor(cSalida);
	}
	void AlarmaOff (void)
	{
		String cTexto = "telegram-:-Julian-:-Alarma Off";
		MensajeServidor(cSalida);
	}


#endif    