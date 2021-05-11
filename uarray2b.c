/*
 *    uarray2b.c
 *    BY Anesu Gavhera 02/18/2021
 *
 *    Interface for urray2b.c which declares implementation and member
 *    functions
 *
 *
 *    Last Updated: 03.03.21
 */

#include <uarray2.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <mem.h>
#include <uarray.h>
#include <uarray2b.h>
#include <math.h>

#define T UArray2b_T
/* Struct for which the basis of uarray2b is made. elems is an array of
    arrays */
struct T {
    UArray2_T elems;
    int width;
    int height;
    int size;
    int blocksize;
    int blockWidth;
    int blockHeight;
};
/*  UArray2b_new
 *
 *  Purpose: Defines a new 2-dimensional array from the UArray data type given
 *           appropriate size defintions
 *
 *  Parameters:
 *
 *    width:     The width of the 2D grid or length of a single row
 *    height:    The height of the 2D grid or number of rows
 *    size:      The size of a single element that should be designated to be
 *               stored on the heap.
 *  blocksize:   Number of elements to be included in each block of the array
 *
 *  Returns: An empty UArray2b_T of the specified width and height
 *
 *  Note: Memory is allocated on the heap which means it needs to be cleaned up
 *
 */
T UArray2b_new (int width, int height, int size, int blocksize) {
    assert(width > 0 && height > 0 && blocksize > 1);

    /* Setup new UArray2b struct */
    T uarray2b;
    NEW(uarray2b);
    uarray2b->width = width;
    uarray2b->height = height;
    uarray2b->size = size;
    uarray2b->blockWidth = width / blocksize;
    uarray2b->blockHeight =  height / blocksize;
    uarray2b->blocksize = blocksize;

    /* Increment size if not enough blocks */
    if (width % blocksize > 0){
        uarray2b->blockWidth++;
    }
    if (height % blocksize > 0){
        uarray2b->blockHeight++;
    }

    /* Build the 2-D blocked UArray2 */
    uarray2b->elems = UArray2_new(uarray2b->blockWidth, uarray2b->blockHeight,
                        sizeof(UArray_T));

    /* Create 1-D UArray in each block */
    for (int i = 0; i < uarray2b->blockHeight; i++){
        for (int j = 0; j < uarray2b->blockWidth; j++){
            UArray_T *anArray = UArray2_at(uarray2b->elems, j, i);
            *anArray =  UArray_new(blocksize * blocksize, size);
        }
    }
    return uarray2b;
}
/*  UArray2b_new_64K_block
 *
 *  Purpose: Defines a new 2-dimensional array from the UArray data type given
 *           appropriate size defintions. Sets blocksize to be 64kB
 *
 *  Parameters:
 *
 *    width:     The width of the 2D grid or length of a single row
 *    height:    The height of the 2D grid or number of rows
 *    size:      The size of a single element that should be designated to be
 *               stored on the heap.
 *
 *
 *  Returns: An empty UArray2b_T of the specified width and height
 *
 *  Note: Memory is allocated on the heap which means it needs to be cleaned up
 *
 */
T UArray2b_new_64K_block(int width, int height, int size) {
    int blocksize = sqrt(64 * 1024 / size);
    /* Blocksize should be at least 1 */
    if (blocksize < 1) {
        blocksize = 1;
    }
    return UArray2b_new(width, height, size, blocksize);
}

/*  UArray2b_free
 *
 *  Purpose:
 *
 *    Deallocates the memory of the UArray2_T passed in
 *
 *  Parameters:
 *
 *    uarray2b: the address of the array to be freed
 *
 *  Returns: nothing
 *
 *    Notes: closes with a runtime error if array parameter is NULL
 */
void UArray2b_free (T *array2b) {
    assert(array2b != NULL);

    /* Free each of the inner UArrays in this 2-D UArray2 */
    for (int i = 0; i < (*array2b)->blockHeight; i++) {
        for (int j = 0; j < (*array2b)->blockWidth; j++) {
            UArray_T *anArray = UArray2_at((*array2b)->elems, j, i);
            UArray_free(anArray);
        }
    }

    UArray2_free(&((*array2b)->elems));
    FREE(*array2b);
}
/* UArray2b_width
 *
 *  Purpose:
 *
 *    Returns the width of the 2D array (number of columns)
 *
 *  Parameters:
 *
 *    uarray2b: target array of enquiry
 *
 *  Returns:
 *
 *     Integer representing the width of the given array
 *
 *  Errors:
 *
 *    Raises runtime error if array parameter is a NULL pointer
 *
 */

