#line 1 "/home/nunu/Arduino/scdtr_final_pedro/utils.cpp"
#include "utils.h"

void swap(uint8_t *a, uint8_t *b) {
    uint8_t aux;
    aux = *a;
    *a = *b;
    *b = aux;
}

void sort3(uint8_t *vec3) {
    if (vec3[0] > vec3[2]) {
        swap(&vec3[0], &vec3[2]);
    }

    if (vec3[0] > vec3[1]) {
        swap(&vec3[0], &vec3[1]);
    }

    if (vec3[1] > vec3[2]) {
        swap(&vec3[1], &vec3[2]);
    }
}

int index_of(uint8_t element, uint8_t *array, int len) {
    for (int i = 0; i < len; i++) {
        if (element == array[i]) {
            return i;
        }
    }
    return - 1;
}