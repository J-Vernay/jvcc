
#include <stdlib.h>
#include <string.h>

#include "general/IO.h"

static void showHelp();
static void showUsage();
static void showLicenses();

int main(int argc, char** argv) {
    if (argc <= 1) {
        showHelp();
    } else if (strcmp(argv[1], "help") == 0) {
        showHelp();
    } else if (strcmp(argv[1], "show-licenses") == 0) {
        showLicenses();
    } else {
        printf("ERROR: Unknown subcommand '%s'. Did you mean:\n", argv[1]);
        showUsage();
        exit(EXIT_FAILURE);
    }

    return 0;
}

static void showLicenses() {
    puts(
        "jvcc is being made by Julien Vernay ( jvernay.fr ).\n"
        "Currently all-rights-reserved.\n"
        "\n"
        "Here are license attributions from its dependencies:\n"
        "- jvcmd: Copyright (c) 2021 Julien Vernay ( jvernay.fr )\n"
        "         MIT-licensed C library for command-line interface.\n"
        "         https://github.com/J-Vernay/jvcmd");
}

static void showUsage() {
    puts(
        "    jvcc help\n"
        "    jvcc show-licenses\n");
}

static void showHelp() {
    puts(
        "jvcc is a C compiler project being made by Julien Vernay.\n"
        "For now, it is only an incomplete prototype.\n"
        "\n"
        "EXAMPLES:\n"
        "    jvcc compile test.o    test.c\n"
        "    jvcc link    libtest.a test.o test2.o\n"
        "\n"
        "USAGE:");
    showUsage();
}