#include "TouchTermostato.h"

TOUCHTERMOSTATO::TOUCHTERMOSTATO()
{
}

TOUCHTERMOSTATO::~TOUCHTERMOSTATO (void)
{
}

void TOUCHTERMOSTATO::begin(int AddrMPR121, int PinIRQ)
{
    
    
    if (!MPR121.begin(AddrMPR121)) {
        Serial.println("error en configuracion de  MPR121");
        switch (MPR121.getError()) {
          case NO_ERROR:
            Serial.println("No hay error");
            break;
          case ADDRESS_UNKNOWN:
            Serial.println("ireccion incorrecta");
            break;
          case READBACK_FAIL:
            Serial.println("Fallo ReadBack");
            break;
          case OVERCURRENT_FLAG:
            Serial.println("sobrecorriente en REXT pin");
            break;
          case OUT_OF_RANGE:
            Serial.println("electrodo fuera de rango");
            break;
          case NOT_INITED:
            Serial.println("no inicializado");
            break;
          default:
            Serial.println("error desconocido");
            break;
        }
    
    }



    MPR121.setInterruptPin(PinIRQ);

    if (MPR121_DATASTREAM_ENABLE) {
      MPR121.restoreSavedThresholds();
      MPR121_Datastream.begin(&Serial);
    } else {
      MPR121.setTouchThreshold(15);    // Umbral para pulsado 40 ahora 50   25  20
      MPR121.setReleaseThreshold(10);  // Umbral para soltado 30 ahora 40   20  15 
    }

    MPR121.setFFI(FFI_10);              //10
    MPR121.setSFI(SFI_10);              //10
    MPR121.setGlobalCDT(CDT_4US);     // Para capacidades grandes  4US
    
    MPR121.autoSetElectrodes();  
    MPR121.setNumDigPins(1);          //Pin 11 Salida
    MPR121.pinMode(CtrlDisplay, OUTPUT);

}
void TOUCHTERMOSTATO::Refresh(void)
{
    int nBotonPulsado = -1;
    if(MPR121.touchStatusChanged()){
      MPR121.updateAll();
      for (int i = 0; i < 12; i++) 
      {
        
        if (MPR121.isNewRelease(i) ) {
          Serial.print("Electrodo ");
          Serial.print(i, DEC);
          Serial.println(" soltado");
          nBotonPulsado = i;
        }      
      } 
    }  
    if (MPR121_DATASTREAM_ENABLE) {
        MPR121_Datastream.update();
    }
  
}
void TOUCHTERMOSTATO::Scan(void)
{
    int nBotonPulsado = -1;
    if(MPR121.touchStatusChanged()){
      MPR121.updateAll();
      for (int i = 0; i < 12; i++) 
      {
        if (MPR121.isNewTouch(i)) {
          Serial.print("Electrodo ");
          Serial.print(i, DEC);
          Serial.println(" pulsado");
        }        
        if (MPR121.isNewRelease(i) ) {
          Serial.print("Electrodo ");
          Serial.print(i, DEC);
          Serial.println(" soltado");
          nBotonPulsado = i;
        }      
      } 
    }  
    if (MPR121_DATASTREAM_ENABLE) {
        MPR121_Datastream.update();
    }

    if (nBotonPulsado >-1)
    {
        this->OrdenTouch (nBotonPulsado);
    }        
}
/**
******************************************************
* @brief Recibe una tecla pulsada y ejecuta la orden asignada
*
*/
void TOUCHTERMOSTATO::OrdenTouch (int nTouch)
{
  switch (nTouch)
  {
  case TouchEnter:
    this->Enter();
    break;
  case TouchLeft:
    this->Left();
    break;
  case TouchRight:
    this->Right();
    break; 
  case TouchAM:
    if (this->nModo == ModoNormal)
    {
      this->ProgAM();
    }  
    break;
  case TouchOnOff:
    if (this->nModo == ModoNormal)
    {
      this->ProgOnOff();
    }
    break;
  }
}


