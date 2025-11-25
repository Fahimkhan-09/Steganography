#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc, char *argv[])
{
    EncodeInfo E1;
    DecodeInfo D1;

    if (argc < 3)
    {
        printf("Invalid Input\n");
        printf("For Encoding:\n./a.out -e beautiful.bmp secret.txt [stego.bmp]\n");
        printf("For Decoding:\n./a.out -d stego.bmp [decode.txt]\n");
        return 0;
    }

    OperationType ret = check_operation_type(argv);

    if (ret == e_encode)
    {
        printf("Selected Encoding\n");
        if (read_and_validate_encode_args(argv, &E1) == e_success)
        {
            printf("Info: Read and Validate encode args are success\n");
            if (do_encoding(&E1) == e_success)
                printf("Info: Encoding is success\n");
            else
                printf("Failed to encode\n");
        }
        else
            printf("ERROR: Read and Validate encode args failed\n");
    }
    else if (ret == e_decode)
    {
        printf("Selected Decoding\n");
        if (read_and_validate_decode_args(argv, &D1) == e_success)
        {
            printf("Info: Read and Validate decode args are success\n");
            if (open_files_decode(&D1) == e_success)
            {
                if (do_decoding(&D1) == e_success)
                    printf("Info: Decoding is success\n");
                else
                    printf("Error: Decoding failed\n");
            }
            else
                printf("Error: Unable to open stego image\n");
        }
        else
            printf("Error: Invalid decode arguments\n");
    }
    else
    {
        printf("Invalid Operation Type\n");
    }

    return 0;
}

OperationType check_operation_type(char *argv[])
{
    if (strcmp(argv[1], "-e") == 0)
        return e_encode;
    else if (strcmp(argv[1], "-d") == 0)
        return e_decode;
    else
        return e_unsupported;
}
