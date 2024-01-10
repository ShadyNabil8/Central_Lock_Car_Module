# I have learned

1. Dealing with SysTick timer.
I have added a function called ```Millis``` that returns a ```uint32_t``` number called ```uwTick```. Tihs number is incremented every 1ms using the ```systick``` timer. This timer runs with 1KHZ by default. I placed it in ```time.c``` file I have created in src folder.

2. I haved added a helper function that calculate the absolute of a number and called it ```ABS```. I placed it in ```stm32f1xx_it.c```. To make this function take any type of parameters, simply make it as a MACRO. It is easy and logical but it wasn't the first thing I have tryied.

# Chalenges

## Syncrhonozation between TX and RX

When I tried to use this function  ```HAL_UART_Receive_IT(&huart1, CodeBuffer, CODE_LENGTH);``` to receive the 6 bytes from the key I encountered a problem. If there is an error while sending like a byte didn't reach the RX UART. For example if this byte is the 3rd byte. Then the 4th byte will be placed in the location of the 3rd byte. This is the problem. So I can't depend on this feature from the provided UART driver and I should receive each byte alone and use sequence numbers mechanism ti check if the incomming byte is the write byte or not and if so I will place it in its right location. If it was not the expected byte, I will reset the sequence.

## Debouncing problem

I handled this issue using a function called ```millies``` that calculate the milliseconds since the MCU start. I made a variable that carries the time of last time the interrupt button is pressed lets call it ```prev```. when there is an interrupt I will compare the value  of ```prev``` with the value of the current time from the ```millies``` and if the difference was greater then say 200ms then it is surlly a debouncing effect so ignore this interrupt.

I wrote this function in the STM32F108C8T6 MCU myself. The idea was using the sysTick timer. this timer is trigggered and call a certain function evety 1ms because he runs on 1KHZ by default. inside this funcion there is a varible which is incremented inside this function. so I use this variable inside my ```millies``` function to get the time elapced since the MCU started. Surrly when this variable overflows it will return back to zero.

But this function is bult-in in AArduino framework.

## Difference in operatin voltages in the two UART modules.

I am using STM32F108C8T6 which operates with 3.3V and Arduino UNO which operates on 5V so the
