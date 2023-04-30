#define F_CPU 16000000UL
#define address 0x20
#define serial_clock 50000UL
#include<avr/io.h>
#include<util/delay.h>


void masterInit(); //initializes master
void start(); //sends the start condition
void writeAddress();  //sends the slave address with write mode
void writeData(char data); //transmits data on the SDA line
void stop(); //sends the stop condition
void ERROR(); //shows error condition by turning led on A1 pin
int main()
{
	DDRA=0xFF;
	unsigned char data =0x00;
	PORTA=0x00;
	masterInit();
	while(1)
	{
		start();
		writeAddress();
		writeData(data);
		stop();
		data+=1;
		_delay_ms(300); //so that UART transmission is completed for slave during this time
	}
	return 0;
}

void masterInit()
{
	TWBR=0x98;   //bit rate register corresponding to SCL frequency of 50kHz
	TWSR=(0<<TWPS1)|(0<<TWPS0);    //setting up prescaler bits to 0 in status register
}

void start()
{
	TWCR = (1<<TWINT)|(1<<TWSTA)| (1<<TWEN); //enables TWI , sends start condition, clears interrupt flag
	while(!(TWCR&(1<<TWINT)));  //waits till the start condition is transmitted
	if ((TWSR & 0xF8) != 0x08) ERROR();  //checks the status register, 0x08 is code for successful transmission of start condition, goes to error if condition not met
}

void writeAddress()
{
	TWDR = address; //loads the slave address into data register; here slave address is 0x20. The LSB is 0 which shows SLA+W condition
	TWCR =(1<<TWINT) | (1<<TWEN); //clears interrupt, enables TWI
	//Note that clearing TWINT flag (by setting it high) starts TWI operation (given that TWEN is also set)
	while(!(TWCR & (1<<TWINT))); //wait for TWINT bit to be set. This indicates that address has been transmitted and ack/nack been received
	if(TWSR & 0xF8c != 0x18) ERROR(); //compares status code in TWSR for successful data transfer and ack received
}

void writeData(char data)
{
	TWDR = data;  //loads data on the TWI data register
	TWCR=(1<<TWINT)|(1<<TWEN); //clears interrupt, enables TWI
	while(!(TWCR & (1<<TWINT))); //wait for TWINT bit to be set. This indicates that address has been transmitted and ack/nack been received
	if(TWSR & 0xF8 != 0x28) ERROR(); //compares status code in TWSR for successful data transfer and ack received
}

void stop()
{
	TWCR=(1<<TWINT)|(1<<TWSTO)|(1<<TWEN); //sends STOP condition on the SDA line to terminate transmission to the current slave device
}

void ERROR()
{
	PORTA=0xFF;
	_delay_ms(300);
}