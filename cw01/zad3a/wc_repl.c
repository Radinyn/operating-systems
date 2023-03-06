#ifdef USE_DLL
    #include "libwc_so.h"
#else
    #include "libwc.h"
#endif

#include "dllmanager.h"
#include <regex.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/times.h>

#define PATH_BUFFER_SIZE 2048
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_RESET "\x1b[0m"
#define ANSI_COLOR_BOLD  "\x1b[1m"

typedef enum {
    EXIT,
    INVALID,
    INIT,
    COUNT,
    SHOW,
    DELETE,
    DESTROY,
} COMMAND_ID;

bool running = true;
int _;

COMMAND_ID id;
int num_input;
char path[PATH_BUFFER_SIZE];

size_t COUNT_LEN = strlen("count ");

LibWCMemory* memo = NULL;
bool is_initialised = false;

regex_t rINIT;
regex_t rCOUNT;
regex_t rSHOW;
regex_t rDELETE;
regex_t rDESTROY;
regex_t rEXIT;

int compile_regex() {
    int status = 0;
    status += regcomp(&rINIT, "init [0-9][0-9]*", 0);
    status *= 10;
    status += regcomp(&rCOUNT, "count ..*", 0); // spij splodko aniolku
    status *= 10;
    status += regcomp(&rSHOW, "show [0-9][0-9]*", 0);
    status *= 10;
    status += regcomp(&rDELETE, "delete [0-9][0-9]*", 0);
    status *= 10;
    status += regcomp(&rDESTROY, "destroy", 0);
    status *= 10;
    status += regcomp(&rEXIT, "exit", 0);
    return status;
}

bool is_whitespace(const char *s) {
  while (*s != '\0') {
    if (!isspace((unsigned char)*s))
      return false;
    s++;
  }
  return true;
}

size_t min(size_t a, size_t b) {
    return a < b ? a : b;
}

void parse_input(char* input, size_t input_len) {
    if (regexec(&rINIT, input, 0, NULL, 0) == 0) {
        id = INIT;
        sscanf(input, "init %d", &num_input);
    } else if (regexec(&rCOUNT, input, 0, NULL, 0) == 0) {
        id = COUNT;
        input += COUNT_LEN;
        size_t newline_position = strcspn(input, "\n");
        strncpy(path, input, min(newline_position, PATH_BUFFER_SIZE));
    } else if (regexec(&rSHOW, input, 0, NULL, 0) == 0) {
        id = SHOW;
        sscanf(input, "show %d", &num_input);
    } else if (regexec(&rDELETE, input, 0, NULL, 0) == 0) {
        id = DELETE;
        sscanf(input, "delete %d", &num_input);
    } else if (regexec(&rDESTROY, input, 0, NULL, 0) == 0) {
        id = DESTROY;
    } else if (regexec(&rEXIT, input, 0, NULL, 0) == 0) {
        id = EXIT;
    } else {
        id = INVALID;
    }
    
    if ((id != INVALID) && (id != DELETE) && (id != COUNT)) {
        if (num_input < 0) {
            id = INVALID;
            fprintf(stderr, "[WC REPL] EXPECTED A POSITIVE ARGUMENT\n");
        } else if ((num_input < 1) && (id == INVALID)) {
            id = INVALID;
            fprintf(stderr, "[WC REPL] SIZE HAS TO BE GREATER THAN 0\n");
        }
    }
}

void react_to_command() {
    if ((id > INIT) && (!is_initialised)) {
        fprintf(stderr, "[WC REPL] RUN 'init {size}' FIRST\n");
        return;
    }

    if ((id == INIT) && is_initialised) {
        fprintf(stderr, "[WC REPL] RUN 'destroy' BEFORE INITIALISING AGAIN\n");
        return;
    }

    switch (id) {
    case INIT:
        LibWCMemory_init(memo, num_input);
        is_initialised = true;
        break;
    case COUNT:
        LibWCMemory_push(memo, path);
        break;
    case SHOW:
        puts(LibWCMemory_get(memo, num_input));
        break;
    case DELETE:
        LibWCMemory_pop(memo, num_input);
        break;
    case DESTROY:
        LibWCMemory_destruct(memo);
        is_initialised = false;
        break;
    case EXIT:
        running = false;
        break;
    case INVALID:
        fprintf(stderr, "[WC REPL] INVALID COMMAND, TRY INSTEAD:\n");
        fprintf(stderr, "[WC REPL] init {size > 0}\n");
        fprintf(stderr, "[WC REPL] count {path}\n");
        fprintf(stderr, "[WC REPL] show {index >= 0}\n");
        fprintf(stderr, "[WC REPL] delete {index >= 0}\n");
        fprintf(stderr, "[WC REPL] destroy\n");
    }
}

int main(int argc, char** argv) {
    load_dll_symbols("libwc.so");
    int regex_status = compile_regex();
    if (regex_status) {
        fprintf(stderr, "[WC REPL] REGEX COMPILATION FAIL (%i)\n", regex_status);
        return 1;
    }

    memo = malloc(sizeof(LibWCMemory));

    struct timespec timespec_buff_start, timespec_buff_end;
    struct tms tms_buff_start, tms_buff_end;

    while (running) {
        printf(ANSI_COLOR_GREEN ANSI_COLOR_BOLD "[WC REPL] >>> " ANSI_COLOR_RESET);

        char* line = NULL;
        size_t line_len;

        _ = getline(&line, &line_len, stdin);
        fflush(NULL);

        if (is_whitespace(line)) {
            free(line);
            continue;
        }

        parse_input(line, line_len);
        fflush(NULL);

        // CLOCK REALTIME jest definiowane gdzies pozniej

        clock_gettime(CLOCK_REALTIME, &timespec_buff_start);
        times(&tms_buff_start);
        react_to_command();
        clock_gettime(CLOCK_REALTIME, &timespec_buff_end);
        times(&tms_buff_end);
        fflush(NULL);

        puts("EXECUTION TIME:");
        printf("REAL: %ldns\n", timespec_buff_end.tv_nsec - timespec_buff_start.tv_nsec);

        // na moim systemie zdefiniowane jako

        /*
        inline clock_t times(struct tms *t) {
            struct timespec ts;
            clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ts);
            clock_t ticks(static_cast<clock_t>(static_cast<double>(ts.tv_sec)  * CLOCKS_PER_SEC +
                                                static_cast<double>(ts.tv_nsec) * CLOCKS_PER_SEC / 1000000.0));
            t->tms_utime  = ticks/2U;
            t->tms_stime  = ticks/2U;
            t->tms_cutime = 0; // vxWorks is lacking the concept of a child process!
            t->tms_cstime = 0; // -> Set the wait times for childs to 0
            return ticks;
        }
        */

       // ciezko by to kiedykolwiek nie bylo 0

        printf("USER: %ldticks\n", tms_buff_end.tms_cutime - tms_buff_start.tms_cutime);
        printf("SYST: %ldticks\n", tms_buff_end.tms_cstime - tms_buff_start.tms_cstime);
        fflush(NULL);
        free(line);
    }

    return 0;
}
