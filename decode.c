#include "fec.h"
#define CUR_FILE "DECODER.C"

typedef struct node_info{
    unsigned int cur_state; // current state
    int error_at_cur_node; // latest and shortest error at current node will be in this node.
    unsigned int prev_node_for_error_at_cur_node; // the previous node which caused the error at this node.
    unsigned int input_to_prev_node; // input given to previous node to reach current node.
}NODE_INFO;


/// @brief allocates a dynamic memory to store metrics table and decoded data.
/// @param rows 
/// @param cols 
/// @return 
NODE_INFO *createNodeInfo2D(int rows, int cols);

/// @brief decodes the data using viterbi decoder.
/// @param _fec fec configuration
/// @param enc_msg encoded message
/// @param sz_enc_msg encoded message size
/// @param dec_msg decoded message
/// @return returns decoded message size
size_t perform_viterbi(FEC _fec, uint8_t *enc_msg, size_t sz_enc_msg, uint8_t *dec_msg);

/// @brief prints the branch metrics table (for fast debugging purpose)
/// @param metrics 
/// @param size 
void print_metrics(NODE_INFO *metrics, unsigned int rows, unsigned int cols);

/// @brief calculates the next state for the give input from current state using configured fec
/// @param _fec 
/// @param cur_state 
/// @param input 
/// @return 
unsigned int calculate_next_state(FEC _fec, unsigned int cur_state, unsigned int input);

/// @brief calculates the hamming distances for the given bits. (no of different bits in case of hard decoder)
/// @param _fec 
/// @param ref_msb 
/// @param ref_lsb 
/// @param msb 
/// @param lsb 
/// @param msb_punc 
/// @param lsb_punc 
/// @return 
unsigned int calculate_distance(FEC _fec, uint8_t ref_msb, uint8_t ref_lsb, uint8_t msb, uint8_t lsb, uint8_t msb_punc, uint8_t lsb_punc);

/// @brief depunctures the punctured data. (inserts 0 at the punctured positions)
/// @param _fec 
/// @param enc_msg 
/// @param sz_enc_msg 
/// @param depunctured 
/// @return 
size_t depuncture(FEC _fec, uint8_t *enc_msg, size_t sz_enc_msg, uint8_t *depunctured);

/// @brief decodes the encoded data.
/// @param _fec 
/// @param enc_msg 
/// @param sz_enc_msg 
/// @param dec_msg 
/// @return 
size_t decode(FEC _fec, uint8_t *enc_msg, size_t sz_enc_msg, uint8_t *dec_msg);

size_t decode(FEC _fec, uint8_t *enc_msg, size_t sz_enc_msg, uint8_t *dec_msg)
{
    // handling error cases for enc msg size.
    if(sz_enc_msg <= 0)
    {
        printf("\n[%s][%d]: invalid input size(%d) to decode\n", CUR_FILE, __LINE__, sz_enc_msg);
        return 0;
    }
    if(sz_enc_msg % _fec.ones_cnt_punc_pat != 0)
    {
        printf("\ninvalid input size to decoder function : %d expected %d\n", sz_enc_msg, ceil(sz_enc_msg / (float)_fec.ones_cnt_punc_pat)*_fec.ones_cnt_punc_pat);
        return 0;
    }

    uint8_t *depunctured = calloc(sz_enc_msg*2, sizeof(enc_msg[0]));
    size_t sz_depunctured = 0;
    sz_depunctured = depuncture(_fec, enc_msg, sz_enc_msg, depunctured);

    print_array(depunctured, sz_depunctured, "depunctured data : ");

    size_t sz_decoded = perform_viterbi(_fec, depunctured, sz_depunctured, dec_msg);

    free(depunctured);
    return sz_decoded;
}

