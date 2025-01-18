#include "main.h"
#include <raylib.h>

#ifndef loop
#	define loop(I,N) for(size_t I = 0; I < (N); I++)
#endif


#ifndef MIN
#	define MIN(A,B) ((A) > (B) ) ? (B) : (A)
#endif

#ifndef MAX
#	define MAX(A,B) ((A) < (B) ) ? (B) : (A)
#endif

#define SCREEN_DIMENSIONS	 	\
(Clay_Dimensions) {				\
	.width = GetScreenWidth(),	\
	.height = GetScreenHeight()	\
}								\

#define OUTLINE(THICKNESS,COLOR) \
	CLAY( CLAY_RECTANGLE({.color = COLOR}),			\
			CLAY_LAYOUT({								\
				.padding = {(THICKNESS),(THICKNESS)},	\
				.sizing = SZ_GROW,						\
				.childAlignment =  {					\
				CLAY_ALIGN_X_CENTER,					\
				CLAY_ALIGN_Y_CENTER,					\
	}}))

Clay_Sizing SZ_GROW = {
	.width  = CLAY_SIZING_GROW(),
	.height = CLAY_SIZING_GROW(),
};

Clay_ChildAlignment ALIGN_CENTER = {
	CLAY_ALIGN_X_CENTER,
	CLAY_ALIGN_Y_CENTER,
};


const int 		FONT_ID_BODY_16 	= 0							;


typedef enum {
	Button_None,
	Button_Hovered,
	Button_Pressed,
	Button_Held,
} ButtonState;


float lerp(float a, float b, float f)
{
    return a + f * (b - a);
}

Clay_Color Clay_ColorLerp(Clay_Color c1, Clay_Color c2, float factor) {
	Vector3 r = Vector3Lerp(
			(Vector3) {c1.r, c1.g, c1.b},
			(Vector3) {c2.r, c2.g, c2.b},
			factor
		);
	return (Clay_Color) { r.x, r.y, r.z, 255 };
}

Color ClayColor_to_Color(Clay_Color c) {
	return (Color){c.r, c.g, c.b, c.a};
}

Clay_Color Color_to_ClayColor (Color c) {
	return (Clay_Color) {c.r, c.g, c.b, c.a};
}

// GUI FUNCTIONS: {
ButtonState UiButton(Clay_String name, size_t id) {

	float 		hover_time 	= APP_STATE.buttons_hover_time[id];
	ButtonState response	= Button_None;
	
	Clay_Color 	bg_idle		= SCAL_CONFIG.button_stock;	// {100,100,140,255};
	Clay_Color 	bg_hover	= SCAL_CONFIG.button_hover;	// {120,120,200,255};
	Clay_Color 	bg_pressed	= SCAL_CONFIG.button_click;	// {40 ,40 ,80 ,255};
	Clay_Color 	bg_now		= bg_idle;
	
	Clay_Color 	tx_idle		= SCAL_CONFIG.text_stock;	
	Clay_Color 	tx_hover	= SCAL_CONFIG.text_hover;	
	Clay_Color 	tx_pressed	= SCAL_CONFIG.text_click;	
	//Clay_Color 	tx_now		= tx_idle;

	Clay_Color 	outline_now	= bg_pressed;
	bool		hovered 	= Clay_Hovered();
	bool 		pressed		= hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
	bool 		held		= hovered && IsMouseButtonDown(MOUSE_BUTTON_LEFT);
	float 		text_sz_nor = SCAL_CONFIG.text_size_normal;
	float 		text_sz_big = SCAL_CONFIG.text_size_big;
	float 		text_sz 	= lerp(text_sz_nor, text_sz_big, hover_time);
	
	#define MAX_FACTOR 1.0

	float factor_time_in 	= 4.0;
	float factor_time_out 	= 16.0;

	if (hovered) {
		if(hover_time < MAX_FACTOR)
			APP_STATE.buttons_hover_time[id] += GetFrameTime() * factor_time_in;
	} 
	else {
		if (hover_time > 0)
			APP_STATE.buttons_hover_time[id] -= GetFrameTime() * factor_time_out;
		else 
			APP_STATE.buttons_hover_time[id] = 0;
	}


	float lerp_factor = APP_STATE.buttons_hover_time[id];
	response 	= Button_Hovered;

	if (!pressed && !held) {
		bg_now 		= Clay_ColorLerp(bg_idle,bg_hover,lerp_factor);
		outline_now	= Clay_ColorLerp(tx_idle,tx_hover,lerp_factor);
	} 
	else {
		bg_now 		= Clay_ColorLerp(bg_hover,bg_pressed,lerp_factor);
		outline_now = Clay_ColorLerp(tx_hover,tx_pressed,lerp_factor);
	}

	
	OUTLINE(2,outline_now) {

		CLAY( CLAY_RECTANGLE({.color = bg_now }),
			CLAY_LAYOUT({
				.sizing = SZ_GROW,
				.childAlignment =  ALIGN_CENTER,
			})
		)

		{
			CLAY_TEXT(name,CLAY_TEXT_CONFIG({
						.fontId = FONT_ID_BODY_16,
						.fontSize = text_sz,
						.textColor = {255,255,255,255}
						}));
		} // button body
	} // outline

	if (pressed)
		response = Button_Pressed;

	return response;
}


