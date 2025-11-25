#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Get image size */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    fseek(fptr_image, 18, SEEK_SET);
    fread(&width, sizeof(int), 1, fptr_image);
    fread(&height, sizeof(int), 1, fptr_image);
    return width * height * 3;
}

/* Open files */
Status open_files(EncodeInfo *encInfo)
{
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        return e_failure;
    }

    encInfo->fptr_secret = fopen(encInfo->secret_fname, "rb");
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        return e_failure;
    }

    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        return e_failure;
    }
    return e_success;
}

/* Read and validate encode args */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if (strstr(argv[2], ".bmp"))
        encInfo->src_image_fname = argv[2];
    else
        return e_failure;

    if (strstr(argv[3], ".txt"))
        encInfo->secret_fname = argv[3];
    else
        return e_failure;

    if (argv[4] != NULL && strstr(argv[4], ".bmp"))
        encInfo->stego_image_fname = argv[4];
    else
        encInfo->stego_image_fname = "default.bmp";

    return e_success;
}

/* Get file size */
uint get_file_size(FILE *fptr)
{
    fseek(fptr, 0, SEEK_END);
    return ftell(fptr);
}

/* Check image capacity */
Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    if (encInfo->image_capacity > 16 + 32 + 32 + 32 + encInfo->size_secret_file * 8)
        return e_success;
    else
        return e_failure;
}

/* Copy BMP header */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char header[54];
    rewind(fptr_src_image);
    fread(header, 54, 1, fptr_src_image);
    fwrite(header, 54, 1, fptr_dest_image);
    return e_success;
}

/* Encode 1 byte into 8 bytes’ LSBs */
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for (int i = 0; i < 8; i++)
        image_buffer[i] = (image_buffer[i] & 0xFE) | ((data >> i) & 1);
    return e_success;
}

/* Encode data into image */
Status encode_data_to_image(char *data, int size, EncodeInfo *encInfo)
{
    for (int i = 0; i < size; i++)
    {
        fread(encInfo->image_data, 8, 1, encInfo->fptr_src_image);
        encode_byte_to_lsb(data[i], encInfo->image_data);
        fwrite(encInfo->image_data, 8, 1, encInfo->fptr_stego_image);
    }
    return e_success;
}

/* Encode integer (32-bit) */
Status encode_size_to_lsb(int size, EncodeInfo *encInfo)
{
    char buffer[32];
    fread(buffer, 32, 1, encInfo->fptr_src_image);
    for (int i = 0; i < 32; i++)
        buffer[i] = (buffer[i] & 0xFE) | ((size >> i) & 1);
    fwrite(buffer, 32, 1, encInfo->fptr_stego_image);
    return e_success;
}

/* Encode magic string */
Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{
    encode_data_to_image(magic_string, strlen(magic_string), encInfo);
    return e_success;
}

/* Encode secret extension size */
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    encode_size_to_lsb(size, encInfo);
    return e_success;
}

/* Encode secret extension */
Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
    encode_data_to_image(file_extn, strlen(file_extn), encInfo);
    return e_success;
}

/* Encode secret file size */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    encode_size_to_lsb(file_size, encInfo);
    return e_success;
}

/* Encode secret file data */
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    long sz = encInfo->size_secret_file;
    char *data = malloc(sz);
    if (!data) return e_failure;

    rewind(encInfo->fptr_secret);
    fread(data, 1, sz, encInfo->fptr_secret);
    encode_data_to_image(data, sz, encInfo);
    free(data);
    return e_success;
}

/* Copy remaining image data */
Status copy_remaining_img_data(EncodeInfo *encInfo)
{
    int size = encInfo->image_capacity + 54 - ftell(encInfo->fptr_src_image);
    char *remaining_data = malloc(size);
    fread(remaining_data, size, 1, encInfo->fptr_src_image);
    fwrite(remaining_data, size, 1, encInfo->fptr_stego_image);
    free(remaining_data);
    return e_success;
}

/* Perform full encoding */
Status do_encoding(EncodeInfo *encInfo)
{
    if (open_files(encInfo) == e_success)
    {
        printf("Info: Files opened successfully\n");
        if (check_capacity(encInfo) == e_success)
        {
            printf("Secret data can fit in given image file\n");
            copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image);
            printf("Copied BMP header successfully\n");
            encode_magic_string(MAGIC_STRING, encInfo);
            printf("Encoded magic string successfully\n");

            strcpy(encInfo->extn_secret_file, strstr(encInfo->secret_fname, "."));
            encode_secret_file_extn_size(strlen(encInfo->extn_secret_file), encInfo);
            printf("Encoded secret file extn size successfully\n");
            encode_secret_file_extn(encInfo->extn_secret_file, encInfo);
            printf("Encoded secret file extn successfully\n");
            encode_secret_file_size(encInfo->size_secret_file, encInfo);
            printf("Encoded secret file size successfully\n");
            encode_secret_file_data(encInfo);
            printf("Encoded secret file data successfully\n");
            copy_remaining_img_data(encInfo);
            printf("Copied remaining data successfully\n");
            return e_success;
        }
        else
            printf("Cannot fit the secret data in given image file\n");
    }
    else
        printf("Failed to open files\n");

    return e_failure;
}
