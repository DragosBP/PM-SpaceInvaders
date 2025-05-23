/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * PLEASE MODIFY THIS FILE * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

#include "twi.h"

void twi_init(void) {
	/* Reset I2C control register */
	TWCR = 0;
	
    /* Set I2C clock frequency */
    TWBR = (uint8_t)TWBR_VAL;
    
    // Set bitrate in TWSR register (check twi.h to find prescaler value)
    TWSR &= ~(1 << TWPS0);
    TWSR &= ~(1 << TWPS1);
}

void twi_start(void) {    
    /* Enable I2C communication and clear interrupt flag */
    // Send START condition (corresponding bit in TWCR)
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA);

	/* Mandatory: wait for START condition to be sent */
	while (!(TWCR & (1 << TWINT)));
}

void twi_write(uint8_t data) {
    // Send a byte of data (TWCR + TWDR)
    TWDR = data;

    /* Enable I2C communication and clear interrupt flag */
    // Set acknowledge bit (corresponding bit in TWCR)
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
	
    // Wait for transfer to complete (TWINT flag)
    while (bit_is_clear(TWCR, TWINT));
}

void twi_read_ack(uint8_t *data) {
    /* Enable I2C communication and clear interrupt flag */
    // Set acknowledge bit (corresponding bit in TWCR)
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);

    // Wait for transfer to complete (TWINT flag)
    while (bit_is_clear(TWCR, TWINT));
    *data = TWDR;
}

void twi_read_nack(uint8_t *data) {
    // Read a byte of data with ACK disabled 
	TWCR = (1 << TWINT) | (1 << TWEN);

    // Wait for transfer to complete (TWINT flag)
    while (bit_is_clear(TWCR, TWINT));
    *data = TWDR;
}

void twi_stop(void) {
    /* Enable I2C communication and clear interrupt flag */
    // Send STOP condition (corresponding bit in TWCR)
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}

void twi_discover(void) {
    /* Search for I2C slaves */
    for (uint8_t i = 0x00; i < 0x7F; i++)  {
        twi_start();
		// Write address (as seen in OCW hints)
        twi_write(i << 1 | 1);
        
        // Check TWSR (see util/twi.h documentation for constants!)
        if (((TWSR & 0xF8) == 0x40))
            printf("Device discovered on 0x%x\n", i);
    }
    twi_stop();
}