void UiButtonGrid(Clay_String* labels, size_t rows, size_t columns) {
	loop(r,rows) {
		
		CLAY(
			//CLAY_RECTANGLE({.color = {100*r,100,100,255}}),
			CLAY_RECTANGLE({.color = SCAL_CONFIG.grid_background}),
			CLAY_LAYOUT({
				.layoutDirection = CLAY_TOP_TO_BOTTOM,
				.sizing = SZ_GROW,
		})){
			
			loop(c,columns) {
				CLAY(
					CLAY_RECTANGLE({.color = SCAL_CONFIG.grid_background}),
					//CLAY_RECTANGLE({.color = {100,100*c,100,100}}),
					CLAY_LAYOUT({
						.sizing = SZ_GROW,
						.padding = {4,4},
				})) {


					size_t index 	= c*rows+r;
					//size_t pressed 	= APP_STATE.pressed_button_id;
					size_t hovered 	= APP_STATE.hovered_button_id;
					float  hover_t  = APP_STATE.buttons_hover_time
						[index];
					const float hover_max_t = 1.0;


					// animation
					if (index == hovered) {
						if (hover_t >= hover_max_t)
							APP_STATE.buttons_hover_time[index] += GetFrameTime();
					} 


					ButtonState response = UiButton(labels[index],index);
					if (response == Button_Pressed) 
						APP_STATE.pressed_button_id = index;
				}
			}
		}
	}	
}

void UiDisplay(float height) {

	CLAY(
		CLAY_SCROLL({.horizontal = true}),
		CLAY_RECTANGLE({.color = SCAL_CONFIG.grid_background}),
		CLAY_LAYOUT({
			.padding = {8,0},
			.childAlignment =  {
				CLAY_ALIGN_X_LEFT,
				CLAY_ALIGN_Y_CENTER,
			},

			.layoutDirection = CLAY_LEFT_TO_RIGHT,
			.sizing = {
				.width  = CLAY_SIZING_GROW(),
				.height = CLAY_SIZING_FIXED(height),
			}
		})) 

	{
		//int id = APP_STATE.pressed_button_id;
		Clay_String label = {0};
		size_t expr_empty = APP_STATE.input_expr_len == 0;

		if ( expr_empty && 
				APP_STATE.eval_count == 0) 
		{
			label = CLAY_STRING("None");
		} 

		else if (APP_STATE.had_error && expr_empty) {
			label = CLAY_STRING("Syntax error");
		}

		else if (expr_empty) {
			float prec = APP_STATE.eval_result - (float)((int)APP_STATE.eval_result);
			
			// simple leftover check
			if (prec) // 1
				sprintf(APP_STATE.scratch_buffer,"%.6f",APP_STATE.eval_result);
			else 
				sprintf(APP_STATE.scratch_buffer,"%.0f",APP_STATE.eval_result);

			label = (Clay_String) {
				.chars = APP_STATE.scratch_buffer,
				.length = strlen(APP_STATE.scratch_buffer)
			};//CLAY_STRING(APP_STATE.scratch_buffer);
		} 

		else {
			// macro does its job bad :(
			label = (Clay_String) { 
				.length = APP_STATE.input_expr_len, 
				.chars = APP_STATE.input_expression 
			};
		}

		// FIGURE OUT THE BUG
		CLAY_TEXT(label, CLAY_TEXT_CONFIG({
					.fontId = FONT_ID_BODY_16,
					.fontSize = 32,
					.textColor = {255,255,255,255}
				})
		);
	}
}

void UiRoot(void) {
	CLAY(
		CLAY_RECTANGLE({.color = SCAL_CONFIG.window_background}),
		CLAY_LAYOUT({
			.layoutDirection = CLAY_TOP_TO_BOTTOM,
			.padding = { 10, 10 },
			.childGap = 10,
			.sizing = 	SZ_GROW,
		})
	) {
		Clay_Color outline = {120,120,120,255};
		
		CLAY(
				CLAY_RECTANGLE({.color = outline}),
				CLAY_LAYOUT({
					.padding = { 2, 2 },
					.sizing = {
					.width  = CLAY_SIZING_GROW(),
				}})) 
		{
			UiDisplay(100);
		}

		CLAY(
			CLAY_RECTANGLE({.color = SCAL_CONFIG.grid_background}),
			CLAY_LAYOUT({
				.padding = { 2, 2 },
				//.layoutDirection = CLAY_TOP_TO_BOTTOM,
				.sizing = {
					.width  = CLAY_SIZING_GROW(),
					.height = CLAY_SIZING_GROW(),
				}
		})) 

		{
			UiButtonGrid((Clay_String*)KEY_LABELS,4,5);
		}
	}
}
