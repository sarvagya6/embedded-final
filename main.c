#include <msp430.h>
#include <stdint.h>

#define LCD_ADDR 0x3F // Replace with the I2C address of your display module
#define LCD_COLS 20
#define LCD_ROWS 4

void delay_ms(unsigned int ms) {
    while (ms--) {
        __delay_cycles(1000); // 1000 for 1MHz clock
    }
}

void i2c_init() {
    P1SEL0 |= BIT6 | BIT7; // P1.6 (SCL) and P1.7 (SDA) as UCB0SCL and UCB0SDA
    P1SEL1 &= ~(BIT6 | BIT7);

    UCB0CTLW0 |= UCSWRST; // Enable SW reset
    UCB0CTLW0 |= UCMODE_3 | UCMST | UCSYNC | UCSSEL__SMCLK; // I2C mode, master, synchronous, SMCLK as source
    UCB0BRW = 10; // Prescaler for 100kHz (assuming 1MHz SMCLK)
    UCB0CTLW0 &= ~UCSWRST; // Disable SW reset
}

void i2c_start(uint8_t addr, uint8_t rw) {
    while (UCB0CTLW0 & UCTXSTP); // Ensure stop condition was sent

    UCB0I2CSA = addr; // Set slave address
    UCB0CTLW0 |= UCTR | UCTXSTT; // Transmit mode, send start condition

    while (!(UCB0IFG & UCTXIFG0)); // Wait for TX buffer to be ready
    UCB0TXBUF = rw; // Write data to TX buffer
    while (!(UCB0IFG & UCTXIFG0)); // Wait for TX complete
    UCB0IFG &= ~UCTXIFG0; // Clear UCTXIFG0
}

void i2c_stop() {
    UCB0CTLW0 |= UCTXSTP; // Send stop condition
    while (UCB0CTLW0 & UCTXSTP); // Wait for stop condition to be sent
}

void i2c_write(uint8_t addr, uint8_t *data, uint8_t len) {
    i2c_start(addr, 0x00); // Start condition and write mode

    while (len--) {
        UCB0TXBUF = *data++; // Write data to TX buffer
        while (!(UCB0IFG & UCTXIFG0)); // Wait for TX complete
        UCB0IFG &= ~UCTXIFG0; // Clear UCTXIFG0
    }

    i2c_stop();
}

void lcd_init() {
    // Initialize the display module (refer to the datasheet for specific init sequence)
    uint8_t init_sequence[] = {0x38, 0x39, 0x14, 0x70, 0x5E, 0x6C};
    i2c_write(LCD_ADDR, init_sequence, sizeof(init_sequence));
    delay_ms(200);
    uint8_t post_init_sequence[] = {0x38, 0x0C, 0x01};
    i2c_write(LCD_ADDR, post_init_sequence, sizeof(post_init_sequence));
    delay_ms(2);
}

void lcd_set_cursor(uint8_t row, uint8_t col) {
    uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
    uint8_t cmd = 0x80 | (col + row_offsets[row]);
    i2c_write(LCD_ADDR, &cmd, 1);
    delay_ms(2);
}

void lcd_print(const char *text) {
    while (*text) {
        i2c_write(LCD_ADDR, (uint8_t *)text, 1);
        text++;
        delay_ms(2);
    }
}

int main(void) {
    WDTCTL = WDTPW | WDTHOLD; // Stop the watchdog timer

    // Setup I2C
    i2c_init();

    // Initialize the display
    lcd_init();

    // Set cursor position (row, column)
    lcd_set_cursor(0, 0);

    // Print sample text
    lcd_print("Hello, World!");

    // Main loop
    while (1) {
        // Your main code
    }
}


