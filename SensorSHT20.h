#ifndef SENSORSHT20_H
#define SENSORSHT20_H

/**
******************************************************
* @file SensoTemperatura.h
* @brief Clase SENSORSHT20
* @author Julian Salas Bartolome
* @version 1.0
* @date Septiembre 2020
*
* Esta clase gestiona un sensor de temperatura basado en el SHT20
*
* La funcion principal se encarga de hacer una medida con el sensor y almacenarla en una variable
* que se ira incrementando con medidas conforme se vaya llamando a la funcion. Cuando se han realizado 
* un numero concreto de medidas ( pasado como parametro en el constructor o 10 por defecto), se calcula 
* la media de todas las medidas y se le asigna a la variable nMedida que es accesible desde el exterior
* de la clase. La temperatura esta corregida con el offset registrado en el sensor.
*
* El sensor tiene la posibilidad de establecer unos umbrales para aquello que nos pueda interesar. por ejemplo
* para encender una calefaccion cuando la temperatura esta por debajo de un umbral y apagarla cuando este por encima
* de otro umbral.
* Cuando se obtien una medida, se compara con el umbral inferior y el superior y se le asigna una zona a la medida
* Si la medida esta por debajo del umbral inferior, estara en zona 1, si esta por enciam del superior esta en zona 3
* y si esta entre los umbrales, esta en zona 2
* Cuando la medida esta poroxima a un umbral, para que no haya cambios de zona no deseados, se establece que la zona
* es valida cuando la medida se localiza en esa zona durante un numero determinado de ocasiones consecutivas, el
* numero de ocurrencias se establece con la función SetUmbrales.
* Al programa principal, ademas del valor de la medida ( nMedida accesible desde el exterior ) le interesara cuando 
* se produce un cambio de zona ( para apagar/encender la calefaccion en nuestro ejemplo ) y esto la clase lo indica
* al programa principal devolviendo un 1 o 0 en la funcion de medida para indicar si ha habiso o no cambio de zona
* La facilidad de determinar la zona se puede habilitar/deshabilitar suegun la necesidad.
* El sensor se puede habilitar/deshabilitar segun necesidad.
* para que cuando se conecte el sensor al servidor haya una medida valida, existe la funcion MeidasInicio () que lo que 
* hace es un ciclo completo de medidas 
* Para conocer las posibilidades de la clase, es ecesario consultar las variables y funciones publicas
* 
*
*
*****************************************************
**/


#if ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif

    #include <Wire.h>
    #include "DFRobot_SHT20.h"

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





//extern WiFiClient Cliente;  
extern Telegrama oMensaje;
extern String cDispositivo;




class SENSORSHT20 //Definicion de la clase
{
    public:

        float nMedida;                                                                              //Valor de la ultima medida ( valor medio del total de muestras)    
        int nZona = 0;                                                                              //Zona en la que se encuntra la medida del sensor ( inicialmente zona indeterminada, 0 )

        SENSORSHT20( byte Direccion,  int nMuestras );                                              //!<Constructor
        SENSORSHT20( byte Direccion );                                                              //!<Constructor        
        ~SENSORSHT20();                                                                             //!<Destructor

        void begin(void);                                                                           //!<Arranque sensor
        void MedidasInicio (void);                                                                  //Realiza un ciclo de medidas completo para obtener el primer dato de media                                                     
        void SetUmbrales (float UmbralSuperior, float UmbralInferior, int nOcurrencias);            //!<Fija los unmbrales y las veces que el valor tiene que estar en una zona para fijarlo en esa zona
        void SetUmbrales (float UmbralSuperior, float UmbralInferior );                             //!<Fija los unmbrales sin cambiar las veces que el valor tiene que estar en una zona para fijarlo en esa zona es 
//        void Init (void);
    	void On (void);                                                                             //!<Habillita el sensor                                                               
    	void Off (void);                                                                            //!<Deshabillita el sensor                                                            
        boolean GetOnOff (void);                                                                    //!<Indica si el sensor esta o no habilitado

        void ZonaOn (void);                                                                         //!<Habillita la determinacion de zona                                                               
        void ZonaOff (void);                                                                        //!<Deshabillita la determinacion de zona                                                            
        int TiempoZona (void);                                                                      //!<Devuelve el numero de segundos en la zona actual
        boolean Medida (void);                                                                      //!<Realiza una medida y actualiza el valor acumulado
        void Alarma ( boolean lTipoAlarma, int nSegundosUS, int nSegundosUI );                      //!<Establece los parametros de la alarma y la habilita
        boolean TestSensor (void);                                                                  //!<Realiza una medida y testea si ha habido alarma            
        boolean TestConexionSensor(void);                                                           //!<Indica si ha habido un cambio o no en la conexion del sensor
        boolean ConexionSensor (void);                                                              //!<Indica si el sensor está desconectado o o
        void SetOffset (float nOffset);                                                             //!<Establece el Offset del sensor
        float ReadOffset (void);                                                                    //!<Devuelve el Offset fijado para el sensor

