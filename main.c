#include "main.h"


# if 0
int main(void) {

	char* source = "1+2+3+4";
	float result = parse(source);
	printf("\nRes: %f\n",result);

	result = parse(source);
	printf("\nRes: %f\n",result);

	result = parse(source);
	printf("\nRes: %f\n",result);
}
#endif

#define DEBUG

#ifdef DEBUG
# define debug printf
#endif

#ifndef DEBUG
#define debug //
#endif


void Command_process_input(ScalInputCommand command) {
	bool had_errors = false;

	switch (command) {
		case Command_None:
			break;

		case Command_StepBack:
			buffer_popfront(
					APP_STATE.input_expression,
					&APP_STATE.input_expr_len,
					MAX_EXPR_SIZE
				); 
			break;
		
		case Command_Clear:
			memset(
				APP_STATE.input_expression,
				0,
				APP_STATE.input_expr_len
			);
			APP_STATE.input_expr_len = 0;
			break;

		case Command_Eval:
			if (APP_STATE.input_expr_len == 0)
				return;

			debug("'%s'\n",APP_STATE.input_expression);
			APP_STATE.eval_result = parse(APP_STATE.input_expression, &had_errors);
			APP_STATE.eval_count++;
			debug("$ = %f\n",APP_STATE.eval_result);

			float prec = APP_STATE.eval_result - (float)((int)APP_STATE.eval_result);
			APP_STATE.had_error = had_errors;
			debug("prec: %f\n",prec / 100);
			debug("has_errors : %s\n",had_errors ? "true" : "false");
			memset(
				APP_STATE.input_expression,
				0,
				APP_STATE.input_expr_len
			);
			APP_STATE.input_expr_len = 0;
				
			debug("%f\n",APP_STATE.eval_result);
			fflush(stdout);
			break;
	}
}

// ~~todo~~
// ~~make global state passable everywhere~~
// ~~since it introduces gui bugs due to memory corruption occuring~~
//
// It actually doesn't, i guess it has something to do with 
// fork | printing | my parser, causing oveflow somewhere, which causes clay
// to freak out. this doesn't happen when printing is disabled.
//
// dubious to say at least. I guess i have to put more work into my parser :/


static char		EXPRESSION_BUFFER	[MAX_EXPR_SIZE]		      	; // ok
static float	HOVER_BUFFER		[KEYS_COUNT]		      	;	// ok
static char 	SCRATCH_BUFFER		[MAX_EXPR_SIZE]				; // ok

#if 1
int main(void) {
	

	APP_STATE.scratch_buffer 		= SCRATCH_BUFFER;
	APP_STATE.buttons_hover_time 	= HOVER_BUFFER;
	APP_STATE.input_expression 		= EXPRESSION_BUFFER;

	Clay_Raylib_Initialize(300,450,"Scal - Silly Calculator",0);
	SetTargetFPS(60);

	uint64_t clay_memory_sz = Clay_MinMemorySize();
	Clay_Arena memory;
	Clay_Dimensions screen;
	Clay_RenderCommandArray render_que;

	screen = SCREEN_DIMENSIONS;
	memory = (Clay_Arena) {
			.memory = malloc(clay_memory_sz),
			.capacity = clay_memory_sz,
	};
	
	Clay_Initialize(memory, screen);
	Clay_SetMeasureTextFunction(Raylib_MeasureText);
	
#ifdef NO_ASSET_FONT
	Font app_font = GetFontDefault();
#endif

#ifndef NO_ASSET_FONT
	Font app_font = LoadFontEx("./assets/AppFont.ttf",16,0,110);
#endif

	// on windows this causes crash, no auto font adding i guess.
	if (!FileExists("./assets/AppFont.ttf")) {
		app_font = GetFontDefault();
	}

	Raylib_fonts[FONT_ID_BODY_16] = (Raylib_Font) {
		.font = app_font,
		.fontId = FONT_ID_BODY_16
	};


	while(!WindowShouldClose()) {
	
		Clay_SetLayoutDimensions(SCREEN_DIMENSIONS);

		Vector2 mouse_position = GetMousePosition();
		Vector2 scroll_delta   = GetMouseWheelMoveV();
		
		Clay_SetPointerState(
			(Clay_Vector2) {mouse_position.x, mouse_position.y},
			IsMouseButtonDown(0)
		);
		Clay_UpdateScrollContainers (
				true,
				(Clay_Vector2) {scroll_delta.x,scroll_delta.y},
				GetFrameTime()
		);

		ScalInputCommand comm = {0};

		comm = buffer_read_keyboard(
				APP_STATE.input_expression,
				&APP_STATE.input_expr_len,
				MAX_EXPR_SIZE
			);
		Command_process_input(comm);
		
		comm = buffer_read_screen_keys(
				APP_STATE.input_expression,
				&APP_STATE.input_expr_len,
				MAX_EXPR_SIZE,
				&APP_STATE.pressed_button_id
			);
		Command_process_input(comm);


		Clay_BeginLayout();
		UiRoot();
		render_que = Clay_EndLayout();

		BeginDrawing();
		Clay_Raylib_Render(render_que);
		EndDrawing();
	}
}


#endif
