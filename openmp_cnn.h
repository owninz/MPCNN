#include "cnn.h"
#include "omp.h"

using namespace std;

class openmp_cnn
{
	global_config_t global_config;
	block_config_t block_config;
public:
	openmp_cnn(global_config_t g, block_config_t b){
		global_config = g;
		block_config = b;
	};
	
    void block_conv(const int * images, const int * filters, int * result, 
        int B, int C, int K, int W, int H, int RP, int RPP, int SP, int SPP){
        int b = 0, c = 0, k = 0, w = 0, h = 0, rp = 0, rpp = 0, sp = 0, spp = 0;

        int block_B = min (global_config.B - B, block_config.block_B);
        int block_C = min (global_config.C - C, block_config.block_C);
        int block_K = min (global_config.K - K, block_config.block_K);
        int block_W = min (global_config.W - W, block_config.block_W);
        int block_H = min (global_config.H - H, block_config.block_H);
        int block_Rp = min (global_config.R/global_config.sigW-RP, block_config.block_Rp);
        int block_Rpp = min (global_config.sigW - RPP, block_config.block_Rpp);
        int block_Sp = min (global_config.S/global_config.sigH - SP, block_config.block_Sp);
        int block_Spp = min (global_config.sigH - SPP, block_config.block_Spp);
        int tmp;
        #pragma omp parallel for
        for (b = 0;b < block_B; ++b)
        {
            for (c = 0;c < block_C; ++c)
            {
                for (k = 0; k < block_K; ++k)
                {
                    for (w = 0; w < block_W; ++w)
                    {
                        for (h = 0; h < block_H; ++h)
                        {
                            for (rp = 0 ; rp < block_Rp; ++rp)
                            {
                                for (rpp = 0 ; rpp < block_Rpp; ++rpp)
                                {
                                    for (sp = 0; sp < block_Sp; ++sp)
                                    {
                                        for (spp = 0; spp < block_Spp; ++spp)
                                        {
                                            //cerr << (k+K)*global_config.H*global_config.W*global_config.B+(h+H)*global_config.W*global_config.B+(w+W)*global_config.B+b+B << " ";
                                            tmp = images[(rpp+RPP+global_config.sigW*(rp+RP+w+W))*((global_config.H-1)*global_config.sigH+global_config.S)*global_config.C*global_config.B
                                                        + (SPP+spp+global_config.sigH*(sp+SP+h+H))*global_config.C*global_config.B+(c+C)*global_config.B+b+B]
                                                * filters[(k+K)*global_config.S*global_config.R*global_config.C+(global_config.sigW*(rp+RP)+rpp+RPP)*global_config.S*global_config.C+(global_config.sigH*(sp+SP)+spp+SPP)*global_config.C+c+C];
                                            #pragma omp critical
                                            result[(k+K)*global_config.H*global_config.W*global_config.B+(h+H)*global_config.W*global_config.B+(w+W)*global_config.B+b+B] += tmp;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

	void conv(const int * images, const int * filters, int * result){
		int b = 0, c = 0, k = 0, w = 0, h = 0, rp = 0, rpp = 0, sp = 0, spp = 0;
        // for (b = 0;b <= global_config.B - block_config.block_B; b += block_config.block_B)
        for (b = 0;b < global_config.B ; b += block_config.block_B)
        {
            // for (c = 0;c <= global_config.C - block_config.block_C; c += block_config.block_C)
            for (c = 0;c < global_config.C; c += block_config.block_C)
            {
                // for (k = 0; k <= global_config.K - block_config.block_K; k += block_config.block_K)
                for (k = 0; k < global_config.K ; k += block_config.block_K)
                {
                    // for (w = 0; w <= global_config.W - block_config.block_W; w += block_config.block_W)
                    for (w = 0; w < global_config.W; w += block_config.block_W)
                    {
                        // for (h = 0; h <= global_config.H - block_config.block_H; h += block_config.block_H)
                        for (h = 0; h < global_config.H; h += block_config.block_H)
                        {
                            // for (rp = 0; rp <= global_config.R / global_config.sigW - block_config.block_Rp; rp += block_config.block_Rp)
                            for (rp = 0; rp < global_config.R / global_config.sigW; rp += block_config.block_Rp)
                            {
                                // for (rpp = 0 ; rpp <= global_config.sigW - block_config.block_Rpp; rpp += block_config.block_Rpp)
                                for (rpp = 0 ; rpp < global_config.sigW; rpp += block_config.block_Rpp)
                                {
                                    // for (sp =0 ; sp <= global_config.S / global_config.sigH - block_config.block_Sp; sp += block_config.block_Sp)
                                    for (sp =0 ; sp < global_config.S / global_config.sigH; sp += block_config.block_Sp)
                                    {
                                        // for (spp = 0; spp <= global_config.sigH - block_config.block_Spp; spp += block_config.block_Spp)
                                        for (spp = 0; spp < global_config.sigH; spp += block_config.block_Spp)
                                        {
                                            block_conv(images, filters, result, b, c, k, w, h, rp, rpp, sp, spp);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
	}
};