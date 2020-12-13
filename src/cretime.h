#ifndef __CRETIME_H
#define __CRETIME_H

/**
 * 14 is the max length of a time that
 * can be input into speedrun.com, +1 for '\0'
 */
#define FTIME_BUF 15

/**
 * Exit status
 */
#define BAD_YT_DEBUG    2
#define BAD_FPS         3

/**
 * Help message
 */
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
Source code: <https://www.github.com/Mango0x45/Cretime>\n"

/**
 * License information
 */
#define VERSION "\
cretime v1.3 \n\
Licence Unlicense: <https://unlicense.org> \n\
This is free software: you are free to change and redistribute it. \n\
There is NO WARRANTY, to the extent permitted by law. \n\
\n\
Written by Thomas Voss; see \n\
<https://www.github.com/Mango0x45/Cretime>\n"


float str_to_float(const char *str_time);
void invalid_fps(void);
unsigned int check_fps(char *string);
float get_time(void);
char *format_time(const float time);

#endif