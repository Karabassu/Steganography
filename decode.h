#ifndef DECODE_H
#define DECODE_H

#include "types.h"  // Include custom types defined in types.h
typedef struct _DecodeInfo
{
    char *src_image_fname;   // Source image file name (stego image containing the secret)
    FILE *fptr_src_image;    // File pointer for the source image
    char *secret_fname;      // Output file name for the decoded secret
    FILE *fptr_secret;       // File pointer for the secret file
    uint secret_extn_size;    // Size of the secret file extension
    uint size_secret_file;     // Size of the secret file
} DecodeInfo;

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);
Status do_decoding(DecodeInfo *decInfo);
Status decode_open_files(DecodeInfo *decInfo);
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo);
char decode_byte_to_lsb(char *image_buffer);
int decode_secret_file_extn_size(DecodeInfo *decInfo);
int decode_size_to_lsb(char *image_buffer);
Status decode_secret_file_extn(DecodeInfo *decInfo);
int decode_secret_file_size(DecodeInfo *decInfo);
Status decode_secret_file_data(DecodeInfo *decInfo);

#endif