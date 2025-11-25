#ifndef DECODE_H
#define DECODE_H

#include "types.h"
#include <stdio.h>

/* 
 * Structure to store information required for
 * decoding the secret file from stego image
 */
typedef struct _DecodeInfo
{
    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

    /* Secret File Info */
    char secret_fname[50];
    FILE *fptr_secret;
    char extn_secret_file[10];
    long size_secret_file;

    /* Temporary buffers */
    char image_data[8];
    char secret_data;
} DecodeInfo;

/* Function Prototypes */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);
Status do_decoding(DecodeInfo *decInfo);
Status open_files_decode(DecodeInfo *decInfo);
Status decode_byte_from_lsb(char *data, char *image_buffer);
Status decode_data_from_image(char *data, int size, DecodeInfo *decInfo);
Status decode_size_from_lsb(int *size, DecodeInfo *decInfo);
Status decode_magic_string(DecodeInfo *decInfo);
Status decode_secret_file_extn_size(int *size, DecodeInfo *decInfo);
Status decode_secret_file_extn(char *file_extn, int size, DecodeInfo *decInfo);
Status decode_secret_file_size(int *file_size, DecodeInfo *decInfo);
Status decode_secret_file_data(DecodeInfo *decInfo);

#endif
