#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * decoding source Image to secret file
 * Info about output and intermediate data is
 * also stored
 */

#define MAGIC_STRING "#*"

typedef struct _DecodeInfo
{
    /* Source Image info */
    char *image_fname;
    FILE *fptr_image;
    uint image_capacity;

    /* Secret File Info */
    char *secret_fname;
    FILE *fptr_secret;
    char *extn_secret_file;
    char *data;
    long file_size;


} DecodeInfo;


/* Encoding function prototype */

/* Check operation type */
OperationType check_operation(char *argv[]);

/* Read and validate Encode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(char *argv[],DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_file(DecodeInfo *decInfo);

/* Get file size */
Status decode_file_size_by_lsb(char *ch,DecodeInfo *decInfo);

/* Decode Magic String */
Status check_magic_string(DecodeInfo *decInfo);

/* Decode secret file extenstion */
Status decode_secret_file_extn(uint file_size, DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_file_size(DecodeInfo *decInfo);

/* Decode secret file data*/
Status open_file_secret(char *argv[],DecodeInfo *decInfo);

/* Decode a byte into LSB of image data array */
Status decode_byte_to_lsb(char *image_buffer,DecodeInfo *decInfo, uint i);

/* Copy data from src to file after decoding */
Status copy_secret_data_file(DecodeInfo *decInfo);

/* closing the file*/
Status close_files_decode(DecodeInfo *decInfo);
#endif
