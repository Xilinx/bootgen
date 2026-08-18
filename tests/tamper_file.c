#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    unsigned char buffer[4096];
    size_t bytes_read;
    FILE *input;
    FILE *output;
    long input_size;
    int last_byte;

    if (argc != 3) {
        fprintf(stderr, "usage: tamper-file <input> <output>\n");
        return EXIT_FAILURE;
    }
    input = fopen(argv[1], "rb");
    if (!input) {
        perror("opening input");
        return EXIT_FAILURE;
    }
    output = fopen(argv[2], "wb");
    if (!output) {
        perror("opening output");
        fclose(input);
        return EXIT_FAILURE;
    }
    while ((bytes_read = fread(buffer, 1, sizeof buffer, input)) != 0) {
        if (fwrite(buffer, 1, bytes_read, output) != bytes_read) {
            perror("writing output");
            fclose(output);
            fclose(input);
            return EXIT_FAILURE;
        }
    }
    if (ferror(input) || fclose(input) != 0 || fclose(output) != 0) {
        fprintf(stderr, "copying input failed\n");
        return EXIT_FAILURE;
    }

    output = fopen(argv[2], "r+b");
    if (!output) {
        perror("reopening output");
        return EXIT_FAILURE;
    }
    if (fseek(output, 0, SEEK_END) != 0 || (input_size = ftell(output)) < 1
        || fseek(output, -1, SEEK_END) != 0 || (last_byte = fgetc(output)) == EOF
        || fseek(output, -1, SEEK_CUR) != 0 || fputc(last_byte ^ 1, output) == EOF
        || fclose(output) != 0) {
        fprintf(stderr, "modifying output failed\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
