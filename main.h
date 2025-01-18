#pragma once

// LIBS
#include <stdio.h>
#include <raylib.h>

// ignore common warnings from clay
// (very noisy)
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wmissing-field-initializers"
# pragma clang diagnostic ignored "-Wsign-compare"
# pragma clang diagnostic ignored "-Wunused-parameter"

// CLAY
#define CLAY_IMPLEMENTATION
#include "clay.h"
#include "./clay_renderers/clay_renderer_raylib.c"

# pragma clang diagnostic pop

// CROSS FILE CONSTANTS

#define BUFFER_NULL_KEY 	0
#define BUFFER_REACHED_END -1
#define BUFFER_OK			1

// scal gs
typedef struct {
	size_t	eval_count; // times expression evaluation happened
	float	eval_result;
	bool	had_error;
	int 	pressed_button_id;
	int 	hovered_button_id;
	size_t	input_expr_len;
	float* 	buttons_hover_time;
	char*	input_expression;
	char* 	scratch_buffer;	
} ScalGlobalState;

// ui 
typedef struct {
	float 	corner_radius;
	float 	transition_in_speed;
	float 	transition_out_speed;

	float text_size_small	;
	float text_size_normal	;
	float text_size_big		;

	Clay_Color 	button_stock;
	Clay_Color 	button_hover;
	Clay_Color	button_click;
	float		button_outline_px;

	Clay_Color 	text_stock;
	Clay_Color	text_hover;
	Clay_Color	text_click;

	Clay_Color 	grid_background;
	Clay_Color 	window_background;

	size_t		max_expr_size;
} ScalConfig;

// SCAL CONFIG 
#include "config.h"

// BUFFER
#include "input.c"

// UI/LAYOUT DEFS
#include "ui.c"

// EVAL
#include "eval.c"

