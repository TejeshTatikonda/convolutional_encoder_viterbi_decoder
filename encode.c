#include "fec.h"
#define CUR_FILE "ENCODE.C"

size_t encode(FEC _fec, unit8_t *msg, size_t sz_msg, unit8_t *enc_msg)
{
    size_t sz_enc = 0;
    if(sz_msg <= 0)
    {
        printf("\n[%s][%d]: invalid input size(%d) to encode\n", CUR_FILE, __LINE__, sz_msg);
    }

    for(int i = 0; i < sz_msg; i++)
    {
        
    }

    return sz_enc;
}