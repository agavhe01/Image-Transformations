/*
 *     uarray2.h
 *     BY Anesu Gavhera 02/18/2021
 *     Comp40 HW2 - iii
 *
 *    Interface for urray2.c which declares implementation and member functions
 *    that uarray2 will have. uarray2 is a 2 dimensional array that is derrived
 *    from the Hanson 1-dimensional array.
 *
 *    Last Updated: 02.25.21
 */

#ifndef UARRAY2_H_INCLUDED
#define UARRAY2_H_INCLUDED

#include "uarray.h"


#define T UArray2_T
typedef struct T *T;

/*  UArray2_new
 *
 *  Purpose:
 *
 *    Defines a new 2-dimensional from the Uarray data type given appropriate
 *    size defintions.
 *
 *  Parameters:
 *
 *    width:     The width of the 2D grid or length of a single row
 *    height:    The height of the 2D grid or number of rows
 *    elem_size: The size of a single element that should be designated to
 *               store on the heap.
 *
 *  Returns: An empty UArray2_T of the specified width and height
 *
 *  Note: Memory is allocated on the heap which means it needs to be cleaned up
 *
 */
extern T UArray2_new(const int width, const int height, const int elem_size);

/*  UArray2_at
 *
 *  Purpose:
 *
 *    Returns the element at a specific position within the 2d array
 *
 *  Parameters:
 *
 *    uarray2: array from which element should be searched from
 *    col:     column index position
 *    row:     row index position
 *
 *  Returns:
 *
 *     Void pointer representing the locaation of the element requested
 *
 *  Errors:
 *
 *    Raises runtime error if i and j index positions are out of range
 *
 */
extern void *UArray2_at(T uarray2, int col, int row);

/* UArray2_width
 *
 *  Purpose:
 *
 *    Returns the width of the 2D array (number of columns)
 *
 *  Parameters:
 *
 *    uarray2: target array of enquiry
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
extern int UArray2_width(T uarray2);

/* UArray2_height
 *
 *  Purpose:
 *
 *    Returns the height of the 2D array (number of columns)
 *
 *  Parameters:
 *
 *    uarray2: target array of enquiry
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
extern int UArray2_height(T uarray2);

/*  UArray2_size
 *
 *  Purpose:
 *
 *    Returns the size an element of the 2D array
 *
 *  Parameters:
 *
 *    uarray2: target array of enquiry
 *
 *  Returns:
 *
 *     Integer representing the size of the given array
 *
 *  Errors:
 *
 *    Raises runtime error if array parameter is a NULL pointer
 *
 */
extern int UArray2_size(T uarray2);

/*  UArray2_map_col_major
 *
 *  Purpose:
 *
 *    Iterates through the array, choosing to vary the column indices more
 *    rapidly than the row indices and calling the apply function on each
 *    iteration.

 *  Parameters:
 *
 *    uarray2: The array the apply function can perform operations on
 *    apply:   The interface for the function the end user will pass in to
 *             perform operations on
 *    cl:      A pointer that allows for any type of data to be passed through
 *             to the apply function
 *
 *  Returns: nothing
 *
 */
extern void UArray2_map_col_major(T uarray2,
                void apply(int col, int row, T uarray2, void *elem, void *cl),
                void *cl);

/*  UArray2_map_row_major
 *
 *  Purpose:
 *
 *    Iterates through the array, choosing to vary the row indices more
 *    rapidly than the column indices and calling the apply function on each
 *    iteration.
 *
 *  Parameters:
 *
 *    uarray2: The array the apply function can perform operations on
 *    apply:   The interface for the function the end user will pass in to
 *             perform operations on
 *    cl:      A pointer that allows for any type of data to be passed through
 *             to the apply function
 *
 *  Returns: nothing
 *
 */
extern void UArray2_map_row_major(T uarray2,
                void apply(int col, int row, T uarray2, void *elem, void *cl),
                void *cl);

/*  UArray2_free
 *
 *  Purpose:
 *
 *    Deallocates the memory of the UArray2_T passed in
 *
 *  Parameters:
 *
 *    uarray2: the address of the array to be freed
 *
 *  Returns: nothing
 *
 */
extern void UArray2_free(T *uarray2);

#undef T
#endif
