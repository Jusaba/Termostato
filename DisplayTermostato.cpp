#include "DisplayTermostato.h"



/**
******************************************************
*@brief Constructor de la clase
*
*
*/ 
DISPLAYTERMOSTATO::DISPLAYTERMOSTATO()
{
  //_tft = tft;
  this->_tft.init(); 
  this->_tft.setRotation(0);
  this->_tft.setPivot(120, 120);
  this->_tft.fillScreen(ColorFondo);
  //this->CreaSpriteMarca();
  //this->CreaSpriteTemperatura();
  this->CreaSpriteLinea();
  this->CreaSpriteNumeros();
  //_marca = TFT_eSprite(&(_tft));
  this->nModo = ModoNormal;

}  
DISPLAYTERMOSTATO::~DISPLAYTERMOSTATO (void)
{

}

/**
******************************************************
* @brief Dibuja un circulo de temperatura 
*
* El circulo representa desde la temperatura TemperaturaMinima a la TemperaturaMaxima con una amplitud de
* AnguloInferior a AnguloSuperior
* El circulo se pinta en azul desde la temperatura inferior hasta la temperatura actual y rojo desde la temperatura actual hasta la temperatura superior
*
* @param nTemperatura. Temperatura actual que se quiere representar
*
*/
void DISPLAYTERMOSTATO::Circulo (float nTemperatura)
{
    int nTLow = TemperaturaMinima;
    int nTHigh = TemperaturaMaxima;
    int32_t nAnguloI = AnguloInferior;
    int32_t nAnguloF = AnguloSuperior;
    int32_t nAnguloTemperatura = ( nTemperatura - nTLow )*(nAnguloF - nAnguloI)/(nTHigh - nTLow) ;
    this->_tft.drawSmoothArc(120, 120, 100, 95, nAnguloI, nAnguloI+nAnguloTemperatura, TFT_BLUE, TFT_DARKCYAN, false);
    this->_tft.drawSmoothArc(120, 120, 100, 95, nAnguloI+nAnguloTemperatura, nAnguloF, TFT_RED, TFT_DARKCYAN, false);
    this->WriteTxtTemperatura (nTemperatura);
    this->ClearTxtConsigna();
    this->ClearTxtAlarmas();
}

