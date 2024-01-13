# I Learned more about

## Debugging

I needed to use the dubugging feature all the time to see the data in different buffers and monitor the variables and the return status of different operations. And that makes it easier to discover any errors that occurred during the development. Also, it helped me to see the content of the registers to be aware of the current state of the MCU and check if the change that I made took place or not. The debugging was very helpful for me in this project.

## Dealing with flash memory in STM32F103C8T6 and Arduino UNO

# The challenges I faced

## Syncrhonozation between TX and RX

When I tried to use this function ```HAL_UART_Receive_IT(&huart1, CodeBuffer, CODE_LENGTH);``` to receive the 6 bytes from the key, I encountered a problem. If there is an error while sending, like a byte didn't reach the RX UART, For example, if this byte is the 3rd byte, Then the 4th byte will be placed in the location of the 3rd byte. This is the problem. So I can't depend on this feature from the provided UART driver, and I should receive each byte alone and use the sequence numbers mechanism to check if the incomming byte is the write byte or not, and if so, I will place it in its right location. If it was not the expected byte, I will reset the sequence.

After more thinking, it is more logical to use the feature provided by the CubeIDE and let the responsibility of receiving all the code bytes rest on it. I will finally check for the correctness of the code. If it was correct, I would use it and then clear the buffer. If Iw was not correct, I will send ```NOT_ACK``` to the sender to make him resend.

## Debouncing problem

I handled this issue using a function called ```millies``` that calculates the milliseconds since the MCU started. I made a variable that carries the time of the last time the interrupt button was pressed. Let's call it ```prev```. When there is an interrupt, I will compare the value of ```prev``` with the value of the current time from the '''millies''', and if the difference is greater than, say, 200 ms, then it is definitely a debouncing effect, so ignore this interrupt.

I wrote this function in the STM32F108C8T6 MCU myself. The idea was using the SysTick timer. This timer is triggered to call a certain function every 1 ms because it runs on 1 KHz by default. Within this function, a variable is incremented. So I use this variable inside my ```millies``` function to get the time elapsed since the MCU started. Surprisingly, when this variable overflows, it will return to zero.

But this function is built-in in the Arduino framework.

## Difference in operatin voltages in the two UART modules

I am using the STM32F108C8T6, which operates at 3.3V, and the Arduino UNO, which operates at 5V. But they work together normally in serial communication without any problems.

## Store data permanently

There is a number that I need to use in the code. And this number need not be reset to zero each time the MCU is on, so there must be a way to store its value permanently. You used the EEPROM inside the Arduino Uno that acts like a key to store this value.

In STM32F103C8T6, it is more complex. The flash memory here is a very big deal and I needed to understand this memory here before using it.

**In the STM32 the flash memory is devided into two sections:**

1. ```Main memory block:``` where we can store the firmware. With blue pill it is 64KBytes or 128KBytes.
2. ```Information block:``` Stores the bootloader and optional bytes.

**The main Flash memory can be programmed 16 bits at a time.**

**Flash memory interface (FLITF) features:**

1. Read interface with prefetch buffer (2 × 64-bit words)
2. Option byte Loader
3. Flash Program / Erase operation
4. Read / Write protection
5. Low-power mode

**The main Flash memory programming sequence in standard mode is as follows:**

1. Unlocking the Flash memory.
2. Check that no main Flash memory operation is ongoing by checking the BSY bit in the FLASH_SR register. The registers are not accessible in write mode when the BSY bit of the FLASH_SR register is set.
3. Set the PG bit in the FLASH_CR register(Flash programming chosen.).
4. Perform the data write (half-word) at the desired address.
5. Wait for the BSY bit to be reset.
6. Read the programmed value and verify.

![procedure](./Flash_Programming_procedure.png)

**The main Flash memory erase sequence in standard mode is as follows:**

1. Unlocking the Flash memory.
2. Check that no Flash memory operation is ongoing by reading the BSY bit in the FLASH_SR register
3. Set the PER bit in the FLASH_CR register(Page Erase chosen)
4. Program the FLASH_AR register to select a page to erase 5. Set the STRT bit in the FLASH_CR register(This bit triggers an ERASE operation when set. This bit is set only by software and reset when the BSY bit is reset.)
5. Wait for the BSY bit to be reset
6. Read the erased option bytes and verify
