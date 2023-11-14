/*
Name	: Md Faizan Quamar
Title	: Stenography by using the bit map image(encoding and decoding)
Date	: 27/07/2023
*/
#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "decode.h"

//main function
int main(int argc,char *argv[])
{
    //checking the CLI
    if(argc < 4)
    {
	printf("Error : Number of Arguments are not Sufficient\n");
	printf("Usage : ./a.out -e beautiful.bmp secreat.txt output.bmp\n");
	return e_failure;
    }
    else
    {
	//condition checking by function call
	if(check_operation_type(argv) == e_encode)
	{
	    //encoding
	    EncodeInfo encinfo;
	    printf("We are going to perform encode Operation\n");
	    if((read_and_validate_encode_args(argv, &encinfo)) == e_success)
	    {
		printf("Read and Validation for encode is Completed\n");
		if( do_encoding(&encinfo) == e_success )
		{
		    printf("Encoding is Completed\n");
		}
	    }
	    else
	    {
		printf("Error : Please pass the valid file name\n");
	        printf("Usage : beautiful.bmp secreat.txt output.bmp\n");
	    }

	}
	else if(check_operation(argv) == e_decode)
	{
	    //decoding
	    DecodeInfo decInfo;
	    printf("We are ready for decoding\n");
	    if((read_and_validate_decode_args(argv, &decInfo)) == e_success)
	    {
		printf("Read and Validation for decode is Completed\n");
		if( do_decoding(argv,&decInfo) == e_success )
		{
		    printf("Decoding is Completed\n");
		}
	    }
	    else
	    {
		printf("Error : Please pass the valid file name\n");
	        printf("Usage : beautiful.bmp\n");
	    }
	}	
	else
	{
	    printf("Error : Second argument not found\n");
	    printf("Usage : -e for encodeing -d or decoding\n");
	}

	

    }



    return 0;
}
