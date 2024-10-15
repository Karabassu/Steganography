#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

/* Function to validate input files for decoding */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
 {
    if (!strcmp(strstr(argv[2], "."), ".bmp")) 
    {
        printf("argv[2] is having .bmp extension\n");
        decInfo->src_image_fname = argv[2];

        if (strstr(argv[3], ".")) 
        {
            char *tok = strtok(argv[3], ".");
            decInfo->secret_fname = tok;
        } 
        else 
        {
            decInfo->secret_fname = "output.txt";
        }
    } 
    else
     {
        printf("argv[2] is not having .bmp extension\n");
        return e_failure;
    }
    return e_success;
}

Status do_decoding(DecodeInfo *decInfo) 
{
    if (decode_open_files(decInfo) == e_failure)
     {
        printf("ERROR: Unable to open files\n");
        return e_failure;
    }

    printf("Decoding Magic String Signature\n");
    printf("enter magic string: ");
    char user_magicstring[10];
    scanf("%s",user_magicstring);
    if (decode_magic_string(user_magicstring, decInfo) == e_failure) 
    {
        printf("ERROR: Decoding magic string failed\n");
        return e_failure;
    }

    printf("Decoding secret file extension size\n");
    decInfo->secret_extn_size = decode_secret_file_extn_size(decInfo);
    if (decInfo->secret_extn_size < 2 && decInfo->secret_extn_size > 4)
     {
        printf("ERROR: Invalid secret file extension size\n");
        return e_failure;
    }
    printf("Decoding secret file extension\n");
    if (decode_secret_file_extn(decInfo) == e_failure) 
    {
        printf("ERROR: Decoding secret file extension failed\n");
        return e_failure;
    }
    decInfo->fptr_secret = fopen(decInfo->secret_fname, "w");
    if (!decInfo->fptr_secret)
     {
        perror("fopen");
        return e_failure;
    }

    printf("Decoding secret file size\n");
    decInfo->size_secret_file = decode_secret_file_size(decInfo);
    if (decInfo->size_secret_file < 1) 
    {
        printf("ERROR: Invalid secret file size\n");
        return e_failure;
    }

    printf("Decoding secret file data\n");
    if (decode_secret_file_data(decInfo) != e_success)
     {
        printf("ERROR: Decoding secret file data failed\n");
        return e_failure;
    }

    return e_success;
}
Status decode_open_files(DecodeInfo *decInfo) 
{
    decInfo->fptr_src_image = fopen(decInfo->src_image_fname, "rb"); // Use "rb" for binary read
    if (!decInfo->fptr_src_image)
     {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->src_image_fname);
        return e_failure;
    }
    return e_success;
}

Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo) 
{
    fseek(decInfo->fptr_src_image, 54, SEEK_SET);
    char buf[8];
    char ch[2]; // Buffer for the magic string (2 chars + null terminator)
    for (int i = 0; i < 2; i++) 
    {
        if (fread(buf, 8, 1, decInfo->fptr_src_image) != 1) 
        {
            printf("ERROR: Unable to read magic string bytes\n");
            return e_failure;
        }
        ch[i] = decode_byte_to_lsb(buf);
    }
    ch[2] = '\0';
    if (strcmp(magic_string, ch) == 0)
     {
        printf("user entered magic string and Decoded Magic String both are Matched\n");
        return e_success;
    }
    return e_failure;
}

char decode_byte_to_lsb(char *image_buffer)
 {
    char ch = 0;
    for (int i = 0; i < 8; i++) {
        ch |= (image_buffer[i] & 1) << i;
    }
    return ch;
}

int decode_secret_file_extn_size(DecodeInfo *decInfo) 
{
    char buf[32];
    if (fread(buf, 32, 1, decInfo->fptr_src_image) != 1)
    {
        printf("ERROR: Unable to read secret file extension size\n");
        return -1;
    }
    int extn_size = decode_size_to_lsb(buf);
    
    // Print the secret file extension size
    //printf("Secret file extension size: %d\n", extn_size);
    printf("Decoded Successfully\n");
    
    return extn_size;
}


int decode_size_to_lsb(char *image_buffer) 
{
    int ch = 0;
    for (int i = 0; i < 32; i++) 
    {
        ch |= (image_buffer[i] & 1) << i;
    }
    return ch;
}
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
   char buf[8];
   char ch[decInfo->secret_extn_size];
   for(int i=0; i<decInfo->secret_extn_size; i++){
   fread(buf,8,1,decInfo->fptr_src_image);
   ch[i]=decode_byte_to_lsb(buf);
   }
   ch[decInfo->secret_extn_size]='\0';
   //printf("%s\n",ch);
   if((!strcmp(ch,".txt")) || (!strcmp(ch,".c")) || (!strcmp(ch,".sh")))
   {
   decInfo->secret_fname =strcat(decInfo->secret_fname,ch);
   printf("Decoded Succcessfully\n");
    return e_success;
   }
   return e_failure;
}
int decode_secret_file_size(DecodeInfo *decInfo)
 {
    char buf[32];
    if (fread(buf, 32, 1, decInfo->fptr_src_image) != 1)
     {
        printf("ERROR: Unable to read secret file size\n");
        return -1;
    }
    printf("Decoded successfully\n");
    return decode_size_to_lsb(buf);
}

Status decode_secret_file_data(DecodeInfo *decInfo) 
{
     char ch[decInfo->size_secret_file];
  char buf[8];

    for (int i = 0; i < decInfo->size_secret_file; i++) 
    {
        if (fread(buf, 8, 1, decInfo->fptr_src_image) != 1) 
        {
            printf("ERROR: Unable to read secret file data bytes\n");
            return e_failure;
        }
        ch[i] = decode_byte_to_lsb(buf);
    }
    ch[decInfo->size_secret_file] = '\0';
    fwrite(ch, decInfo->size_secret_file, 1, decInfo->fptr_secret);
   // printf("%s\n",ch);
   printf("Decoded Successfully\n");
    return e_success;
}