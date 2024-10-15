#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include<string.h>

/* Function Definitions */
OperationType check_operation_type(char *argv[])
{
    if(!strcmp(argv[1],"-e"))
    {
    return e_encode;
    }
    else if(!strcmp(argv[1],"-d"))
    {

    return e_decode;
    }
    return e_unsupported;           
}

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if(strstr(argv[2],".")!=NULL)
    {
    if(!strcmp(strstr(argv[2], "."), ".bmp"))
    {
        printf("INFO: beautiful.bmp file validation is Done\n");
        encInfo->src_image_fname = argv[2];
        if(strstr(argv[3],".")!=NULL)
        {
        if(!strcmp(strstr(argv[3], "."), ".txt") || 
           !strcmp(strstr(argv[3], "."), ".c") || 
           !strcmp(strstr(argv[3], "."), ".sh"))
        {
            printf("INFO: secret.txt file extension Validation is Done\n");
            encInfo->secret_fname = argv[3];
            if(argv[4] != NULL)
            {
                if(!strcmp(strstr(argv[4], "."), ".bmp"))
                {
                    encInfo->stego_image_fname = argv[4];
                    return e_success;
                }
                else
                {
                    printf("argv[4] is not having .bmp extension\n");
                    return e_failure;
                }
            }
            else
            {
                encInfo->stego_image_fname = "stego.bmp";
                return e_success;
            }
        }
        else
        {
            printf("argv[3] is not having .txt, .c, or .sh extension\n");
            return e_failure;
        }
    }
    else
    {
            printf("argv[3] is not having .txt, .c, or .sh extension\n");
            printf("For encoding -> ./a.out -e <.bmp file> <secret file>\n");
            return e_failure; 
    }
    }
    else 
    {
        printf("argv[2] is not having .bmp extension\n");
        printf("For encoding -> ./a.out -e <.bmp file> <secret file>\n");
        return e_failure;
    }

    }
    else 
    {
        printf("error: argv[2] is not having .bmp extension\n");
        printf("For encoding -> ./a.out -e <.bmp file> <secret file>\n");
        return e_failure;
    }
}


Status do_encoding(EncodeInfo *encInfo)
{
    if (open_files(encInfo) == e_failure)
    {
    	printf("ERROR: %s function failed\n", "open_files" );
    	return e_failure;
    }
    else
    {
    	printf("SUCCESS: %s function completed\n", "open_files" );
    }


    printf("INFO: Checking for %s capacity to handle %s\n",encInfo->src_image_fname,encInfo->secret_fname);
    if(check_capacity(encInfo)==e_failure){
       printf(" %s not capable to handle %s\n",encInfo->src_image_fname,encInfo->secret_fname);
       return e_failure;
    }
    else
    {
        printf("Done\n");
    }


    printf("INFO: Copying Image Header\n");
    if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success){
        printf("Copied Successfully\n");
    }
    else{
        printf("Copying not completed\n");
        return e_failure;
    }
    printf("INFO: Encoding Magic String Signature\n");
    if(encode_magic_string(MAGIC_STRING, encInfo)==e_success){
        printf("Encoded Successfully\n");
    }
    else{
        printf("Encoding magic string is not done\n");
        return e_failure;
    }
   printf("INFO: Encoding %s file extension size\n",encInfo->secret_fname);
   char *stor=strstr(encInfo->secret_fname,".");
   int len=strlen(stor);
    encInfo->extn_size=len;
    if(encode_secret_file_extn_size(encInfo->extn_size, encInfo)==e_success)
    {
        printf("Encoded Successfully\n");
    }
    else
    {
         printf("Encoding %s File Extension size\n",encInfo->secret_fname);
    }

    printf("INFO: Encoding %s File Extension\n",encInfo->secret_fname);
    if(encode_secret_file_extn(encInfo -> secret_fname,encInfo)==e_success){
        printf("Encoded Successfully\n");
    }
    else
    {
         printf("Encoding %s File Extension is not done\n",encInfo->secret_fname);
         return e_failure;
    }

    printf("INFO: Encoding %s File size\n",encInfo->secret_fname);
    uint size=get_file_size(encInfo->fptr_secret);
    //printf("size %d\n",size);
    if(encode_secret_file_size(size,encInfo) == e_success)
    {
        printf("Encoded Successfully\n");
    }
    else
    {
         printf("Encoding %s File size is not done\n",encInfo->secret_fname);
         return e_failure;
    }


   printf("INFO: Encoding %s File data\n",encInfo->secret_fname);
   if(encode_secret_file_data(encInfo)==e_success)
   {
       printf("Encoded Successfully\n");
   }
   else
   {
        printf("Encoding %s File data is not done\n",encInfo->secret_fname);
    return e_failure;
    } 
    
   
    printf("INFO: Copying Left Over Data\n");
     if(copy_remaining_img_data(encInfo->fptr_stego_image,encInfo->fptr_src_image )==e_success)
    {
         printf("Copied Successfully\n");
    }
 else
 {
         printf("Not Done\n");
         return e_failure;
     }
     return e_success;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}