/**
******************************************************
* @brief Gestiona la tecla Enter cambiando entre los distintos modos de programacion
*
*/
void TOUCHTERMOSTATO::Enter (void)
{
  switch (this->nModo)
  {
    case ModoNormal:
      this->nModo = ModoProgConsigna;
      this->DisplayPrgConsigna();
      break;
    case ModoProgConsigna:
      this->nModo = ModoProgAlarmaH;
      this->DisplayPrgAlarmaH();
      break;
    case ModoProgAlarmaH:
      this->nModo = ModoProgAlarmaL;
      this->DisplayPrgAlarmaL();
      break;
    case ModoProgAlarmaL:
      this->nModo = ModoProgOffset;
      this->DisplayPrgOffset();
      break;
    case ModoProgOffset:
      this->nModo = ModoNormal;
      this->DisplayNormal();
      break;


  }
}
/**
******************************************************
* @brief Gestiona la tecla Left. Llama a la función del modo en el que nos encontremos
*
*/
void TOUCHTERMOSTATO::Left (void)
{
  switch (this->nModo)
  {
    case ModoNormal:
      break;
    case ModoProgConsigna:
      this->LeftPrgConsigna();
      break;
    case ModoProgAlarmaH:
      this->LeftPrgAlarmaH();
      break;  
    case ModoProgAlarmaL:
      this->LeftPrgAlarmaL();
      break;  
    case ModoProgOffset:
      this->LeftPrgOffset();
      break;  
  }
}

/**
******************************************************
* @brief Gestiona la tecla Right. Llama a la función del modo en el que nos encontremos
*
*/
void TOUCHTERMOSTATO::Right (void)
{
  switch (this->nModo)
  {
    case ModoNormal:
      break;
    case ModoProgConsigna:
      this->RightPrgConsigna();
      break;
    case ModoProgAlarmaH:
      this->RightPrgAlarmaH();
      break;  
    case ModoProgAlarmaL:
      this->RightPrgAlarmaL();
      break;  
    case ModoProgOffset:
      this->RightPrgOffset();
      break;  
  }  
}

//---------------------
//Programacion Consigna
//---------------------
void TOUCHTERMOSTATO::DisplayPrgConsigna (void)
{
  this->_tft.fillScreen(ColorFondo);
  this->WriteTitulo("Consigna");
  this->WriteTxtTemperatura ( this->_nConsigna );
}
void TOUCHTERMOSTATO::LeftPrgConsigna (void)
{
  if ( this->_nConsigna > TemperaturaMinima )
  {
    this->_nConsigna = this->_nConsigna - 0.5;
    this->ClearTxtTemperatura ();
    this->WriteTxtTemperatura ( this->_nConsigna );
  } 
}
void TOUCHTERMOSTATO::RightPrgConsigna (void)
{
  if ( this->_nConsigna < TemperaturaMaxima )
  {
    this->_nConsigna = this->_nConsigna + 0.5;
    this->ClearTxtTemperatura ();
    this->WriteTxtTemperatura ( this->_nConsigna );
  }
}

//------------------------
//Programacion Alarma Alta
//------------------------
void TOUCHTERMOSTATO::DisplayPrgAlarmaH (void)
{
  this->_tft.fillScreen(ColorFondo);
  this->WriteTitulo("Alarma Max");
  this->WriteTxtTemperatura ( this->_nTemperaturaAlarmaMaxima );
}
void TOUCHTERMOSTATO::LeftPrgAlarmaH (void)
{
  if ( this->_nTemperaturaAlarmaMaxima > (this->_nTemperaturaAlarmaMinima + 1 ) )
  {
    this->_nTemperaturaAlarmaMaxima = this->_nTemperaturaAlarmaMaxima - 1;
    this->ClearTxtTemperatura ();
    this->WriteTxtTemperatura ( this->_nTemperaturaAlarmaMaxima );
  }   
}
void TOUCHTERMOSTATO::RightPrgAlarmaH (void)
{
  if ( this->_nTemperaturaAlarmaMaxima < TemperaturaMaxima )
  {
    this->_nTemperaturaAlarmaMaxima = this->_nTemperaturaAlarmaMaxima + 1;
    this->ClearTxtTemperatura ();
    this->WriteTxtTemperatura ( this->_nTemperaturaAlarmaMaxima );
  }   
}

