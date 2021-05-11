/*
 *     a2plain.c
 *     BY Devon Minor and Anesu Gavhera 03/02/2021
 *     Comp40 HW3 - locality
 *
 *     This program creates a 2d array using the uarray2 interface and
 *     implementation.
 *
 *     Last Updated: 03/08/2021
 */
#include <string.h>
#include <a2plain.h>
#include "uarray2.h"

/************************************************/
/* Define a private version of each function in */
/* A2Methods_T that we implement.               */
/************************************************/

/*  new
 *
 *  Purpose: Defines a new 2-dimensional array from the UArray2 data type given
 *           appropriate size defintions
 *
 *  Parameters:
 *
 *    width:     The width of the 2D grid or length of a single row
 *    height:    The height of the 2D grid or number of rows
 *    size:      The size of a single element that should be designated to be
 *               stored on the heap.
 *
 *  Returns: An empty UArray2b_T of the specified width and height
 *
 *  Note: Memory is allocated on the heap which means it needs to be cleaned up
 *
 */
static A2Methods_UArray2 new(int width, int height, int size)
{
  return UArray2_new(width, height, size);
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
 *  blocksize:   Number of elements to be included in each block of the array
 *
 *
 *  Returns: An empty UArray2b_T of the specified width and height
 *
 *  Note: Memory is allocated on the heap which means it needs to be cleaned up
 *
 */
static A2Methods_UArray2 new_with_blocksize(int width, int height,
                                            int size, int blocksize)
{
  (void) blocksize;
  return UArray2_new(width, height, size);
}

typedef void UArray2_applyfun(int i, int j, UArray2_T array2b, void *elem,
                              void *cl);
/*  map_row_major
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
static void map_row_major(A2Methods_UArray2 uarray2,
                          A2Methods_applyfun apply,
                          void *cl)
{
  UArray2_map_row_major(uarray2, (UArray2_applyfun*)apply, cl);
}
/*  map_col_major
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
static void map_col_major(A2Methods_UArray2 uarray2,
                          A2Methods_applyfun apply,
                          void *cl)
{
  UArray2_map_col_major(uarray2, (UArray2_applyfun*)apply, cl);
}

struct small_closure {
  A2Methods_smallapplyfun *apply;
  void                    *cl;
};

static void apply_small(int i, int j, UArray2_T uarray2,
                        void *elem, void *vcl)
{
  struct small_closure *cl = vcl;
  (void)i;
  (void)j;
  (void)uarray2;
  cl->apply(elem, cl->cl);
}
/*  small_map_row_major
 *
 *  Purpose:
 *
 *    Iterates through the array, choosing to vary the row indices more
 *    rapidly than the column indices and calling the apply function on each
 *    iteration.
 *
 *  Parameters:
 *
 *    u2:      The array the apply function can perform operations on
 *    apply:   The interface for the function the end user will pass in to
 *             perform operations on
 *    cl:      A pointer that allows for any type of data to be passed through
 *             to the apply function
 *
 *  Returns: nothing
 *
 */
static void small_map_row_major(A2Methods_UArray2        a2,
                                A2Methods_smallapplyfun  apply,
                                void *cl)
{
  struct small_closure mycl = { apply, cl };
  UArray2_map_row_major(a2, apply_small, &mycl);
}
/*  small_map_col_major
 *
 *  Purpose:
 *
 *    Iterates through the array, choosing to vary the column indices more
 *    rapidly than the row indices and calling the apply function on each
 *    iteration.
 *
 *  Parameters:
 *
 *    a2:      The array the apply function can perform operations on
 *    apply:   The interface for the function the end user will pass in to
 *             perform operations on
 *    cl:      A pointer that allows for any type of data to be passed through
 *             to the apply function
 *
 *  Returns: nothing
 *
 */
static void small_map_col_major(A2Methods_UArray2        a2,
                                A2Methods_smallapplyfun  apply,
                                void *cl)
{
  struct small_closure mycl = { apply, cl };
  UArray2_map_col_major(a2, apply_small, &mycl);
}
/*  a2free
 *
 *  Purpose:
 *
 *    Deallocates the memory of the A2Methods_UArray2 passed in
 *
 *  Parameters:
 *
 *    array2p: the address of the array to be freed
 *
 *  Returns: nothing
 *
 *    Notes: closes with a runtime error if array parameter is NULL
 */
static void a2free(A2Methods_UArray2 * array2p)
{
  UArray2_free((UArray2_T *) array2p);
}
/* width
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
static int width(A2Methods_UArray2 array2)
{
  return UArray2_width(array2);
}

/*  height
 *
 *  Purpose:
 *
 *    Returns the size an element of the 2D array
 *
 *  Parameters:
 *
 *    uarray2b: target array of enquiry
 *
 *  Returns: Integer representing the height of the given array
 *
 *  Errors:
 *
 *    Raises runtime error if array parameter is a NULL pointer
 *
 */
static int height(A2Methods_UArray2 array2)
{
  return UArray2_height(array2);
}
/*  size
 *
 *  Purpose:
 *
 *    Returns the size an element of the 2D array
 *
 *  Parameters:
 *
 *    array2: target array of enquiry
 *
 *  Returns: Integer representing the size of the given array
 *
 *  Errors:
 *
 *    Raises runtime error if array parameter is a NULL pointer
 *
 */
static int size(A2Methods_UArray2 array2)
{
  return UArray2_size(array2);
}
/*  blocksize
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
static int blocksize(A2Methods_UArray2 array2)
{
  (void) array2;
  return 0;
}
/*  A2Methods_at
 *
 *  Purpose:
 *
 *    Returns the element at a specific position within the 2d array
 *
 *  Parameters:
 *
 *    array2: array from which element should be searched from
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
static A2Methods_Object *at(A2Methods_UArray2 array2, int col, int row)
{
  return UArray2_at(array2, col, row);
}

static struct A2Methods_T uarray2_methods_plain_struct = {
  new,
  new_with_blocksize,
  a2free,
  width,
  height,
  size,
  blocksize,
  at,
  map_row_major,
  map_col_major,
  NULL,           // map_block_major
  map_row_major, // map_default
  small_map_row_major,
  small_map_col_major,
  NULL,           // small_map_block_major
  small_map_row_major, // small_map_default
};

// finally the payoff: here is the exported pointer to the struct
A2Methods_T uarray2_methods_plain = &uarray2_methods_plain_struct;
