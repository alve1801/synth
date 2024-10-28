# synth
Stack-based sound synthesis language

This is an interpreter for a stack-based single-byte-command esoteric language used for sound synthesis. Compile with `gcc main.c -lm -funsigned-char`, use with `./a.out <track_file> | aplay` (can also give it another dummy argument to enter debug mode, call that with `./a.out <track_file> - | less` or similar). Included is the nokia jingle (which I also use as my ringtone), program described in more detail on [my website](https://avethenoul.neocities.org/synth)

`bars.c` is a short program I use to visualise the output (for debug purposes). Pipe output of synth into that to see the waveform, I recommend piping that into `less` or similar for ease of use.
