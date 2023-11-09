#include <stdio.h>         //Including all necessary header files
#include "types.h"
#include "encode.h"
#include "decode.h"
#include <string.h>

int main(int argc,char **argv)          //Defining main function with command line arguments
{
	EncodeInfo encInfo;                 //Declaring variable with datatype EncodeInfo
	DecodeInfo decInfo;					//Declaring variable with datatype DecodeInfo
	if(argc>1)                          //checking if argguments passed or greater than 1
	{
		if(check_operation_type(argv) == e_encode)             //calling check operation type function to check operation type is encoding
		{
			printf("Selected Encoding\n");
			if(read_and_validate_encode_args(argv,&encInfo) == e_success)    //Read and validating arguments passed
			{
				printf("Read and Validate Success\n");

				if(do_encoding(&encInfo) == e_success)       //Calling do_encoding function on successfull read and validating arguments
				{
					printf("Encoding Succesfull\n");
				}
				else printf("ERROR in Encoding\n");

			}
			else  printf("Error in passing Arguments:\nEncoding : ./lsb_steg -e <.bmp file> <.txt file> [Output file]\n");
		}
		else if(check_operation_type(argv) == e_decode)           //Checking if operation type is decoding
		{
			printf("Selected Decoding\n");
			if(read_and_validate_decode_args(argv,&decInfo) == e_success)    //Reading and validating decode arguments
			{
				printf("Read and Validate Success\n");

				if(do_decoding(&decInfo) == e_success)      //on successful validating calling decode function
				{
					printf("Decoding Succesfull\n");
				}
				else printf("ERROR in Decoding\n");

			}
			else  printf("Error in passing Arguments:\nDecoding: ./lsb_steg -d <.bmp file> [output file]\n");

		}
		else  printf("Error in passing Arguments:\nEncoding : ./lsb_steg -e <.bmp file> <.txt file> [Output file]\nDecoding: ./lsb_steg -d <.bmp file> [output file]\n");
	}
	else  printf("Error in passing Arguments:\nEncoding : ./lsb_steg -e <.bmp file> <.txt file> [Output file]\nDecoding: ./lsb_steg -d <.bmp file> [output file]\n");
	return 0;
}

OperationType check_operation_type(char *argv[])     //Defining check operation type function
{
	if(!strcmp(argv[1],"-e"))          //comparing argumnent 1 whether it is -e encoding or -d decoding returning respective enum constants
		return e_encode;
	else if(!strcmp(argv[1],"-d"))
		return e_decode;
	else
		return e_unsupported;
}

