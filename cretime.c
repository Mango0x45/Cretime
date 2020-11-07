#include <ctype.h>
#include <getopt.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// 14 is the max length of a time that can be input into speedrun.com, +1 for '\0'
#define FTIME_BUF 15
// Exit status
#define BAD_YT_DEBUG 2
#define BAD_FPS 3
// Long text I don't wanna put in my code
#define HELP "\
Usage: cretime [OPTIONS]... \n\
Retime a segment of a youtube video. \n\
Example: cretime -mf 30 \n\
 \n\
Functionality: \n\
  -b, --bulk-retime         bulk retime videos; \n\
                              the b and m flags are preserved \n\
  -f, --fps=FPS             set the FPS of the video being retimed \n\
  -m, --mod-note            output a mod retime note as opposed to the end duration \n\
 \n\
Miscellaneous: \n\
  -h, --help                display this help text and exit \n\
  -v, --version             display the version information and exit \n\
 \n\
Exit status: \n\
 0  if OK, \n\
 1  if invalid flag or missing option, \n\
 2  if invalid youtube debug info, \n\
 3  if invalid fps. \n\
 \n\
Report bugs to: thomasvoss@live.com \n\
Source code: <https://www.github.com/Mango0x45/retime>\n"
#define VERSION "\
cretime v1.3 \n\
Licence MIT: <https://mit-license.org/> \n\
This is free software: you are free to change and redistribute it. \n\
There is NO WARRANTY, to the extent permitted by law. \n\
\n\
Written by Thomas Voss; see \n\
<https://www.github.com/Mango0x45/retime>\n"


// Convert a float string to a float
float str_to_float(const char *str_time)
{
    unsigned int i = 0;
    unsigned char sig_fig = 1;
    float time = 0;

    // Loop until decimal
    while (str_time[i] != '.')
    {
        // If there is no decimal, return
        if (str_time[i] == '\0')
            return time;

        time = time * 10 + str_time[i++] - '0';
    }

    // Skip the decimal
    i++;

    // Loop until the end of the string
    while (str_time[i] != '\0')
        time = time + (str_time[i++] - '0') / pow(10, sig_fig++);

    return time;
}

// Print a warning and exit
void invalid_fps(void)
{
    fputs("cretime: 'fps' option must be a whole number from 1 to 60\n", stderr);
    exit(BAD_FPS);
}

// Make sure the users fps is valid
char check_fps(char *string)
{
    // Make sure fps is numeric
    for (int i = 0, len = strlen(string); i < len; i++)
        if (!isdigit(string[i]))
            invalid_fps();

    char fps = (char) str_to_float(string);

    // Make sure fps is within the correct range
    if (fps > 60 || fps < 1)
        invalid_fps();

    return fps;
}

// Take YT debug info JSON as input and return the cmt value
float get_time(void)
{
    char *string = malloc(1);
    char *time;
    size_t i = 0;

    // Read in the debug info as input
    while (1)
    {
        string[i] = getchar();

        if (string[i] == '}')
            break;

        string = realloc(string, ++i + 1);
    }

    time = strtok(string, "\"");

    // Keep splitting until the cmt value is found
    while (strcmp("cmt", time) != 0)
        time = strtok(NULL, "\"");

    // Gets the cmt value, find better way maybe
    time = strtok(NULL, "\"");
    time = strtok(NULL, "\"");

    free(string);

    return str_to_float(time);
}

// Format a float in seconds to human readable time
char *format_time(const float time)
{
    const unsigned short hours = time / 3600;
    const unsigned char minutes = fmod(time, 3600) / 60;
    const unsigned char seconds = fmod(time, 60);
    const unsigned short milliseconds = fmod(trunc(round(time * 1000)), 1000);
    char *formatted_time = malloc(FTIME_BUF);

    if (hours == 0)
    {
        if (minutes == 0)
            snprintf(formatted_time, FTIME_BUF, "%d.%03d", seconds, milliseconds);
        else
            snprintf(formatted_time, FTIME_BUF, "%d:%02d.%03d", minutes, seconds, milliseconds);
    }
    else
        snprintf(formatted_time, FTIME_BUF, "%d:%02d:%02d.%03d", hours, minutes, seconds, milliseconds);

    return formatted_time;
}

// Main function
int main(int argc, char **argv)
{
    _Bool bulk_retime = 0;
    _Bool mod_message = 0;
    int flag;
    int option_index = 0;
    unsigned char fps = 0;
    const char short_options[] = ":bf:mhv";
    const struct option long_options[] =
    {
        {"bulk-retime", no_argument,        NULL,   'b'},
        {"fps",         required_argument,  NULL,   'f'},
        {"mod-note",    no_argument,        NULL,   'm'},
        {"help",        no_argument,        NULL,   'h'},
        {"version",     no_argument,        NULL,   'v'},
        {0, 0, 0, 0}
    };

    // Read in program flags
    while (1)
    {
        flag = getopt_long(argc, argv, short_options, long_options, &option_index);

        if (flag == -1)
            break;

        switch (flag)
        {
            case 'b':
                bulk_retime = 1;
                break;

            case 'f':
                // Make sure the fps is numeric
                fps = check_fps(optarg);
                break;

            case 'm':
                mod_message = 1;
                break;

            case 'h':
                fputs(HELP, stderr);
                return EXIT_SUCCESS;

            case 'v':
                fputs(VERSION, stderr);
                return EXIT_SUCCESS;

            default:
                if (optopt == 'f')
                {
                    fputs("cretime: option requires an argument -- 'f'\nTry 'cretime --help' for more information\n", stderr);
                    return BAD_FPS;
                }

                fprintf(stderr, "cretime: invalid option -- '%c'\nTry 'cretime --help' for more information.\n", optopt);
                return EXIT_FAILURE;
        }
    }

LOOP:
    if (fps == 0)
    {
        fputs("Video Framerate: ", stderr);

        size_t i = 0;
        char *temp = malloc(1);

        // Get user input
        while (1)
        {
            temp[i] = getchar();

            if (temp[i] == '\n')
            {
                temp[i] = '\0';
                break;
            }

            temp = realloc(temp, ++i + 1);
        }

        fps = check_fps(temp);

        free(temp);
    }

    // Prompt the user for the start and end of the run
    fputs("Paste the debug info of the start of the run:\n", stderr);
    size_t start_time = trunc(get_time() * fps);
    fputs("Paste the debug info of the end of the run:\n", stderr);
    size_t end_time = trunc(get_time() * fps);

    system("clear");

    // Calculate and output the run duration
    float duration = (end_time - start_time) / (float) fps;
    char *formatted_duration = format_time(duration);

    if (mod_message)
        printf("Mod Note: Retimed (Start: Frame %lu, End: Frame %lu, FPS: %d, Total Time: %s)\n", start_time, end_time, fps, formatted_duration);
    else
        printf("Final Time: %s\n", formatted_duration);

    free(formatted_duration);

    // Loop when bulk_retime is true
    if (bulk_retime)
    {
        fps = 0;
        while ((getchar()) != '\n');
        goto LOOP;
    }

    return EXIT_SUCCESS;
}
