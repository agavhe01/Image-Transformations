/*
 *     uarray2.c
 *     BY Devon Minor and Anesu Gavhera 02/18/2021
 *
 *
 *     Implementation of the UArray2 interface. UArray2.c defines a number of
 *     operations that allow for a 2 dimensional array to be created, accessed,
 *     updated and deleted. The UArray2 is derived from the Hanson 1-D uarray
 *     and uses a single flat UArray with a length of width * height for the
 *     implementation.
 *
 *     Last Updated: 02.25.21
 */

#include <uarray2.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <mem.h>
#include <uarray.h>

#define T UArray2_T
struct T {
  int width;
  int height;
  UArray_T elems;
};

/*  UArray2_new
 *
 *  Purpose: Defines a new 2-dimensional from the UArray data type given
 *           appropriate size defintions
 *
 *  Parameters:
 *
 *    width:     The width of the 2D grid or length of a single row
 *    height:    The height of the 2D grid or number of rows
 *    elem_size: The size of a single element that should be designated to be
 *               stored on the heap.
 *
 *  Returns: An empty UArray2_T of the specified width and height
 *
 *  Note: Memory is allocated on the heap which means it needs to be cleaned up
 *
 */
T UArray2_new(const int width, const int height, const int elem_size) {
  assert(width > 0 && height > 0 && elem_size > 0);

  T uarray2;
  NEW(uarray2);
  uarray2->width = width;
  uarray2->height = height;
  UArray_T temp = UArray_new(width * height, elem_size);
  uarray2->elems = temp;

  return uarray2;
}

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
 *     Void pointer representing the location of the element requested
 *
 *  Errors:
 *
 *    Raises runtime error if col and row index positions are out of range
 *
 */
void *UArray2_at(T uarray2, int col, int row) {
  assert(uarray2);
  assert(col < uarray2->width && row < uarray2->height);
  return UArray_at(uarray2->elems, (row * uarray2->width) + col);
}

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
int UArray2_width(T uarray2) {
  assert(uarray2);
  return uarray2->width;
}

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
int UArray2_height(T uarray2) {
  assert(uarray2);
  return uarray2->height;
}

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
 *  Returns: Integer representing the size of the given array
 *
 *  Errors:
 *
 *    Raises runtime error if array parameter is a NULL pointer
 *
 */
int UArray2_size(T uarray2) {
  assert(uarray2);
  return UArray_size(uarray2->elems);
}

/*  UArray2_map_col_major
 *
 *  Purpose:
 *
 *    Iterates through the array, choosing to vary the column indices more
 *    rapidly than the row indices and calling the apply function on each
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
void UArray2_map_col_major(T uarray2,
                void apply(int col, int row, T uarray2, void *elem, void *cl),
                void *cl) {
  assert(uarray2);
  for (int col = 0; col < uarray2->width; col++) {
    for (int row = 0; row < uarray2->height; row++) {
      apply(col, row, uarray2, UArray2_at(uarray2, col, row), cl);
    }
  }
}

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
void UArray2_map_row_major(T uarray2,
                void apply(int col, int row, T uarray2, void *elem, void *cl),
                void *cl) {
  assert(uarray2);
  for (int row = 0; row < uarray2->height; row++) {
    for (int col = 0; col < uarray2->width; col++) {
      apply(col, row, uarray2, UArray2_at(uarray2, col, row), cl);
    }
  }
}

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
void UArray2_free(T *uarray2) {
  assert(*uarray2);
  UArray_free(&((*uarray2)->elems));
  free(*uarray2);
}

#undef T
