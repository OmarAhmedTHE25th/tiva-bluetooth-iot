#include <stdint.h>
#include <tm4c123gh6pm.h>

// function to configure:
// PF2 as output LED
// PF0 as input Switch
void DIO_init()
{
    // 1. Power port F
    SYSCTL_RCGCGPIO_R |= 1 << 5;

    // 2. wait until port is up
    while ((SYSCTL_PRGPIO_R & 1<<5) == 0) {}

    // 3. unlock the port
    GPIO_PORTF_LOCK_R = 0x4c4f434b;

    // 4. unlock PF2 & PF0
    GPIO_PORTF_CR_R  |= (1 << 2 | 1 << 0);

    // 5.A Set the direction (PF2 ==> Output ==> Set)
    GPIO_PORTF_DIR_R |= 1 << 2;

    // 5.B Set the direction (PF0 ==> Input ==> Clear)
    GPIO_PORTF_DIR_R &= ~(1 << 0);

    // 6.B Set Pull resistor for input pin
    // Pull up (1) ==> not pressed, (0) ==> pressed
    GPIO_PORTF_PUR_R |= 1 << 0;


    // 7. Digital Enable PF2 & PF0
    GPIO_PORTF_DEN_R |= (1 << 2 | 1 << 0);
}
int main()
{
    DIO_init();
    while(1)
    {
        // if switch is pressed
        if ((GPIO_PORTF_DATA_R & 1<< 0) == 1){
            // LED ON
            GPIO_PORTF_DATA_R |= 1 << 2;
        }
        else {
            //LED OFF
            GPIO_PORTF_DATA_R &= ~(1 << 2);
        }
    }
}