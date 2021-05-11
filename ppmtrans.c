/*
 *     ppmtrans.c
 *     BY Anesu Gavhera 03/05/2021
 *
 *     This program opens images in ppm format, stores the pixel information
 *     within a 2d array A2Methods, performs a transformation of somesort on
 *     the original image and outputs the result. In addition, this program
 *     uses CPU timing to time each of the relevant transformations before
 *     writing the timing information on a seperate output file.
 *
 *     Last Updated: 03/08/2021
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mem.h>
#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"
#include "cputiming.h"

#define SET_METHODS(METHODS, MAP, WHAT) do {                    \
        methods = (METHODS);                                    \
        assert(methods != NULL);                                \
        map = methods->MAP;                                     \
        if (map == NULL) {                                      \
                fprintf(stderr, "%s does not support "          \
                                WHAT "mapping\n",               \
                                argv[0]);                       \
                exit(1);                                        \
        }                                                       \
} while (0)
/* usage
      Purpose: Outputs error message instructing user how to correctly use the
               program
   Parameters: Character array storing the previously executed instruction
      Returns: None
        Notes: Terminates program with exit code 1
*/
static void
usage(const char *progname)
{
        fprintf(stderr, "Usage: %s [[-rotate <angle>] "
                        "[-flip [vertical | horizontal]] "
                        "[-transpose]] "
                        "[-time [filename]] "
                        "[-{row,col,block}-major] [filename]\n",
                        progname);
        exit(1);
}
/* Used for declaring pointers to the relevant transformational functions */
typedef void transformation(int *col, int *row, int width, int height);

/* Struct used to store pointer to the relevant transformation function and
    final image */
typedef struct TypeAndImage {
        transformation *transformType;
        Pnm_ppm finalppm;
} *TypeAndImage;

void perform_transformation(int col, int row, A2Methods_UArray2 array2,
                            A2Methods_Object *ptr, void *cl);
void write_time(char *time_file_name, Pnm_ppm ppm, double time, int rotation);
void setup_rotation(Pnm_ppm origppm, Pnm_ppm finalppm,
                        TypeAndImage closure, int rotation);

void transform_0(int *col, int *row, int width, int height);
void transform_90(int *col, int *row, int width, int height);
void transform_180(int *col, int *row, int width, int height);
void transform_270(int *col, int *row, int width, int height);
void transpose(int *col, int *row, int width, int height);
void horizontal(int *col, int *row, int width, int height);
void vertical(int *col, int *row, int width, int height);


