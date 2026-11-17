/***************************** Include Files *********************************/

#include "xparameters.h"
#include "xgpio.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "xil_printf.h"

/************************** Constant Definitions *****************************/

/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are defined here such that a user can easily
 * change all the needed parameters in one place. These constants are used
 * in the functions defined below.
 */

#define GPIO_DEVICE_ID      XPAR_AXI_GPIO_0_DEVICE_ID  /* Push Buttons */
#define LED_DEVICE_ID       XPAR_AXI_GPIO_1_DEVICE_ID  /* LEDs */
#define INTC_DEVICE_ID      XPAR_SCUGIC_SINGLE_DEVICE_ID

#define INTC_GPIO_INTERRUPT_ID  XPAR_FABRIC_AXI_GPIO_0_IP2INTC_IRPT_INTR

#define BTN_INT             XGPIO_IR_CH1_MASK  /* Channel 1 of the GPIO */

/* The following constant determines which buttons and LEDs are on which
 * channel of the GPIO. They are set for the PYNQ-Z2 board.
 */
#define BTN_CHANNEL         1  /* Channel 1 of the GPIO Device */
#define LED_CHANNEL         1  /* Channel 1 of the GPIO Device */

/*
 * The following constant is used to set the direction of the GPIO
 * channels. A bit set to 1 is an input, and a bit set to 0 is an output.
 */
#define BTN_MASK            0xF  /* 4 push buttons, all inputs */
#define LED_MASK            0x0  /* 4 LEDs, all outputs */

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

/* Interrupt handler for push buttons */
void BTN_Intr_Handler(void *InstancePtr);

/* Interrupt system setup function */
int InterruptSystemSetup(XScuGic *XScuGicInstancePtr);

/* Interrupt initialization function */
int IntcInitFunction(u16 DeviceId, XGpio *GpioInstancePtr);

/************************** Variable Definitions *****************************/

XGpio BTNInst;       /* GPIO Instance for Buttons */
XGpio LEDInst;       /* GPIO Instance for LEDs */
XScuGic INTCInst;    /* Interrupt Controller Instance */
static int btn_value;    /* Value read from buttons */
static int led_data = 0; /* LED output value (counter) */

/****************************************************************************/
/**
* This is the interrupt handler function for the push buttons. It is called
* every time an interrupt request from the push buttons is received.
* The handler increments the counter based on which button was pressed and
* writes the value to the LEDs.
*
* @param	InstancePtr is a pointer to the XGpio instance.
*
* @return	None.
*
* @note		None.
*
*****************************************************************************/
void BTN_Intr_Handler(void *InstancePtr)
{
	/* Disable GPIO interrupts */
	XGpio_InterruptDisable(&BTNInst, BTN_INT);

	/* Ignore additional button presses */
	if ((XGpio_InterruptGetStatus(&BTNInst) & BTN_INT) != BTN_INT) {
		return;
	}

	/* Read state of push buttons */
	btn_value = XGpio_DiscreteRead(&BTNInst, BTN_CHANNEL);

	/* Increment counter based on button value
	 * For PYNQ-Z2:
	 *   BTN0 (bit 0) -> increment by 1
	 *   BTN1 (bit 1) -> increment by 2
	 *   BTN2 (bit 2) -> increment by 4
	 *   BTN3 (bit 3) -> increment by 8
	 */
	if (btn_value != 0) {
		led_data = led_data + btn_value;

		/* Wrap around at 16 (4-bit counter for 4 LEDs) */
		if (led_data > 15)
			led_data = 0;
	}

	/* Write output to LEDs */
	XGpio_DiscreteWrite(&LEDInst, LED_CHANNEL, led_data);

	xil_printf("Button pressed: %d, LED value: %d\r\n", btn_value, led_data);

	/* Acknowledge and re-enable GPIO interrupts */
	(void)XGpio_InterruptClear(&BTNInst, BTN_INT);
	XGpio_InterruptEnable(&BTNInst, BTN_INT);
}

/****************************************************************************/
/**
* This function sets up the interrupt system for the application. It
* initializes the interrupt controller (XScuGic), connects the interrupt
* handler, and enables the interrupt.
*
* @param	XScuGicInstancePtr is a pointer to the XScuGic instance.
*
* @return	XST_SUCCESS if successful, else XST_FAILURE.
*
* @note		None.
*
*****************************************************************************/
int InterruptSystemSetup(XScuGic *XScuGicInstancePtr)
{
	/*
	 * Connect the interrupt controller interrupt handler to the
	 * hardware interrupt handling logic in the ARM processor.
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			(Xil_ExceptionHandler)XScuGic_InterruptHandler,
			XScuGicInstancePtr);

	/* Enable interrupts in the ARM */
	Xil_ExceptionEnable();

	return XST_SUCCESS;
}

