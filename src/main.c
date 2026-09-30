#include <stdio.h>
#include <string.h>

typedef void (*forth_word_fn)(void);

typedef struct {
    const char *name;
    forth_word_fn fn;
} forth_word;

static void word_ping(void) {
    puts("PONG");
}

static const forth_word dictionary[] = {
    {"PING", word_ping},
};

static int run_word(const char *name) {
    size_t count = sizeof(dictionary) / sizeof(dictionary[0]);
    for (size_t i = 0; i < count; ++i) {
        if (strcmp(dictionary[i].name, name) == 0) {
            dictionary[i].fn();
            return 0;
        }
    }
    return 1;
}

int main(int argc, char **argv) {
    if (argc == 2) {
        return run_word(argv[1]);
    }

    puts("ForC M0: C IRC core + Forth VM");
    return 0;
}
