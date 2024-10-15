#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include<string.h>

int main(int argc,char *argv[])
{
    EncodeInfo encInfo;
     DecodeInfo decInfo;
    uint img_size;
    if(argc < 4)
    {
        printf("Info : Error : Insufficient arguments\n");
        printf("Usage:-\nFor encoding -> ./a.out -e <.bmp file> <secret file>\nFor Decoding -> ./a.out -d <.bmp file> <output file>\n");
       return e_failure; 
    }
    else
    {
        if(check_operation_type(argv)  == e_encode)
        {
            printf("INFO: ##ENCODING OPERATION IS SELECTED ##\n");
           if(read_and_validate_encode_args(argv, &encInfo) == e_success)
           {
            printf("INFO : read and validation for encoding is success\n");
            if(do_encoding(&encInfo) == e_success)
            {
                printf("Encoded Successfully\n");
            }
            else
            {
                printf("Encoding is not completed\n");
                return 0;
            }
           } 
           else{
            return 0;
           }
        }
        else if(check_operation_type(argv)  == e_decode)
        {
           
            printf("INFO: ##DECODING OPERATION IS SELECTED ##\n");
            if(read_and_validate_decode_args(argv,&decInfo) == e_success)
            {
            printf("INFO : read and validation for decoding is success\n");
            if(do_decoding(&decInfo) == e_success){
                printf(" ## Decoding completed Successfully ## \n");
            }
            else
            {
                printf("decoding is not completed\n");
                return 0;
            }
           } 
        }
        else{
            printf("User option is unsupported\n");
            return 0;
        }
    }
}