int UArray2b_width (T array2b) {
    assert(array2b != NULL);
    return array2b->width;
}

/*  UArray2b_size
 *
 *  Purpose:
 *
 *    Returns the size an element of the 2D array
 *
 *  Parameters:
 *
 *    uarray2b: target array of enquiry
 *
 *  Returns: Integer representing the size of the given array
 *
 *  Errors:
 *
 *    Raises runtime error if array parameter is a NULL pointer
 *
 */
int UArray2b_size (T array2b) {
    assert(array2b != NULL);
    return array2b->size;
}
/* UArray2b_height
 *
 *  Purpose:
 *
 *    Returns the height of the 2D array (number of columns)
 *
 *  Parameters:
 *
 *    uarray2b: target array of enquiry
 *
 *  Returns:
 *
 *     Integer representing the height of the given array
 *
 *  Errors:
 *
 *    Raises runtime error if array parameter is a NULL pointer
 *
 */
int UArray2b_height (T array2b) {
    assert(array2b != NULL);
    return array2b->height;
}
/*  UArray2_blocksize
 *
 *  Purpose:
 *
 *    Returns the blocksize of the 2d array
 *
 *  Parameters:
 *
 *    uarray2b: target array of enquiry
 *
 *  Returns: Integer representing the size of the given array
 *
 *  Errors:
 *
 *    Raises runtime error if array parameter is a NULL pointer
 *
 */
int UArray2b_blocksize(T array2b) {
    assert(array2b != NULL);
    return array2b->blocksize;
}
/*  UArray2b_at
 *
 *  Purpose:
 *
 *    Returns the element at a specific position within the 2d array
 *
 *  Parameters:
 *
 *    uarray2b : array from which element should be searched from
 *    col      : column index position
 *    row      : row index position
 *
 *  Returns:
 *
 *     Void pointer representing the location of the element requested
 *
 *  Errors:
 *
 *    Raises runtime error if col and row index positions are out of range
 *
 */
void *UArray2b_at(T array2b, int column, int row) {

    assert(array2b != NULL);
    /* Retrive the correct block */
    UArray_T *anArray = UArray2_at(array2b->elems,
                                    column / array2b->blocksize,
                                    row / array2b->blocksize);
    /* Get correct element index */
    int index = array2b->blocksize * (row % array2b->blocksize) +
                      (column % array2b->blocksize);
    return UArray_at(*anArray, index);
}

/* UArray2b_map
 *
 *  Purpose:
 *
 *    Iterates through the array, mapping from block to block rather than
 *    column-to-column or row-to-row
 *
 *  Parameters:
 *
 *    uarray2b: The array the apply function can perform operations on
 *    apply:   The interface for the function the end user will pass in to
 *             perform operations on
 *    cl:      A pointer that allows for any type of data to be passed through
 *             to the apply function
 *
 *  Returns: nothing
 *
 */
void UArray2b_map(T array2b,
                void apply(int col, int row, T array2b, void *elem, void *cl),
                void *cl) {
    assert(array2b != NULL);

    /* Iterate over each block and run the apply function */
    for (int row = 0; row < array2b->blockHeight; row++) {
        for (int col = 0; col < array2b->blockWidth; col++) {
            UArray_T *temp = UArray2_at(array2b->elems, col, row);
            for (int i = 0; i < UArray_length(*temp); i++) {
                int correctcol = (i % array2b->blocksize) +
                                (array2b->blocksize * col);
                int correctrow = (i / array2b->blocksize) +
                                (row * array2b->blocksize);
                if (correctcol < array2b->width &&
                    correctrow < array2b->height) {
                    apply(correctcol, correctrow, array2b,
                            UArray_at(*temp, i), cl);
                }
            }
        }
    }
}

#undef T
