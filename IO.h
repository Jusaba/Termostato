#ifndef IO_H
	#define IO_H	
    #ifdef TERMOSTATO_MANUAL_1
		int PinReset = 16;												//Pin empleado para pulsador reset			
		int PinDS18B20 = 2;	   										//OJO, cambiar a 13 en uso normal, para pruebas con ESP11 se ha utilizado el pin 2
		int PinInterrupt = 10;
	#endif
#endif    
