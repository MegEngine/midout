# Midout

Move Intermediate Dynamic-dispatch OUT; the name is obtained by applying
the infamous [middle-out](http://www.piedpiper.com) algorithm on the word
`middle-out`.

## Usage

1. Include `midout.h` in the source code and mark removable regions with
   `MIDOUT_BEGIN` and `MIDOUT_END`.
2. Compile the whole project with macro `MIDOUT_PROFILING` defined.
3. Execute the program on possible inputs to gather trace information on used
   blocks. The trace would be written to `midout_trace.<pid>` by default, and
   this output name can be changed via the environment variable `MIDOUT_OUTPUT`.
4. Generate midout header file by `./gen_header.py`.
5. Recompile the project and ensure that the header generated in the previous
   step is included before `midout.h`. Remember to enable LTO.
6. Now all the blocks marked in step 1 that are not executed in step 3 have been
   removed from the final executable; if any of them is indeed used at runtime,
   a trap would be triggered.

See `run.sh` for a concrete example.
