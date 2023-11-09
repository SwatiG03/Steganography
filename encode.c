#include <stdio.h>         //Including all necessary header files
#include "encode.h"
#include "common.h"
#include "types.h"
#include <string.h>
#include <stdlib.h>

/* Function Definitions */

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
	printf("width = %u\n", width);

	// Read the height (an int)
	fread(&height, sizeof(int), 1, fptr_image);
	printf("height = %u\n", height);

	// Return image capacity
	return width * height * 3;
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

//Validating if all arguments are passed and are correct or not
Status read_and_validate_encode_args(char *argv[],EncodeInfo *encInfo)
{
	//Opening argument 3 to see if it is BMP type file or not
	FILE *fp=fopen(argv[2],"r");
	char *str=malloc(2);         				//Declaring char array to store first 2 bytes of argument 3 file
	if(fp != NULL)           					//validating arg3 file pointer
	{
		fread(str,2,1,fp);         				//Reading first 2 bytes
		if(!strcmp(str,"BM"))      				//Validating if it is BMP file or not
		{
			encInfo->src_image_fname=argv[2];      //storing arg3 as source image
		}	
		else 
		{
			return e_failure;         			   //Return failure if it not BMP file
		}
	}
	char *extn=malloc(4);       				   //Declaring char array to store  extension of secret file
	if(argv[3] != NULL )
	{
		if((extn=strstr(argv[3],".")) != NULL)            //if secret file has . store the addres of . in extn variable
		{
			strcpy(encInfo->extn_secret_file,extn);       //copying extn to struct
			encInfo->secret_fname=argv[3];      		  //storing arg4 in structure member secret_fname
		}
		else
		{
			return e_failure;
		}
	}
	else
	{
		return e_failure;
	}
	if(argv[4] != NULL)                    			//Checking if argument 5 is present or not
	{
		if(strstr(argv[4],".bmp") != NULL)   		//if present checking whether it is bmp file or not
		{
			encInfo->stego_image_fname=argv[4];     //if it is bmp storing file name in struture member stego_image_fname
		}
		else
		{
			return e_failure;           				 //Return e_failure if it is not BMP file
		}
	}
	else encInfo->stego_image_fname="default.bmp"; 		//if arg 5 is not present giving default output image name
	return e_success;

}

uint get_file_size(FILE *fptr)  		  //function to get size of secret file
{
	fseek(fptr,0,SEEK_END);     		  //Moving file indicator to end of secret file
	return ftell(fptr);        			  //return position of file indicator using ftell
}

Status read_magic_string(EncodeInfo *encInfo)        //Function to read magic string frome user
{
	encInfo->magic_string=malloc(20);                //allocating memory to pointer read string
	printf("Enter Magic String\n");
	scanf(" %[^\n]",encInfo->magic_string);         //Reading magic string
//	printf("%s\n",encInfo->magic_string);
	return e_success;
}

Status check_capacity(EncodeInfo *encInfo)         //Function to check data can be encoded in source image or not
{
	int capacity;          
	encInfo->size_secret_file=get_file_size(encInfo->fptr_secret);      //Calling get_file_size function to get size of secret file and storing it in structure mem size_secret_file
	capacity=(strlen(encInfo->magic_string)+4+strlen(encInfo->extn_secret_file)+4+encInfo->size_secret_file)*8; //Calculating no.of bytes to be encoded
	if(get_image_size_for_bmp(encInfo->fptr_src_image) > capacity)       //calling get_image_size_for_bmp function to get size of input image bmp file and checking capcity of image
		return e_success;
	else
		return e_failure;
}

