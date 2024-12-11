
#include <xc.h>

void init_i2c(void)
{
	/* Set SCL and SDA pins as inputs */
	TRISC3 = 1;
	TRISC4 = 1;
	/* Set I2C master mode */
	SSPCON1 = 0x28;

	SSPADD = 0x31;
	/* Use I2C levels, worked also with '0' */
	CKE = 0;
	/* Disable slew rate control  worked also with '0' */
	SMP = 1;
	/* Clear SSPIF interrupt flag */
	SSPIF = 0;
	/* Clear bus collision flag */
	BCLIF = 0;
}

void i2c_idle(void)
{
	while (!SSPIF);
	SSPIF = 0;
}

void i2c_ack(void)    // Acknowledgment check
{
	if (ACKSTAT)
	{
		/* Do debug print here if required */
	}
}

void i2c_start(void) // Enable start I2c
{
	SEN = 1;
	i2c_idle();
}

void i2c_stop(void) // Disable (stop) I2c
{
	PEN = 1;
	i2c_idle();
}

void i2c_rep_start(void) //  Initiate Repeated Start condition on SDA and SCL pins. Automatically cleared by hardware.
{
	RSEN = 1;
	i2c_idle();
}

void i2c_write(unsigned char data) // Write in buffer
{
	SSPBUF = data;
	i2c_idle();
}

void i2c_rx_mode(void)  // Receive Enable bit (Master mode only)
{
	RCEN = 1;
	i2c_idle();
}

void i2c_no_ack(void) 
{
	ACKDT = 1;  //Acknowledge Data bit (Master Receive mode only) 1 = Not Acknowledge
	ACKEN = 1; // : Acknowledge Sequence Enable bit (Master Receive mode only)(1) 1 = Initiate Acknowledge sequence on SDA and SCL pins and transmit ACKDT data bit.
    //Automatically cleared by hardware
}

unsigned char i2c_read(void) // Read from buffer 
{
	i2c_rx_mode();
//	i2c_no_ack();

	return SSPBUF;
}