int main(int argc, char *argv[])
{
        char *time_file_name = NULL;
        int   rotation       = 0;
        int   i;
        FILE *filePointer = NULL;

        /* default to UArray2 methods */
        A2Methods_T methods = uarray2_methods_plain;
        assert(methods);

        /* default to best map */
        A2Methods_mapfun *map = methods->map_default;
        assert(map);

        /* Get arguments from commandline */
        for (i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-row-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_row_major,
                                    "row-major");
                } else if (strcmp(argv[i], "-col-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_col_major,
                                    "column-major");
                } else if (strcmp(argv[i], "-block-major") == 0) {
                        SET_METHODS(uarray2_methods_blocked, map_block_major,
                                    "block-major");
                } else if (strcmp(argv[i], "-rotate") == 0) {
                        if (!(i + 1 < argc)) {      /* no rotate value */
                                usage(argv[0]);
                        }
                        char *endptr;
                        rotation = strtol(argv[++i], &endptr, 10);
                        if (!(rotation == 0 || rotation == 90 ||
                            rotation == 180 || rotation == 270)) {
                                fprintf(stderr,
                                        "Rotation must be 0, 90 180 or 270\n");
                                usage(argv[0]);
                        }
                        if (!(*endptr == '\0')) {    /* Not a number */
                                usage(argv[0]);
                        }
                } else if (strcmp(argv[i], "-transpose") == 0) {
                        rotation = 3000;
                } else if (strcmp(argv[i], "-flip") == 0) {
                        char *flipType = argv[++i];
                        if (strcmp(flipType, "vertical") == 0) {
                                rotation = 1000;
                        } else if (strcmp(flipType, "horizontal") == 0){
                                rotation = 2000;
                        } else {
                               fprintf(stderr, "Invalid flip type\n");
                               usage(argv[0]);
                        }
                } else if (strcmp(argv[i], "-time") == 0) {
                        time_file_name = argv[++i];
                } else if (*argv[i] == '-') {
                        fprintf(stderr, "%s: unknown option '%s'\n", argv[0],
                                argv[i]);
                        usage(argv[0]);
                } else if (argc - i > 1) {
                        fprintf(stderr, "Too many arguments\n");
                        usage(argv[0]);
                } else if (filePointer == NULL) {
                        /* Open file */
                        filePointer = fopen(argv[i], "rb");
                        if (filePointer == NULL){
                                fprintf(stderr, "Unable to open the file\n");
                                exit(EXIT_FAILURE);
                        }
                } else {
                        break;
                }
        }

        /* Get file info from stdin if file not supplied */
        if (filePointer == NULL) {
                filePointer = stdin;
        }

        /* Read into ppm */
        Pnm_ppm origppm = Pnm_ppmread(filePointer, methods);

        /* Setup final ppm */
        Pnm_ppm finalppm;
        NEW(finalppm);
        finalppm->methods = methods;

        /* Define struct which includes a transformation type and final ppm */
        TypeAndImage closure;
        NEW(closure);

        /* Setup finalppm dimensions and transformation */
        setup_rotation(origppm, finalppm, closure, rotation);

        /* Perform method and calculate time */
        CPUTime_T timer = CPUTime_New();
        CPUTime_Start(timer);
        (*map)(origppm->pixels, perform_transformation, closure);
        double timeTaken = CPUTime_Stop(timer);
        write_time(time_file_name, finalppm, timeTaken, rotation);

        /* Write this image */
        Pnm_ppmwrite(stdout, finalppm);

        /* Free up all memory */
        CPUTime_Free(&timer);
        FREE(closure);
        Pnm_ppmfree(&origppm);
        Pnm_ppmfree(&finalppm);
        fclose(filePointer);

        return EXIT_SUCCESS;
}
/* setup_rotation
      Purpose: Set up the required dimensions of the ouput image before
               transformation occurs, and sets the relevant transform function
               to the function pointer within the closure struct.
   Parameters: Original untransformed image Pnm_ppm format,
               Final image Pnm_ppm format, closure storing the transformation
               type and final image, integer representing the type of
               transformation to be executed.
      Returns: None
*/
void setup_rotation(Pnm_ppm origppm, Pnm_ppm finalppm,
                        TypeAndImage closure, int rotation)
{
        /* Set appropriate width, height and transformation type */
        finalppm->width = finalppm->methods->width(origppm->pixels);
        finalppm->height = finalppm->methods->height(origppm->pixels);
        if (rotation == 0) {           /* rotate 0 */
                closure->transformType = transform_0;
        } else if (rotation == 90) {   /* rotate 90 */
                finalppm->width = finalppm->methods->height(origppm->pixels);
                finalppm->height = finalppm->methods->width(origppm->pixels);
                closure->transformType = transform_90;
        } else if (rotation == 180) {  /* rotate 180 */
                closure->transformType = transform_180;
        } else if (rotation == 270) {  /* rotate 270 */
                finalppm->width = finalppm->methods->height(origppm->pixels);
                finalppm->height = finalppm->methods->width(origppm->pixels);
                closure->transformType = transform_270;
        } else if (rotation == 1000) { /* vertical */
                closure->transformType = vertical;
        } else if (rotation == 2000) { /* horizontal */
                closure->transformType = horizontal;
        } else if (rotation == 3000) { /* transpose */
                finalppm->width = finalppm->methods->height(origppm->pixels);
                finalppm->height = finalppm->methods->width(origppm->pixels);
                closure->transformType = transpose;
        }

        /* Set total finalppm pixels and store ppm in closure */
        finalppm->denominator = origppm->denominator;
        finalppm->pixels = finalppm->methods->new(finalppm->width,
                                     finalppm->height,
                                     finalppm->methods->size(origppm->pixels));
        closure->finalppm = finalppm;
}

/* transform_0
      Purpose: Performs transformation math on the row/col cordinates
   Parameters: Column value pointer, Row value pointer, width of the array,
               height of the array
      Returns: None
*/
void transform_0(int *col, int *row, int width, int height)
{
        (void) col;
        (void) row;
        (void) width;
        (void) height;
}