size_t perform_viterbi(FEC _fec, uint8_t *enc_msg, size_t sz_enc_msg, uint8_t *dec_msg)
{
    size_t sz_dec = 0;

    // handling error cases for enc msg size.
    if(sz_enc_msg <= 0)
    {
        printf("\n[%s][%d]: invalid input size(%d) to decode\n", CUR_FILE, __LINE__, sz_enc_msg);
        return 0;
    }
    if(sz_enc_msg % _fec.ones_cnt_punc_pat != 0)
    {
        printf("\ninvalid input size to decoder function : %d expected %d\n", sz_enc_msg, ceil(sz_enc_msg / (float)_fec.ones_cnt_punc_pat)*_fec.ones_cnt_punc_pat);
        return 0;
    }

    unsigned int state_reg = 0;
    unsigned int no_of_states = pow(2,_fec.cl-1);
    unsigned int full_state = no_of_states - 1;

    unsigned int inf =  INT_MAX;
    unsigned int rows = no_of_states; // total no of states in the configured fec. (if cl=7, this will be 2^(7-6) = 2^6 = 64 states).
    unsigned int cols = sz_enc_msg/2 + 1; // needed one extra node column to represent the initial values for all the states.

    NODE_INFO *metrics = createNodeInfo2D(rows, cols);

    // initializing the metric table
    for(unsigned int j = 0; j < cols; j++)
    {
        for(unsigned int i = 0; i < rows; i++)
        {
            unsigned int metric_idx = i*cols + j;

            metrics[metric_idx].cur_state = 0;
            metrics[metric_idx].error_at_cur_node = inf;
            
            metrics[i*cols].error_at_cur_node = 0;
            metrics[metric_idx].prev_node_for_error_at_cur_node = 0;
        }
    }

    // print_metrics(metrics, rows, cols);

    uint8_t enc_msb = 0, enc_lsb = 0;
    unsigned int enc_msg_idx = 0;
    uint8_t msb_punc = 0, lsb_punc = 0;

    // printf("\ndecoder metrics table size : (%d,%d)\n", rows,cols);
    printf("\nsizeof branch metrics = %lld bytes\n", sizeof(metrics)*rows*cols);
    unsigned int punc_cnt = 0;
    for(unsigned int j = 0; j < cols-1; j++)
    {
        enc_msb = enc_msg[enc_msg_idx++];
        enc_lsb = enc_msg[enc_msg_idx++];

        msb_punc = _fec.puncturing_pattern[punc_cnt % _fec.sz_puncturing_pattern];
        punc_cnt++;
        lsb_punc = _fec.puncturing_pattern[punc_cnt % _fec.sz_puncturing_pattern];
        punc_cnt++;

        // printf("\npunc %d %d\n", msb_punc, lsb_punc);
     
        for(unsigned int i = 0; i < rows; i++)
        {
            unsigned int ones_cnt = 0;
            uint8_t G0 = 0, G1 = 0;

            unsigned int cur_state = 0;
            unsigned int next_state = 0;

            uint8_t input_bit = 0;

            int inst_error_at_next_node = 0;

            unsigned int metric_idx = i*cols + j;
            unsigned int next_state_idx = 0;

            
            // let input = 0
            input_bit = 0;
            cur_state = i; // (if cl=7, wrape arounds at 2^(cl-1) = 64 --> 0 to 63)
            metrics[metric_idx].cur_state = cur_state;
            cur_state = cur_state & full_state;
            // printf("\ncur state = %d, ", cur_state);
            
            ones_cnt = 0;
            ones_cnt = __builtin_popcount(cur_state & _fec.msb);
            G1 = ones_cnt % 2;
            ones_cnt = __builtin_popcount(cur_state & _fec.lsb);
            G0 = ones_cnt % 2;

            // error at next node due to this input at current node.
            inst_error_at_next_node = calculate_distance(_fec, G1, G0, enc_msb, enc_lsb, msb_punc, lsb_punc);

            next_state = calculate_next_state(_fec, cur_state, input_bit);
            // printf(" next state : %d", next_state);
            
            next_state_idx = next_state * cols + (j+1);
            // printf(" (cur state idx, next state idx) = (%d,%d)", metric_idx, next_state_idx);
            
            // printf(" inst error : %d", inst_error_at_next_node);
            // if the new error is less than the previous error, then only update the error here.
            if(inst_error_at_next_node < metrics[next_state_idx].error_at_cur_node)
            {
                metrics[next_state_idx].error_at_cur_node = metrics[metric_idx].error_at_cur_node + inst_error_at_next_node;
                // metrics[next_state_idx].prev_node_for_error_at_cur_node = metrics[metric_idx].cur_state;
                metrics[next_state_idx].prev_node_for_error_at_cur_node = metric_idx;
                metrics[next_state_idx].input_to_prev_node = input_bit;
            }
            else
            {
                // we should not update the metrics if it is not the lowest path possible.
                // nothing to do here.
            }
            // printf(" metrics[%d].error_at_cur_node : %d",next_state_idx, metrics[next_state_idx].error_at_cur_node);

            // let input = 1
            input_bit = 1;
            cur_state = i; // (if cl=7, wrape arounds at 2^(cl-1) = 64 --> 0 to 63)
            metrics[metric_idx].cur_state = cur_state;
            cur_state = cur_state & full_state;
            // printf("\ncur state = %d, ", cur_state);
            cur_state = cur_state | (no_of_states);

            ones_cnt = 0;
            ones_cnt = __builtin_popcount(cur_state & _fec.msb);
            G1 = ones_cnt % 2;
            ones_cnt = __builtin_popcount(cur_state & _fec.lsb);
            G0 = ones_cnt % 2;

            // error at next node due to this input at current node.
            inst_error_at_next_node = calculate_distance(_fec, G1, G0, enc_msb, enc_lsb, msb_punc, lsb_punc);

            next_state = calculate_next_state(_fec, cur_state, input_bit);
            // printf(" next state : %d", next_state);
            
            next_state_idx = next_state * cols + (j+1);

            // printf(" (cur state idx, next state idx) = (%d,%d)", metric_idx, next_state_idx);
            
            // printf(" inst error : %d", inst_error_at_next_node);
            // if the new error is less than the previous error, then only update the error here.
            if(inst_error_at_next_node < metrics[next_state_idx].error_at_cur_node)
            {
                metrics[next_state_idx].error_at_cur_node = metrics[metric_idx].error_at_cur_node + inst_error_at_next_node;
                // metrics[next_state_idx].prev_node_for_error_at_cur_node = metrics[metric_idx].cur_state;
                metrics[next_state_idx].prev_node_for_error_at_cur_node = metric_idx;
                metrics[next_state_idx].input_to_prev_node = input_bit;
            }
            else
            {
                // we should not update the metrics if it is not the lowest path possible.
                // nothing to do here.
            }
            // printf(" metrics[%d].error_at_cur_node : %d",next_state_idx, metrics[next_state_idx].error_at_cur_node);
        }

        // printf("\n");
    }

    // print_metrics(metrics, rows, cols);

    unsigned int min_distance = inf;
    unsigned int min_idx = 0;
    printf("\nlast stage metrics :");
    for(int i = 0; i < rows; i++)
    {
        unsigned int idx = i*cols + (cols-1);
        if(min_distance > metrics[idx].error_at_cur_node)
        {
            min_distance = metrics[idx].error_at_cur_node;
            min_idx = idx;
        }
    }

    printf("%d error(s) detected at idx %d ", min_distance, min_idx);

    sz_dec = 0;
    for(int i = cols-1; i > 0; i--)
    {
        dec_msg[i-1] = metrics[min_idx].input_to_prev_node;
        min_idx = metrics[min_idx].prev_node_for_error_at_cur_node;

        sz_dec++;
    }

    typeof(dec_msg) tmp[_fec.cl];

    if(_fec.enable_fulltail_biting)
    {
        memcpy(tmp, dec_msg + (sz_dec - (_fec.cl-1)), (_fec.cl-1));
        memmove(dec_msg + (_fec.cl - 1), dec_msg, (sz_dec - (_fec.cl-1)));
        memcpy(dec_msg, tmp, (_fec.cl-1));
    }

    free(metrics);

    return sz_dec;
}


