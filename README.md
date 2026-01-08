# Convolutional Encoder - Viterbi Decoder

A convolutional encoder and a viterbi decoder for all code rates based on puncturing pattern.

`fec_tests.c` is used to simulate different test cases.

Steps to initiate and run an FEC (Forward Error Correction):

1. Give the neccessary information for the `init_fec()` function to initate an fec like constraint length, trellis polynomials, 
   n_repeats for repeated codes(0 if not a repeated code), puncturing pattern, full tail-biting enabled/disabled.
   - Provide [1,1,1,1] as puncturing pattern if puncturing is not needed.
3. `print_fec()` prints the current configured FEC data.
4. `encode()` function encodes the input binary(bits) stream using provided FEC type.
5. `print_array()` function prints the data in an array. This is used to monitor the arrays at all the stages. (of course, one can use `gdb`).

## Building and Running

This project uses `cmake`. If not installed already, install using your OS's package manager. `sudo apt install cmake` on Ubuntu, for instance.

```
#### build ###
mkdir build    # only required for the first time
cd build
cmake ..
make

###  run (on linux) ###
./fec_test

###  run (on windows) ###
./fec_test.exe

```

