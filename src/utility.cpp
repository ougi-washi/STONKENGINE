// Abstract City - Jed Fakhfekh - https://github.com/ougi-washi
#include "utility.h"

#include <stdio.h>
#include <stdlib.h>

c8* ac::read_file_text(const c8 *path)
{
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        return NULL; // Failed to open file
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    c8 *buffer = (c8 *)malloc(file_size + 1);
    if (buffer == NULL) {
        fclose(file);
        return NULL; // Failed to allocate memory
    }

    size_t read_size = fread(buffer, 1, file_size, file);
    if (read_size != file_size) {
        fclose(file);
        free(buffer);
        return NULL; // Failed to read file
    }

    buffer[file_size] = '\0'; // Null-terminate the buffer

    fclose(file);
    return buffer;
}
