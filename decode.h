#ifndef DECODE_H
#define DECODE_H

#include "types.h"

#define MAX_FILE_SUFFIX 4
#define MAX_FILE_SIZE 20

typedef struct _DecodeInfo
{
	int magic_string_size;
	//Stego image
	char *stego_img_fname;
	FILE *fptr_stego;

	//Output file
	char output_fname[20];
	FILE *fptr_output;
	int output_file_extn_size;
	char extn_output_file[MAX_FILE_SUFFIX];
//	char output_file[MAX_FILE_SIZE];
	long output_file_size;
}DecodeInfo;


/* Decoding function prototype */

/* Check operation type */
OperationType check_operation_type(char *argv[]);

/* Read and validate Encode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the encoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_files_decode(DecodeInfo *decInfo);
	
/*Skip header content in stego image*/
Status skip_header(DecodeInfo *decInfo);

/*decoding magic string size*/
Status decode_magic_string_size(DecodeInfo *decInfo);

/*decoding magic string */
Status decode_magic_string(DecodeInfo *decInfo);

/*to get secret file extension size*/
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/*to get secret file extension*/
Status decode_secret_file_extn(DecodeInfo *decInfo);

/*to get secret file size*/
Status decode_secret_file_size(DecodeInfo *decInfo);

/*to get secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

/*decode each byte to get char */
char decode_byte_from_lsb(char *buffer);

/*decode each size to get char */
int decode_size_from_lsb(char *buffer);

#endif