/**
******************************************************
* @brief Dibuja un circulo de temperatura con la consigna
*
* Circulo con temperatura y consigna
* El circulo representa desde la temperatura TemperaturaMinima a la TemperaturaMaxima con una amplitud de
* AnguloInferior a AnguloSuperior
* El circulo se p0inta en gris y en rojo desde lka temperatura actual hasta la consgina siempre que la temperatura sea inferior a la consgina
* en caso contrario, se pinta de azul desde la consigna hasta la tyemperatura actual
*
* @param nTemperatura. Temperatura actual que se quiere representar
* @param nConsigna. Temperatura de consigna del termostato
*/
void DISPLAYTERMOSTATO::Circulo ( float nTemperatura, float nConsigna )
{
  int nTLow = TemperaturaMinima;
  int nTHigh = TemperaturaMaxima;
  int32_t nAnguloI = AnguloInferior;
  int32_t nAnguloF = AnguloSuperior;
  int32_t nAnguloTemperatura = ( nTemperatura - nTLow )*(nAnguloF - nAnguloI)/(nTHigh - nTLow) ;
  int32_t nAnguloConsigna = ( nConsigna - nTLow )*(nAnguloF - nAnguloI)/(nTHigh - nTLow) ;
  if ( nConsigna > nTemperatura )
  {
    this->_tft.drawSmoothArc(120, 120, 100, 95, nAnguloI, nAnguloI+nAnguloTemperatura, TFT_DARKGREY, TFT_DARKCYAN, false);
    this->_tft.drawSmoothArc(120, 120, 100, 95, nAnguloI+nAnguloTemperatura, nAnguloI+nAnguloConsigna, TFT_RED, TFT_DARKCYAN, false);
    this->_tft.drawSmoothArc(120, 120, 100, 95, nAnguloI+nAnguloConsigna, nAnguloF, TFT_DARKGREY, TFT_DARKCYAN, false);
  }else{
    if ( nTemperatura > nConsigna)
    {
      this->_tft.drawSmoothArc(120, 120, 100, 95, nAnguloI, nAnguloI+nAnguloConsigna, TFT_DARKGREY, TFT_DARKCYAN, false);
      this->_tft.drawSmoothArc(120, 120, 100, 95, nAnguloI+nAnguloConsigna, nAnguloI+nAnguloTemperatura, TFT_BLUE, TFT_DARKCYAN, false);
      this->_tft.drawSmoothArc(120, 120, 100, 95, nAnguloI+nAnguloTemperatura, nAnguloF, TFT_DARKGREY, TFT_DARKCYAN, false);
    }else{
      this->_tft.drawSmoothArc(120, 120, 100, 95, nAnguloI, nAnguloF, TFT_DARKGREY, TFT_DARKCYAN, false); 
    }  
  }
  this->WriteTxtTemperatura (nTemperatura);
  this->WriteTxtConsigna (nConsigna);
  this->ClearTxtAlarmas();

  //this->WriteMarcaConsigna(nConsigna);
}
/**
******************************************************
* @brief Dibuja un circulo de temperatura con la consigna
*
* Circulo con temperatura y consigna
* El circulo representa desde la temperatura TemperaturaMinima a la TemperaturaMaxima con una amplitud de
* AnguloInferior a AnguloSuperior
* El circulo se p0inta en gris y en rojo desde lka temperatura actual hasta la consgina siempre que la temperatura sea inferior a la consgina
* en caso contrario, se pinta de azul desde la consigna hasta la tyemperatura actual
* Visualiza  en color azul claro el segmento de alarma limitado por los valores de temperatura de alarma y reposicion de alarma
* @param nTemperatura. Temperatura actual que se quiere representar
* @param nConsigna. Temperatura de consigna del termostato
* @param nTemperaturaAlarmaMaxima. Limite superior de la zona de alarma. Temperatura a la que repone la alarma
* @param nTemperaturaAlarmaMinima. Limite inferior de la zona de alarma. Temperatura a la que se dispara la alarma
*
*/
void DISPLAYTERMOSTATO::Circulo ( float nTemperatura, float nConsigna, float nTemperaturaAlarmaMaxima, float nTemperaturaAlarmaMinima  )
{
  int nTLow = TemperaturaMinima;
  int nTHigh = TemperaturaMaxima;
  int32_t nAnguloI = AnguloInferior;
  int32_t nAnguloF = AnguloSuperior;
  int32_t nAnguloTemperatura = ( nTemperatura - nTLow )*(nAnguloF - nAnguloI)/(nTHigh - nTLow) ;
  int32_t nAnguloConsigna = ( nConsigna - nTLow )*(nAnguloF - nAnguloI)/(nTHigh - nTLow) ;
  if ( nConsigna > nTemperatura )
  {
    this->_tft.drawSmoothArc(120, 120, 100, 95, nAnguloI, nAnguloI+nAnguloTemperatura, TFT_DARKGREY, TFT_DARKCYAN, false);
    this->_tft.drawSmoothArc(120, 120, 100, 95, nAnguloI+nAnguloTemperatura, nAnguloI+nAnguloConsigna, TFT_RED, TFT_DARKCYAN, false);
    this->_tft.drawSmoothArc(120, 120, 100, 95, nAnguloI+nAnguloConsigna, nAnguloF, TFT_DARKGREY, TFT_DARKCYAN, false);
  }else{
    if ( nTemperatura > nConsigna)
    {
      this->_tft.drawSmoothArc(120, 120, 100, 95, nAnguloI, nAnguloI+nAnguloConsigna, TFT_DARKGREY, TFT_DARKCYAN, false);
      this->_tft.drawSmoothArc(120, 120, 100, 95, nAnguloI+nAnguloConsigna, nAnguloI+nAnguloTemperatura, TFT_BLUE, TFT_DARKCYAN, false);
      this->_tft.drawSmoothArc(120, 120, 100, 95, nAnguloI+nAnguloTemperatura, nAnguloF, TFT_DARKGREY, TFT_DARKCYAN, false);
    }else{
      this->_tft.drawSmoothArc(120, 120, 100, 95, nAnguloI, nAnguloF, TFT_DARKGREY, TFT_DARKCYAN, false); 
    }  
  }
  int32_t nAnguloAlarmaMinima = ( nTemperaturaAlarmaMinima - nTLow )*(nAnguloF - nAnguloI)/(nTHigh - nTLow) ;
  int32_t nAnguloAlarmaMaxima = ( nTemperaturaAlarmaMaxima - nTLow )*(nAnguloF - nAnguloI)/(nTHigh - nTLow) ;
  this->_tft.drawSmoothArc(120, 120, 100, 95, nAnguloI+nAnguloAlarmaMinima, nAnguloI+nAnguloAlarmaMaxima, colorAlarmaBaja, TFT_DARKCYAN, false); 
  this->WriteTxtTemperatura (nTemperatura);
  this->WriteTxtConsigna (nConsigna);
  this->WriteTxtAlarmas(nTemperaturaAlarmaMaxima, nTemperaturaAlarmaMinima);
}
/**
******************************************************
* @brief Dibuja un circulo de temperatura con la consigna
*
* Circulo con temperatura 
* El circulo representa desde la temperatura TemperaturaMinima a la TemperaturaMaxima con una amplitud de
* AnguloInferior a AnguloSuperior
* El circulo se pinta en azul desde la temperatura inferior hasta la temperatura actual y rojo desde la temperatura actual hasta la temperatura superior
* Visualiza  en color azul claro el segmento de alarma limitado por los valores de temperatura de alarma y reposicion de alarma
* @param nTemperatura. Temperatura actual que se quiere representar
* @param nTemperaturaAlarmaMaxima. Limite superior de la zona de alarma. Temperatura a la que repone la alarma
* @param nTemperaturaAlarmaMinima. Limite inferior de la zona de alarma. Temperatura a la que se dispara la alarma
*
*/
void DISPLAYTERMOSTATO::Circulo ( float nTemperatura, float nTemperaturaAlarmaMaxima, float nTemperaturaAlarmaMinima  )
{
  int nTLow = TemperaturaMinima;
  int nTHigh = TemperaturaMaxima;
  int32_t nAnguloI = AnguloInferior;
  int32_t nAnguloF = AnguloSuperior;
  int32_t nAnguloTemperatura = ( nTemperatura - nTLow )*(nAnguloF - nAnguloI)/(nTHigh - nTLow) ;
  this->_tft.drawSmoothArc(120, 120, 100, 95, nAnguloI, nAnguloI+nAnguloTemperatura, TFT_BLUE, TFT_DARKCYAN, false);
  this->_tft.drawSmoothArc(120, 120, 100, 95, nAnguloI+nAnguloTemperatura, nAnguloF, TFT_RED, TFT_DARKCYAN, false);
  int32_t nAnguloAlarmaMinima = ( nTemperaturaAlarmaMinima - nTLow )*(nAnguloF - nAnguloI)/(nTHigh - nTLow) ;
  int32_t nAnguloAlarmaMaxima = ( nTemperaturaAlarmaMaxima - nTLow )*(nAnguloF - nAnguloI)/(nTHigh - nTLow) ;
  this->_tft.drawSmoothArc(120, 120, 100, 95, nAnguloI+nAnguloAlarmaMinima, nAnguloI+nAnguloAlarmaMaxima, colorAlarmaBaja, TFT_DARKCYAN, false); 
  this->WriteTxtTemperatura (nTemperatura);
  this->ClearTxtConsigna();
  this->WriteTxtAlarmas(nTemperaturaAlarmaMaxima, nTemperaturaAlarmaMinima);
}

