#include "fec.h"

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

    a.ones_cnt_punc_pat = 0;
    for(int i = 0; i < a.sz_puncturing_pattern; i++)
    {
        a.ones_cnt_punc_pat += a.puncturing_pattern[i];
    }

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
    printf("] -> one's count : %d\n", _fec.ones_cnt_punc_pat);
    printf("pattern size : %d\n", _fec.sz_puncturing_pattern);
}

void print_array(uint8_t *a, size_t b, const char *c)
{
    printf("\n%s : (%d)\n", c, b);
    for(int i = 0; i < b; i++){
        if(i%32 == 0)
        printf("\n");
        printf("%d ", a[i]);
    }
    printf("\n");
}