#include "fec.h"
#include <time.h>

#define MAX_MSG_SIZE 10*8

uint8_t msg[MAX_MSG_SIZE] = {0};
size_t sz_msg = MAX_MSG_SIZE;

uint8_t enc[MAX_MSG_SIZE*2] = {0};
size_t sz_enc = 0;

int rand_range(int min, int max);

int main()
{
    srand(time(NULL)); // Seed with current time

    bool punc[4] = {1,1,1,1};
    size_t sz_punc = 4;

    FEC _fec = init_fec(7, 0133, 0171, 0, punc, sz_punc, false);

    print_fec(_fec);

    for(size_t i = 0; i < sz_msg; i++)
    {
        int x = rand_range(0, 1);   // random int from 10 to 20
        msg[i] = (bool)x;
        // msg[i] = (bool)1;
    }

    print_array(msg, sz_msg, "raw msg");

    sz_enc = encode(_fec, msg, sz_msg, enc);

    print_array(enc, sz_enc, "encoded data");

    _fec = init_fec(7, 0133, 0171, 0, punc, sz_punc, true);

    print_fec(_fec);

    print_array(msg, sz_msg, "raw msg");

    sz_enc = encode(_fec, msg, sz_msg, enc);

    print_array(enc, sz_enc, "encoded data");

    return 0;
}

int rand_range(int min, int max) {
    return min + rand() % (max - min + 1);
}
