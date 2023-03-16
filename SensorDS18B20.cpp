
#include "SensorDS18B20.h"

/**
******************************************************
*@brief Constructor de la clase
*
*@param PinDS18B20.- Pin que se va a utilizar para la comunicacion OneWire
*@param nMuestras.- Nuemro de muestras que se van a utilizar para obtener el valor medio del sensor
*
*/ 
SENSORDS18B20::SENSORDS18B20( int PinDS18B20,  int nMuestras )
{

	this->_PinDS18B20 = PinDS18B20;
	this->_nMuestrasMedida = nMuestras;
	this->_lOnOff = 1;
	this->_lSensorDesconectado = 0;	
	this->_lSensorDesconectadoAnterior = 0;	
	this->begin();
}
/**
******************************************************
*@brief Constructor de la clase
*
*Este constructor. a diferencia del anterior no recibe el parametro nMuestras que por defecto lo fija a 10
*
*@param PinDS18B20.- Pin que se va a utilizar para la comunicacion OneWire
*
*/ 
SENSORDS18B20::SENSORDS18B20( int PinDS18B20 )
{

	this->_PinDS18B20 = PinDS18B20;
	this->_nMuestrasMedida = 10;
	this->_lOnOff = 1;	
	this->_lSensorDesconectado = 0;
	this->_lSensorDesconectadoAnterior = 0;			
	this->begin();
}

