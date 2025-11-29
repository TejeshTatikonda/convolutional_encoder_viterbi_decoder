#ifndef _FEC_H_
#define _FEC_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <limits.h>


typedef struct trellis{
    unsigned int cl; // constraint length (K value. eg: K = 7)

    unsigned int msb; // msb characteristic for the trellis (eg: 0133)
    unsigned int lsb; // lsb characteristic for the trellis (eg: 0171)

    unsigned int n_repeats; // to produce repeated codes (eg: 1/3 rate could be produced by 2 2/3 rate encoders placed next to next);

    bool puncturing_pattern[18]; // supports upto 9/16 code rate.

    size_t sz_puncturing_pattern; // puncturing pattern size.

    bool enable_fulltail_biting; // if true, last (cl-1) bits will be loaded into the encoder state register 
                                // and then starts encoding.

}FEC;

/// @brief initializes a customized trellis object for the convolutional encoder (K, trellis polynomials, puncturing pattern, fultailbiting).
/// @param cl 
/// @param msb 
/// @param lsb 
/// @param n_repeats
/// @param punc_pat 
/// @param sz_punc_pat 
/// @param enable_fulltail_biting 
/// @return returns a trellis object which contains the specified configuration of the fec. (K, trellis polynomials, puncturing pattern, fultailbiting).
FEC init_fec(unsigned int cl, unsigned int msb, unsigned int lsb, unsigned int n_repeats, bool *punc_pat, size_t sz_punc_pat, bool enable_fulltail_biting);

/// @brief prints the configurations of the configured fec.
/// @param _fec 
void print_fec(FEC _fec);

/// @brief prints the input array with 32 elements per line.
/// @param  
/// @param  
/// @param  
void print_array(uint8_t *, size_t , const char *);

/// @brief encodes the binary data as specified by the FEC configuration.
/// @param  
/// @param  
/// @param  
/// @param  
/// @return 
size_t encode(FEC , uint8_t *, size_t , uint8_t *);

/// @brief decodes the data using viterbi algorithm as pwe specified FEC configuration.
/// @param  
/// @param  
/// @param  
/// @param  
/// @return 
size_t decode(FEC , uint8_t *, size_t , uint8_t *);

#endif
