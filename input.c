#include "main.h"
#include <assert.h>

typedef enum {
	Command_None,
	Command_Clear,
	Command_StepBack,
	Command_Eval
} ScalInputCommand;


// key table replica but as chars.
const char KEY_TABLE[] = {
	'1', '2' , '3', '+',
	'4', '5' , '6', '-',
	'7', '8' , '9', '*',
	'(', '0' , ')', '/',
	'C', 'B' , '=', '.'
};

int buffer_append(char* buffer, size_t* len, size_t capacity, char key) {
	if (!key)
		return BUFFER_NULL_KEY;

	size_t length = *len;
	if (length + 1 < capacity) {
		buffer[(*len)++] = key;
		return BUFFER_OK;
	} 
	return BUFFER_REACHED_END;
}

void buffer_popfront(char* buffer, size_t* len, size_t capacity) {
	assert(*len > capacity && "Undesired state of buffer happened... somehow");
	if ((*len) == 0)
		return;
	buffer[--(*len)] = 0;
}


// TODO: make it so function return a ScalInputCommand or something
// and the other part of the calculator handles the thing.
ScalInputCommand buffer_read_keyboard
	(	char* buffer, 
		size_t* len, 
		size_t capacity) 

{
	int 	key 	= 	GetKeyPressed();
	char 	c;

	static bool down;

	if (IsKeyDown(KEY_LEFT_SHIFT))
		down = IsKeyDown(KEY_LEFT_SHIFT);

	switch (key) {
		case KEY_ENTER:
				return Command_Eval;
		case KEY_EQUAL:
			{
				if (down)
					c = '+';
				else
					return Command_Eval;
		
				buffer_append(buffer,len,capacity,c);
				printf("%s\n",buffer);
				fflush(stdout);
			} break;

		case KEY_X: // same as *
			{
				c = '*';
				buffer_append(buffer,len,capacity,c);
				printf("%s\n",buffer);
				fflush(stdout);
			} break;

		case KEY_ONE:
		case KEY_TWO:
		case KEY_THREE:
		case KEY_FOUR:
		case KEY_FIVE:
		case KEY_SIX:
		case KEY_SEVEN:
		case KEY_EIGHT:
		case KEY_NINE:
		case KEY_ZERO:
			{

				if 		(down && key == KEY_EIGHT) 
					c = '*';
				else if (down && key == KEY_NINE) 
					c = '(';
				else if (down && key == KEY_ZERO)
					c = ')';
				else 
					c = '0' + (key - KEY_ZERO);

				// nasty hack
				buffer_append(buffer,len,capacity,c);
				printf("%s\n",buffer);
				fflush(stdout);
			} break;

		case KEY_KP_0:
		case KEY_KP_1:
		case KEY_KP_2:
		case KEY_KP_3:
		case KEY_KP_4:
		case KEY_KP_5:
		case KEY_KP_6:
		case KEY_KP_7:
		case KEY_KP_8:
		case KEY_KP_9:
			{
				c = '0' + (key - KEY_KP_0);
				buffer_append(buffer,len,capacity,c);
				printf("%s\n",buffer);
				fflush(stdout);
			} break;
	
		case KEY_PERIOD:
			{
				c = '.';
				buffer_append(buffer,len,capacity,c);
				printf("%s\n",buffer);
				fflush(stdout);
			} break;

		case KEY_MINUS:
			{
				c = '-';
				buffer_append(buffer,len,capacity,c);
				printf("%s\n",buffer);
				fflush(stdout);
			} break;

		case KEY_SLASH:
			{
				c = '/';
				buffer_append(buffer,len,capacity,c);
				printf("%s\n",buffer);
				fflush(stdout);
			} break;

		case KEY_BACKSPACE:
			{
				return Command_StepBack;
				buffer_popfront(buffer,len,capacity);
				printf("%s\n",buffer);
				fflush(stdout);
			} break;

		case KEY_C:
			{
				return Command_Clear;
				memset(buffer,0,*len);
				*len = 0;
			} break;
		default: break;
	}
	down = false;
	return Command_None;
}


ScalInputCommand buffer_read_screen_keys(
		char* buffer, 
		size_t* len, 
		size_t capacity, 
		int* id) 

{
	if  (*id < 0) {
		return Command_None;
	}

	char c = KEY_TABLE[*id];

	if ((*id) >= 0) {
		switch (c) {

			case 'C':
				return Command_Clear;
			
			case 'B':
				return Command_StepBack;

			case '=':
				return Command_Eval;

			default:
				buffer_append(buffer,len,capacity,c);
				break;
		}

		(*id) = -1;
		printf("%s\n",buffer);
		fflush(stdout);
	}

	return Command_None;
}
