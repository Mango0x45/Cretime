#!/bin/sh

# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# ~ A tool for retiming parts of youtube videos, primarily for speedruns ~
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

# Invalid flag error message
invalid_flag () {
    echo "retime: invalid option -- '$OPTARG'
Try 'retime --help' for more information."
    exit 1
}

# Help message
get_help () {
    cat << EOF
Usage: retime [OPTIONS]...
Retime a segment of a youtube video.
Example: retime -cf 30

Functionality:
  -b, --bulk-retime         bulk retime videos;
                              the b and c flags are preserved
  -c, --copy                copy a mod message to the clipboard
  -f, --fps=FPS             set the FPS of the video being retimed

Miscellaneous:
  -h, --help                display this help text and exit
  -v, --version             display the version information and exit

Exit status:
 0  if OK,
 1  if invalid flag or missing option,
 2  if invalid youtube debug info,
 3  if invalid fps.

Report bugs to: thomasvoss@live.com
Source code: <https://www.github.com/Mango0x45/retime>
EOF
    exit 0
}

# Version information
version_info () {
    cat << EOF
retime v1.1
Licence MIT: <https://mit-license.org/>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.

Written by Thomas Voss; see
<https://www.github.com/Mango0x45/retime>
EOF
    exit 0
}

# Ensure the entered FPS is valid
check_fps () {
    if echo "$1" | grep -Eq "^[0-9]+$"; then
        FRAME_RATE="$1"
    elif [ -z "$1" ]; then
        no_option
    else
        exit 3
    fi
}

no_option () {
    echo "retime: option requires an argument -- '$OPTARG'
Try 'retime --help' for more information."
    exit 1
}

# Check flags
while getopts ":bcf:hv-:" FLAG; do
    # Parse long arguments
    case "$FLAG" in
        -)
            case "$OPTARG" in
                help)
                    get_help ;;
                bulk-retime)
                    BULK_RETIME=true ;;
                copy)
                    COPY=true ;;
                fps)
                    check_fps "$(eval echo \$$OPTIND)" ;;
                fps=*)
                    check_fps "$(echo "$OPTARG" | cut -d "=" -f 2)" ;;
                version)
                    version_info ;;
                *)
                    invalid_flag ;;
            esac ;;

        # Parse standard arguments
        b)
            BULK_RETIME=true ;;
        c)
            COPY=true ;;
        f)
            check_fps "$OPTARG" ;;
        h)
            get_help ;;
        v)
            version_info ;;
        :)
            no_option ;;
        ?)
            invalid_flag ;;
    esac
done

# Get the framerate
if [ -z "$FRAME_RATE" ]; then
    printf "Framerate: "
    read -r FRAME_RATE
fi

# Create a new text file to write the debug info to
[ -z "$EDITOR" ] && EDITOR="nano"
echo "Paste the youtube debug info for the start of the video segment,
and then paste the debug info for the end of the video segment underneath" > yt_debug_info
$EDITOR yt_debug_info

# Ensure the YT debug info is valid
if [ "$(grep -c "cmt" yt_debug_info)" -ne 2 ]; then
    rm yt_debug_info
    exit 2
fi

# Get the "start" and "end" of the run from the cmt value
START=$(awk -F\" '/cmt/ {if(count==0) {print $4} count++}' yt_debug_info)
END=$(awk -F\" '/cmt/ {if(count==1) {print $4} count++}' yt_debug_info)

# Get the actual start and end by multiplying the cmt by the framerate and flooring
START=$(echo "$START * $FRAME_RATE / 1" | bc)
END=$(echo "$END * $FRAME_RATE / 1" | bc)

# Convert frames into seconds
DURATION=$(echo "scale=3; ($END - $START) / $FRAME_RATE" | bc)

# Split the seconds into hours/minutes/etc.
H=$(echo "$DURATION / 3600" | bc)
M=$(echo "$DURATION % 3600 / 60" | bc)
# Avoids running bc twice
SMS=$(echo "$DURATION % 60" | bc)
S=$(echo "$SMS" | cut -f1 -d ".")
MS=$(echo "$SMS" | cut -f2 -d ".")

# Seconds can sometimes be null, this is a quick fix
[ -z "$S" ] && S=0

# Make the duration human readable
if [ "$H" = 0 ]; then
    if [ "$M" = 0 ]; then
        DURATION=$(printf "%d.%s" "$S" "$MS")
    else
        DURATION=$(printf "%d:%02d.%s" "$M" "$S" "$MS")
    fi
else
    DURATION=$(printf "%d:%02d:%02d.%s" "$H" "$M" "$S" "$MS")
fi

# Echo the final time and copy a mod note to clipboard
echo "Final Time: $DURATION"
if [ "$COPY" = true ]; then
    MOD_NOTE=$(printf "Mod Note: Retimed (Start: Frame %s, End: Frame %s, FPS: %s, Total Time: %s)" "$START" "$END" "$FRAME_RATE" "$DURATION")
    if [ "$(uname)" = "Darwin" ]; then
        echo "$MOD_NOTE" | pbcopy
    else
        echo "$MOD_NOTE" | xclip -selection clipboard || echo "$OUTPUT" | xsel -ib
    fi
fi

# Remove the debug info file
rm yt_debug_info

# Loop if in bulk retime mode
if [ "$BULK_RETIME" = true ]; then
    ([ "$COPY" = true ] && retime -bc) || retime -b
fi

exit 0
