#define F_CPU 8000000UL

#include <avr/io.h>

void TWI_START(unsigned char bitrate);
void TWI_ADDRESS(unsigned char address);
void MASTER_SEND(char *data);
void TWI_STOP(void);

int main(void){
	TWI_START(0xC0);
	TWI_ADDRESS(0x20);
	
	MASTER_SEND("Hello World!");
	
	TWI_STOP();
}

/*!
 *	@brief Send START condition to confirm master status in current iteration.
 *	@param Bitrate of operation to determine SCL period (unsigned char).
 */

void TWI_START(unsigned char bitrate){
	TWBR = 0x00;
	TWBR = bitrate; 									///< Set the division factor of bitrate generator.
	TWCR = (1<<TWEN)|(1<<TWINT)|(1<<TWSTA); 			///< Enable TWI and set TWINT bit to 1. Send START condition.
	while(!(TWCR & (1<<TWINT)));						///< Wait while TWI completes current action.
	while((TWSR & 0xF8) != 0x08); 						///< Wait while ACK bit is received after START condition is transmitted.
}

/*!
 *	@brief Make a call to the slave device to write data to it.
 *	@param Address of the slave device (unsigned char).
 */

void TWI_ADDRESS(unsigned char address){
	while(1){
		TWDR = address; 									///< Load slave device's address to SDA.
		TWCR = (1<<TWEN)|(1<<TWINT);						///< Enable TWI and set TWINT bit to 1.
		while(!(TWCR & (1<<TWINT)));						///< Wait while TWI completes current action.
		if((TWSR & 0xF8) != 0x18){
			continue;										///< If ACK has not been received, repeat the loop.
		}
		break;												///< If ACK has been received break from loop.
	}
}

/*!
 *	@brief Transmit data to the slave device.
 *	@param Data to be transmitted (char).
 */

void MASTER_SEND(char *data){
	for(uint8_t i=0;data[i]!=0;i++){
		TWDR = data[i]; 								///< Load data to TWDR register to be transmitted.
		TWCR = (1<<TWEN)|(1<<TWINT);					///< Enable TWI and set TWINT bit to 1.					
		while(!(TWCR & (1<<TWINT)));					///< Wait while TWI completes current action.
		if((TWSR & 0xF8) != 0x28){
			break;										///< If ACK has not been received stop sending data.
		}
	}
}

/*!
 *	@brief Send STOP condition once all data has been received.
 */

void TWI_STOP(void){
	TWCR = (1<<TWEN)|(1<<TWINT)|(1<<TWSTO); 			///< Enable TWI and set TWINT bit to 1. Send STOP condition.
}