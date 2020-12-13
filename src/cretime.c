#include <ctype.h>
#include <getopt.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cretime.h"

/**
 * Convert a float string to a float
 */
float str_to_float(const char *str_time)
{
    unsigned int i = 0;
    unsigned char sig_fig = 1;
    float time = 0;

    while (str_time[i] != '.') {
        if (str_time[i] == '\0')
            return time;

        time = time * 10 + str_time[i++] - '0';
    }

    i++;
    while (str_time[i] != '\0')
        time = time + (str_time[i++] - '0') / pow(10, sig_fig++);

    return time;
}

/**
 * Print a warning and exit
 */
void invalid_fps(void)
{
    fputs("cretime: 'fps' option must be a whole number from 1 to 60\n", stderr);
    exit(BAD_FPS);
}

/**
 * Make sure the users fps is valid
 */
unsigned int check_fps(char *string)
{
    /* Make sure fps is numeric */
    for (int i = 0, len = strlen(string); i < len; i++)
        if (!isdigit(string[i]))
            invalid_fps();

    unsigned int const fps = (unsigned int) str_to_float(string);

    if (fps > 60 || fps < 1)
        invalid_fps();

    return fps;
}

/**
 * Take YT debug info JSON as input and return the cmt value
 */
float get_time(void)
{
    char *string = malloc(1);
    char *time;
    size_t i = 0;

    /* Read in the debug info as input */
    while (true) {
        string[i] = getchar();

        if (string[i] == '}')
            break;

        string = realloc(string, ++i + 1);
    }

    time = strtok(string, "\"");

    while (strcmp("cmt", time) != 0)
        time = strtok(NULL, "\"");

    time = strtok(NULL, "\"");
    time = strtok(NULL, "\"");

    free(string);

    return str_to_float(time);
}

/**
 * Format a float in seconds to human readable time
 */
char *format_time(const float time)
{
    unsigned int const hours = time / 3600;
    unsigned int const minutes = fmod(time, 3600) / 60;
    unsigned int const seconds = fmod(time, 60);
    unsigned int const milliseconds = fmod(trunc(round(time * 1000)), 1000);
    char *formatted_time = malloc(FTIME_BUF);

    if (hours == 0) {
        if (minutes == 0)
            snprintf(formatted_time, FTIME_BUF, "%d.%03d", seconds, milliseconds);
        else
            snprintf(formatted_time, FTIME_BUF, "%d:%02d.%03d", minutes, seconds, milliseconds);
    } else
        snprintf(formatted_time, FTIME_BUF, "%d:%02d:%02d.%03d", hours, minutes, seconds, milliseconds);

    return formatted_time;
}

/**
 * Main function
 */
int main(int argc, char **argv)
{
    bool bulk_retime = false;
    bool mod_message = false;
    int flag;
    int option_index = 0;
    unsigned int fps = 0;
    char const short_options[] = ":bf:mhv";
    struct option const long_options[] = {
        {"bulk-retime", no_argument,        NULL,   'b'},
        {"fps",         required_argument,  NULL,   'f'},
        {"mod-note",    no_argument,        NULL,   'm'},
        {"help",        no_argument,        NULL,   'h'},
        {"version",     no_argument,        NULL,   'v'},
        {0, 0, 0, 0}
    };

    /* Read in program flags */
    while (true) {
        flag = getopt_long(argc, argv, short_options, long_options, &option_index);

        if (flag == -1)
            break;

        switch (flag) {
        case 'b':
            bulk_retime = true;
            break;

        case 'f':
            fps = check_fps(optarg);
            break;

        case 'm':
            mod_message = true;
            break;

        case 'h':
            fputs(HELP, stderr);
            return EXIT_SUCCESS;

        case 'v':
            fputs(VERSION, stderr);
            return EXIT_SUCCESS;

        default:
            if (optopt == 'f') {
                fputs("cretime: option requires an argument -- 'f'\nTry 'cretime --help' for more information\n", stderr);
                return BAD_FPS;
            }

            fprintf(stderr, "cretime: invalid option -- '%c'\nTry 'cretime --help' for more information.\n", optopt);
            return EXIT_FAILURE;
        }
    }

LOOP:
    if (fps == 0) {
        fputs("Video Framerate: ", stderr);

        size_t i = 0;
        char *temp = malloc(1);

        // Get user input
        while (true) {
            temp[i] = getchar();

            if (temp[i] == '\n') {
                temp[i] = '\0';
                break;
            }

            temp = realloc(temp, ++i + 1);
        }

        fps = check_fps(temp);

        free(temp);
    }

    /* Prompt the user for the start and end of the run */
    fputs("Paste the debug info of the start of the run:\n", stderr);
    size_t start_time = trunc(get_time() * fps);
    fputs("Paste the debug info of the end of the run:\n", stderr);
    size_t end_time = trunc(get_time() * fps);

    system("clear");

    float duration = (end_time - start_time) / (float) fps;
    char *formatted_duration = format_time(duration);

    if (mod_message)
        printf("Mod Note: Retimed (Start: Frame %lu, End: Frame %lu, FPS: %d, Total Time: %s)\n", start_time, end_time, fps, formatted_duration);
    else
        printf("Final Time: %s\n", formatted_duration);

    free(formatted_duration);

    /* Loop when bulk_retime is true */
    if (bulk_retime) {
        fps = 0;
        while ((getchar()) != '\n');
        goto LOOP;
    }

    return EXIT_SUCCESS;
}
