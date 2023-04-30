#define F_CPU 16000000UL
#include<avr/io.h>
#include<util/delay.h>
#define baud 9600UL
#define address 0x20

void slaveInit();    //initialises slave devices for TWI
void matchAddress();  //keeps listening to the SDA line for a call by the master to its address
char receive();      //receives the data transmitted by master and stores in TWDR register
void configUART();   //initialises UART protocol
void UART_Transmit(char data);//transmits the received data by the master to the terminal using UART
void ERROR();


int main()
{
	DDRA=0xFF;
	PORTA=0x00;
	configUART();
	slaveInit();
	char recv_data;
	while(1)
	{
		matchAddress();
		recv_data=receive();
		UART_Transmit(recv_data);
	}
	return 0;
}

void slaveInit()
{
	TWAR=address;
}

void matchAddress()
{
	while((TWSR & 0xF8)!= 0x60) //own SLA+W received; ack has  been returned
	{
		TWCR=(1<<TWEA)|(1<<TWEN)|(1<<TWINT);
		while(!(TWCR & (1<<TWINT)));
	}
}

char receive()
{
	TWCR=(1<<TWEA)|(1<<TWEN)|(1<<TWINT);
	while(!(TWCR & (1<<TWINT)));
	if((TWSR & 0xF8)!=0x80) ERROR();
	return TWDR;
}

void configUART()
{
	UCSR0B=(1<<TXEN0);
	UCSR0C=(1<<USBS0)|(3<<UCSZ00);
	uint16_t baud_val = ((F_CPU/(16*baud))-1);
	UBRR0H = (baud_val>>8);
	UBRR0L=baud_val;
}

void UART_Transmit(char data)
{
	while(!(UCSR0A & (1<<UDRE0)));
	UDR0=data;
}

void ERROR()
{
	PORTA=0xFF;
	_delay_ms(2000);
}