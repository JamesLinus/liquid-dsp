//
// firdes_kaiser_example.c
//
// This example demonstrates finite impulse response filter design
// using a Kaiser window.
// SEE ALSO: firdespm_example.c
//

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>

#include "liquid.h"

#define OUTPUT_FILENAME "firdes_kaiser_example.m"

// print usage/help message
void usage()
{
    printf("firdes_kaiser_example:\n");
    printf("  u/h   : print usage/help\n");
    printf("  f     : filter cutoff frequency,       0 < f < 1.0, default: 0.4\n");
    printf("  t     : filter transition bandwidth,   0 < t < 0.5, default: 0.2\n");
    printf("  s     : filter sidelobe level [dB],    0 < s,       default: 60\n");
    printf("  m     : fractional sample delay,    -0.5 < m < 0.5, default: 0\n");
}

int main(int argc, char*argv[]) {
    // options
    float fc=0.4f;          // filter cutoff frequency
    float ft=0.2f;          // filter transition
    float slsl=60.0f;       // sidelobe suppression level
    float mu=0.0f;          // fractional timing offset

    int dopt;
    while ((dopt = getopt(argc,argv,"uhf:t:s:m:")) != EOF) {
        switch (dopt) {
        case 'u':
        case 'h': usage();                      return 0;
        case 'f': fc = atof(optarg);            break;
        case 't': ft = atof(optarg);            break;
        case 's': slsl = atof(optarg);          break;
        case 'm': mu = atof(optarg);            break;
        default:
            fprintf(stderr,"error: %s, unknown option\n", argv[0]);
            usage();
            return 1;
        }
    }
    printf("filter design parameters\n");
    printf("    cutoff frequency            :   %12.8f\n", fc);
    printf("    transition bandwidth        :   %12.8f\n", ft);
    printf("    sidelobe level [dB]         :   %12.8f\n", slsl);
    printf("    fractional sample offset    :   %12.8f\n", mu);

    // derived values
    unsigned int h_len = estimate_req_filter_len(ft,slsl);
    printf("h_len : %u\n", h_len);

    // generate the filter
    unsigned int i;
    float h[h_len];
    fir_kaiser_window(h_len,fc,slsl,mu,h);

    // print coefficients
    for (i=0; i<h_len; i++)
        printf("h(%4u) = %16.12f;\n", i+1, h[i]);

    // output to file
    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"h_len=%u;\n",h_len);
    fprintf(fid,"fc=%12.4e;\n",fc);
    fprintf(fid,"slsl=%12.4e;\n",slsl);

    for (i=0; i<h_len; i++)
        fprintf(fid,"h(%4u) = %12.4e;\n", i+1, h[i]);

    fprintf(fid,"nfft=1024;\n");
    fprintf(fid,"H=20*log10(abs(fftshift(fft(h*fc,nfft))));\n");
    fprintf(fid,"f=[0:(nfft-1)]/nfft-0.5;\n");
    fprintf(fid,"figure; plot(f,H,'Color',[0 0.5 0.25],'LineWidth',2);\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"xlabel('normalized frequency');\n");
    fprintf(fid,"ylabel('PSD [dB]');\n");
    fprintf(fid,"title(['Filter design/Kaiser window f_c: %f, S_L: %f, h: %u']);\n",
            fc, -slsl, h_len);
    fprintf(fid,"axis([-0.5 0.5 -slsl-40 10]);\n");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}

