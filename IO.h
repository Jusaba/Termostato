#ifndef IO_H
	#define IO_H	
    #ifdef TERMOSTATO_MANUAL_1
		int PinReset = 0;													//Pin empleado para pulsador reset			
		#ifdef DS18B20 		
			int PinDS18B20 = 13;	   										//OJO, cambiar a 13 en uso normal, para pruebas con ESP11 se ha utilizado el pin 2
		#endif
		int PinInterrupt = 10;
	#endif
#endif    
