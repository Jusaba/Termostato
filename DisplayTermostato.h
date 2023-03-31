#ifndef DISPLAYTERMOSTATO_H
#define DISPLAYTERMOSTATO_H


    #if ARDUINO >= 100
        #include "Arduino.h"
    #else
        #include "WProgram.h"
    #endif

    #include <TFT_eSPI.h> // Hardware-specific library
    #include <SPI.h>

    #define TemperaturaMaxima  40
    #define TemperaturaMinima   0
    #define AnguloInferior      65
    #define AnguloSuperior      295

    #define ColorFondo            TFT_DARKCYAN
    #define colorTxtTemperatura   TFT_WHITE
    #define ColorMarcaTemperatura TFT_BLUE
    #define colorMarcaConsigna    TFT_GREEN
    #define colorLineaLarga       TFT_YELLOW
    #define colorLineaCorta       TFT_WHITE
    #define colorAlarmaAlta       TFT_RED
    #define colorAlarmaBaja       TFT_CYAN

    #define TouchLeft   1
    #define TouchRight  3
    #define TouchEnter  2
    #define TouchAM     0
    #define TouchOnOff  4 
    
    #define ModoNormal          0
    #define ModoProgConsigna    1
    #define ModoProgAlarmaH     2
    #define ModoProgAlarmaL     3
    #define ModoProgOffset      4
    #define ModoProgAM          5
    #define ModoProgOnOff       6



class DISPLAYTERMOSTATO : public TFT_eSPI  //Definicion de la clase
{
    public:
    //TFT_eSPI tft ;   
        explicit DISPLAYTERMOSTATO ();           //Constructor
        ~DISPLAYTERMOSTATO (void);

        void DrawLineas(void);
        void DisplayMarcas ( );
        void DisplayMarcas ( float nTemperatura );
        void DisplayMarcas ( float nTemperatura, float nConsigna );
        void DisplayMarcas ( float nTemperatura, float nTemperaturaAlarmaMaxima, float nTemperaturaAlarmaMinima  );
        void DisplayMarcas ( float nTemperatura, float nConsigna, float nTemperaturaAlarmaMaxima, float nTemperaturaAlarmaMinima  );

        void WriteTxtOffset (float nOffset);

        void WriteTxtAutomatico (void);
        void WriteTxtManual (void);

        void WriteTxtOn (void);
        void WriteTxtOff (void);

        void WriteTitulo (String cTitulo);
        void ClearTitulo (void);

        float GetConsigna (void);
        float GetTemperaturaMaximaAlarma (void);
        float GetTemperaturaMinimaAlarma (void);
        float GetOffset (void);
        boolean GetAutomaticoManual (void);
        boolean GetOnOff (void);

         int nModo;

    


    private:  

        TFT_eSprite _marca = TFT_eSprite(&(this->_tft));
        TFT_eSprite _linea = TFT_eSprite(&(this->_tft));
        TFT_eSprite _temperatura = TFT_eSprite(&(this->_tft));
        TFT_eSprite _numeros = TFT_eSprite(&(this->_tft));

        void Circulo ( );
        void Circulo ( float nTemperatura );
        void Circulo ( float nTemperatura, float nConsigna );
        void Circulo ( float nTemperatura, float nTemperaturaAlarmaMaxima, float nTemperaturaAlarmaMinima );
        void Circulo ( float nTemperatura, float nConsigna, float nTemperaturaAlarmaMaxima, float nTemperaturaAlarmaMinima );

        void WriteMarcaConsigna ( float nTemperaturaConsigna );
        void ClearMarcaConsigna (void);
        void WriteMarcaTemperatura (float nTemperatura );
        void ClearMarcaTemperatura (void);

        void WriteTxtConsigna (float nConsigna);
        void ClearTxtConsigna (void);
        void WriteTxtAlarmas (float nTemperaturaAlarmaMaxima, float nTemperaturaAlarmaMinima);
        void ClearTxtAlarmas (void);

        void CreaSpriteMarca (void);
        void CreaSpriteLinea (void);
        void CreaSpriteNumeros (void);
        void CreaSpriteTemperatura (void);

        void ClearTxtMA (void);
        void ClearTxtOnOff (void);
        void ClearTxtOffset (void);

    protected:


        TFT_eSPI _tft = TFT_eSPI();


        void WriteTxtTemperatura ();
        void WriteTxtTemperatura (float nTemperatura );
        void ClearTxtTemperatura (void);
    



//--------------


        float _nTemperatura; 
        float _nConsigna; 
        float _nTemperaturaAlarmaMaxima; 
        float _nTemperaturaAlarmaMinima;
        float _nOffset;
        bool _lAM;
        bool _lOnOff;



        int _nMododVisualizacion;

};

//TFT_eSPI& tft();            // add a declaration of a free function

#endif