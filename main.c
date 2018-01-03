/*
 * MIT License
 * 
 * Copyright (c) 2018 Fabvalaaah - fabvalaaah@laposte.net
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * DONATION:
 * As I share these sources for commercial use too, maybe you could consider
 * sending me a reward (even a tiny one) to my Ethereum wallet at the address
 * 0x1fEaa1E88203cc13ffE9BAe434385350bBf10868
 * If so, I would be forever grateful to you and motivated to keep up the good
 * work for sure :oD Thanks in advance !
 * 
 * FEEDBACK:
 * You like my work? It helps you? You plan to use/reuse/transform it? You have
 * suggestions or questions about it? Just want to say "hi"? Let me know your
 * feedbacks by mail to the address fabvalaaah@laposte.net
 * 
 * DISCLAIMER:
 * I am not responsible in any way of any consequence of the usage
 * of this piece of software. You are warned, use it at your own risks.
 */

/* 
 * File:   main.c
 * Author: Fabvalaaah
 *
 * 01/02/2018
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#define TIME_BUFFER_LENGTH 25
#define MAX_LINE_SIZE 4096 /* Maximum length of a line in the unlinked file */
#define LINE_COUNT 100 /* Number of lines to write in the unlinked file */

void printCurrentTime(time_t time) {
    char timbuf[TIME_BUFFER_LENGTH];
    struct tm* timeinfo;

    timeinfo = localtime(&time);
    memset(timbuf, '\0', TIME_BUFFER_LENGTH);
    strftime(timbuf, TIME_BUFFER_LENGTH, "%b %d %Y %H:%M:%S", timeinfo);
    fprintf(stdout, "%s", timbuf);
}

void printFileSpecs(char* filename, struct stat specs) {
    fprintf(stdout, "*****************\n");
    fprintf(stdout, "SPECS OF THE FILE \"%s\"\n", filename);
    fprintf(stdout, "*****************\n");
    fprintf(stdout, "ID of device containing file: %lu\n", specs.st_dev);
    fprintf(stdout, "inode number: %lu\n", specs.st_ino);
    fprintf(stdout, "protection: %u\n", specs.st_mode);
    fprintf(stdout, "number of hard links: %lu\n", specs.st_nlink);
    fprintf(stdout, "user ID of owner: %u\n", specs.st_uid);
    fprintf(stdout, "group ID of owner: %u\n", specs.st_gid);
    fprintf(stdout, "device ID (if special file): %lu\n", specs.st_rdev);
    fprintf(stdout, "total size, in bytes: %ld\n", specs.st_size);
    fprintf(stdout, "time of last access: ");
    printCurrentTime(specs.st_atime);
    fprintf(stdout, "\n");
    fprintf(stdout, "time of last modification: ");
    printCurrentTime(specs.st_mtime);
    fprintf(stdout, "\n");
    fprintf(stdout, "time of last status change: ");
    printCurrentTime(specs.st_ctime);
    fprintf(stdout, "\n");
    fprintf(stdout, "blocksize for filesystem I/O: %ld\n", specs.st_blksize);
    fprintf(stdout, "number of blocks allocated: %ld\n", specs.st_blocks);
    fprintf(stdout, "*****************\n");
}

int main(int argc, char** argv) {
    FILE* fp = NULL;
    char line[MAX_LINE_SIZE];
    int i;
    struct stat specs;

    /* Opening or creating the file */
    if (2 != argc || !(fp = fopen(argv[1], "a+"))) {
        fprintf(stderr, "Error: File could not be opened or created\n");
        return EXIT_FAILURE;
    }
    /* ------- */

    fprintf(stdout, "Info: File \"%s\" is created and opened\n", argv[1]);

    sleep(5); /* Pausing for demonstration purpose only */

    /* Unlinking the file from the file-system */
    /* 
     * The file seems to be deleted but its data blocks are still opened and
     * protected against overwriting as long as we keep its handler reference
     */
    if (-1 == unlink(argv[1])) {
        fprintf(stderr, "Error: File \"%s\" could not be unlinked\n", argv[1]);
        if (fp) {
            fclose(fp);
        }
        return EXIT_FAILURE;
    }
    /* ------- */

    fprintf(stdout, "Info: File \"%s\" is unlinked/masked and ready for duty\n",
            argv[1]);

    sleep(2); /* Pausing for demonstration purpose only */

    /* Printing specs of the unlinked file (by using its file descriptor) */
    if (-1 == fstat(fileno(fp), &specs)) {
        fprintf(stderr, "Error: File \"%s\" specs could not be retrieved\n",
                argv[1]);
        if (fp) {
            fclose(fp);
        }
        return EXIT_FAILURE;
    }
    printFileSpecs(argv[1], specs);
    /* ------- */

    sleep(5); /* Pausing for demonstration purpose only */

    /* Modifying the unlinked file by adding lines */
    for (i = 1; i <= LINE_COUNT; i++) {
        fprintf(fp, "Line n°%d\n", i);
    }
    /* ------- */

    /* Reading and printing the content of the unlinked file */
    rewind(fp);
    while (NULL != fgets(line, MAX_LINE_SIZE, fp)) {
        fprintf(stdout, "%s", line);
    }
    /* ------- */

    /* Printing specs of the unlinked file (by using its file descriptor) */
    /* 
     * Differences with the initial specs can be noticed despite the "unlinked"
     * status
     */
    if (-1 == fstat(fileno(fp), &specs)) {
        fprintf(stderr, "Error: File \"%s\" specs could not be retrieved\n",
                argv[1]);
        if (fp) {
            fclose(fp);
        }
        return EXIT_FAILURE;
    }
    printFileSpecs(argv[1], specs);
    /* ------- */

    sleep(5); /* Pausing for demonstration purpose only */

    /* Closing the unlinked file (and definitely losing its reference) */
    if (NULL != fp) {
        fclose(fp);
    }
    /* ------- */

    return EXIT_SUCCESS;
}