NODE_INFO *createNodeInfo2D(int rows, int cols) {
    // return malloc(rows * cols * sizeof(NODE_INFO));
    return calloc(rows * cols, sizeof(NODE_INFO));
}

void print_metrics(NODE_INFO *metrics, unsigned int rows, unsigned int cols)
{
    printf("\nmetrics table : \n");
    for(int i = 0; i < rows; i++)
    {
        for(int j = 0; j < cols; j++)
        {
            unsigned int idx = i*cols + j;
            int value = metrics[idx].error_at_cur_node;
            printf("%d ", value);
        }
        printf("\n");
    }
}

unsigned int calculate_next_state(FEC _fec, unsigned int cur_state, unsigned int input)
{
    // unsigned int no_of_states = pow(2,_fec.cl-1);
    // unsigned int full_state = no_of_states - 1;
    unsigned int full_state = pow(2,_fec.cl-1) - 1;

    return ((cur_state & full_state) | (input << (_fec.cl-1))) >> 1;
}

unsigned int calculate_distance(FEC _fec, uint8_t ref_msb, uint8_t ref_lsb, uint8_t msb, uint8_t lsb, uint8_t msb_punc, uint8_t lsb_punc)
{
    unsigned int error = 0;
    // error = error +  ((ref_lsb == lsb) ? 0 : 1);
    // error = error +  ((ref_msb == msb) ? 0 : 1);
    // return error;

    if(lsb_punc == 1)
    {
        error = error +  ((ref_lsb == lsb) ? 0 : 1);
    }
    if(msb_punc == 1)
    {
        error = error +  ((ref_msb == msb) ? 0 : 1);
    }
    return error;
}

size_t depuncture(FEC _fec, uint8_t *enc_msg, size_t sz_enc_msg, uint8_t *depunctured)
{
    
    if(sz_enc_msg % _fec.ones_cnt_punc_pat != 0)
    {
        printf("\ninvalid input size to depuncture : %d expected %d\n", sz_enc_msg, ceil(sz_enc_msg / (float)_fec.ones_cnt_punc_pat)*_fec.ones_cnt_punc_pat);
        return 0;
    }
    
    size_t sz_depunctured = 0;
    // uint8_t *tmp = calloc(sz_enc_msg*2, sizeof(enc_msg[0]));

    // printf("\ndebug depuncture : \n");
    for(int i = 0; i < sz_enc_msg; )
    {
        for(int j = 0; j < _fec.sz_puncturing_pattern; j++)
        {
            bool puncture = _fec.puncturing_pattern[j];

            if(puncture == 0)
            {
                depunctured[sz_depunctured] = 0;
            }
            else{
                depunctured[sz_depunctured] = enc_msg[i];
                // printf("%d ", enc_msg[i]);
                i++;
            }

            // printf("%d ", depunctured[sz_depunctured]);

            sz_depunctured++;
        }
    }

    return sz_depunctured;
}