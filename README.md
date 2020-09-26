# Portable Color Console library
The Portable Color Console (portcolcon for short) library is a C library providing cross-platform functions to use color in console output.

When used on Windows this library will use Windows API Console functions if the console can be opened.
Otherwise it uses VT/ANSI terminal codes if the TERM environment variable is set.
If the NO_COLOR environment variable is set then no color will be used.