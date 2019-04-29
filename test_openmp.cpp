#include "openmp_cnn.h"
#include <assert.h> 

using namespace std;

double read_timer( )
{
    static bool initialized = false;
    static struct timeval start;
    struct timeval end;
    if( !initialized )
    {
        gettimeofday( &start, NULL );
        initialized = true;
    }
    gettimeofday( &end, NULL );
    return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}

int main(int argc, char **argv)
{
    // Initialize input image
    srand(time(NULL));

    global_config_t test_global = {32, 16, 16, 8, 3, 6, 6, 2, 2};
    block_config_t test_block = {2, 2, 2, 2, 3, 3, 3, 1, 1};
    // block_config_t test_block = {100, 6, 6, 6, 3, 3, 3, 1, 1};
    // global_config_t test_global = {1, 4, 4, 1, 1, 1, 1, 1, 1};
    // block_config_t test_block = {1, 2, 2, 1, 1, 1, 1, 1, 1};  
    int input_w = test_global.sigW * (test_global.W - 1) + test_global.R;
    int input_h = test_global.sigH * (test_global.H - 1) + test_global.S;

    const int input_size = input_h * input_w * test_global.C * test_global.B;
    const int output_size = test_global.K * test_global.H * test_global.W * test_global.B;
    const int filter_size = test_global.K * test_global.R * test_global.S * test_global.C;

    int * filter = new int[filter_size];

    for (int i = 0; i < filter_size; i++)
    {
        filter[i] = 1;
    }

    int * test_input = new int[input_size];

    for (int i = 0; i < input_size; i++)
    {
        test_input[i] = rand() % 256;
    }

    int * output_naive = new int[output_size]();
    int * output_mpcnn = new int[output_size]();

    openmp_cnn mp_cnn(test_global, test_block);

    double sim1 = read_timer();
    mp_cnn.conv(test_input, filter, output_mpcnn);
    sim1 = read_timer() - sim1;
    double sim2 = read_timer();
    naive_cnn(test_input, filter, output_naive, test_global);
    sim2 = read_timer() - sim2;

    cout << "openmp: " << sim1 << "  naive: " << sim2 << endl;

    for (int i = 0; i < output_size; i++)
    {
        // cout << output_naive[i] << "  VS.  " << output_mpcnn[i] << endl;
        assert(output_naive[i] == output_mpcnn[i]);
    }
    delete[] test_input; 
    delete[] output_naive;
    delete[] output_mpcnn;
    return 0;
};
