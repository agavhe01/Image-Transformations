#include <stdio.h>
#include <stdlib.h>
#include <uarray2b.h>
#include <assert.h>
#include <math.h>

int main () {
    const int width = 10;
    const int height = 10;
    const int size = sizeof(int);
    const int blocksize = sqrt(64 * 1024 / size);

    UArray2b_T uarray2b = UArray2b_new_64K_block(width, height, size);

    assert(UArray2b_width(uarray2b) == width);
    assert(UArray2b_height(uarray2b) == height);
    assert(UArray2b_size(uarray2b) == size);
    assert(UArray2b_blocksize(uarray2b) == blocksize);
    
    UArray2b_free(&uarray2b);
    
    return EXIT_SUCCESS;
}