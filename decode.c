#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "decode.h"
#include "common.h"

/* Validate arguments */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if (argv[2] == NULL)
    {
        printf("ERROR: No stego image file provided\n");
        return e_failure;
    }
    if (strstr(argv[2], ".bmp"))
        decInfo->stego_image_fname = argv[2];
    else
    {
        printf("ERROR: Please provide a .bmp file for decoding\n");
        return e_failure;
    }
    return e_success;
}

/* Open stego file */
Status open_files_decode(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "rb");
    if (decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        return e_failure;
    }
    return e_success;
}

/* Decode 1 byte */
Status decode_byte_from_lsb(char *data, char *image_buffer)
{
    char ch = 0;
    for (int i = 0; i < 8; i++)
        ch |= ((image_buffer[i] & 1) << i);
    *data = ch;
    return e_success;
}

/* Decode string data */
Status decode_data_from_image(char *data, int size, DecodeInfo *decInfo)
{
    for (int i = 0; i < size; i++)
    {
        fread(decInfo->image_data, 8, 1, decInfo->fptr_stego_image);
        decode_byte_from_lsb(&data[i], decInfo->image_data);
    }
    data[size] = '\0';
    return e_success;
}

/* Decode 32-bit int */
Status decode_size_from_lsb(int *size, DecodeInfo *decInfo)
{
    char buffer[32];
    *size = 0;
    fread(buffer, 32, 1, decInfo->fptr_stego_image);
    for (int i = 0; i < 32; i++)
        *size |= ((buffer[i] & 1) << i);
    return e_success;
}

/* Decode magic string */
Status decode_magic_string(DecodeInfo *decInfo)
{
    char magic[3];
    decode_data_from_image(magic, strlen(MAGIC_STRING), decInfo);
    if (strcmp(magic, MAGIC_STRING) == 0)
    {
        printf("Info : Magic string matched successfully\n");
        return e_success;
    }
    else
    {
        printf("Error : Magic string not found — not a valid stego image\n");
        return e_failure;
    }
}

/* Extension size */
Status decode_secret_file_extn_size(int *size, DecodeInfo *decInfo)
{
    decode_size_from_lsb(size, decInfo);
    return e_success;
}

/* Extension */
Status decode_secret_file_extn(char *file_extn, int size, DecodeInfo *decInfo)
{
    decode_data_from_image(file_extn, size, decInfo);
    return e_success;
}

/* File size */
Status decode_secret_file_size(int *file_size, DecodeInfo *decInfo)
{
    decode_size_from_lsb(file_size, decInfo);
    return e_success;
}

/* Secret file data */
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    FILE *fptr = fopen(decInfo->secret_fname, "wb");
    if (fptr == NULL)
    {
        perror("fopen");
        return e_failure;
    }

    char data;
    for (int i = 0; i < decInfo->size_secret_file; i++)
    {
        fread(decInfo->image_data, 8, 1, decInfo->fptr_stego_image);
        decode_byte_from_lsb(&data, decInfo->image_data);
        fputc(data, fptr);
    }

    fclose(fptr);
    return e_success;
}

/* Full decoding */
Status do_decoding(DecodeInfo *decInfo)
{
    printf("Decoding started...\n");

    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);

    if (decode_magic_string(decInfo) == e_failure)
        return e_failure;

    int extn_size;
    decode_secret_file_extn_size(&extn_size, decInfo);
    printf("Info : Secret file extension size decoded: %d\n", extn_size);

    decode_secret_file_extn(decInfo->extn_secret_file, extn_size, decInfo);
    printf("Info : Secret file extension decoded: %s\n", decInfo->extn_secret_file);

    int secret_size;
    decode_secret_file_size(&secret_size, decInfo);
    decInfo->size_secret_file = secret_size;
    printf("Info : Secret file size decoded: %d bytes\n", secret_size);

    sprintf(decInfo->secret_fname, "decoded%s", decInfo->extn_secret_file);

    decode_secret_file_data(decInfo);

    printf("Info : Secret file successfully decoded and saved as %s\n", decInfo->secret_fname);
    return e_success;
}
