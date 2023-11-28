#include "ServerPic.h"
#include "IO.h"



#ifdef DS18B20
	SENSORDS18B20 oSondaTemperatura(2, 3);
#endif
#ifdef SHT20 
	SENSORSHT20 oSondaTemperatura (5, 5); 			//Direccion por defecto 0x40
#endif





void setup(void) {
   	
  	#ifdef Debug			// Usamos el puereto serie solo para debugar
	  Serial.begin(9600); // Si no debugamos quedan libres los pines Tx, Rx para set urilizados
	  Serial.println("Iniciando........");
  	#endif
	EEPROM.begin(256); // Reservamos zona de EEPROM
	//BorraDatosEprom ( 0, 256);										//Borramos 256 bytes empezando en la posicion 0
	pinMode(PinReset, INPUT_PULLUP);           							//Configuramos el pin de reset como entrada
	if ( LeeByteEprom ( FlagConfiguracion ) == 0 )						//Comprobamos si el Flag de configuracion esta a 0
	{																	// y si esta
		ModoAP();														//Lo ponemos en modo AP
	}else{																//Si no esta
		#ifdef LedRojoVerde
			EnciendeLedRojo();
		#endif
		if ( ClienteSTA() )												//Lo poenmos en modo STA y nos conectamos a la SSID
		{																//Si ha conseguido conectarse a ls SSID en modo STA
			if ( ClienteServerPic () )									//Intentamos conectar a ServerPic
			{															//Si se consigue
	//			CheckFirmware();    									//Comprobamos si el firmware esta actualizado a la ultima version
				#ifdef Debug
					Serial.println(" ");
					Serial.println("Conectado al servidor");
				#endif
				cSalida = " " ;
				DataConfig aCfg = EpromToConfiguracion ();     							 //Leemos la configuracin de la EEprom
				char USUARIO[1+aCfg.Usuario.length()];
				(aCfg.Usuario).toCharArray(USUARIO, 1+1+aCfg.Usuario.length());          //Almacenamos en el array USUARIO el nombre de usuario
				cDispositivo = USUARIO;
				lHomeKit = aCfg.lHomeKit;												 //Almacenamos el estado de habilitacion de HomeKit
				lPush = aCfg.lPush;														 //Almacenamos el estado de habilitacion de Push
				char CLIENTEPUSH[1+aCfg.Push.length()];
				(aCfg.Push).toCharArray(CLIENTEPUSH, 1+1+aCfg.Push.length());            //Almacenamos en el array CLIENTEPUSH el nombre del cliente push
				cPush = CLIENTEPUSH;
				lEstadisticas = aCfg.lEstadisticas;										 //Almacenamos el estado de habilitacion de Estadisticas
				lTelegram = aCfg.lTelegram;
				lInfoTec = aCfg.lInfo;
				cSalida = LeeValor();
				//DatosSensor = LeeDatosSensores ();									 //Leemos los datos del sensor y los registramos en la sonda. Grabados en Eprom
				#ifdef SensorTemperatura
					DatosSensor = ReadDatosSensores ();										 //Leemos los datos del sensor y los registramos en la sonda. Grabados en Servidor
					if (DatosSensor.Offset > 15 || DatosSensor.Offset < -15 )				 //Si los datos de offset son incongruentes se fijan a 0 
					{
						DatosSensor.Offset = 0;												
						MensajeServidor("save-:-Offset-:-0");								 //Grabamos en el servidor Offset 0
						//GrabaDatosSensores(DatosSensor);									 //Graba los datos en EPROM
					}
					oSondaTemperatura.SetOffset(DatosSensor.Offset);
					oSondaTemperatura.SetUmbrales(DatosSensor.UmbralSuperiorTemperatura, DatosSensor.UmbralInferiorTemperatura, 5 );
					oSondaTemperatura.Alarma ( DatosSensor.lTipoAlarmaTemperatura, DatosSensor.AlarmaUSTemperatura, DatosSensor.AlarmaUITemperatura );
					if(DatosSensor.lEnableAlarma)
					{
						oSondaTemperatura.EnableAlarm();
					}else{
						oSondaTemperatura.DisableAlarm();
					}
					if(DatosSensor.lAutomaticoManual)
					{
						oSondaTemperatura.Manual();
					}else{
						oSondaTemperatura.Automatico();
					}
				#endif
//				oSondaTemperatura.begin();
				if ( lEstadisticas )													 //Si están habilitadas las estadisticas, actualizamos el numero de inicios
				{
					GrabaVariable ("inicios", 1 + LeeVariable("inicios") );
				}
				if (lPush)																 //Si esta habilitado el Push, notificamos el arranque
				{
					cSalida = "sendpush-:-"+cPush+"-:-"+cDispositivo+"-:-Inicio del modulo ";
					MensajeServidor(cSalida);
					cSalida = String(' ');
				}
				if (lTelegram)
				{
					cSalida = "telegram-:-"+cPush+"-:-Inicio del modulo "+cDispositivo;
					MensajeServidor(cSalida);
					cSalida = String(' ');

				}
    		}
      	}
    }    

	#ifdef SensorTemperatura
		if (!oSondaTemperatura.ConexionSensor())												 // Si la sonda esta conectada
  		{
			oSondaTemperatura.MedidasInicio();												 	 //Realiza las medidas iniciales
			if ( oSondaTemperatura.GetTemperaturaInstantanea () < DatosSensor.Consigna + 0.5 )	 //Si la temperatura real esta por debajo de la consigna
			{
				CalefaccionOn();																 //Encendemos la calefaccion	
			}else{																				 //Si no 	
				CalefaccionOff();																 //apagamos la calefaccion
			}
  		}
	#endif	

	//Si tenemos la pantalla G4940_MPR121
  	#ifdef GA940_MPR121
		Display.begin(MPR121_ADDR, PinInterrupt);										//Inicializamos display
		attachInterrupt(digitalPinToInterrupt(PinInterrupt), ScanPulsa, FALLING);		//Habilitamos las interrupciones para pulsadores tactiles
		Display.DrawLineas();															//Dibujamos las lineas del dial

		DisplayDatosSensor (oSondaTemperatura.nMedida, DatosSensor);
		Display.DisplayOn(); 															//Encendemos la pantalla												
		lPantallaEncendida = 1;															//Reflejamos en el flag lPantallaEncendida qwue la pantalla esta encendida
  	#endif  
	#ifdef GA940_MPR121
		lPantalla = 1;
		nTiempoPantalla = millis();
	#endif
  
}