/****************************************************************************/
/**
* This function initializes the interrupt controller and connects the
* interrupt handler for the GPIO push buttons.
*
* @param	DeviceId is the XPAR_SCUGIC_SINGLE_DEVICE_ID value from
*		xparameters.h.
* @param	GpioInstancePtr is a pointer to the GPIO instance for buttons.
*
* @return	XST_SUCCESS if successful, else XST_FAILURE.
*
* @note		None.
*
*****************************************************************************/
int IntcInitFunction(u16 DeviceId, XGpio *GpioInstancePtr)
{
	XScuGic_Config *IntcConfig;
	int status;

	/* Interrupt controller initialization */
	IntcConfig = XScuGic_LookupConfig(DeviceId);
	status = XScuGic_CfgInitialize(&INTCInst, IntcConfig,
			IntcConfig->CpuBaseAddress);
	if (status != XST_SUCCESS) return XST_FAILURE;

	/* Call to interrupt setup */
	status = InterruptSystemSetup(&INTCInst);
	if (status != XST_SUCCESS) return XST_FAILURE;

	/* Connect GPIO interrupt to handler */
	status = XScuGic_Connect(&INTCInst,
			INTC_GPIO_INTERRUPT_ID,
			(Xil_ExceptionHandler)BTN_Intr_Handler,
			(void *)GpioInstancePtr);
	if (status != XST_SUCCESS) return XST_FAILURE;

	/* Enable GPIO interrupts */
	XGpio_InterruptEnable(GpioInstancePtr, BTN_INT);
	XGpio_InterruptGlobalEnable(GpioInstancePtr);

	/* Enable GPIO interrupts in the controller */
	XScuGic_Enable(&INTCInst, INTC_GPIO_INTERRUPT_ID);

	return XST_SUCCESS;
}

/****************************************************************************/
/**
* Main function. Initializes the GPIO devices and interrupt controller,
* then enters an infinite loop waiting for interrupts.
*
* @param	None.
*
* @return	XST_SUCCESS if successful, else XST_FAILURE.
*
* @note		This function does not return.
*
*****************************************************************************/
int main(void)
{
	int status;

	xil_printf("-- Interrupt Counter Application --\r\n");
	xil_printf("-- PYNQ-Z2 Board --\r\n");

	/*----------------------------------------------------
	 * INITIALIZE THE PERIPHERALS & SET DIRECTIONS OF GPIO
	 *----------------------------------------------------*/

	/* Initialize Push Buttons */
	status = XGpio_Initialize(&BTNInst, GPIO_DEVICE_ID);
	if (status != XST_SUCCESS) {
		xil_printf("GPIO Initialization for BUTTONS Failed\r\n");
		return XST_FAILURE;
	}

	/* Initialize LEDs */
	status = XGpio_Initialize(&LEDInst, LED_DEVICE_ID);
	if (status != XST_SUCCESS) {
		xil_printf("GPIO Initialization for LEDs Failed\r\n");
		return XST_FAILURE;
	}

	/* Set the direction for all signals as inputs (buttons) */
	XGpio_SetDataDirection(&BTNInst, BTN_CHANNEL, BTN_MASK);

	/* Set the direction for all signals as outputs (LEDs) */
	XGpio_SetDataDirection(&LEDInst, LED_CHANNEL, LED_MASK);

	/*----------------------------------------------------
	 * SETUP THE INTERRUPT SYSTEM
	 *----------------------------------------------------*/

	status = IntcInitFunction(INTC_DEVICE_ID, &BTNInst);
	if (status != XST_SUCCESS) {
		xil_printf("Interrupt System Setup FAILED\r\n");
		return XST_FAILURE;
	}

	xil_printf("Interrupt System Setup PASSED\r\n");
	xil_printf("Press buttons to increment counter...\r\n");

	/* Clear LEDs initially */
	XGpio_DiscreteWrite(&LEDInst, LED_CHANNEL, 0);

	/*----------------------------------------------------
	 * INFINITE LOOP - WAITING FOR INTERRUPTS
	 *----------------------------------------------------*/

	while(1);

	return 0;
}
