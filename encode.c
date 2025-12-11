#include "fec.h"
#include "colors.h"
#include "colordlg.h"
#define CUR_FILE "ENCODE.C"

size_t encode(FEC _fec, uint8_t *msg, size_t sz_msg, uint8_t *enc_msg)
{
    size_t sz_enc = 0;
    if(sz_msg <= 0)
    {
        printf("\n[%s][%d]: invalid input size(%d) to encode\n", CUR_FILE, __LINE__, sz_msg);
        return 0;
    }
    if(sz_msg % 8 != 0)
    {
        printf( REDB "raw msg size is not exact bytes. continuing anyway..!" reset "\n");
    }

    unsigned int state_reg = 0;
    unsigned int full_state = pow(2,_fec.cl-1) - 1;

    uint8_t *enc_half_rate = calloc(sz_msg*2, sizeof(msg[0]));
    unsigned int sz_enc_half_rate = 0;

    if(_fec.enable_fulltail_biting)
    {
        uint8_t temp[20] = {0};
        for(size_t i = 0; i < (_fec.cl-1); i++)
        {
            temp[i]=msg[i];
            state_reg = (state_reg & full_state);
            state_reg = state_reg | ((msg[i])<< (_fec.cl-1));
            state_reg = state_reg>>1;
        }

        for(size_t i = (_fec.cl-1); i < sz_msg; i++)
        {
            state_reg = state_reg & full_state;
            state_reg = state_reg | (msg[i] << (_fec.cl-1));

            unsigned int ones_cnt = 0;
            uint8_t G0 = 0, G1 = 0;
            ones_cnt = __builtin_popcount(state_reg & _fec.msb);
            G1 = ones_cnt % 2;
            ones_cnt = __builtin_popcount(state_reg & _fec.lsb);
            G0 = ones_cnt % 2;

            enc_half_rate[sz_enc_half_rate++] = G1;
            enc_half_rate[sz_enc_half_rate++] = G0;

            state_reg = state_reg >> 1;
        }

        for(size_t i = 0; i < (_fec.cl-1); i++)
        {
            state_reg = state_reg & full_state;
            state_reg = state_reg | (temp[i] << (_fec.cl-1));

            unsigned int ones_cnt = 0;
            uint8_t G0 = 0, G1 = 0;
            ones_cnt = __builtin_popcount(state_reg & _fec.msb);
            G1 = ones_cnt % 2;
            ones_cnt = __builtin_popcount(state_reg & _fec.lsb);
            G0 = ones_cnt % 2;

            enc_half_rate[sz_enc_half_rate++] = G1;
            enc_half_rate[sz_enc_half_rate++] = G0;

            state_reg = state_reg >> 1;
        }
    }
    else
    {
        for(size_t i = 0; i < sz_msg; i++)
        {
            state_reg = state_reg & full_state;
            state_reg = state_reg | (msg[i] << (_fec.cl-1));

            unsigned int ones_cnt = 0;
            uint8_t G0 = 0, G1 = 0;
            ones_cnt = __builtin_popcount(state_reg & _fec.msb);
            G1 = ones_cnt % 2;
            ones_cnt = __builtin_popcount(state_reg & _fec.lsb);
            G0 = ones_cnt % 2;

            enc_half_rate[sz_enc_half_rate++] = G1;
            enc_half_rate[sz_enc_half_rate++] = G0;

            state_reg = state_reg >> 1;
            // printf("\nstate reg = %d", state_reg & full_state);
        }
    }

    // print_array(enc_half_rate, sz_enc_half_rate, "encoded msg");    

    uint8_t *repeated = NULL;
    size_t sz_repeated = 0;
    unsigned int n_repeats = (_fec.n_repeats > 0) ? _fec.n_repeats : 1;
    repeated = calloc(sz_enc_half_rate*n_repeats, sizeof(enc_half_rate[0]));
    if(_fec.n_repeats > 0)
    {
        for(int i = 0; i < sz_enc_half_rate; i = i+2)
        {
            for(int j = 0; j < _fec.n_repeats; j++)
            {
                repeated[sz_repeated++] = enc_half_rate[i];
                repeated[sz_repeated++] = enc_half_rate[i+1];
            }
        }
    }
    else if(_fec.n_repeats == 0)
    {
        memcpy(repeated, enc_half_rate, sizeof(enc_half_rate[0])*sz_enc_half_rate);
        sz_repeated = sz_enc_half_rate;
    }
    else
    {
        printf("\ninvalid n_repeats selected (%d)\n\n\n\n\n\n", n_repeats);
        assert(false);
    }

    // print_array(repeated, sz_repeated, "repeated enc msg");    

    uint8_t *punctured = NULL;
    punctured = calloc(sz_repeated, sizeof(repeated[0]));
    size_t sz_punctured = 0;
    if(_fec.sz_puncturing_pattern > 0)
    {
        for(int i = 0; i < sz_repeated; i++)
        {
            bool puncture = !(_fec.puncturing_pattern[(i % _fec.sz_puncturing_pattern)]);

            if(!puncture)
            {
                punctured[sz_punctured] = repeated[i];
                // enc_msg[sz_punctured] =  punctured[sz_punctured];
                sz_punctured++;
            }
        }        
    }
    else
    {
        printf("\ninvalid puncturing sequence size (%d)\n\n\n\n\n\n", sz_punctured);
    }

    sz_enc = sz_punctured;

    memcpy(enc_msg, punctured, sizeof(punctured[0])*sz_punctured);

    
    
    free(repeated);
    free(punctured);
    free(enc_half_rate);
    
    // print_array(enc_msg, sz_enc, "punctured msg");
    return sz_enc;
}
