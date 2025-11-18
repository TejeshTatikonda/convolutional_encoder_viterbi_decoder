#include "fec.h"

// unsigned int trelis_structure[10][2] = {
//     [7][0] = 0133, // K = 7, MSB
//     [7][1] = 0171, // K = 7, LSB

//     [9][0] = 0133, // k = 9, MSB
//     [9][1] = 0171, // K = 9, LSB
// };

FEC init_fec(unsigned int cl, unsigned int msb, unsigned int lsb, unsigned int n_repeats, bool *punc_pat, size_t sz_punc_pat, bool enable_fulltail_biting)
{
    FEC a;
    a.cl = cl;
    a.msb = msb;
    a.lsb = lsb;

    if(n_repeats < 0)
    {
        printf("\ncan't create an fec for the specified n_repeats for a repeated code.\n");
        assert(false);
    }
    a.n_repeats = n_repeats;

    if(sz_punc_pat > 18 || sz_punc_pat < 0)
    {
        printf("\ncan't create an fec for the specified puncturing pattern.\n");
        assert(false);
    }
    a.sz_puncturing_pattern = sz_punc_pat;

    memcpy(a.puncturing_pattern, punc_pat, sz_punc_pat);

    a.enable_fulltail_biting = enable_fulltail_biting;

    return a;
}

void print_fec(FEC _fec)
{
    printf("\nGiven configuration :\nK = %d\ntrellis polynomials : %o %o\nfull tail biting ? %s\nn_repeats : %d",_fec.cl, _fec.msb, _fec.lsb, (_fec.enable_fulltail_biting ? "Yes" : "No"), _fec.n_repeats);
    printf("\npuncturing pattern : [ ");
    for(int i = 0; i < _fec.sz_puncturing_pattern; i++)
    {
        printf("%d ", _fec.puncturing_pattern[i]);
    }
    printf("]\n");
}
