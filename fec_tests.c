#include "fec.h"

int main()
{
    bool punc[4] = {1,1,1,1};
    size_t sz_punc = 4;

    FEC _fec = init_fec(7, 0133, 0171, 0, punc, sz_punc, false);

    print_fec(_fec);

    return 0;
}