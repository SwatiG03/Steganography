#include <stdio.h>         //Including all necessary header files
#include "decode.h"
#include "types.h"
#include <string.h>
#include <stdlib.h>
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)    //validating args of decoding
{
	if(argv[2] != NULL)                      //validating whether argument 2 passed is bmp file or not
	{
		FILE *fptr=fopen(argv[2],"r");       //opening stego image 
		if(fptr != NULL)
		{
			char *buff=malloc(2);
			fread(buff,2,1,fptr);         			  //reading 2 butes of image
			if(!strcmp(buff,"BM"))
			{
				decInfo->stego_img_fname=argv[2];     //storing the string to variable
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
	}
	else
	{
		return e_failure;
	}
	if(argv[3]!=NULL)         								//checking if argument is present or not
	{
		strcpy(decInfo->output_fname,argv[3]);              //if present storing it variable
//		decInfo->output_fname=argv[3];
	}
	else
	{
		//decInfo->output_fname=malloc(20);
		strcpy(decInfo->output_fname,"default");           //if not giving a default filename
		//decInfo->output_fname="default";
		
	}
	return e_success;

}
Status open_files_decode(DecodeInfo *decInfo)                       //opening files to decode
{
	decInfo->fptr_stego=fopen(decInfo->stego_img_fname,"r");        //opening stego image
	if(decInfo->fptr_stego == NULL )                                //printing error if file is not opened
	{
		perror("fopen");
		fprintf(stderr,"Error in opening file : %s\n",decInfo->stego_img_fname);
		return e_failure;
	}
	return e_success;

}

Status skip_header(DecodeInfo *decInfo)       			  //skipping header content becoz no encoded information in header
{
	fseek(decInfo->fptr_stego,54,SEEK_SET);               //setting file indicator to 54th byte
//	printf("Ftell=%d\n",ftell(decInfo->fptr_stego));
	if(ferror(decInfo->fptr_stego))                 
	{
		return e_failure;
	}
	else
	{
		return e_success;
	}
}
Status decode_magic_string_size(DecodeInfo *decInfo)               //decoding magic string size to know how many bytes contain magic string
{
	char buffer[32];
	fread(buffer,32,1,decInfo->fptr_stego);        					//reading 32 bytes of image for 4 byes of integer
	if(ferror(decInfo->fptr_stego))
	{
		return e_failure;
	}
	decInfo->magic_string_size=decode_size_from_lsb(buffer);       //calling decode_size_from_lsb to decode size of magic string
//	printf("%d\n",decInfo->magic_string_size);
	return e_success;
}

Status decode_magic_string(DecodeInfo *decInfo)           //decoding magic string
{
	char str[decInfo->magic_string_size+1],buffer[8];      //reading 8 bytes of image for 1 char
	for(int i=0;i<decInfo->magic_string_size;i++)
	{
		fread(buffer,8,1,decInfo->fptr_stego);             //reading 8 bytes for each character
		if(ferror(decInfo->fptr_stego))
		{
			return e_failure;
		}
		str[i]=decode_byte_from_lsb(buffer);         //decpding and stori
	}
	str[decInfo->magic_string_size]='\0';
	//printf("magic string is %s\n",str);
	char *user_str=malloc(decInfo->magic_string_size+1);     //declaring char pointer to store user magic string
	printf("Enter the Magic string : ");
	scanf(" %[^\n]",user_str);                            	//Reading user magic string
	if(!strcmp(str,user_str))                            	//comparing user and decoded magic string
	{
		printf("Magic String matched\n");
	}
	else
	{
		return e_failure;         //Returning failure if magic string not matched
	}
	return e_success;	
}

Status decode_secret_file_extn_size(DecodeInfo *decInfo)            //Decoding secret file extension size to know how much data to be fetched for decoding extension
{
	char buffer[32];
	fread(buffer,32,1,decInfo->fptr_stego);            				//reading 32 bytes of image to get extn size
	if(ferror(decInfo->fptr_stego))
	{
		return e_failure;
	}
	decInfo->output_file_extn_size=decode_size_from_lsb(buffer);    //calling decode_size_from_lsb function to store size(length) of extension
	return e_success;

}

Status decode_secret_file_extn(DecodeInfo *decInfo)                //Decoding secret file extension
{
	char extn[decInfo->output_file_extn_size+1],buffer[8];         //Declaring char array to store extension and buffer for storing 8 byes of image
	for(int i=0;i<decInfo->output_file_extn_size;i++)              //loop to read and decode each char of extension fro image
	{
		fread(buffer,8,1,decInfo->fptr_stego);         //Reading 8 byets of image to decode one char           
		if(ferror(decInfo->fptr_stego))
		{
			return e_failure;
		}
		extn[i]=decode_byte_from_lsb(buffer);         //Calling decode_byte_from_lsb to decode each char
	}
	extn[decInfo->output_file_extn_size]='\0';
//	printf("extn -> %s\n", extn);
//	printf("dec -> output fname -> %s\n", decInfo -> output_fname);
	strcat(decInfo-> output_fname,extn);             					 //concatenating filename with extension               
	return e_success;
}

Status decode_secret_file_size(DecodeInfo *decInfo)             //Decoding secret file size
{
	char buffer[32];                                  //to decode secret file size we need to get 32 byytes of image
	fread(buffer,32,1,decInfo->fptr_stego);           //reading 32 bytes of image
	if(ferror(decInfo->fptr_stego))
	{
		return e_failure;
	}
	decInfo->output_file_size=decode_size_from_lsb(buffer);        //Decoding size by calling decode_size_from_lsb and store it in structure member
//	printf("Secret file size %ld\n",decInfo->output_file_size);
	return e_success;

}

Status decode_secret_file_data(DecodeInfo *decInfo)		//Decoding secret file data
{
	char buffer[8];                                //to decode each char of secret data we need to
	decInfo -> fptr_output = fopen(decInfo -> output_fname, "w+");       //opening secret file to store secret data
	if( decInfo->fptr_output == NULL )			  //Checking error in opening secret file  
	{
		perror("fopen");
		fprintf(stderr,"Error in creating file");
		return e_failure;
	}
	for(int i=0;i<decInfo->output_file_size;i++)       //Decoding each char from 8 byes of image
	{
		fread(buffer,8,1,decInfo->fptr_stego);         //Reading 8 byest of image
		if(ferror(decInfo->fptr_stego))                //checking error in reading
		{
			return e_failure;
		}
		char c=decode_byte_from_lsb(buffer);          //Decoding char
		fwrite(&c,1,1,decInfo->fptr_output);          //writing char to file
		if(ferror(decInfo->fptr_output))              //checking error in writing
		{
			return e_failure;
		}
	}
	return e_success;
}
char decode_byte_from_lsb(char *buffer)     //Logic to decode 8 bytes of image 
{
	char res=0;                  //Declaring char
	for(int i=0;i<8;i++)
	{
		if(buffer[i] & 01)
		{
			res=res|(1<<i);     //setting each bit of char from lsb of each byte
		}
	}
	return res;
}

int decode_size_from_lsb(char *buffer)     //Logic to decode 32 bytes of image
{
	int	res=0;                  //Declaring int
	for(int i=0;i<32;i++)
	{
		if(buffer[i] & 01)
		{
			res=res|(1<<i);     //setting each bit of char from lsb of each byte
		}
	}
	return res;
}

Status do_decoding(DecodeInfo *decInfo)             //function to call all necessary function required to do decoding
{
	if(open_files_decode(decInfo) == e_success)     //Calling open files function to open Required files
	{
		printf("Opened files\n");
	}
	else
	{
		printf("Error in opening files\n");        //Printing Error on failure of opening files
		return e_failure;
	}
	if(skip_header(decInfo) == e_success)          //function to skip header bytes
	{
		printf("Skipped header file\n");
	}
	else
	{
		printf("Error in skipping header content\n");      //error on calling functions
		return e_failure;
	}
	
	if(decode_magic_string_size(decInfo) == e_success)        //calling decoding magic string size files
	{
		printf("Magic String size decoded Successfully\n");
	}
	else
	{
		printf("Error in decoding Magic string size\n");
		return e_failure;
	}
	
	if(decode_magic_string(decInfo) == e_success)             //calling decode magic string function
	{
		printf("Magic String decoded Successfully\n");
	}
	else
	{
		printf("Error in decoding Magic string\n");
		return e_failure;
	}
	if(decode_secret_file_extn_size(decInfo) == e_success)   //Calling decode secret file extn size function
	{
		printf("File extension size decoded\n");
	}
	else
	{
		printf("Error in decoding file extension size\n");
		return e_failure;
	}
	if(decode_secret_file_extn(decInfo) == e_success)       //Calling decode secret file extension function
	{
		printf("File extension decoded\n");
	}
	else
	{
		printf("Error in decoding file extension\n");
		return e_failure;
	}
	if(decode_secret_file_size(decInfo) == e_success)       //calling decode secret file size function
	{
		printf("File size decoded\n");
	}
	else
	{
		printf("Error in decoding file size\n");
		return e_failure;
	}
	if(decode_secret_file_data(decInfo) == e_success)      //Calling decoding Secret file data function
	{
		printf("File data decoded\n");
	}
	else
	{
		printf("Error in decoding file data\n");
		return e_failure;
	}
	return e_success;      //Returning e_success after successful execution of all functions
}
