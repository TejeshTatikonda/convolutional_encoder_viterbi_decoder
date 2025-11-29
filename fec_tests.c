#include "fec.h"
#include "colors.h"
#include <time.h>

#define MAX_MSG_SIZE 800000

uint8_t msg[MAX_MSG_SIZE] = {0};
size_t sz_msg = MAX_MSG_SIZE;

uint8_t enc[MAX_MSG_SIZE*2] = {0};
size_t sz_enc = 0;

uint8_t dec[MAX_MSG_SIZE] = {0};
size_t sz_dec = 0;

int rand_range(int min, int max);

/// @brief compares two arrays and returns 1 if they are same and 0 if not. returns -1 if sizes are not equal.
/// @param input1 
/// @param sz_input1 
/// @param input2 
/// @param sz_input2 
int validate_data(uint8_t *input1, size_t sz_input1, uint8_t *input2, size_t sz_input2);

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

    // print_array(msg, sz_msg, "raw msg");

    sz_enc = encode(_fec, msg, sz_msg, enc);

    // print_array(enc, sz_enc, "encoded data");

    sz_dec = decode(_fec, enc, sz_enc, dec);

    // print_array(dec, sz_dec, "decoded msg : ");

    validate_data(msg, sz_msg, dec, sz_dec);

    

    // _fec = init_fec(7, 0133, 0171, 0, punc, sz_punc, true);
    _fec = init_fec(3, 05, 07, 0, punc, sz_punc, false);

    print_fec(_fec);

    // print_array(msg, sz_msg, "raw msg");

    sz_enc = encode(_fec, msg, sz_msg, enc);

    // print_array(enc, sz_enc, "encoded data");

    sz_dec = decode(_fec, enc, sz_enc, dec);

    // print_array(dec, sz_dec, "decoded msg : ");

    validate_data(msg, sz_msg, dec, sz_dec);


    return 0;
}

int rand_range(int min, int max) {
    return min + rand() % (max - min + 1);
}

int validate_data(uint8_t *input1, size_t sz_input1, uint8_t *input2, size_t sz_input2)
{
    if(sz_input1 != sz_input2)
    {
        printf( REDB "\ngiven sizes are not equal\n" reset);
        return -1;
    }
    for(size_t i = 0; i < sz_input1; i++)
    {
        if(input1[i] != input2[i])
        {
            printf( REDB "\ngiven two inputs are not equal\n" reset);
            return 0;
        }
    }
    printf( GRNB "\ngiven two inputs are matching..!\n" reset "\n");
    return 1;
}