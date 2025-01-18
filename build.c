#define NOB_IMPLEMENTATION
#include "nob.h"

char *cc 	= "clang";
char *debug = "-ggdb";
char *in 	= "main.c";
char *out	= "scal";
char *result_dir	
			= "./build_output/";

// -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 
char *libs = "";

bool DISABLED_FONT = false;

typedef enum {
	Target_Posix,
	Target_Window,
} BuildTarget;

void build_gui(Nob_Cmd* cmd, bool full_build, BuildTarget target) {
	
	char* build_path = nob_temp_sprintf("%s%s",result_dir,out);
	
	if (full_build)
		nob_mkdir_if_not_exists(result_dir);

	switch(target) {
		case Target_Posix:
			nob_cmd_append(cmd, 
					cc, 
					"-Wall", "-Wextra",
					"-lraylib", "-lGL",
					"-lm", "-lpthread",
					"-ldl", "-lrt", "-lX11",
					// "-fsanitize=address", // clay has use-after-free, cool :/
					(full_build) ? "" : debug, 
					"-o", 
					(full_build) ?  build_path : out, 
					in); 
			break;

		case Target_Window:
			nob_cmd_append(cmd,
					"x86_64-w64-mingw32-gcc",
					"main.c",
					"-o",
					nob_temp_sprintf("%s%s%s", result_dir,out,".exe"),
					"-I./lib/raylib-5.5_win64_mingw-w64/include/",
					"-L./lib/raylib-5.5_win64_mingw-w64/lib/",
					"-lraylib",
					"-lwinmm",
					"-lgdi32"
			);
			break;
	}

}

// dont use this, it was for debugging
// and learning. 
void build_eval(Nob_Cmd* cmd) {
	nob_cmd_append(cmd,
		cc,
		"-Wall", "-Wextra",
	//	"-fsanitize=address",
		debug, "-o", 
		"eval", "eval.c"
	); 
}

int main(int argc, char** argv) {
	NOB_GO_REBUILD_URSELF(argc,argv);
	
	Nob_Cmd cmd = {0};

	BuildTarget target = Target_Posix;
	if (argc>2 && strcmp(argv[2],"linux") == 0) {
		target = Target_Posix;
	} else if (argc>2 && strcmp(argv[2],"windows") == 0) {
		target = Target_Window;
	} else {
		nob_log(NOB_INFO, "No target speicified, building for 'Target_Posix'");
	}
			

	build_gui(
			&cmd,
			(argc>1 && strcmp(argv[1],"release")==0),
			target
	);

	#define flag(STR) ((STR) && strcmp(argv[i],(STR))==0)
	if (argc > 2) for(int i = 2; i < argc; i++) {
		if flag("--default-font") {
			DISABLED_FONT = true;
			nob_cmd_append(&cmd, "-DNO_ASSET_FONT");
			nob_log(NOB_INFO,"Using default raylib font due to flag");
		}
	}

	if (!DISABLED_FONT && !nob_file_exists("./assets/AppFont.ttf")) {
		nob_cmd_append(&cmd, "-DNO_ASSET_FONT");
		nob_log(NOB_WARNING,"No Application font found, using default raylib font.");
	}

	if (!nob_cmd_run_sync(cmd))
		return 1;

	if (argc > 1 && strcmp(argv[1],"run")==0) {
		cmd.count = 0;
		nob_cmd_append(&cmd,
				nob_temp_sprintf("./%s", out));
		if (!nob_cmd_run_sync(cmd))
			return 1;
	} 

	
	return 0;
}
