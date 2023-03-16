
#include "SensorSHT20.h"

/**
******************************************************
*@brief Constructor de la clase
*
*@param PinDS18B20.- Pin que se va a utilizar para la comunicacion OneWire
*@param nMuestras.- Nuemro de muestras que se van a utilizar para obtener el valor medio del sensor
*
*/ 
SENSORSHT20::SENSORSHT20( byte Direccion,  int nMuestras )
{
	this->_Direccion = Direccion;
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
SENSORSHT20::SENSORSHT20( byte Direccion )
{
	this->_Direccion = Direccion;
	this->_nMuestrasMedida = 10;
	this->_lOnOff = 1;	
	this->_lSensorDesconectado = 0;
	this->_lSensorDesconectadoAnterior = 0;			
	this->begin();


}
/**
******************************************************
*@brief realiza un ciclo entero de medidas para los datos iniciales
*
*
*/
void SENSORSHT20::MedidasInicio (void)
{
	int nMedidas = 0;
	while ( nMedidas < this->_nMuestrasMedida )
	{
		this->Medida();
		nMedidas++;
	}
}
/**
******************************************************
*@brief Destructor de la clase
*
*/
SENSORSHT20::~SENSORSHT20( )
{

}

/**
******************************************************
*@brief Arranque del sensor
*
*/
void SENSORSHT20::begin (void)
{
	_sht20.initSHT20();                                  // Init SHT20 Sensor

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
void SENSORSHT20::SetUmbrales (float UmbralSuperior, float UmbralInferior, int nOcurrencias )
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
void SENSORSHT20::SetUmbrales (float UmbralSuperior, float UmbralInferior )
{	

	this->UmbralSuperior = UmbralSuperior;
	this->UmbralInferior = UmbralInferior;

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
void SENSORSHT20::_Zona ()
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
boolean SENSORSHT20::_CambioZona (void)
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
int SENSORSHT20::TiempoZona (void)
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
*@brief Habilita la determinacion de la zona donde se encuentra el sensor
*
*/ 
void SENSORSHT20::ZonaOn (void)
{
	this->_lFlagZona = 1;
}
/**
******************************************************
*@brief Deshabilita la determinacion de la zona donde se encuentra el sensor
*
*/ 
void SENSORSHT20::ZonaOff (void)
{
	this->_lFlagZona = 0;
}
/**
******************************************************
*@brief Funcion para conocer si el si la determinacion de zona esta habilitado/deshabilitado 
*
*return.- Devuelve 1/0 para indicar si el sensor esta habilitado/Deshabilitado
*/ 
boolean SENSORSHT20::_GetZonaOnOff (void)
{
	return (this->_lFlagZona);
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
void SENSORSHT20::Alarma ( boolean lTipoAlarma, int nSegundosUS, int nSegundosUI )
{
	this->_lAlarmaEnable = 1;			//habilitamos la alarma
	this->_lTipoAlarma = lTipoAlarma;
	this->_nSegundosAlarmaUmbralSuperior = nSegundosUS;
	this->_nSegundosAlarmaUmbralInferior = nSegundosUI;
}
/**
******************************************************
*@brief Asigna un offset al sensor
*
*@param 
*
*/ 
void SENSORSHT20::SetOffset ( float nOffset)
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
float SENSORSHT20::ReadOffset (void)
{
	return (this->_Offset);
}
/**
******************************************************
*@brief Asigna una consigna al sensor
*
*@param 
*
*/ 
void SENSORSHT20::SetConsigna ( float nConsigna)
{
	this->_Consigna = nConsigna;
}
/**
******************************************************
*@brief Devuelve la consigna del sensor
*
*@return Deveulve la consigna del sensor
*
*/ 
float SENSORSHT20::GetConsigna (void)
{
	return (this->_Consigna);
}
/**
******************************************************
*@brief Habilita el sensor 
*
*/ 
void SENSORSHT20::On (void)
{
	this->_lOnOff = 1;
}
/**
******************************************************
*@brief Deshabilita el sensor 
*
*/ 
void SENSORSHT20::Off (void)
{
	this->_lOnOff = 0;
}
/**
******************************************************
*@brief Funcion para conocer si el sensor esta habilitado/deshabilitado 
*
*return.- Devuelve 1/0 para indicar la medida es corre
*/ 
boolean SENSORSHT20::GetOnOff (void)
{
	return (this->_lOnOff);
}

/**
******************************************************
*@brief Funcion para conocer si la alarma esta habilitada o no 
*
*return.- Devuelve 1/0 para indicar si esta habilitada o no
*/ 

boolean SENSORSHT20::_GetEstadoAlarma (void)
{
	//Serial.println("Hola");
	return (this->lAlarma);
}

//-------
//PRUEBAS
//-------

boolean SENSORSHT20::Medida (void)
{
//	_sht20.checkSHT20();    
//	Serial.println ("Chequeado");
//	float humd = _sht20.readHumidity(); 
	this->_Valor = _sht20.readTemperature();  

	if ( this->_Valor > 250 )							//Si la lectura no es logica, dareos la sonda por desconectada
	{
Serial.print ( "Sonda Off ");
		_lSensorDesconectado = 1;
		this->_ValorAcumulado=0;		
		this->_nMuestra = 0;
	}else{
		_lSensorDesconectado = 0;
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
	}

    
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
boolean SENSORSHT20::TestSensor(void)
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
boolean SENSORSHT20::TestConexionSensor(void)
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
boolean SENSORSHT20::ConexionSensor (void)
{
	return (_lSensorDesconectado);
}
/**
******************************************************
*@brief Habilita la alarma
*
* Esta función sirve para habililtar la alarma
*
*/ 
void SENSORSHT20::EnableAlarm (void)
{
	this->_lAlarmaEnable = 1;
}
/**
******************************************************
*@brief Deshabilita la alarma
*
* Esta función sirve para hdesabililtar la alarma
*
*/ 
void SENSORSHT20::DisableAlarm (void)
{
	this->_lAlarmaEnable = 1;
}
/**
******************************************************
*@brief Interroga si la alarma esta´habilitada o deshabilitada
*
* Esta función sirve para hdesabililtar la alarma
*
*/ 
boolean SENSORSHT20::GetAlarm (void)
{
	return(this->_lAlarmaEnable);
}
/**
******************************************************
*@brief Pone el dispositivo en Automatico
*
*/ 
void SENSORSHT20::Automatico (void)
{
	this->_lAutomaticoManual = 0;
}
/**
******************************************************
*@brief POne el dispositivo en manual
*
* Esta función sirve para hdesabililtar la alarma
*
*/ 
void SENSORSHT20::Manual (void)
{
	this->_lAutomaticoManual = 1;
}
/**
******************************************************
*@brief Interroga si la alarma esta´habilitada o deshabilitada
*
* Esta función sirve para hdesabililtar la alarma
*
*/ 
boolean SENSORSHT20::GetAutomaticoManual (void)
{
	return(this->_lAutomaticoManual);
}
/**
******************************************************
*@brief Realiza una medida y devuelve su valor con offset
*
* Realiza una medida y devuelve la temepratura sin esperar a tener
* un numero de muetsras determinadas
*
*/ 
float SENSORSHT20::GetTemperaturaInstantanea (void)
{
	this->_Valor = _sht20.readTemperature();  
	return (this->_Valor + this->_Offset);
}