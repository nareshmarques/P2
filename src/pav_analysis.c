#include <math.h>
#include "pav_analysis.h"

float compute_power(const float *x, unsigned int N) {
    float resultado = 0;
    float suma = 0;
    
    for(int i = 0; i < N; i++){
        suma = suma + pow(x[i], 2);
    }
    resultado = 10 * log10(suma / N);
    
    return resultado;
}

float compute_am(const float *x, unsigned int N) {
    float resultado = 0;
    float suma = 0;

    for(int i = 0; i < N; i++){
        suma = suma + fabs(x[i]);
    }
    resultado = suma / N;

    return resultado;
}

float compute_zcr(const float *x, unsigned int N, float fm) {
    float resultado = 0;
    float suma = 0;

    for(int i = 0; i < N; i++){
        if(x[i] * x[i-1] < 0){
            suma = suma + 1;
        }
    }
    resultado = (fm/2.0) / (N-1.0) * suma;

    return resultado;
}

