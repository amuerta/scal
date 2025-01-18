#ifndef ARRAY_LEN
#	define ARRAY_LEN(array) (sizeof(array)/sizeof(array[0]))
#endif


#define MAX_EXPR_SIZE		 256

static const ScalConfig SCAL_CONFIG = {
	.corner_radius 			= 2.0,
	.transition_in_speed 	= 10.0,
	.transition_out_speed 	= 16.0,

	.text_size_small		= 24.0,
	.text_size_normal		= 32.0,
	.text_size_big			= 36.0,


	.button_stock			= {100,110,150,255},
	.button_hover			= {120,120,200,255},
	.button_click			= {40 ,40 ,80 ,255},
	.button_outline_px		= 2,

	.text_stock			   	= {70 ,70 ,90 ,255},
	.text_hover			   	= {100,100,140,255},
	.text_click			   	= {190,190,240,255},
	
	.grid_background		= {60 ,60 ,70 ,255},
	.window_background		= {30 ,35 ,45 ,255},

	// maximum size of input string
	// for evaluation by scal, on overflow
	// throws INVALID EXPRESSION error
	.max_expr_size 			= MAX_EXPR_SIZE,
};

// compile-time computet constants for gui and GS
const Clay_String KEY_LABELS[]  = {
	CLAY_STRING("1"), CLAY_STRING("2" ), CLAY_STRING("3"), CLAY_STRING("+"  ),
	CLAY_STRING("4"), CLAY_STRING("5" ), CLAY_STRING("6"), CLAY_STRING("-"  ),
	CLAY_STRING("7"), CLAY_STRING("8" ), CLAY_STRING("9"), CLAY_STRING("*"  ),
	CLAY_STRING("("), CLAY_STRING("0" ), CLAY_STRING(")"), CLAY_STRING("/"  ),
	CLAY_STRING("C"), CLAY_STRING("B" ), CLAY_STRING("="), CLAY_STRING("."	)
};

// mingw-gcc is upset
#define KEYS_COUNT 			 ARRAY_LEN(KEY_LABELS)		

/* very upset.
// this generates a warning, but its perfectly fine
static char		EXPRESSION_BUFFER	[MAX_EXPR_SIZE]		      	= {0}; // ok
static float	HOVER_BUFFER		[KEYS_COUNT]		      	;	// ok
static char 	SCRATCH_BUFFER		[MAX_EXPR_SIZE]				= {0}; // ok
*/

ScalGlobalState APP_STATE = {
	
	.scratch_buffer			= 0,//SCRATCH_BUFFER,
	.input_expr_len 		= 0,
	.input_expression 		= 0,//EXPRESSION_BUFFER,
	.buttons_hover_time 	= 0,//HOVER_BUFFER,
	.had_error				= false,

	.pressed_button_id 		= -1,
	.hovered_button_id	 	= -1,
};