void DISPLAYTERMOSTATO::DisplayMarcas ( float nTemperatura )
{
  this->_nMododVisualizacion = 1;
  this->_nTemperatura = nTemperatura;
  this->CreaSpriteTemperatura();
  this->ClearMarcaTemperatura();
  this->WriteMarcaTemperatura ( nTemperatura );
  this->_temperatura.deleteSprite();
  this->Circulo ( nTemperatura );
}
void DISPLAYTERMOSTATO::DisplayMarcas ( float nTemperatura, float nConsigna )
{
  this->_nMododVisualizacion = 2;
  this->_nTemperatura = nTemperatura;
  this->_nConsigna = nConsigna;
  this->CreaSpriteTemperatura();
  this->ClearMarcaTemperatura();
  this->WriteMarcaTemperatura ( nTemperatura );
  this->WriteMarcaConsigna ( nConsigna );
  this->_temperatura.deleteSprite();
  this->Circulo(nTemperatura,nConsigna);
  this->WriteTxtTemperatura (nTemperatura);
}
void DISPLAYTERMOSTATO::DisplayMarcas ( float nTemperatura, float nTemperaturaAlarmaMaxima, float nTemperaturaAlarmaMinima  )
{
  this->_nMododVisualizacion = 3;
  this->_nTemperatura = nTemperatura;
  this->_nTemperaturaAlarmaMaxima = nTemperaturaAlarmaMaxima;
  this->_nTemperaturaAlarmaMinima = nTemperaturaAlarmaMinima;
  this->CreaSpriteTemperatura();
  this->ClearMarcaTemperatura();
  this->WriteMarcaTemperatura ( nTemperatura );
  this->_temperatura.deleteSprite();
  this->Circulo(nTemperatura,nTemperaturaAlarmaMaxima,nTemperaturaAlarmaMinima );
}
void DISPLAYTERMOSTATO::DisplayMarcas ( float nTemperatura, float nConsigna, float nTemperaturaAlarmaMaxima, float nTemperaturaAlarmaMinima )
{
  this->_nMododVisualizacion = 4;
  this->_nTemperatura = nTemperatura;
  this->_nConsigna = nConsigna;
  this->_nTemperaturaAlarmaMaxima = nTemperaturaAlarmaMaxima;
  this->_nTemperaturaAlarmaMinima = nTemperaturaAlarmaMinima;
  this->CreaSpriteTemperatura();
  this->ClearMarcaTemperatura();
  this->WriteMarcaTemperatura ( nTemperatura );
  this->WriteMarcaConsigna ( nConsigna );
  this->_temperatura.deleteSprite();
  this->Circulo(nTemperatura,nConsigna,nTemperaturaAlarmaMaxima,nTemperaturaAlarmaMinima );
}