void loop() {

	#ifdef GA940_MPR121
	/*---------------
	Apagado de pantalla
	------------------*/
		if ( (millis() > nTiempoPantalla + nTiempoPantallaOn) && lPantalla )				//Si la pantalla esta encendida y ha transcurrido el tiempo de inactividad establecido ( nTiempoPantallaOn )
		{
			Display.nModo = 0;																//Ponemos la pantalla en modo 0
			DatosSensor = ActualizaDatosSensor(DatosSensor);								//Miramos si ha habido algun cambio en el display y actualizamos lo que se haya cambiado
			//Display.DrawLineas();															//Dibujamos lineas
			Display.DisplayNormal();														//Dibujamos la pantalla normal
			DisplayDatosSensor (oSondaTemperatura.nMedida, DatosSensor);					//Reflejamos los datos en la pantalla
			Display.DisplayOff();															//Apagamos la pantalla
			lPantalla = 0;																	//Incdicamos que la pantalla esta apagada
			lPantallaEncendida = 0;															//Indicamos en enste otro flag la pantalla apagada
		}
	#endif

#ifdef GA940_MPR121
	if (!lPantalla)						//Este flag en principio es para anular procesos mientras la pantalla esta encendida
	{									//Si no hay pantalla siempre estara a 0 por lo que no modificamos si no hay pantalla
#endif		
		/*----------------
		Comprobacion Reset
		------------------*/
		TestBtnReset(PinReset);


		/*----------------
		Comprobacion Conexion
		------------------*/
		//Se aprovecha la periodicidad del test para temporizar las medidas
		//Cuando se han producido test de conexion un numero de veces igual a nCiclosMedida
		//nTemporizacionMedida se pone a 0 para indicar al bloque Medida del Sensor que se ha de hacere una medida  

		
		if (TiempoTest > 0) 											// Si esta habilitado el test de conexion
		{
			
			if (millis() > (nMiliSegundosTest + TiempoTest)) 			// Comprobamos si ha transcurrido el tiempo de test
			{
				//Determinamos si ha transcurrido el tiempo entre medidas
				#ifdef SensorTemperatura
					nTemporizacionMedida++;					   				// Incrementamos el temporizador de medida
					if (nTemporizacionMedida == nCiclosMedida) 				// Si ha llegado al valor establecido para hacer una medida
					{
						nTemporizacionMedida = 0; 							// Ponemos a 0 el flag a nTemporizacionMedida para que se realice medida de temperatura
					}
				#endif	
				nMiliSegundosTest = millis();	  						// Reseteamos el temporizador de Test
				if (!TestConexion(lEstadisticas)) 						// Si se ha perdido la conexion despues de IntentosConexionServidor intentos ( Definido en Global.h )
				{
					nTemporizadorArranque = 0;				  			// Reseteamos el temporizador de arranque que servira para encender la utilizacion ( si procede ) cuando haya una conexion estable de un tiempo TiempoTest*nCiclosArranque
					nTemporizadorParada++;					  			// Incrementamos el temporizador de parada
					if (nTemporizadorParada == nCiclosParada) 			// Si ha transcurrdio el tiempo establecido para la parada en ausencia de conexion
					{
						lConexionPerdida = 1;		   					// Ponemos el flag de perdida conexion a 1
					}
				}
				else
				{											   			// Si si que hay conexion
					if (lInfoTec && (nTemporizadorParada > 0)) 			// Si esta habilitada la información tecnica y se viene de una parada	(  nTemporizadorParada > 0 )
					{										   		  	// Mandamos aviso a Telegrma de que se ha recuperado l aconexion
						cSalida = "telegram-:-Julian-:-" + cDispositivo + " Informacion Tecnica: Conexion recuperada con " + (String)nTemporizadorParada + " ciclos de parada";
						MensajeServidor(cSalida);
						cSalida = String(' ');
					}
					nTemporizadorParada = 0;						  	// Reseteamos el temporizador de parada
					if (lConexionPerdida)							  	// Comprobamos si es una reconexion ( por perdida anterior )
					{												  	// Si lo es
						nTemporizadorArranque++;					  	//
						if (nTemporizadorArranque == nCiclosArranque) 	// Miramos si ha pasado la temorizacion para arranque
						{
							lConexionPerdida = 0; 						// Reseteamos flag de reconexion
							if (lInfoTec)
							{
								cSalida = "telegram-:-Julian-:-" + cDispositivo + " recuperada conexion con utilización On con " + (String)nTemporizadorArranque + " ciclos de arranque";
								MensajeServidor(cSalida);
								cSalida = String(' ');
							}
						}
					}
				}
			}
		#ifdef GA940_MPR121
			}
		#endif


		/*----------------
		Medida del sensor
		------------------*/
		#ifdef SensorTemperatura
			if (lOnOff && nTemporizacionMedida == 0) 						// Si tenemos habilitado el termostato y
			{										 						// y se ha cumplido el ciclo de temporizacin de medida ( nTemporizacionMedida = 0 )
				if (oSondaTemperatura.GetOnOff())	 						// Si la sonda está habilitada
				{
					if (oSondaTemperatura.TestSensor()) 					// Hacemos una medida y testeamos si hay alarma de temperatura o ha repuesto y, Si hay alarma o reposicion....
					{
						nTemporizacionMedida++;
						delay(100);
						if (lTelegram) 											// Si esta habilitado el push
						{
							lAlarma = oSondaTemperatura._GetEstadoAlarma();
							Serial.print("lAlarma: ");
							Serial.println(lAlarma);
							if (oSondaTemperatura._GetEstadoAlarma())		// Notificamos la alarma o la reposicion
							{
								cSalida = "telegram-:-" + cPush + "-:-Alarma temperatura  " + (String)(oSondaTemperatura.nMedida);
							}
							else
							{
								cSalida = "telegram-:-" + cPush +"-:-Alarma temperatura  repuesta " + (String)(oSondaTemperatura.nMedida);
							}
							MensajeServidor(cSalida);
							cSalida = String(' ');
						}
						delay(100);
					}
					#ifdef GA940_MPR121
						if (!oSondaTemperatura.ConexionSensor())								//Si el sensor esta conectado
						{
							DisplayDatosSensor (oSondaTemperatura.nMedida, DatosSensor);		//Actualizamos el display
						}
					#endif	
					if (lTelegram) // Si esta habilitado el push
					{
						if (oSondaTemperatura.TestConexionSensor()) 						//Comprobamos si la sonda sigue conectada
						{
							if (oSondaTemperatura.ConexionSensor()) 						// Notificamso si ha habido algun cambio en el estado de la sonda
							{
								cSalida = "telegram-:-" + cPush + "-:-Sensor desconectado  ";
							}
							else
							{
								cSalida = "telegram-:-" + cPush + "-:-Sensor conectado  ";
							}
						}
					}	
					nTemporizacionMedida++;
				}else{
					#ifdef GA940_MPR121
						DisplayDatosSensorOff (DatosSensor);		//Actualizamos el display
					#endif
				}
			}
		#endif	

		/*----------------
		Analisis comandos
		------------------*/
		oMensaje = Mensaje(); // Iteractuamos con ServerPic, comprobamos si sigue conectado al servidor y si se ha recibido algun mensaje

		if (oMensaje.lRxMensaje) // Si se ha recibido ( oMensaje.lRsMensaje = 1)
		{
			#ifdef Debug
				Serial.println(oMensaje.Remitente); // Ejecutamos acciones
				Serial.println(oMensaje.Mensaje);
			#endif

			// En este punto empieza el bloque de programa particular del dispositivo segun la utilizacion
			//--------------------------------------------------------------------------------------------
			// Comandos generales de Dispositivo
			//
			//  On.- Habilita el dispositivo
			//  Off.- Deshabilita el dispositivo
			//  Change.- Cambia el estado ( habilita/deshabilita ) del dispositivo
			//  ChangeGet.- Cambia el estado ( habilita/deshabilita ) del dispositivo y responde del nuevo estado al remitente
			//  Get.- Responde al remitente con el estado del dispositivo
			//
			//  El dispositivo habilitado pone a 1 el flag lOnOff y a 0 en caso contrario
			//--------------------------------------------------------------------------------------------

			if (oMensaje.Mensaje == "On") // Si se recibe "On", se pone PinSalida a '1'
			{
				DispositivoOn();
				cSalida = "On"; // Se pone cSalida a 'On' para  se envie a HomeKit
			}
			if (oMensaje.Mensaje == "Off") // Si se recibe "Off", se pone PinSalida a '0'
			{
				DispositivoOff();
				cSalida = "Off"; // Se pone cSalida a 'Off' para  se envie a HomeKit
			}
			if (oMensaje.Mensaje == "Change") // Si se recibe 'Change', cambia el estado de PinSalida  y se pone cSalida con el nuevo estado para comunicar a Homekit
			{
				if (GetDispositivo())
				{
					DispositivoOff();
					cSalida = "Off";
				}
				else
				{
					DispositivoOn();
					cSalida = "On";
				}
			}
			if (oMensaje.Mensaje == "ChangeGet") // Si se recibe 'ChangeGet', cambia el estado de PinSalida, devuelve el nuevo estado al remitente  y se pone cSalida con el nuevo estado para comunicar a Homekit
			{
				if (GetDispositivo())
				{
					DispositivoOff();
					cSalida = "Off";
				}
				else
				{
					DispositivoOn();
					cSalida = "On";
				}
				oMensaje.Mensaje = cSalida; // Confeccionamos el mensaje a enviar hacia el servidor
				oMensaje.Destinatario = oMensaje.Remitente;
				EnviaMensaje(oMensaje); // Y lo enviamos
			}
			if (oMensaje.Mensaje == "Get") // Si se recibe 'Get', se devuelve el estado de PinSalida al remitente
			{
				if (GetDispositivo())
				{
					cSalida = String("On"); // No ha habido cambio de estado, Vaciamos cSalida para que no se envie a HomeKit
				}
				else
				{
					cSalida = String("Off"); // No ha habido cambio de estado, Vaciamos cSalida para que no se envie a HomeKit
				}
				oMensaje.Mensaje = cSalida;
				oMensaje.Destinatario = oMensaje.Remitente;
				EnviaMensaje(oMensaje);
				cSalida = String(' '); // No ha habido cambio de estado, Vaciamos cSalida para que no se envie a HomeKit
			}
			//--------------------------------------------------------------------------------------------
			// Comandos para el sensor de temperatura
			//
			// Los comandos de orden, si hay display,  modifica el contenido del display. Los comandos de consulta no modifican el contenido del display 
			//
			// Termometro-:-(On/Off).- Habilita/deshabilita la sonda de temperatura, responde al remitente indicando si la sonda esta On u Off
			// Consigna-:-Valor.- Establece la consigna del termostato
			// SetUmbralesTemperatura-:-ValorI-:-ValorF .- Establece los umbrales inferior y superior de temperatura y los graba en datos particulares de usuario. Responde al remitente con los datos grabados
			// GetUmbralesTemperatura.- Devuelve al remitente los datos de los umbrales de temperatura. Si se ha pedido desde Web, los remite anteponienddo web-:-Umbrales-:-
			// SetAlarmaTemperatura-:-Tipo-:-SgInferior-:-SgSuperior.- Fija los parametros para la alarma  y los graba en datos particulares de usuario. Responde al remitente con los datos grabados
			// GetAlarmaTemperatura.- Devuelve al remitente los parametros de la alarma. Si se ha pedido desde Web, los remite anteponienddo web-:-AlarmaTemperatura-:-
			// GetTermometro.- Devuelve los datos de temepratura. Si se pide desde web, la envia anteponiendo web-:-Termometro-:- a la temperatura
			// SetOffset.- Establece el Offset del sensor
			// GetOffset.- Lee el Offset del sensor
			// Automatico.- Pone el dispositivo en automatico
			// Manual.- Pone el dispositivo en manual
			// Calefaccion.- Pone el la calefacción en On/Off
			// Alarma.- Habilita/Deshabilita la alarma 
			//--------------------------------------------------------------------------------------------



			// Habilta/Deshabilita Sonda
//PENDIENTE ACTUALIZAR PANTALLA		
			//Mensajes para la sonda	
			#ifdef SensorTemperatura

			if ((oMensaje.Mensaje).indexOf("Read") == 0) 
			{
				DatosSensor = ReadDatosSensores();
			}



				if ((oMensaje.Mensaje).indexOf("Termometro-:-") == 0) // Si se recibe una orden de "Termometro-:-", se analida el estado recibido y se habilita/deshabilita la sonda segun la orden recibida
					{
					String cEstado = String(oMensaje.Mensaje).substring(3 + String(oMensaje.Mensaje).indexOf("-:-"), String(oMensaje.Mensaje).length()); // Extraemos segundo parametro pasado
					String cEStado = String(cEstado).substring(3 + String(cEstado).indexOf("-:-"), String(cEstado).length());
					if (cEstado == "On")																												 //Sie es On
					{
						DatosSensor.TermometroOnOff = 1;																								 //Lo trasladamos a la esturctura DataSensor	
						MensajeServidor("save-:-Termometro-:-1");																						 //Actualizamos la variable Termometro en servidor	
						oSondaTemperatura.On();																											 //Lo trasladamos a la sonda
						#ifdef GA940_MPR121	
							DisplayDatosSensor (oSondaTemperatura.GetTemperaturaInstantanea(),  DatosSensor);											 //Actualizamos Display 
						#endif
					}
					if (cEstado == "Off")
					{
						DatosSensor.TermometroOnOff = 0;																								 //Lo trasladamos a la esturctura DataSensor	
						MensajeServidor("save-:-Termometro-:-0");																						 //Actualizamos la variable Termometro en servidor
						oSondaTemperatura.Off();																										 //Lo trasladamos a la sonda	
						#ifdef GA940_MPR121	
							DisplayDatosSensorOff(DatosSensor);																							 //Actualizamos Display para sonda Off ( valor temperatura ---- )
						#endif	
					}
					oMensaje.Mensaje = "Termometro-:-" + cEstado;																						 //Devolvemos al cliente que lo ha solicitado el estado actual
					oMensaje.Destinatario = oMensaje.Remitente;
					EnviaMensaje(oMensaje);
					cSalida = String(' '); 																												 // Vaciamos cSalida para que no se envie a HomeKit
				}
				// Establece la consigna, 
				if ((oMensaje.Mensaje).indexOf("Consigna-:-") == 0) // Si se recibe una orden de "SetOffset-:-", se analiza el valor recibido y se fija el offset del sensor
				{
					String cValor = String(oMensaje.Mensaje).substring(3 + String(oMensaje.Mensaje).indexOf("-:-"), String(oMensaje.Mensaje).length()); //Extraemos el valor TA deseado
					DatosSensor.Consigna = (cValor + '\0').toFloat();																					//Lo trasladamos a la esturctura DataSensor	
					MensajeServidor("save-:-Consigna-:-"+cValor);																						//Actualizamos la variable Consigna en servidor
					oSondaTemperatura.SetConsigna(DatosSensor.Consigna);																				//Lo trasladamos a la sonda
					#ifdef GA940_MPR121				
						DisplayDatosSensor (oSondaTemperatura.GetTemperaturaInstantanea(),  DatosSensor);												//Actualizamos Display 
					#endif
					oMensaje.Mensaje = "Consigna-:-" + cValor;			     																			//Devolvemos al cliente que lo ha solicitado el estado actual				
					oMensaje.Destinatario = oMensaje.Remitente;
					EnviaMensaje(oMensaje);
					cSalida = String(' '); 																												// Vaciamos cSalida para que no se envie a HomeKit
				}			
				//Establecimiento umbrales temperatura
				if ((oMensaje.Mensaje).indexOf("SetUmbralesTemperatura-:-") == 0)																		// Si se recibe una orden de "SeetUmbralesTemperatura-:-", se extraen los umbrales, se graban en los datos de usuario del sensor y en la sonda
				{																																		// Se notifica al remitente umbrales registrados
					String cValor = String(oMensaje.Mensaje).substring(3 + String(oMensaje.Mensaje).indexOf("-:-"), String(oMensaje.Mensaje).length()); // Extraemos el valor superior
					String cValorI = String(cValor).substring(0, String(cValor).indexOf("-:-"));
					String cValorS = String(cValor).substring(3 + String(cValor).indexOf("-:-"), String(cValor).length());
					DatosSensor.UmbralSuperiorTemperatura = (cValorS + '\0').toFloat();
					DatosSensor.UmbralInferiorTemperatura = (cValorI + '\0').toFloat();
					MensajeServidor("save-:-UmbralSuperiorTemperatura-:-"+cValorS);
					MensajeServidor("save-:-UmbralInferiorTemperatura-:-"+cValorI);
					oSondaTemperatura.SetUmbrales(DatosSensor.UmbralSuperiorTemperatura, DatosSensor.UmbralInferiorTemperatura);
					cSalida = (String)DatosSensor.UmbralInferiorTemperatura + "-:-" + (String)DatosSensor.UmbralSuperiorTemperatura;
					oMensaje.Mensaje = "GrabadosUmbrales " + cSalida;
					oMensaje.Destinatario = oMensaje.Remitente;
					EnviaMensaje(oMensaje);
					cSalida = String(' '); // No ha habido cambio de estado, Vaciamos cSalida para que no se envie a  a HomeKit
					#ifdef GA940_MPR121
						DisplayDatosSensor (oSondaTemperatura.nMedida, DatosSensor);
					#endif
				}
				//Devuelve los umbrales de temperatura
				if ((oMensaje.Mensaje).indexOf("GetUmbralesTemperatura") == 0) // Si se recibe una orden de "GetUmbralesTemperatura", se devuelve el valor de los umbrales diferenciando si se ha pedido desde web o desde otro ususario
				{
					String cRemitente = ("websocket");
					if (oMensaje.Remitente == cRemitente)
					{
						cSalida = "web-:-Umbrales-:-" + (String)DatosSensor.UmbralInferiorTemperatura + "-:-" + (String)DatosSensor.UmbralSuperiorTemperatura;
						MensajeServidor(cSalida);
					}
					else
					{
						cSalida = (String)DatosSensor.UmbralInferiorTemperatura + "-:-" + (String)DatosSensor.UmbralSuperiorTemperatura;
						oMensaje.Mensaje = "Umbrales-:-" + cSalida;
						oMensaje.Destinatario = oMensaje.Remitente;
						EnviaMensaje(oMensaje);
					}
					cSalida = String(' '); // Vaciamos cSalida para que no se envie a  HomeKit
				}
				//Fija los parametros de alarma
				if ((oMensaje.Mensaje).indexOf("SetAlarmaTemperatura-:-") == 0) // Si se recibe una orden de "SeetAlarmaTemperatura-:-", se extraen los datos de alarma, se graban en los datos de usuario del sensor y en la sonda
				{
					String cValor = String(oMensaje.Mensaje).substring(3 + String(oMensaje.Mensaje).indexOf("-:-"), String(oMensaje.Mensaje).length()); // Extraemos el valor TA deseado
					String cTipoAlarma = String(cValor).substring(0, String(cValor).indexOf("-:-"));
					cValor = String(cValor).substring(3 + String(cValor).indexOf("-:-"), String(cValor).length());
					String cValorI = String(cValor).substring(0, String(cValor).indexOf("-:-"));
					String cValorS = String(cValor).substring(3 + String(cValor).indexOf("-:-"), String(cValor).length());
					DatosSensor.lTipoAlarmaTemperatura = (cTipoAlarma == "1") ? 1 : 0;
					DatosSensor.AlarmaUSTemperatura = (cValorS + '\0').toInt();
					DatosSensor.AlarmaUITemperatura = (cValorI + '\0').toInt();
					MensajeServidor("save-:-lTipoAlarmaTemperatura-:-"+cTipoAlarma);
					MensajeServidor("save-:-AlarmaUSTemperatura-:-"+cValorS);
					MensajeServidor("save-:-AlarmaUITemperatura-:-"+cValorI);
					oSondaTemperatura.Alarma(DatosSensor.lTipoAlarmaTemperatura, DatosSensor.AlarmaUSTemperatura, DatosSensor.AlarmaUITemperatura);
					cSalida = "AlarmaTemperatura-:-" + (String)DatosSensor.lTipoAlarmaTemperatura + "-:-" + (String)DatosSensor.AlarmaUITemperatura + "-:-" + (String)DatosSensor.AlarmaUSTemperatura;
					oMensaje.Mensaje = cSalida;
					oMensaje.Destinatario = oMensaje.Remitente;
					EnviaMensaje(oMensaje);
					cSalida = String(' '); // Vaciamos cSalida para que no se envie a  a HomeKit
					#ifdef GA940_MPR121
						DisplayDatosSensor (oSondaTemperatura.GetTemperaturaInstantanea(), DatosSensor);
					#endif
				}
				//Devuelve los parametros de la alarma
				if ((oMensaje.Mensaje).indexOf("GetAlarmaTemperatura") == 0) // Si se recibe una orden de "GetAlarmaTemperatura" , se devuelve el valor de los datos de alarma diferenciando si se ha pedido desde web o desde otro ususario
				{
					String cRemitente = ("websocket");
					if (oMensaje.Remitente == cRemitente)
					{
						cSalida = "web-:-AlarmaTemperatura-:-" + (String)DatosSensor.lTipoAlarmaTemperatura + "-:-" + (String)DatosSensor.AlarmaUITemperatura + "-:-" + (String)DatosSensor.AlarmaUSTemperatura;
						MensajeServidor(cSalida);
					}
					else
					{
						cSalida = "AlarmaTemperatura-:-" + (String)DatosSensor.lTipoAlarmaTemperatura + "-:-" + (String)DatosSensor.AlarmaUITemperatura + "-:-" + (String)DatosSensor.AlarmaUSTemperatura;
						oMensaje.Mensaje = cSalida;
						oMensaje.Destinatario = oMensaje.Remitente;
						EnviaMensaje(oMensaje);
					}
					cSalida = String(' '); // No ha habido cambio de estado, Vaciamos cSalida para que no se envie a  HomeKit
				}
				//Devuelve los datos de temperatura
				if ((oMensaje.Mensaje).indexOf("GetTermometro") == 0) // Si se recibe una orden de "GetTermometro", manda la temperatura de la sonda en funcion del remitente
				{
					String cRemitente = ("websocket");
					if (oMensaje.Remitente == cRemitente)
					{
						cSalida = "web-:-Termometro-:-" + (String)(oSondaTemperatura.nMedida);
						MensajeServidor(cSalida);
					}
					else
					{
						oMensaje.Mensaje = (String)(oSondaTemperatura.nMedida);
						oMensaje.Destinatario = oMensaje.Remitente;
						EnviaMensaje(oMensaje);
					}
					cSalida = String(' '); // Vaciamos cSalida para que no se envie a  HomeKit
				}	
				//Establece el Offset de la sonda
				if ((oMensaje.Mensaje).indexOf("SetOffset-:-") == 0) // Si se recibe una orden de "SetOffset-:-", se analiza el valor recibido y se fija el offset del sensor
				{
					String cValor = String(oMensaje.Mensaje).substring(3 + String(oMensaje.Mensaje).indexOf("-:-"), String(oMensaje.Mensaje).length()); // Extraemos el valor TA deseado
					DatosSensor.Offset = (cValor + '\0').toFloat();
					MensajeServidor("save-:-Offset-:-"+cValor);
					oSondaTemperatura.SetOffset(DatosSensor.Offset);
					oMensaje.Mensaje = "Offset establecido en " + (String)DatosSensor.Offset;
					oMensaje.Destinatario = oMensaje.Remitente;
					EnviaMensaje(oMensaje);
					cSalida = String(' '); // Vaciamos cSalida para que no se envie a HomeKit
					#ifdef GA940_MPR121
						DisplayDatosSensor (oSondaTemperatura.GetTemperaturaInstantanea(), DatosSensor);
					#endif
				}
				//Devuelve lso datos de Offset de la sonda
				if (oMensaje.Mensaje == "GetOffset") // Si se recibe una orden de "GetOffset", se devuelve el offset del sensor
				{
					oMensaje.Mensaje = "Offset del sensor " + (String)DatosSensor.Offset + " grados";
					oMensaje.Destinatario = oMensaje.Remitente;
					EnviaMensaje(oMensaje);
					cSalida = String(' '); // Vaciamos cSalida para que no se envie a HomeKit
				}					
				//Pone el termostato en automatico
				if (oMensaje.Mensaje == "Automatico") // Si se recibe una orden de Automatico, se pone el termostato en automatico
				{
					DatosSensor.lAutomaticoManual = 0;
					MensajeServidor("save-:-lAutomaticoManual-:-0");
					#ifdef GA940_MPR121
						DisplayDatosSensor (oSondaTemperatura.GetTemperaturaInstantanea(), DatosSensor);
					#endif
					oMensaje.Mensaje = "Termostato en automatico ";
					oMensaje.Destinatario = oMensaje.Remitente;
					EnviaMensaje(oMensaje);
					cSalida = String(' '); // Vaciamos cSalida para que no se envie a HomeKit
				}					
				//Pone el termostato en manual
				if (oMensaje.Mensaje == "Manual") // Si se recibe una orden de Manual, se pone el termostato en manual
				{
					DatosSensor.lAutomaticoManual = 1;
					MensajeServidor("save-:-lAutomaticoManual-:-1");
					#ifdef GA940_MPR121
						DisplayDatosSensor (oSondaTemperatura.GetTemperaturaInstantanea(), DatosSensor);
					#endif
					oMensaje.Mensaje = "Termostato en manual ";
					oMensaje.Destinatario = oMensaje.Remitente;
					EnviaMensaje(oMensaje);
					cSalida = String(' '); // Vaciamos cSalida para que no se envie a HomeKit
				}
				//Enciende o apaga la calefaccion
				if (oMensaje.Mensaje.indexOf("Calefaccion-:-") == 0) // Si se recibe una orden de "Calefacción", se pone la calefacción como se indica en el segundo parametro
				{                                                    //Siempre que el termostato este en manual
					if (DatosSensor.lAutomaticoManual)				 //Si esta en manual
					{	
						String cValor = String(oMensaje.Mensaje).substring(3 + String(oMensaje.Mensaje).indexOf("-:-"), String(oMensaje.Mensaje).length()); // Extraemos el valor TA deseado		
						if ( cValor == "On")
						{
							DatosSensor.lOnOff = 1;
							MensajeServidor("save-:-Calefaccion-:-1");
							CalefaccionOn();
						}
						if ( cValor == "Off")
						{
							DatosSensor.lOnOff = 0;
							MensajeServidor("save-:-Calefaccion-:-0");
							CalefaccionOff();
						}

						#ifdef GA940_MPR121
							if ( cValor == "On")
							{
								Display.WriteTxtOn();
							}
							if ( cValor == "Off")
							{
								Display.WriteTxtOff();
							}
						#endif 
						oMensaje.Mensaje = "Calefaccion en  " + cValor;
					}else{
						oMensaje.Mensaje = "El termostato esta en automatico  ";
					}
					oMensaje.Destinatario = oMensaje.Remitente;
					EnviaMensaje(oMensaje);
					cSalida = String(' '); // Vaciamos cSalida para que no se envie a HomeKit	

				}
				//Habilita/Deshabilita la alarma
				if ((oMensaje.Mensaje).indexOf("Alarma-:-") == 0) // Si se recibe una orden de "Alarma", habilita o deshabilita la alarma (On/Off)
				{
					String cEstado = String(oMensaje.Mensaje).substring(3 + String(oMensaje.Mensaje).indexOf("-:-"), String(oMensaje.Mensaje).length()); // Extraemos el valor TA deseado
					String cEStado = String(cEstado).substring(3 + String(cEstado).indexOf("-:-"), String(cEstado).length());
					if (cEstado == "On")							// Si se recibe Alarma-:-On
					{
						DatosSensor.lEnableAlarma = 1;
						MensajeServidor("save-:-Alarma-:-1");
						oSondaTemperatura.EnableAlarm();
					}
					if (cEstado == "Off")							// Si se recibe Alarma-:-Off
					{
						DatosSensor.lEnableAlarma = 0;
						MensajeServidor("save-:-Alarma-:-0");
						oSondaTemperatura.DisableAlarm();
					}
					oMensaje.Mensaje = "Alarma " + cEstado;			//Informamos del estado
					oMensaje.Destinatario = oMensaje.Remitente;
					EnviaMensaje(oMensaje);
					cSalida = String(' '); // Vaciamos cSalida para que no se envie a HomeKit
				}
			#endif
		}
	}
	/*----------------
	Actualizacion ultimo valor
	------------------*/
	if (cSalida != String(' ')) // Si hay cambio de estado
	{
		EnviaValor(cSalida); // Actualizamos ultimo valor
	}
	cSalida = String(' '); // Limpiamos cSalida para iniciar un nuevo bucle

	wdt_reset(); // Refrescamos WDT	

	//Testeo tecla pulsada
	#ifdef GA940_MPR121
		if (lFlagInterrupcion )								//Si se ha pulsado una tecla
		{
			if ( !lPantalla )								//Si la pantalla estaba apagada
			{
				Display.DisplayOn();						//La encendemos
				lPantalla = 1;								//Ponemos el flag de pantalla encendida a 1
				lPantallaEncendida = 1;
				while ( lFlagInterrupcion == 1 )
		   		{
					Display.Refresh();						//Refrescamos teclado y esperamos a que se suelte la tecla
		    		if (Display.TeclasPulsadas() == 0 )
		    		{
		      			lFlagInterrupcion = 0;				//Reseteamos flag de interrupcion
		    		}
		  		}		
			}else{											//Si estaba encendida
		   		while ( lFlagInterrupcion == 1 )
		   		{
		    		Display.Scan();							//Scaneamos el boton pulsado
		    		if (Display.TeclasPulsadas() == 0 )		//Cuando se suelta
		    		{
		      			lFlagInterrupcion = 0;				//Reseteamos el flag de interrupcion
		    		}
		  		}
			}
			nTiempoPantalla = millis();						//Iniciamos el contador de tiempo encendido
		  	attachInterrupt(digitalPinToInterrupt(PinInterrupt), ScanPulsa, FALLING); //Habilitamos las interrupciones
		}
	#endif	
 //oSondaTemperatura.Medida();
}


