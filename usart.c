#define F_CPU 16000000UL
#include<avr/io.h>
#include<util/delay.h>

void uart_initialize(long baud)
{
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	UCSR0C = (0<<USBS0)|(3<<UCSZ00);
	uint16_t baud_val =((F_CPU/(baud*16))-1);
	UBRR0H=(baud_val>>8);
	UBRR0L=baud_val;
}

char receive()
{
	while ( !(UCSR0A & (1<<RXC0)) );
	return (UDR0);
}

void transmit (char ch)
{
	while(!(UCSR0A & (1<<UDRE0)));
	UDR0=ch;
}

void  sendString(char* str)
{
	int j=0;
	while(str[j]!=0)
	{
		transmit(str[j]);
		j++;
	}
	//while(!(UCSR0A & (1<<UDRE0)));
}
int main()
{
	DDRA=0xFF;
	PORTA=0x00;
	uart_initialize(9600);
	sendString("press 1 ");
	char data;
	while(1)
	{
		
		data = receive();
		transmit(data+1);
	}
	return 0;
}