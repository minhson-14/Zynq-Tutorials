/*
 * LED_test_tut_1C.c
 *
 * Lab 01 - First Zynq Design
 * Tool flow: Vivado / Vitis 2022.2
 *
 * The application controls a 4-bit AXI GPIO peripheral connected to board LEDs.
 * It alternates between two LED patterns to verify PS-to-PL communication.
 */

#include "xparameters.h"
#include "xgpio.h"
#include "xil_printf.h"
#include "xstatus.h"
#include "sleep.h"

#define GPIO_DEVICE_ID   XPAR_AXI_GPIO_0_DEVICE_ID
#define LED_CHANNEL      1U
#define LED_ALL_OUTPUT   0x0U

#define LED_PATTERN_A    0x5U
#define LED_PATTERN_B    0xAU
#define DELAY_US         500000U

int main(void)
{
    XGpio gpio;
    int status;

    xil_printf("Lab01 AXI GPIO LED test started.\r\n");

    status = XGpio_Initialize(&gpio, GPIO_DEVICE_ID);
    if (status != XST_SUCCESS) {
        xil_printf("GPIO initialization failed.\r\n");
        return XST_FAILURE;
    }

    XGpio_SetDataDirection(&gpio, LED_CHANNEL, LED_ALL_OUTPUT);
    xil_printf("GPIO initialized. Running blink pattern.\r\n");

    while (1) {
        XGpio_DiscreteWrite(&gpio, LED_CHANNEL, LED_PATTERN_A);
        xil_printf("LEDs: 0101\r\n");
        usleep(DELAY_US);

        XGpio_DiscreteWrite(&gpio, LED_CHANNEL, LED_PATTERN_B);
        xil_printf("LEDs: 1010\r\n");
        usleep(DELAY_US);
    }

    return XST_SUCCESS;
}