/**
******************************************************
* @brief Escribe el texto de la temperatura en la pantalla
*
* @param nTemperatura. Temperatura actual que se quiere representar
*
*/
void DISPLAYTERMOSTATO::WriteTxtTemperatura ( float nTemperatura )
{
    ClearTxtTemperatura();
    this->_tft.setTextDatum(MC_DATUM);
    this->_tft.setTextColor(TFT_WHITE);
    this->_tft.drawString( String(nTemperatura)+" C", 120, 120, 4);
}
/**
******************************************************
* @brief Borra el texto de la temperatura
*
*/
void DISPLAYTERMOSTATO::ClearTxtTemperatura ( void )
{
  this->_tft.fillRect(69,107,100,22,ColorFondo);
}

/**
******************************************************
* @brief Visualiza la marca de consigna en el circulo
*
* @param nTemperaturaConsigna. valor de la Temperatura de la consigna
*
*/
void DISPLAYTERMOSTATO::WriteMarcaConsigna ( float nTemperaturaConsigna )
{
  int nTLow = TemperaturaMinima;
  int nTHigh = TemperaturaMaxima;
  int32_t nAnguloI = AnguloInferior;
  int32_t nAnguloF = AnguloSuperior;
  int32_t nAngulo = ( nTemperaturaConsigna - nTLow )*(nAnguloF - nAnguloI)/(nTHigh - nTLow) ;
  this->_temperatura.fillTriangle (0, 0, 10, 15, 20, 0, colorMarcaConsigna);
  this->_temperatura.pushRotated ( 180 + nAngulo + nAnguloI, TFT_BLACK);

/*
 int nTLow = TemperaturaMinima;
  int nTHigh = TemperaturaMaxima;
  int32_t nAnguloI = AnguloInferior;
  int32_t nAnguloF = AnguloSuperior;
  int32_t nAngulo = ( nTemperaturaConsigna - nTLow )*(nAnguloF - nAnguloI)/(nTHigh - nTLow) ;
  this->_marca.fillTriangle (0,15, 10, 0, 20, 15, colorMarcaConsigna);
  this->_marca.pushRotated ( 180 + nAngulo + nAnguloI, TFT_BLACK);
*/
}
/**
******************************************************
* @brief Borra la marca de la consigna
*
* Borra todo el circulo que contiene las marcas de consigna
*/
void DISPLAYTERMOSTATO::ClearMarcaConsigna (void)
{
  int32_t nAnguloI = AnguloInferior;
  int32_t nAnguloF = AnguloSuperior;
  this->_tft.drawSmoothArc(120, 120, 90, 75, nAnguloI, nAnguloF, ColorFondo, TFT_DARKCYAN, false);
}

