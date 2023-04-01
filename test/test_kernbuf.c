/* Test program for kern_buf 
 * Puts out different size buffers from 1 to string length 
 * And then see if it comes back the same
 * James Skon, 2023  
 */
#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#define BUFFERSIZE 1000

size_t read_kernbuf(char * buffer, size_t len) 
{

        FILE * fp = fopen("/dev/kernbuf", "r");

        if (!fp) {
                printf("/dev/kernbuf failed to open for read\n");
                exit(1);
        }

        size_t i = 0;
        while (fread( & buffer[i], sizeof(char), 1, fp)) {
                i++;
        };

        fclose(fp);

        return i;

}

void write_kernbuf(char * buffer, size_t len) 
{

        FILE * fp = fopen("/dev/kernbuf", "w");

        if (!fp) {
                printf("/dev/kernbuf failed to open for write\n");
                exit(1);
        }

        fwrite(buffer, len, 1, fp);

        fclose(fp);

}

void cprint(char * buf, size_t len) 
{
        for (int i = 0; i < len; i++) {
                printf("%c", buf[i]);
        }
}

int main() 
{
        char in_buffer[BUFFERSIZE];
        char out_buffer[] = "1234567890abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz";

        int size = 1;
        size_t len = strlen(out_buffer);

        while (size <= len) {
                printf("Output size: %d ", size);
                size_t sent = 0;

                // put out size bytes at a time
                for (int i = 0; i < len / size; i++) {
                        write_kernbuf( & out_buffer[i * size], size);
                        sent += size;
                }
                // put out the rest
                write_kernbuf( & out_buffer[sent], len - sent);

                size++;
                size_t in_len = read_kernbuf(in_buffer, BUFFERSIZE);
                size_t wrong = strncmp(in_buffer, out_buffer, len);
                if (wrong) {
                        printf("Mismatch at position %zu\nSent:", wrong);
                        cprint(out_buffer, len);
                        printf("\nReceived:");
                        cprint(out_buffer, in_len);
                        printf("\n");
                } else {
                        printf(" Match\n");
                }

        }
        printf("Done!");

}