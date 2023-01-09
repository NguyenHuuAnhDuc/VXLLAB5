#include "fsm.h"
#include "main.h"
#include <inttypes.h>

enum Command_state {
		Begin,
		Body
};
static enum Command_state command_state = Begin;

uint8_t index_command = 0;
void command_parser_fsm (){
	switch (command_state){
	case Begin:
		if(buffer[curr_index_buffer]=='!') {
			curr_index_buffer++;
			index_command = 0;
			command_flag = 0;
			command_state = Body;
		}
		break;
	case Body:
		if(buffer[curr_index_buffer]=='!') break;
		if(buffer[curr_index_buffer]=='#') {

			command_data[index_command] = '\0';
			if(strcmp(command_data, "RST") == 0) {
				command_flag = 1;
			}
			else if(strcmp(command_data,"OK")  == 0) {
				command_flag = 1;
			}

			command_state = Begin;
		}
		else {
			command_data[index_command++] = buffer[curr_index_buffer];
		}
		curr_index_buffer++;
		if(curr_index_buffer==30) curr_index_buffer=0;
		break;
	}
}
uint8_t strr[30];
uint32_t ADC_value;
enum Uart_state {
		Init,
		Wait
};
static enum Uart_state uart_state = Init;

void uart_communiation_fsm () {
	switch (uart_state){
	case Init:
		if(command_flag==1 && strcmp(command_data, "RST") == 0) {
			HAL_GPIO_TogglePin ( GPIOA , GPIO_PIN_5 ) ;
			ADC_value = HAL_ADC_GetValue (&hadc1);
			int len = sprintf ( strr , "\r\n!ADC=%d#\r\n", ADC_value );
			HAL_UART_Transmit (&huart2 , (void *) strr, len, 1000);
			command_flag = 0;
			uart_state = Wait;
			setTimer0 (3000);
		}
		break;
	case Wait:
		if(command_flag==1 && strcmp(command_data, "OK") == 0) {
			uart_state = Init;
			command_flag = 0;
		}
		else if (timer0_flag){
			int len = sprintf ( strr , "\r\n!ADC=%d#\r\n", ADC_value );
			HAL_UART_Transmit (&huart2 , (void *) strr, len, 1000);
			setTimer0 (3000);
		}
		break;
	}
}
