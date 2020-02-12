// ------------------------------------------------------------
// File:        supersafeprogram.c
// Created by:  Secure D Center Team
// Date:        11 Feb 2020
// Description: Target file for fuzzing example
// Compiling:   afl-gcc supersafeprogram.c -o supersafeprogram
// Usage:       ./supersafeprogram input_file
// ------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *strrev(char *str)
{
    if (!str || ! *str)
        return str;

    int i = strlen(str) - 2, j = 0;

    char ch;
    while (i > j)
    {
        ch = str[i];
        str[i] = str[j];
        str[j] = ch;
        i--;
        j++;
    }
    return str;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s input_file\n", argv[0]);
        return 1;
    }

    int line_num = 1;
    char * line = NULL;
    size_t len = 0;
    ssize_t read_count;
    char buffer[20];
    int mode = 0;
    FILE *file_pointer;

    // Can't open file
    if ((file_pointer = fopen(argv[1], "r")) == NULL) {
        printf("Can't open %s\n", argv[1]);
        exit(1);
    }
    
     while ((read_count = getline(&line, &len, file_pointer)) != -1) {    
        // Line 1 must be "SAFEFILEHEADER"  
        if (line_num == 1) {
            if (read_count != 15 || strncmp("SAFEFILEHEADER\n", line, 15) != 0) {
                printf("Invalid file!\n");
                exit(1);
            }
            line_num++;
            continue;
        }
        // Line 2 must be "A" or "Z"
        else if (line_num == 2) {
            if (read_count != 2) {
                printf("Invalid mode!\n");
                exit(1);
            }
            switch(line[0]) {
                // Mode A prints out each line
                case 'A':
                    mode = 1;
		    printf("==Mode A==\n");
                    break;
                // Mode Z copies line to buffer variable and prints the variable
                case 'Z':
                    mode = 2;
		    printf("==Mode Z==\n");
                    break;
                // Others are invalid
                default:
                    printf("Invalid mode!\n");
                    exit(1);
            }
        }
        // Use dangerous function in mode 2
        else {
            if (mode == 1) {
                printf("%s", line);
            }
            else if (mode == 2) {
                strcpy(buffer, line);
                printf("%s", strrev(buffer));
            }
        }
        line_num++;
    }

    return 0;
}
