#include <msp430.h>
#include <stdint.h>
#include "driverlib.h"

// Screen I2C address
#define SCREEN_ADDR 0x27

void init_screen() {
    // Setup the screen with your custom configuration
    // (e.g., setting 4-bit mode, 2-line display, 5x8 dots)
    EUSCI_B_I2C_masterSendMultiByteStart(EUSCI_B0_BASE, SCREEN_ADDR);
    // Your command here
    // (Continue with any other necessary setup commands)
}

void display_message(char *message) {
    // Set the screen's DDRAM address to the starting position
    EUSCI_B_I2C_masterSendMultiByteStart(EUSCI_B0_BASE, SCREEN_ADDR);
    // Your command here

    // Write the message to the screen
    while (*message) {
        EUSCI_B_I2C_masterSendMultiByteNext(EUSCI_B0_BASE, *message++);
    }
}

int main() {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;       // Disable the GPIO power-on default high-impedance mode

    // Initialize the I2C peripheral
    EUSCI_B_I2C_initMasterParam i2cParams = {
        .selectClockSource = EUSCI_B_I2C_CLOCKSOURCE_SMCLK,
        .i2cClk = CS_getSMCLK(),
        .dataRate = EUSCI_B_I2C_SET_DATA_RATE_100KBPS
    };
    EUSCI_B_I2C_initMaster(EUSCI_B0_BASE, &i2cParams);

    // Configure the I2C pins (P4.6 as SDA and P4.7 as SCL)
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4, GPIO_PIN6 + GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION);

    // Initialize the screen
    init_screen();

    // Display a test message
    char test_message[] = "Hello, World!";
    display_message(test_message);

    // Enter an infinite loop
    while (1);

    return 0;
}