Status check_capacity(EncodeInfo *encInfo)
{
     //uint file=get_file_size(encInfo->fptr_secret);
    encInfo->image_capacity=get_image_size_for_bmp(encInfo->fptr_src_image);
    encInfo->size_secret_file=get_file_size(encInfo->fptr_secret);
    if(encInfo->image_capacity >54+((2+4+4+4+encInfo->size_secret_file)*8))
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    //printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    //printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

uint get_file_size(FILE *fptr)
{
    fseek(fptr,0,SEEK_END);
    return ftell(fptr);
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
      fseek(fptr_src_image,0,SEEK_SET);
      char buf[54];
      fread(buf,54,1,fptr_src_image);
      fwrite(buf,54,1,fptr_dest_image);
      return e_success;
}


Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    int size=strlen(magic_string);
    if(encode_data_to_image(magic_string, size,encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_failure){

        return e_failure;
    }
    return e_success;

}

Status encode_data_to_image(const char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char buffer[8];
    for(int i=0; i<size; i++){
        fread(buffer,8,1,fptr_src_image);
        encode_byte_to_lsb(data[i],buffer);
        //printf("%c\n",data[i]);
        fwrite(buffer,8,1,fptr_stego_image);
    }
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for(int i=0; i<8; i++)
    {
        image_buffer[i]=(image_buffer[i] & 0xFE) | ((data &1<<i)>>i) ;
    }
}



Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    char *s=strstr(file_extn,".");
    int len=strlen(s);
    //printf("%d\n",len);
    if(encode_data_to_image(s, len,encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_failure){

        return e_failure;
    }
    return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
     char buf[32];
     //printf("Yes\n");
     fread(buf,32,1,encInfo->fptr_src_image);
    encode_size_to_lsb(file_size, buf);
      fwrite(buf,32,1,encInfo->fptr_stego_image);

      return e_success;
}

Status encode_size_to_lsb(long file_size, char *image_buffer)
{
    for(int i=0; i<32; i++){
        image_buffer[i]=(image_buffer[i] & 0xFE) | ((file_size & 1<<i)>>i) ;
    }
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch[1];
	while(fread(ch,1,1,fptr_dest)>0)
    {
    fwrite(ch,1,1,fptr_src);
    }

     return e_success;
}
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    long int size = get_file_size(encInfo->fptr_secret);
    fseek(encInfo->fptr_secret,0,SEEK_SET);
    char ch[size];

    char buff[8];
	for(int i=0;i<size;i++)
	{
	     ch[i]=fgetc(encInfo->fptr_secret);
	     fread(buff,8,1,encInfo->fptr_src_image);
	     encode_byte_to_lsb(ch[i], buff);
	     fwrite(buff,8,1,encInfo->fptr_stego_image);
	     
	}
	return e_success;
}
Status encode_secret_file_extn_size(uint extn_size, EncodeInfo *encInfo)
{
    char buf[32];
    //printf("%d\n",extn_size);
     fread(buf,32,1,encInfo->fptr_src_image);
    encode_size_to_lsb(extn_size, buf);
      fwrite(buf,32,1,encInfo->fptr_stego_image);

      return e_success;
}