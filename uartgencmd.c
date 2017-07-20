#include <avr/io.h>
#include <inttypes.h>

char COMs[5][5] = {"COM0","COM1","COM2","COM3"}; 
char paridads[5][5] = {"N","R","E","O"};
char parte1[10], parte2[10], parte3[10], parte4[10], parte5[10];
char partes[5][6];

void UART_Config(char *str);
void partString(char *str, char partes[4][4]);
void breakString(char *str, char parte1[10],char parte2[10], char parte3[10], char parte4[10], char parte5[10] );
void break_String(char *str, char partes[5][6] );
char *_stringCopy(char *destino, char *fuente);
int stringCompare(char *cadena1, char *cadena2);
uint16_t buscar( char *str, char COMs[5][5] );
int strlen(char *str);
void UART_puts(char *str);
char UART0_getchar(void);
void UART0_putchar(char data);
uint32_t atoi(char *str);

int main(void){

   UART_Config("COM3:38400,8,N,1");

  while(1){
   UART0_putchar('H');
   UART0_putchar('O');
   UART0_putchar('L');
   UART0_putchar('A');
  }
   return 0;
}

void UART_Config(char *str){
     uint16_t *ubrr = 0xC4, *registroA = 0xC0, *registroB =0xC1, *registroC=0xC2;  
	 uint16_t vcom, czbits, paridad, bitsparo;
	 uint32_t baudaje;  
	 uint32_t temp = 16000000; 
     
	 //breakString(str,parte1,parte2,parte3,parte4,parte5);
     break_String(str, partes);
	 // Extraer partes
	 vcom = buscar(partes[0],COMs);
	 baudaje = atoi(partes[1]); /* ajustar*/
     czbits = atoi(partes[2]) - 5;
     paridad = buscar(partes[3],paridads);
     bitsparo = atoi(partes[4]);
	 
	 //baudaje
	 baudaje = (baudaje*16);
     baudaje = (temp/baudaje)-1;
	 // Ajustar los apuntadores
	 if( vcom == 3){
	      *ubrr += (vcom*8 + 36);
	      *registroA += (vcom*8 + 36);
	      *registroB += (vcom*8 + 36);
	      *registroC += (vcom*8 + 36);
	 } else {
	   *ubrr += (vcom*8);
	   *registroA += (vcom*8);
	   *registroB += (vcom*8);
	   *registroC += (vcom*8);
	 }

	  // Configuracion final 
       *ubrr = (uint32_t)baudaje;     //Baudaje
	   *registroA |= 0x20;
	   *registroB |= (1<<3) |(1<<2); //TXEN RXEN
	   *registroC |= (bitsparo<<3) | (czbits<<1);
	   *registroC &= (paridad<<4);
}

/* Cadena en pezados optima */
void break_String(char *str, char partes[5][6] ){
     int i=0,j=0;
	 while( *str != 0 && i<5){
	    
		  while( *str != ':' && *str != ',' )
		      partes[i][j++] = *str++;
          
		  partes[i][j++] = 0; // Fin de cadena
		  i++; // Incrementa String
		  j=0; // Reinicia 
		  *str++; //Incrementa cadena
	 }
}

/* Cadena partir en padazitos */
void breakString(char *str, char parte1[10],char parte2[10], char parte3[10], char parte4[10], char parte5[10] ){
	int i=0, j=0, k=0, m=0, n=0;
	
	while( *str == ' ' ) *str++;

	while( *str != ':' )
	    	parte1[j++] = *str++; // COM
	parte1[j++]= 0;
	*str++;
	while( *str != ',' )
	    	parte2[k++] = *str++; // BAUD
	
    parte2[k++]= 0;
	*str++;
	while( *str != ',' )
	    	parte3[m++] = *str++; // 8Bits
    parte3[m++]= 0;
	*str++;
    while( *str != ',' )
	    	parte4[n++] = *str++; // E, N Paridad
	parte4[n++]= 0;
    *str++;
	while(  *str != 0 )
	    	parte5[i++] = *str++; // Bits paro
	parte5[i++]= 0;
	
}

char *_stringCopy(char *destino, char *fuente){
	
	while( *fuente != '\0'){
		*destino++ = *fuente++;
	}
	*destino='\0';
	return destino;
}

int stringCompare(char *cadena1, char *cadena2){
    int bandera=1;
	if( strlen(cadena1) != strlen(cadena2) ){
	    return 0;
	}
	while( *cadena1 != 0 && *cadena2 != 0){
	    if( *cadena1 != *cadena2){
		      bandera=0;
			  return bandera;
		}
       *cadena1++;
	   *cadena2++;
	}
	return bandera;
}

uint16_t buscar( char *str, char COMs[5][5] ){
    int posicion=0; 
    while( posicion < 4){
	    
		if( stringCompare(str,COMs[posicion]) ){
		     return posicion;
		}
		posicion++;
	}    
    return -1;
}

int strlen(char *str){
   int i=0;
   while( *str++ != 0){
	   i++;
   }
   return i;
}

void UART_puts(char *str){
	
	while( *str != 0){
		UART0_putchar( *str++ );
	}
}

char UART0_getchar(void)
{
    /* UCSR0A se hace 1 verificando a RXC0*/
	while(!(UCSR0A & (1<<RXC0)));  /* Espera a que termine de recibir el dato completo*/
    return UDR0;
	
}

void UART0_putchar(char data)
{
    /* UCSR0A se hace 1 verificando a UDRE0*/
	while( !( UCSR0A & (1<<UDRE0))); /*Espera a que este vacio el buffer*/
	UDR0 = data;
}

uint32_t atoi(char *str){
	unsigned char temp, i=0;
    uint32_t resultado=0;

	while(str[i] != '\x0')
	{ 
	   if(str[i] >= '0' && str[i] <= '9')
	   {
	      temp = str[i] - '0'; /* 0 ASCII = 0*/
		  resultado = (resultado*10) + temp;
		  i++;
	   } else {
	      return resultado;
	   }

	}
	return resultado;
} 