/**
******************************************************
*@brief Destructor de la clase
*
*/
SENSORDS18B20::~SENSORDS18B20( )
{

}
/**
******************************************************
*@brief Arranque del sensor
*
*/
void SENSORDS18B20::begin (void)
{
	OneWire oneWire(_PinDS18B20);
	oneWire.reset();
	delay(1000);
	DallasTemperature sensors(&oneWire);
	this->_sensors = sensors;
	this->_sensors.begin();	
}
void SENSORDS18B20::Elimina (void)
{
	this->_sensors.~DallasTemperature();;
	this->begin();
}
/**
******************************************************
*@brief Fija los umbrales Superior e Inferior establecidos para este sensor. Asigna el numero 
*de veces consecutivas que el sensor se encuentra en una zona para darla como valida
*
*@param UmbralSuperior.- Valor del umbral superior establecido para este sensor
*@param UmbralInferior.- Valor del umbral inferior establecido para este sensor
*@param nOcurrecias.- Numero de veces consecutivas que el sensor debe localizarse un una zona para validarla
*
*/ 
void SENSORDS18B20::SetUmbrales (float UmbralSuperior, float UmbralInferior, int nOcurrencias )
{	

	this->UmbralSuperior = UmbralSuperior;
	this->UmbralInferior = UmbralInferior;
	if ( nOcurrencias > 0)
	{
		this->_nOcurrenciasParam = nOcurrencias-1;
	}else{
		this->_nOcurrenciasParam = 5;
	}
	this->_lFlagZona = 1;

}	
/**
******************************************************
*@brief Fija los umbrales Superior e Inferior establecidos para este sensor. Asigna el numero 
*de veces consecutivas que el sensor se encuentra en una zona para darla como valida
*
*A diferencia de la funcion anterior, esta no reibe el parametro nOcurrencias
*Esta fucion se utilizara para fijar los umbrales sin variar nOcurrencias 
*
*@param UmbralSuperior.- Valor del umbral superior establecido para este sensor
*@param UmbralInferior.- Valor del umbral inferior establecido para este sensor
*
*/ 
void SENSORDS18B20::SetUmbrales (float UmbralSuperior, float UmbralInferior )
{	

	this->UmbralSuperior = UmbralSuperior;
	this->UmbralInferior = UmbralInferior;

}	
/**
******************************************************
*@brief Funcion que inicia las medidas del sensro. Hace una medidas para determinar 
*un valor de medida inicial.
*Se utilizará para tener una medida disponible en el momento que se conecta al servidor
*
*/ 
void SENSORDS18B20::Init (void)
{
	
	int nMedida = 0;

	while (nMedida < this->_nMuestrasMedida )
	{
		this->Medida();
		nMedida++;
		wdt_reset();		
	}

}	
/**
******************************************************
*@brief Habilita el sensor 
*
*/ 
void SENSORDS18B20::On (void)
{
	this->_lOnOff = 1;
}
/**
******************************************************
*@brief Deshabilita el sensor 
*
*/ 
void SENSORDS18B20::Off (void)
{
	this->_lOnOff = 0;
}
/**
******************************************************
*@brief Funcion para conocer si el sensor esta habilitado/deshabilitado 
*
*return.- Devuelve 1/0 para indicar si el sensor esta habilitado/Deshabilitado
*/ 
boolean SENSORDS18B20::GetOnOff (void)
{
	return (this->_lOnOff);
}
/**
******************************************************
*@brief Habilita la determinacion de la zona donde se encuentra el sensor
*
*/ 
void SENSORDS18B20::ZonaOn (void)
{
	this->_lFlagZona = 1;
}
/**
******************************************************
*@brief Deshabilita la determinacion de la zona donde se encuentra el sensor
*
*/ 
void SENSORDS18B20::ZonaOff (void)
{
	this->_lFlagZona = 0;
}
/**
******************************************************
*@brief Funcion para conocer si el si la determinacion de zona esta habilitado/deshabilitado 
*
*return.- Devuelve 1/0 para indicar si el sensor esta habilitado/Deshabilitado
*/ 
boolean SENSORDS18B20::_GetZonaOnOff (void)
{
	return (this->_lFlagZona);
}
/**
******************************************************
*@brief Funcion para que el sensor ejecute una medida
*
*El motor qeu maneja los sensores, va ordenando a los sensores que hagan medidas con esta funcion y las va almacenando en this->_ValorAcumulado
*cuando el numero de medidas alcanza el valor que se paso en el constructor como nMuestras y que internamente
*esta almacenado en la variable this->_nMuestrasMedida, almacena en this->_nMedida el valor medio de las
*nMuestras, e inicia this->_ValorAcumulado para otro ciclo de medidas
*Cuando completa el ciclo, manda al servidor la medida y si esta habilitado la determinacion de zona, determina la zona
*a la que corresponde el valor this->_nMedida, en caso de que se haya producido un cambio de zona, devuelve un 1, en caso de que no haya
*cambio devolvera un 0   
*
*return.- Devuelve 1/0 para indicar si ha habido o no un cambio de zona
*/ 
boolean SENSORDS18B20::Medida (void)
{
		static boolean lSondaConectada = 1;

		this->_sensors.requestTemperatures();
		this->_Valor = this->_sensors.getTempCByIndex(0);
Serial.print ("Desconectado ?");
Serial.println(this->_Valor);
		if ( this->_Valor != DEVICE_DISCONNECTED_C)
		{
			if (!lSondaConectada)
			{
				MensajeServidor( "sendpush-:-Julian-:-"+cDispositivo+"-:-Sonda reconectada ");
				lSondaConectada = 1;
			}
			this->_lSensorDesconectado = 0;														//El Sensor esta conectado				
			this->_ValorAcumulado = this->_ValorAcumulado + this->_Valor + this->_Offset;
Serial.print("---->");
Serial.print(this->_nMuestra);
Serial.print("-->");
Serial.print(this->_Valor);
Serial.print("-->");
Serial.println(this->_ValorAcumulado);
			if  (this->_nMuestra == this->_nMuestrasMedida - 1 )
			{
				this->_Valormedio = round(this->_ValorAcumulado)/this->_nMuestrasMedida;
				this->nMedida = this->_Valormedio;
Serial.print("--->");
Serial.println(this->_Valormedio);
  				this->_ValorAcumulado=0;		
  				this->_nMuestra = 0;
  				if ( this->_lFlagZona )
				{
					this->_Zona();
				}	
				MensajeServidor("medida-:-Termometro-:-"+(String)(this->nMedida));
 			}else{
  				this->_nMuestra++;
  			}
  		}else{
			if (lSondaConectada)
			{			  
				MensajeServidor( "sendpush-:-Julian-:-"+cDispositivo+"-:-Sonda caida ");
				lSondaConectada = 0;
			}

this->Elimina();
  			//this->begin();
Serial.println("Reintento conexion" + cDispositivo);			  
			this->_sensors.requestTemperatures();
			this->_Valor = this->_sensors.getTempCByIndex(0);
			if ( this->_Valor == DEVICE_DISCONNECTED_C && this->_lSensorDesconectado == 0 )
			{
				MensajeServidor("medida-:-Termometro-:-"+(String)("Off"));					  	
				this->_lSensorDesconectado = 1;												  //El Sensor esta desconectado				
			}
  		}
  		return (this->_CambioZona()); 
}	
/**
******************************************************
*@brief Determina la zona en la que se encuentra la ultima medida 
*
*Los umbrales superior e inferior delimitan tres zonas: la que esta por debajo del umbral inferior (1), la que esta por encima
*del umbral superior (3) y la que esta entre ambos umbrales (2).
*La función compara this->nMedida con los umbrales y determina la zona que le toca, comprueba si la medida se encuentra en la
*zona durante un numero de veces consecutivas (this->_nOcurrenciasParam), si es asi, establece la zona del sensor registrandola
*en this->nZona que es un parametro accesible fuera de la clase.
*Si en la medida actual se produce un cambio de zona, el flag this->_lZona se pone a 1, este flag lo utiliza la funcion this->Medida
*para informar al programa que llama a la funcion de que ha habido un cambio de zona 
*/ 
void SENSORDS18B20::_Zona ()
{
		if ( this->nMedida < this->UmbralInferior  )
		{	
			this->_nOcurrenciasZona2 = 0;
			this->_nOcurrenciasZona3 = 0;
			if ( this->_nOcurrenciasZona1 > this->_nOcurrenciasParam )
			{
				this->nZona = 1;
			}else{
				this->_nOcurrenciasZona1++;			
			}
		}else{
			if ( this->nMedida > this->UmbralSuperior )
			{
				this->_nOcurrenciasZona1 = 0;
				this->_nOcurrenciasZona2 = 0;
				if (this->_nOcurrenciasZona3 > this->_nOcurrenciasParam )
				{
					this->nZona = 3;
				}else{
					this->_nOcurrenciasZona3++;
				}	
			}else{
				this->_nOcurrenciasZona1 = 0;
				this->_nOcurrenciasZona3 = 0;
				if (this->_nOcurrenciasZona2 > this->_nOcurrenciasParam )
				{
					this->nZona = 2;
				}else{
					this->_nOcurrenciasZona2++;				
				}	
			}
		}


		if ( this->nZona != this->_nZonaAnterior )
		{
			this->_nMicrosegundosZona = micros();
			this->_lZona = 1;
			this->_nZonaAnterior = this->nZona;
		}
}
/**
******************************************************
*@brief Indica si ha habido cambio de zona
*
* Devuelve el flag _lZona que indica con un 1 si ha habido cambio de zona 
* resetea el flag
*
*@return Deveulve 1 si ha habido cambio de zon, 0 en caso contrario
*
*/ 
boolean SENSORDS18B20::_CambioZona (void)
{
	boolean _lZonaTemp = this->_lZona;
	this->_lZona = 0;
	return (_lZonaTemp);
}
/**
******************************************************
*@brief Devuelve el numero de segundos que el sensor se encuentra en la zona actual
*
*@return Deveulve el numero de segundos que el sensor se encuntra en la zona actual
*
*/ 
int SENSORDS18B20::TiempoZona (void)
{
	long nMicrosegundos = micros();
	long nSegundos = 0;
	if ( nMicrosegundos < this->_nMicrosegundosZona )
	{
		nSegundos = pow (2,32) -  ( this->_nMicrosegundosZona ) + nMicrosegundos;
	}else{
		nSegundos = (nMicrosegundos - this->_nMicrosegundosZona);
	}
	return ((int)(nSegundos/1000000));
}  
/**
******************************************************
*@brief Habilita la alarma con los parametros transferidos
*
*El sensor puede manejar una alarma de cuando se esceden los umbrales, por ejemplo supongamos que el sensor se utiliza 
*como termostato para encender una calefaccion. Cuando la temperatura cae por debajo del umbral inferior y esta en 
*esa zona durante al menos un tiempo ( nSegundosUI ), se pone el flag de alarma a 1 para indicar al programa principal
*la situacion y este, encendera la calefacción. La temperatura empezara a subir y cuando supere el umbral superior durante
*un tiempo concreto ( nSegundosUS ), el flag se pondra a 0 para indicar al programa principal que puede apagar la calefaccion
*
*
*@param lTipoAlarma.- Tipo de alarma 0 alarma por debajo del umbral inferior, 1 alarma por encima del umbral superior
*@param nSegundosUS.- Segundos que la medida debe estar por encima del US para activar o desacivar la alarma
*@param nSegundosUI.- Segundos que la medida debe estar por debajo del UI para activar o desacivar la alarma
*/ 
void SENSORDS18B20::Alarma ( boolean lTipoAlarma, int nSegundosUS, int nSegundosUI )
{
	this->_lAlarmaEnable = 1;			//habilitamos la alarma
	this->_lTipoAlarma = lTipoAlarma;
	this->_nSegundosAlarmaUmbralSuperior = nSegundosUS;
	this->_nSegundosAlarmaUmbralInferior = nSegundosUI;
}
/**
******************************************************
*@brief Realiza una medida y testea si ha habido alarma
*
*Esta funcion realiza una medida y comprueba el valor de la medida permanece en zona 1 o 3, los 
*tiempos que se han pasado en la funcion alarma para responder con disparo de alarma si en esta medida
*se han excedido esos tiempos. Se pone el flag lAlarma a 1 si hay alarma y 0 si se ha quitado la alarma
*
*Esta funcion se utiliza en el motor del programa principal para que se haga una medida y le avise de
*que ha ocurrido un evento qu epuede ser alarma on o alarma off 
*
*@return Devuelve 1 si en la ultima medida se ha disparado la alrama ( si se ha activado o se ha desactivado), 0 en caso contrario
*/ 
boolean SENSORDS18B20::TestSensor(void)
{
	boolean lAlarmaAnterior = this->lAlarma;
	this->Medida();
	if (this->_lAlarmaEnable)						//Si la alarma esta habilitada
	{
		if (this->_lTipoAlarma)						//Si el tipo de alarma es por umbral superior
		{
			if ( this->nZona == 3 )					//Si se encuentra en Zona 3
			{
				if ( this->TiempoZona() >= this->_nSegundosAlarmaUmbralSuperior )
				{
					this->lAlarma = 1;
				}
			}
			if ( this->nZona == 1)
			{
				if ( this->TiempoZona() >= this->_nSegundosAlarmaUmbralInferior )
				{
					this->lAlarma = 0;
				}				
			}
		}else{
			if ( this->nZona == 1)
			{
				if ( this->TiempoZona() >= this->_nSegundosAlarmaUmbralInferior )
				{
					this->lAlarma = 1;
				}				
			}			
			if ( this->nZona == 3 )					//Si se encuentra en Zona 3
			{
				if ( this->TiempoZona() >= this->_nSegundosAlarmaUmbralSuperior )
				{
					this->lAlarma = 0;
				}
			}
		}
	}
	if ( this->lAlarma == lAlarmaAnterior )
	{
		return(0);
	}else{
		return(1);
	}	
}
/**
******************************************************
*@brief Devuelve si ha habido un cambio de conectado/desconectado del sensor
*
* Esta función sirve para suervisar si se ha desconectado accidentalmente el sensor o se ha vuelto a conectar
*@return Deveulve 1 si ha habido cambio de estado, 0 en caso contrario
*
*/ 
boolean SENSORDS18B20::TestConexionSensor(void)
{
		boolean lSalida = 0;
		if ( _lSensorDesconectado != _lSensorDesconectadoAnterior )
		{
			lSalida = 1;
			_lSensorDesconectadoAnterior = _lSensorDesconectado;
		};
		return ( lSalida );
}
/**
******************************************************
*@brief Devuelve si el Sensor esta conectado o no
*
* Esta función sirve para suervisar si se ha desconectado accidentalmente el sensor o se ha vuelto a conectar
*@return Deveulve 0 si el sensor esta conectado, 1 en caso contrario
*
*/ 
boolean SENSORDS18B20::ConexionSensor (void)
{
	return (_lSensorDesconectado);
}
/**
******************************************************
*@brief Asigna un offset al sensor
*
*@param 
*
*/ 
void SENSORDS18B20::SetOffset ( float nOffset)
{
	this->_Offset = nOffset;
}
/**
******************************************************
*@brief Devuelve el offset asignado al sensor
*
*@return Deveulve el offset asigndo al sensor
*
*/ 
float SENSORDS18B20::ReadOffset (void)
{
	return (this->_Offset);
}