/**
******************************************************
* @brief Visualiza la marca de temperatura en el circulo
*
* @param nTemperatura. Temperatura actual que se quiere representar
*
*/
void DISPLAYTERMOSTATO::WriteMarcaTemperatura ( float nTemperatura )
{

  int nTLow = TemperaturaMinima;
  int nTHigh = TemperaturaMaxima;
  int32_t nAnguloI = AnguloInferior;
  int32_t nAnguloF = AnguloSuperior;
  int32_t nAngulo = ( nTemperatura - nTLow )*(nAnguloF - nAnguloI)/(nTHigh - nTLow) ;
  this->_temperatura.fillTriangle (0, 0, 10, 15, 20, 0, ColorMarcaTemperatura);
  this->_temperatura.pushRotated ( 180 + nAngulo + nAnguloI, TFT_BLACK);

}
/**
******************************************************
* @brief Borra la marca de la temperatura
*
* Borra todo el circulo que contiene las marcas de temperatura
*/
void DISPLAYTERMOSTATO::ClearMarcaTemperatura (void)
{
  int32_t nAnguloI = AnguloInferior;
  int32_t nAnguloF = AnguloSuperior;
  this->_tft.drawSmoothArc(120, 120, 120, 100, nAnguloI, nAnguloF, ColorFondo, TFT_DARKCYAN, false);
}
/**
******************************************************
* @brief Dibuja las lineas de temperaturas en el circulo 
*
*/
void DISPLAYTERMOSTATO::DrawLineas (void)
{
  int nMarca;
  int32_t nAngulo;
  int nTLow = TemperaturaMinima;
  int nTHigh = TemperaturaMaxima;
  int32_t nAnguloI = AnguloInferior;
  int32_t nAnguloF = AnguloSuperior;
  
  this->CreaSpriteLinea ();
  this->CreaSpriteNumeros();
  //Marcas de grados, lineas pequeñas
  for ( nMarca = 0; nMarca < 1 + TemperaturaMaxima; nMarca++)
  {
    this->_linea.drawLine (2, 0, 2, 10, ColorFondo);
    this->_linea.drawLine (2, 0, 2, 5, colorLineaCorta);
    nAngulo = ( nMarca - nTLow )*(nAnguloF - nAnguloI)/(nTHigh - nTLow) ;
    this->_linea.pushRotated ( 180 + nAngulo + nAnguloI, TFT_BLACK);
  }
    
  //Marcas de 5 grados, lineas grandes y numeros
  for ( nMarca = 0; nMarca < 1 + TemperaturaMaxima; nMarca = nMarca + 5)
  {
    this->_linea.drawLine (2, 0, 2, 10, ColorFondo);
    this->_linea.drawLine (2, 0, 2, 10, colorLineaLarga);
    this->_numeros.drawNumber((long)nMarca,20,15, 2);
    nAngulo = ( nMarca - nTLow )*(nAnguloF - nAnguloI)/(nTHigh - nTLow) ;
    this->_linea.pushRotated ( 180 + nAngulo + nAnguloI, TFT_BLACK);
    this->_numeros.pushRotated ( 180 + nAngulo + nAnguloI, TFT_BLACK);
    this->_numeros.fillSprite(ColorFondo);
  }
  this->_numeros.deleteSprite();
  this->_linea.deleteSprite();
}
/**
******************************************************
* @brief Visualiza El Offset de la sonda
*
* @param nOffset. Offset asignado a la sonda
*
*/
void DISPLAYTERMOSTATO::WriteTxtOffset (float nOffset)
{
    this->_nOffset = nOffset;
    ClearTxtOffset();
    this->_tft.setTextDatum(TC_DATUM);
    this->_tft.setTextColor(TFT_WHITE);
    this->_tft.drawString( "Offset: " + String(nOffset), 120, 160, 2);
}
/**
******************************************************
* @brief Borra el texto del Offset de la sonda
*
*/
void DISPLAYTERMOSTATO::ClearTxtOffset (void)
{
    this->_tft.fillRect(62,159,115,19,ColorFondo);
}  
/**
******************************************************
* @brief Visualiza la consigna del termostato  
*
* @param nConsigna. Consigna del termostato
*
*/
void DISPLAYTERMOSTATO::WriteTxtConsigna (float nConsigna)
{
    ClearTxtConsigna();
    this->_tft.setTextDatum(TC_DATUM);
    this->_tft.setTextColor(TFT_WHITE);
    this->_tft.drawString( "Consigna: " + String(nConsigna), 120, 175, 2);
}
/**
******************************************************
* @brief Borra el texto de consigna del termostato
*
*/
void DISPLAYTERMOSTATO::ClearTxtConsigna (void)
{
    this->_tft.fillRect(62,174,115,19,ColorFondo);
}  
/**
******************************************************
* @brief Visualiza los datos de alarma  
*
* @param nTemperaturaAlarmaMaxima. Umbral superior de alarma
* @param nTemperaturaAlarmaMinima. Umbral inferior de alarma
*
*/
void DISPLAYTERMOSTATO::WriteTxtAlarmas (float nTemperaturaAlarmaMaxima, float nTemperaturaAlarmaMinima)
{
    this->ClearTxtAlarmas();
    this->_tft.setTextDatum(TC_DATUM);
    this->_tft.setTextColor(TFT_WHITE);
    this->_tft.drawString( "Alarma: " + String(nTemperaturaAlarmaMinima) + "-"+ String(nTemperaturaAlarmaMaxima), 120, 190, 2);
}
/**
******************************************************
* @brief Borra el texto de consigna del termostato
*
*/
void DISPLAYTERMOSTATO::ClearTxtAlarmas (void)
{
    this->_tft.fillRect(62,189,115,19,ColorFondo);
}  
/**
******************************************************
* @brief Escribe el texto 'A'  de automatico
*
*/
void DISPLAYTERMOSTATO::WriteTxtAutomatico (void)
{
  this->_lAM = 0;
  ClearTxtMA();
  this->_tft.setTextDatum(TC_DATUM);
  this->_tft.setTextColor(TFT_GREEN);
  this->_tft.drawString("A", 143,  135 , 4);
}
/**
******************************************************
* @brief Escribe el texto 'M'  de Manual
*
*/
void DISPLAYTERMOSTATO::WriteTxtManual (void)
{
   this->_lAM = 1;
  ClearTxtMA();
  this->_tft.setTextDatum(TC_DATUM);
  this->_tft.setTextColor(TFT_YELLOW);
  this->_tft.drawString("M", 143,  135 , 4);
}
/**
******************************************************
* @brief Borra el texto M/A ( Manual/Automatico )
*
*/
void DISPLAYTERMOSTATO::ClearTxtMA (void)
{
  this->_tft.fillRect(129, 134, 29, 22,ColorFondo);
}
/**
******************************************************
* @brief Escribe el texto 'ON' de que esta encendida la calefaccion
*
*/
void DISPLAYTERMOSTATO::WriteTxtOn (void)
{
  this->_lOnOff = 0;
  ClearTxtOnOff();
  this->_tft.setTextDatum(TC_DATUM);
  this->_tft.setTextColor(TFT_GREEN);
  this->_tft.drawString("ON", 120,  80 , 4);
}
/**
******************************************************
* @brief Escribe el texto 'OFF' de que esta apagada la calefaccion
*
*/
void DISPLAYTERMOSTATO::WriteTxtOff (void)
{
  this->_lOnOff = 1;
  ClearTxtOnOff();
  this->_tft.setTextDatum(TC_DATUM);
  this->_tft.setTextColor(TFT_RED);
  this->_tft.drawString("OFF",120,  80 , 4);

}
/**
******************************************************
* @brief Borra el texto On/OFF
*/
void DISPLAYTERMOSTATO::ClearTxtOnOff (void)
{
  this->_tft.fillRect(92,77,56,25,ColorFondo);
}
/**
******************************************************
* @brief Escribe un titulo en las pantallas se configuracion
*/
void DISPLAYTERMOSTATO::WriteTitulo (String cTitulo)
{
  this->ClearTitulo();
  this->_tft.setTextDatum(TC_DATUM);
  this->_tft.setTextColor(TFT_YELLOW);
  this->_tft.drawString(cTitulo,120,  40 , 4);
}
/**
******************************************************
* @brief Borra el titulo
*/
void DISPLAYTERMOSTATO::ClearTitulo (void)
{
  this->_tft.fillRect(20,30,220,60,ColorFondo);
}
/*-----------------------------
* Recuperacion Datos
*/
/**
******************************************************
* @brief Devuelve la consigna
*/
float DISPLAYTERMOSTATO::GetConsigna (void)
{
    return (this->_nConsigna);
}
/**
******************************************************
* @brief Devuelve la Temperatura maxima de alarma
*/
float DISPLAYTERMOSTATO::GetTemperaturaMaximaAlarma (void)
{
    return (this->_nTemperaturaAlarmaMaxima);
}
/**
******************************************************
* @brief Devuelve la Temperatura minima de alarma
*/
float DISPLAYTERMOSTATO::GetTemperaturaMinimaAlarma (void)
{
    return (this->_nTemperaturaAlarmaMinima);
}
/**
******************************************************
* @brief Devuelve la Temperatura minima de alarma
*/
float DISPLAYTERMOSTATO::GetOffset (void)
{
    return (this->_nOffset);
}
/*-----------------------------
*Creacion de sprites
*/
void DISPLAYTERMOSTATO::CreaSpriteMarca (void)
{
  this->_marca.deleteSprite();
  this->_marca.setRotation(0); 
  this->_marca.setColorDepth(16);         
  this->_marca.createSprite(20, 15);     
  this->_marca.setPivot(10,90);           
  this->_marca.setTextColor(TFT_RED);    
  this->_marca.setTextDatum(MC_DATUM);   
  this->_marca.fillSprite(TFT_BLACK);    
}
void DISPLAYTERMOSTATO::CreaSpriteLinea (void)
{
  _linea.deleteSprite();
  _linea.setRotation(0); 
  _linea.setColorDepth(16);         
  _linea.createSprite(4,10);    
  _linea.setPivot(2,90);           
  _linea.setTextColor(TFT_RED);    
  _linea.setTextDatum(MC_DATUM);   
  _linea.fillSprite(TFT_BLACK);         
}
void DISPLAYTERMOSTATO::CreaSpriteNumeros (void)
{
  _numeros.deleteSprite();
  _numeros.setRotation(0); 
  _numeros.setColorDepth(16);         
  _numeros.createSprite(40,30);    
  _numeros.setPivot(20,80);           
  _numeros.setTextColor(TFT_YELLOW);    
  _numeros.setTextDatum(MC_DATUM);   
  _numeros.fillSprite(TFT_BLACK);         
}

void DISPLAYTERMOSTATO::CreaSpriteTemperatura (void)
{
  _temperatura.deleteSprite();
  _temperatura.setRotation(0); 
  _temperatura.setColorDepth(16);         
  _temperatura.createSprite(20, 15);     
  _temperatura.setPivot(10,115);           
  _temperatura.setTextColor(TFT_RED);    
  _temperatura.setTextDatum(MC_DATUM);   
  _temperatura.fillSprite(TFT_BLACK);         
}


/*

TFT_eSPI& tft() {           // add definition of the free function
    static TFT_eSPI instance;
    return instance;
}

*/