/* transform_90
      Purpose: Performs transformation math on the row/col cordinates
   Parameters: Column value pointer, Row value pointer, width of the array,
               height of the array
      Returns: None
*/
void transform_90(int *col, int *row, int width, int height)
{
        (void) width;
        int newRow = *col;
        *col = height - *row - 1;
        *row = newRow;
}

/* transform_180
      Purpose: Performs transformation math on the row/col cordinates
   Parameters: Column value pointer, Row value pointer, width of the array,
               height of the array
      Returns: None
*/
void transform_180(int *col, int *row, int width, int height)
{
       *col = width - *col - 1;
        *row = height - *row - 1;
}

/* transform_270
      Purpose: Performs transformation math on the row/col cordinates
   Parameters: Column value pointer, Row value pointer, width of the array,
               height of the array
      Returns: None
*/
void transform_270(int *col, int *row, int width, int height)
{
        (void) height;
        int newRow = width - *col - 1;
        *col = *row;
        *row = newRow;
}

/* horizontal
      Purpose: Performs transformation math on the row/col cordinates
   Parameters: Column value pointer, Row value pointer, width of the array,
               height of the array
      Returns: None
*/
void horizontal(int *col, int *row, int width, int height){
        (void) height;
        (void) row;
        *col = width - *col - 1;
}

/* vertical
      Purpose: Performs transformation math on the row/col cordinates
   Parameters: Column value pointer, Row value pointer, width of the array,
               height of the array
      Returns: None
*/
void vertical(int *col, int *row, int width, int height){
        (void) width;
        int newRow = height - *row - 1;
        *col = *col;
        *row = newRow;
}

/* transpose
      Purpose: Performs transformation math on the row/col cordinates
   Parameters: Column value pointer, Row value pointer, width of the array,
               height of the array
      Returns: None
*/
void transpose(int *col, int *row, int width, int height) {
        (void) height;
        (void) width;
        int newRow = *col;
        *col = *row;
        *row = newRow;
}

/* write_time
      Purpose: Writes the CPU time taken to a file given in the parameter
   Parameters: Character array of the filename, Image file in ppm type,
               Total time taken in double format, integer representing the
               performed transformation
      Returns: None
        Notes: If character array is empty, function halts with a break command
*/
void write_time(char *time_file_name, Pnm_ppm ppm, double time, int rotation)
{
        if (time_file_name == NULL) { return; }

        /* Get statistics */
        int totalPixelsInImage = ppm->width * ppm->height;
        double averageTimePerPixel = time / totalPixelsInImage;

        /* Open file and write necessary data */
        FILE *fp = fopen(time_file_name, "a");

        fprintf(fp, "**************************************************\n");

        if (rotation == 1000) {
                fprintf(fp, "VERTICAL TRANSFORMATION\n");
        } else if (rotation == 2000) {
                fprintf(fp, "HORIZONTAL TRANSFORMATION\n");
        } else if (rotation == 3000) {
                fprintf(fp, "TRANSPOSE TRANSFORMATION\n");
        } else {
                fprintf(fp, "ROTATION: %d\n", rotation);
        }

        fprintf(fp, "Total Number of pixels: %d\n", totalPixelsInImage);
        fprintf(fp, "Total Time Taken:       %f\n", time);
        fprintf(fp, "Time Taken Per Pixel:   %f\n", averageTimePerPixel);

        fclose(fp);
}

/* perform_transformation
      Purpose: Apply function used to write the new row/col cordinates to the
               output image
   Parameters: Column value int, row value int, array storing image pixels,
               Pnm_rgb of the current pixel in the iteration, void pointer to
               to the struct storing final image & transformation type pointer
      Returns: None
*/
void perform_transformation(int col, int row, A2Methods_UArray2 array2,
                            A2Methods_Object *elem, void *cl)
{
        (void) elem;
        Pnm_rgb origPixel = elem;
        Pnm_rgb finalPixel;
        /* Get struct values */
        TypeAndImage closure = cl;
        Pnm_ppm finalppm = closure->finalppm;
        transformation *transform = closure->transformType;

        /* Perform transformation */
        transform(&col, &row, finalppm->methods->width(array2),
                               finalppm->methods->height(array2));

        finalPixel = finalppm->methods->at(finalppm->pixels, col, row);
        *finalPixel = *origPixel;
}