        //Nuevas funciones no existentes en DS18B20
        void EnableAlarm (void);                                                                    //!<Habilita la alarma                                              
        void DisableAlarm (void);                                                                   //!Deshabilita la alarma 
        boolean GetAlarm (void);                                                                    //!<Devuelve si la alarma esta habilitada o no
 
        boolean _GetEstadoAlarma (void);                                                            //!<Devuelve el estado de la alarma



        void Automatico (void);                                                                     //!<Pone el dispositivo en Automatico
        void Manual (void);                                                                         //!<Pone el dispositivo en manual
        boolean GetAutomaticoManual (void);                                                         //!<Devuelve 1 o 0 para indicar si esta en automatico o manual

        float GetTemperaturaInstantanea (void);                                                     //!<Realiza una medida y devuewlve su valor teniendo en cuenta el Offset

        void SetConsigna (float nConsigna);                                                         //!<Establece la consigna
        float GetConsigna (void);                                                                   //!<Devuelve el valor de la consigna


    protected:


    private:   
    	
        DFRobot_SHT20    _sht20;                                                                    //Objeto SHT20

        byte _Direccion ;                                                                           //Direccion I2c del  sensor SHT20
        int _nMuestrasMedida = 0;                                                                   //Numero de muestras que tomará el sensor para obtener un valor medio
        int _nMuestra = 0;                                                                          //Numero de muestra actual. Indice de la muestra actual dentro del paquete _nMuestrasMedida

        float _Valor = 0;                                                                           //Volor de la medida actual realizada por el sensor
        float _ValorAcumulado = 0;                                                                  //Valor de la suma de las medidas actuales 
        float _Valormedio = 0;                                                                      //Valor medio de las medidas acumuladas (_ValorAcumulado/_nMuestrasMedida)

        int _nOcurrenciasParam = 0;                                                                 //Numero de veces consecutivas que la medida debe etar en una zona para considerar que se encuentra de forma estable en esa zona
        int _nOcurrenciasZona1 = 0;                                                                 //Numero de veces consecutivas que la medida se encuentra en zona 1
        int _nOcurrenciasZona2 = 0;                                                                 //Numero de veces consecutivas que la medida se encuentra en zona 2
        int _nOcurrenciasZona3 = 0;                                                                 //Numero de veces consecutivas que la medida se encuentra en zona 3

        boolean _lFlagZona = 0;                                                                     //Flag que habilita la determinacion de zona
        boolean _lZona = 0;                                                                         //Flag que indica si ha habido un cambio de zona
        int _nZonaAnterior = 0;                                                                     //Zona anterior

        long _nMicrosegundosZona;                           

        boolean _lOnOff = 0;                                                                        //Flag que indica si el sensor está habilitado o no
        boolean _lAlarmaEnable = 0;                                                                 //Flag que habillita/deshabilita la alarma
        boolean _lTipoAlarma = 0;                                                                   //Tipo de alarma, 0 por umbral bajo, 1 por umbral alto
        boolean _lAutomaticoManual = 0;                                                             //Flag Automatico/Manual (0/1)
        int _nSegundosAlarmaUmbralSuperior;                                                         //Segundos en zona 3 para activar alarma
        int _nSegundosAlarmaUmbralInferior;                                                         //Segundos en zona 1 para activar alarma
        float UmbralSuperior;                                                                       //Umbral Superior para determinar alarma 
        float UmbralInferior;                                                                       //Umbral Inferior para determinar alarma
        boolean lAlarma;                                                                            //Flag que indica si el sensor se encuentra o no en alarma

        boolean _lSensorDesconectado;                                                               //Flag que indica si el sensor SHT20 está desconcetado o no ( 1/0 )
        boolean _lSensorDesconectadoAnterior;                                                       //Flag que indica el estado anterior de la conexion del sensor

        void _Zona ( void );                                                                        //Determina la zona donde se encuentra la medida                                                       
        boolean _GetZonaOnOff (void);                                                               //Indica si la determinacion de zona esta o no habilitado
        boolean _CambioZona ();                                                                     //Indica si en la ultima medida ha habido cambio de zona

        float _Offset;                                                                              //Offset del sensor
        float _Consigna;                                                                            //Consigna del sensor    

};

#endif
