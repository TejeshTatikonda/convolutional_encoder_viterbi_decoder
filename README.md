# convolutional_encoder_viterbi_decoder
A convolutional encoder and a viterbi decoder for all code rates based on puncturing pattern.

fec_tests.c is used to simulate different test cases.
steps to initiate and run an fec:
1. give the neccessary information for the init_fec() function to initate an fec like constarint length, trellis polynomials, 
   n_repeats for repeated codes(0 if not a repeated code), puncturing pattern([1,1,1,1] if puncturing is not needed), fulltail biting enable/disable
2. print_fec() prints the current configured fec data.
3. encode() function encodes the input binary(bits) stream using provided fec type.
4. print_array() function prints the data in an array. used to monitor the arrays at all the stages.(ofcourse one can use gdb).

how to build and run?
-> Go to build directory if it is there. if not, create one as follows in the terminal
-> cd build
-> cmake ..
-> ./fec_test.exe