//------------------------
//Programacion Alarma Baja
//------------------------
void TOUCHTERMOSTATO::DisplayPrgAlarmaL (void)
{
  this->_tft.fillScreen(ColorFondo);
  this->WriteTitulo("Alarma Min");
  this->WriteTxtTemperatura ( this->_nTemperaturaAlarmaMinima );
}
void TOUCHTERMOSTATO::LeftPrgAlarmaL (void)
{
  if ( this->_nTemperaturaAlarmaMinima > ( TemperaturaMinima) )
  {
    this->_nTemperaturaAlarmaMinima = this->_nTemperaturaAlarmaMinima - 1;
    this->ClearTxtTemperatura ();
    this->WriteTxtTemperatura ( this->_nTemperaturaAlarmaMinima );
  }   
}
void TOUCHTERMOSTATO::RightPrgAlarmaL (void)
{
  if ( this->_nTemperaturaAlarmaMinima < (this->_nTemperaturaAlarmaMaxima - 1 ))
  {
    this->_nTemperaturaAlarmaMinima = this->_nTemperaturaAlarmaMinima + 1;
    this->ClearTxtTemperatura ();
    this->WriteTxtTemperatura ( this->_nTemperaturaAlarmaMinima );
  }   
}
//------------------------
//Programacion Offset
//------------------------
void TOUCHTERMOSTATO::DisplayPrgOffset (void)
{
  Serial.print("----------->");
  Serial.print(this->_nOffset);
  Serial.println ("<--------");
  this->_tft.fillScreen(ColorFondo);
  this->WriteTitulo("Offset");
  this->WriteTxtTemperatura ( this->_nOffset );
}
void TOUCHTERMOSTATO::LeftPrgOffset(void)
{
    this->_nOffset = this->_nOffset - 0.5;
    this->ClearTxtTemperatura ();
    this->WriteTxtTemperatura ( this->_nOffset );

}
void TOUCHTERMOSTATO::RightPrgOffset(void)
{
    this->_nOffset= this->_nOffset + 0.5;
    this->ClearTxtTemperatura ();
    this->WriteTxtTemperatura ( this->_nOffset );

}
//------------------------
//Programacion Manual Automatico
//------------------------
void TOUCHTERMOSTATO::ProgAM (void)
{
  if ( this->_lAM )
  {
    this->WriteTxtAutomatico();
    this->_lAM = 0;
  }else{
    this->_lAM = 1;
    this->WriteTxtManual();
  }
}

void TOUCHTERMOSTATO::ProgOnOff (void)
{
   if ( this->_lAM )
   {
      if ( this->_lOnOff )
      {
        this->_lOnOff = 1;
        this->WriteTxtOn();
      }else{
        this->_lOnOff = 0;
        this->WriteTxtOff();
      }    
   }
}


//---------------
//Pantalla Normal
//---------------
void TOUCHTERMOSTATO::DisplayNormal (void)
{
  this->_tft.fillScreen(ColorFondo);
  this->DrawLineas();
  switch (_nMododVisualizacion)
  {
    case (1):
        this->DisplayMarcas(this->_nTemperatura);
        break;
    case (2):
        this->DisplayMarcas(this->_nTemperatura, this->_nConsigna);
        break;
    case (3):
        this->DisplayMarcas(this->_nTemperatura, this->_nTemperaturaAlarmaMaxima, this->_nTemperaturaAlarmaMinima);
        break;
    case (4):
        this->DisplayMarcas(this->_nTemperatura, this->_nConsigna, this->_nTemperaturaAlarmaMaxima, this->_nTemperaturaAlarmaMinima);
        break;
  }
  if ( !this->_lAM)
  {
    this->WriteTxtAutomatico ();
  }else{
    this->WriteTxtManual ();
  }
  if ( this->_lOnOff)
  {
    this->WriteTxtOff ();
  }else{
    this->WriteTxtOn ();
  }
  this->WriteTxtOffset(this->_nOffset);
}


uint8_t TOUCHTERMOSTATO::TeclasPulsadas (void)
{
  return ( MPR121.getNumTouches() );
}

void TOUCHTERMOSTATO::DisplayOn (void)
{
   MPR121.digitalWrite(CtrlDisplay, 1);
}
void TOUCHTERMOSTATO::DisplayOff (void)
{
   MPR121.digitalWrite(CtrlDisplay, 0);
}