Status encode_magic_string_size(int length,EncodeInfo *encInfo) 	//Encoding magic string size
{
	char buffer[32];             
	fread(buffer,32,1,encInfo->fptr_src_image);        				//reading 32 bytes of source image to encode 4 bytes of int
	if(ferror(encInfo->fptr_src_image))
	{
		return e_failure;
	}
	if(encode_size_to_lsb(length,buffer) == e_success)         	//calling encode size to lab data
	{
		fwrite(buffer,32,1,encInfo->fptr_stego_image);       	//Writing encoded 32 bytes to steo image
		return e_success;
	}
	else
	{
		return e_failure;
	}

}
Status copy_bmp_header(FILE *fptr_src_image,FILE *fptr_dest_image)     //function to copy header content
{
	char buffer[54];                     				//Declaring char array to read 54 bytes i.e, header
	fseek(fptr_src_image,0,SEEK_SET);    				//setting file indicator position to beginning
	fseek(fptr_dest_image,0,SEEK_SET);
	fread(buffer,54,1,fptr_src_image);        			//Reading 54B from source and writing it to output image
	fwrite(buffer,54,1,fptr_dest_image);
	if(ferror(fptr_src_image) || ferror(fptr_dest_image))        //Checking for error in reading file
	{
		return e_failure;
	}
	return e_success;
}
Status encode_data_to_image(const char *data,int size,FILE *fptr_src_image,FILE *fptr_stego_image)     //Encoding data into image
{
	char buffer[8];                //for each char we need 8 byest of image
	for(int i=0;i<size;i++)
	{
		fread(buffer,8,1,fptr_src_image);       	 //reading 8 bytes from image
		encode_byte_to_lsb(*(data+i),buffer);        //calling encode_byte_to_lsb 
		fwrite(buffer,8,1,fptr_stego_image);    	 //writing 8 byes of encoded data to image stego
	}
	if(ferror(fptr_src_image) || ferror(fptr_stego_image))
	{
		return e_failure;
	}
	return e_success;

}
Status encode_magic_string(const char *magic_string,EncodeInfo *encInfo)     //encoding magic string to stego image
{
	if(encode_data_to_image(magic_string,strlen(magic_string),encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)	//calling encode_data_to_image to image function to encode magic string to stego image
	{
		return e_success;
	}
	else
	{
		return e_failure;
	}
}

Status encode_secret_file_extn_size(int data,EncodeInfo *encInfo)       //encoding secret file extn size
{
	char buffer[32];
	fread(buffer,32,1,encInfo->fptr_src_image);      //reading 32 Byets for encodeing 4 bytes of data
	if(ferror(encInfo->fptr_src_image))
	{
		return e_failure;
	}
	if(encode_size_to_lsb(data,buffer) == e_success)
	{
		fwrite(buffer,32,1,encInfo->fptr_stego_image);        //writing encoded data to stego image
		return e_success;
	}
	else
	{
		return e_failure;
	}
}

Status encode_secret_file_extn(const char *file_extn,EncodeInfo *encInfo)        //encoding secret file extension
{
	if(encode_data_to_image(file_extn,strlen(file_extn),encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)	//calling function to encode extn to image
	{
		return e_success;
	}
	else
	{
		return e_failure;
	}

}

Status encode_secret_file_size(long file_size,EncodeInfo *encInfo)               //encoding secret file size
{
	char buffer[32];
	fread(buffer,32,1,encInfo->fptr_src_image);             //reading 32 byest to encode 4 bytes 
	if(encode_size_to_lsb(file_size,buffer) == e_success)          //calling encode_size_to_lsb to encode 32 bytes with size
	{
		fwrite(buffer,32,1,encInfo->fptr_stego_image);           //writing 32 bytes to stego image
		if(ferror(encInfo->fptr_src_image) || ferror(encInfo->fptr_stego_image))
		{
			return e_failure;
		}
		return e_success;
	}
	else
	{
		return e_failure;
	}

}

Status encode_secret_file_data(EncodeInfo *encInfo)    //encoding secret file data 
{
	fseek(encInfo->fptr_secret,0,SEEK_SET);          //setting file indicator of secret file to beginning
	while(fread(encInfo->secret_data,1,1,encInfo->fptr_secret)>0)      //reading and encoding each char from secret file to stego image
	{
		if(encode_data_to_image(encInfo->secret_data,1,encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success);	
		else
		{
			return e_failure;
		}
	}
	return e_success;

}

Status encode_byte_to_lsb(char data, char *image_buffer)         //encoding char in 8 bytes
{
	for(int i=0;i<sizeof(char)*8;i++)
	{
		image_buffer[i]=(image_buffer[i]&0xFE)|((data&1<<i)>>i);       //logic for setting each byte of char in LSB of each byte
	}
	return e_success;
}

Status encode_size_to_lsb(int data, char *image_buffer)         //encoding int in 32 bytes
{
	for(int i=0;i<(signed)(sizeof(int)*8);i++)
	{
		image_buffer[i]=(image_buffer[i]&0xFE)|((data&1<<i)>>i);       //logic for setting each byte of char in LSB of each byte
	}
	return e_success;
}
Status copy_remaining_img_data(FILE *fptr_src,FILE *fptr_dest)
{
	char c;
	while(fread(&c,1,1,fptr_src)>0)         //reading and writing all remaining bytes from source image to stego image
		fwrite(&c,1,1,fptr_dest);
	if(ferror(fptr_src) || ferror(fptr_dest))
	{
		return e_failure;
	}
	return e_success;
}


Status do_encoding(EncodeInfo *encInfo)      //calling all necessary function encoding  on successsful calling of each function we move forward on failure we return failure
{
	if(open_files(encInfo) == e_success)              //Calling function to open files
	{
		printf("Opened required files\n");
	}
	else
	{
		printf("Error in opening files\n");
		return e_failure;
	}
	if(read_magic_string(encInfo) == e_success)     //Calling function to read magic string
	{
		printf("Magic string read successfully\n");
	}
	else
	{
		printf("Error in reading magic string\n");
		return e_failure;
	}
	if(check_capacity(encInfo) == e_success)       //Calling check capacity function 
	{
		printf("Capacity is checked\n");
	}	
	else
	{
		printf("Error in capacity\n");
		return e_failure;
	}
	if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)       //Calling copy bmp header function
	{
		printf("Header content copied\n");
	}
	else
	{
		printf("Error in Header copy\n");
		return e_failure;
	}
	if(encode_magic_string_size(strlen(encInfo->magic_string),encInfo) == e_success )        //Calling encoding magic string size function
	{
		printf("Magic string size Encoded\n");
	}
	else
	{
		printf("Error in Magic string size encode\n");
		return e_failure;
	}
//	if(encode_magic_string(MAGIC_STRING,encInfo) == e_success )
	if(encode_magic_string(encInfo->magic_string,encInfo) == e_success )          //Calling encoding magic string function
	{
		printf("Magic string Encoded\n");
	}
	else
	{
		printf("Error in Magic string encode\n");
		return e_failure;
	}
	if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file),encInfo) == e_success)   //Calling sencode secret file extension size function
	{
		printf("Secret file Extension size is Encoded\n");
	}
	else
	{
		printf("Error in encoding secret file extension size\n");
		return e_failure;
	}
	if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo) == e_success)        //calling encode secret file extension
	{
		printf("Secret File extension Encoded\n");
	}
	else
	{
		printf("Error in encoding secret file extension\n");
		return e_failure;
	}
	if(encode_secret_file_size(encInfo->size_secret_file,encInfo) == e_success)       //Calling encoding secret file size function
	{
		printf("Secret file size is Encoded\n");
	}
	else
	{
		printf("Error in encoding secret file size\n");
		return e_failure;
	}
	if(encode_secret_file_data(encInfo) == e_success)             //Calling encoing secret filse data function
	{
		printf("Secret file is Encoded\n");
	}
	else
	{
		printf("Error in encoding secret file \n");
		return e_failure;
	}
	if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)    //Calling copy remaining data funxtion
	{
		printf("Remaining data is copied\n");
	}
	else
	{
		printf("Error in copying Remaining data\n");
		return e_failure;
	}
	return e_success;       //returning success on successfull execution of function else returns failure
}
