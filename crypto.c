#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aes.h"
#include "b64.h"

#define allocator(element, type) _allocator(element, sizeof(type))

int getch();
void* _allocator(size_t, size_t);
char* append(const char*, const char);
void encrypt_file(FILE*, FILE*);
void decrypt_file(FILE*, FILE*);
char* read_file(FILE*);


struct AES_ctx ctx;
uint8_t key[500];

int
main(int argc, char **argv) {

    if (argc != 4 || ( argc == 2 && strcmp(argv[1], "-h") == 0 ))  {
        puts("Usage:\n./crpyto <-enc/-dec> <input file path> <output file path>");
        return EXIT_FAILURE;
    }

    FILE *input_file = fopen(argv[2], "r");
    if (input_file == NULL) {
        printf("Input file: %s couldn't be found\n", argv[2]);
        return EXIT_FAILURE;
    }

    FILE *output_file = fopen(argv[3], "wb+");

    printf("Please input password: ");
    int ch;
    char password[500] = "";
    for (;;) {
        if (ch == 10) break;
        ch = getch();
        char *newString;
        newString = append(password, ch);
        strcpy(password, newString);
        free(newString);
        newString = NULL;
    }
    strcpy(key, password);

    AES_init_ctx(&ctx, key);
    if (strcmp(argv[1], "-enc") == 0) encrypt_file(input_file, output_file);
    else decrypt_file(input_file, output_file);

    fclose(input_file);
    fclose(output_file);

    return EXIT_SUCCESS;
}

int
getch() {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

void*
_allocator(size_t element, size_t typeSize) {
    void *ptr = NULL;
    if((ptr = calloc(element, typeSize)) == NULL) {
        puts("No memory! Exiting...");
        exit(1);
    }
    return ptr;
}


char*
append(const char *input, const char c) {
    char *newString, *ptr;

    newString = allocator((strlen(input) + 2), char);
    ptr = newString;
    for(; *input; input++) {*ptr = *input; ptr++;}
    *ptr = c;
    return newString;
}

void
encrypt_file(FILE* input, FILE* output) {
    char* buffer = read_file(input);

    AES_ECB_encrypt(&ctx, (uint8_t*)buffer);

    char* enc = b64_encode(buffer, strlen(buffer));
    fputs(enc, output);
    puts("\nSuccesfully encrypted. Checkout output file.");
}

void
decrypt_file(FILE* input, FILE* output) {
    char* buffer = read_file(input);

    char* dec = b64_decode(buffer, strlen(buffer));
    AES_ECB_decrypt(&ctx, (uint8_t*) dec);

    fputs(dec, output);
    puts("\nSuccesfully decrypted with given password. Checkout output file.");
}

char*
read_file(FILE* f) {
    char* buffer = 0;
    long length;
    fseek (f, 0, SEEK_END);
    length = ftell(f);
    fseek (f, 0, SEEK_SET);
    buffer = malloc(length);
    if (buffer) {
      fread (buffer, 1, length, f);
    }
    return buffer;
}
