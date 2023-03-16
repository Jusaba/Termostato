#ifndef TOUCHTERMOSTATO_H
#define TOUCHTERMOSTATO_H

#include "DisplayTermostato.h"
#include <MPR121.h>
#include <MPR121_Datastream.h>
#include <Wire.h>

#define CtrlDisplay 11

const bool MPR121_DATASTREAM_ENABLE = false;


class TOUCHTERMOSTATO : public DISPLAYTERMOSTATO 
{

    public:

        explicit TOUCHTERMOSTATO ();           //Constructor
        ~TOUCHTERMOSTATO (void);
        void begin (int AddrMPR121, int PinIRQ);

        void OrdenTouch (int nTouch);
        void Scan  (void);
        void Refresh (void);
        void DisplayNormal (void);       

        void DisplayOn (void);
        void DisplayOff (void);

       uint8_t TeclasPulsadas (void);



    protected:

    private:  


        void Enter (void);
        void Left (void);
        void Right (void);

        //Programacion Consigna
        void DisplayPrgConsigna (void);
        void LeftPrgConsigna (void);
        void RightPrgConsigna (void);

        //Programacion Offset
        void DisplayPrgOffset (void);
        void LeftPrgOffset (void);
        void RightPrgOffset (void);

        //Programacion Alarma Alta
        void DisplayPrgAlarmaH (void);
        void LeftPrgAlarmaH (void);
        void RightPrgAlarmaH (void);

        //Programacion Alarma Baja
        void DisplayPrgAlarmaL (void);
        void LeftPrgAlarmaL (void);
        void RightPrgAlarmaL (void);

        //Programacion Manual Automatrico
        void ProgAM (void);
        //Programacion On / Off
        void ProgOnOff (void);



        



};

#endif
