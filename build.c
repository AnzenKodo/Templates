#include "base/base_include.h"
#include "os/os_include.h"

#include "base/base_include.c"
#include "os/os_include.c"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PROJECT_NAME    "Scuttle"
#define PROJECT_VERSION "0.1"
#define BUILD_MAIN_FILE "main.c"
#define BUILD_DIR       "build"

const char *help_message = "build.c: C file that build's C projects.\n"
"Version: "PROJECT_VERSION"\n"
"Options:\n"
"   build                   Build project\n"
"   run                     Run project\n"
"   build-run               Build and Run project\n"
"   test                    Test project\n"
"   version --version -v    Print project version\n"
"   help --help -h          Print help\n";

char cmd[100] = "";

fn void build_cmd_append(char *cmd, char *src);
fn void build_cmd_finish(char *cmd);
fn void build_cmd_run(char *cmd);
fn bool build_is_cpp_file(Str8 filename);

fn void build_compile_cc(char *cmd) {
    build_cmd_append(cmd, "cc "BUILD_MAIN_FILE);
    build_cmd_append(cmd, " -o ./"BUILD_DIR"/"PROJECT_NAME); // Output
    build_cmd_append(cmd, " -ggdb -g3");                     // Debug
    build_cmd_append(cmd, " -Wall -Wextra");                 // Warnings
    build_cmd_append(cmd, " -lX11 -lXrandr -lm");            // Libs
    build_cmd_append(cmd, " -DBUILD_DEBUG -O3 -fsanitize=address");             // Define
}

fn void build_compile_mingw(char *cmd) {
    build_cmd_append(cmd, "x86_64-w64-mingw32-gcc  "BUILD_MAIN_FILE);
    build_cmd_append(cmd, " -o ./"BUILD_DIR"/"PROJECT_NAME); // Output
    build_cmd_append(cmd, " -ggdb -g3");                     // Debug
    build_cmd_append(cmd, " -Wall -Wextra");                 // Warnings
}

fn void build_compile(char *cmd)
{
    if (os_dir_make(str8(BUILD_DIR))) {
        printf("Created `"BUILD_DIR"` directory.\n");
    }

    printf("Compiling:\n");
    build_compile_cc(cmd);
    build_cmd_append(cmd, " -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-unused-but-set-variable -Wno-missing-braces"); // Disable useless warnings
    if (!build_is_cpp_file(str8(BUILD_MAIN_FILE))) {
        build_cmd_append(cmd, "  -Wno-incompatible-pointer-types -Wno-override-init"); // Disable useless warnings in C
    }
    build_cmd_finish(cmd);
}

fn void build_run(char *cmd)
{
    build_cmd_append(cmd, "./"BUILD_DIR"/"PROJECT_NAME);
    printf("Running:\n");
    build_cmd_finish(cmd);
}

fn void build_run_wine(char *cmd) {
    build_cmd_append(cmd, "wine ./"BUILD_DIR"/"PROJECT_NAME);
    printf("Running:\n");
    build_cmd_finish(cmd);
}

fn void build_test(char *cmd)
{
    printf("Test Compiling in C:\n");
    build_cmd_append(cmd, "cc -x c ");
    build_compile_cc(cmd);
    build_cmd_append(cmd, " -Wno-unused-function -Wno-incompatible-pointer -Wno-incompatible-pointer-types -Wno-override-init -Wno-missing-braces"); // Disable useless warnings
    build_cmd_finish(cmd);

    printf("Test Compiling in C++:\n");
    build_cmd_append(cmd, "cc -x c++ ");
    build_compile_cc(cmd);
    build_cmd_append(cmd, " -Wno-unused-function -Wno-missing-braces"); // Disable useless warnings
    build_cmd_finish(cmd);

    printf("Test Typos:\n");
    build_cmd_append(cmd, "typos");
    build_cmd_finish(cmd);

    printf("Test Memory Leaks:\n");
    build_cmd_append(cmd, "valgrind ");
    build_cmd_append(cmd, " --leak-check=full --track-origins=yes ");
    build_cmd_append(cmd, "./"BUILD_DIR"/"PROJECT_NAME);
    // build_cid_append(cmd, " --leak-check=full --show-leak-kinds=all");
    build_cmd_finish(cmd);
}

int main(int argc, char *argv[])
{
    // char *cmd = getenv("CMDLINE");
    char *option = argv[1];

    if (!option) {
        fprintf(stderr, "Error: no options provided.\n\n");
        printf(help_message);
    } else if (str8_match(str8_from_cstr(option), str8("build"))) {
        build_compile(cmd);
    } else if (str8_match(str8_from_cstr(option), str8("build-run"))) {
        build_compile(cmd);
        build_run(cmd);
    } else if (str8_match(str8_from_cstr(option), str8("run"))) {
        build_run(cmd);
    } else if (str8_match(str8_from_cstr(option), str8("test"))) {
        build_test(cmd);
    } else if (
        str8_match(str8_from_cstr(option), str8("version")) ||
        str8_match(str8_from_cstr(option), str8("--version")) ||
        str8_match(str8_from_cstr(option), str8("-v"))
    ) {
        printf(PROJECT_VERSION);
    } else if (
        str8_match(str8_from_cstr(option), str8("help")) ||
        str8_match(str8_from_cstr(option), str8("--help")) ||
        str8_match(str8_from_cstr(option), str8("-h"))
    ) {
        printf(help_message);
    } else {
        fprintf(stderr, "Error: wrong option provided `%s`.\n\n", option);
        printf(help_message);
    }
}


fn bool build_is_cpp_file(Str8 filename)
{
    return str8_ends_with(filename, str8(".cpp"));
}

fn void build_cmd_append(char *cmd, char *src)
{
	strcat(cmd, src);
}

fn void build_cmd_run(char *cmd)
{
	printf("%s\n\n", cmd);
	int err = system(cmd);
	if (err)
	{
		fprintf(stderr, "\nError: %s\n", strerror(err));
		os_exit(err);
	}
}

fn void build_cmd_finish(char *cmd)
{
	build_cmd_run(cmd);
	mem_set(cmd, 0, cstr8_length(cmd));
}
