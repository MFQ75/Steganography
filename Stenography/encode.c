#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include <stdlib.h>

/* Function declartion and checking the condition by calling functions*/
Status do_encoding(EncodeInfo *encInfo)                                                                                  
{
    if( open_files(encInfo) == e_success)
    {
	printf("Open File Operation is success\n");
	if(check_capacity(encInfo) == e_success)
	{
	    printf("Capacity checking Completed\n");
	    if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success )
	    {
		printf("BMP Header File Coppied Successfully\n");
		if(encode_magic_string(MAGIC_STRING,encInfo) == e_success )
		{
		    printf("Magic string encoded Successfully\n");
		    encInfo->extn_secret_file = strstr(encInfo->secret_fname,".");
		    if(encode_secret_file_size(strlen(encInfo->extn_secret_file), encInfo) == e_success )
		    {
			printf("Secret File Extension Size encoded Successfully\n");
			if(encode_magic_string(encInfo->extn_secret_file,encInfo) == e_success )
			{
			    printf("Secret file Extension encode completed\n");
			    if(encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success )
			    {
				printf("Secret file size encoded Successfully\n");
				if(encode_secret_file_extn(encInfo->fptr_secret,encInfo) == e_success )
				{
				    printf("Secret File data encoded successfully\n");
				    if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)
				    {
					printf("File Coppied Successfully\n");
					if(close_files(encInfo) == e_success)
					{
					    printf("File Closed Successfully\n");
					    return e_success;
					}
				    }
				}

			    }
			}
		    }
		}
	    }
	}
	else
	{
	    printf("%s Size is not Enough to Encode\n",encInfo->src_image_fname);
	    return e_failure;
	}
    }
}
/* copying the remaining the data from source file*/
Status copy_remaining_img_data(FILE *fptr_src,FILE *fptr_dest)
{
    char ch;
    while(fread(&ch,1,1,fptr_src) > 0)
	fwrite(&ch,1,1,fptr_dest);
}

/* Encoding the file extension */
Status encode_secret_file_extn(FILE *file_extn, EncodeInfo *encInfo)
{
    char buffer[8],ch;
    fseek(file_extn,0,SEEK_SET);
    for(int i=0; i<encInfo->size_secret_file; i++)
    {
	fread(&ch,1,sizeof(char),encInfo->fptr_secret);
	fread(buffer,1,8,encInfo->fptr_src_image);
	encode_byte_to_lsb(ch,buffer);
	fwrite(buffer,1,8,encInfo->fptr_stego_image);
    }
    return e_success;
}

/* encoding the secret file size */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char buffer[32];
    fread(buffer,1,32,encInfo->fptr_src_image);
    for(int i=0; i<32; i++)
    {
	buffer[i] = ((buffer[i] & 0xfe) | (file_size & 1<<i)>>i);
    }
    fwrite(buffer,1,32,encInfo->fptr_stego_image);
    return e_success;
}

/* encoding the magic string */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    if(encode_data_to_image(magic_string, strlen(magic_string), encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success )
    {
	return e_success;
    }
}

/* encoding the the magic string one by one  */
Status encode_data_to_image(const char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char buffer[8];
    for(int i=0; i<size; i++)
    {
	fread(buffer,1,8,fptr_src_image);
	encode_byte_to_lsb(data[i],buffer);
	fwrite(buffer,1,8,fptr_stego_image);
    }
    return e_success;
}

/* encoding the the character to lsb */
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for(int i=0; i<8; i++)
    {
	image_buffer[i]=((image_buffer[i] & 0xfe) | (( data & (1<<i))>>i )); 
    }
}

/* copying the bmp header file */
Status copy_bmp_header(FILE *fptr_src_image,FILE *fptr_dest_image)
{
    char buffer[54];
    fseek(fptr_src_image,0,SEEK_SET);
    fread(buffer,1,54,fptr_src_image);
    fwrite(buffer,1,54,fptr_dest_image);
    return e_success;
}

/* checking whether the user has passed encode or not */
OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1],"-e") == 0 )
    {
	return e_encode;
    }
}

/* checking the command line arguments */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if(!strstr(argv[2],"."))
    {
	return e_failure;
    }

    if(strcmp(strstr(argv[2],"."),".bmp") == 0)
    {
	encInfo->src_image_fname = argv[2];
	if(!strstr(argv[3],"."))
	{
	    return e_failure;
	}
	if((strcmp(strstr(argv[3],"."),".txt") == 0) || (strcmp(strstr(argv[3],"."),".c") == 0))
	{
	    encInfo->secret_fname = argv[3];
	    if(argv[4]==NULL)
	    {
		encInfo->stego_image_fname = "Default.bmp";
		return e_success;
	    }
	    else if(!strstr(argv[4],"."))
	    {
	      	    encInfo->stego_image_fname = strcat(argv[4],".bmp");
		    return e_success;
	    }
	    else
	    {
		if(strcmp(strstr(argv[4],"."),".bmp") == 0)
		{
		    encInfo->stego_image_fname = argv[4];
		    return e_success;
		}
		else
		{
		    encInfo->stego_image_fname = strcat(strtok(argv[4],"."),".bmp");
		    return e_success;
		}

	    }
	}	
	else
	    return e_failure;
    }
    else
	return e_failure;


}

/* checking the capacity of src and secret file*/
Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    if(encInfo->image_capacity > (54+2*8+4*8+4*8+4*8+encInfo->size_secret_file*8))
    {
	return e_success;
    }
    else
	return e_failure;
}

/*checking the file size of secret file*/
uint get_file_size(FILE *fptr)
{
    fseek(fptr,0,SEEK_END);
    return ftell(fptr);
}

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

/* closing the files */
Status close_files(EncodeInfo *encInfo)
{
    fclose(encInfo->fptr_src_image);
    fclose(encInfo->fptr_secret);
    fclose(encInfo->fptr_stego_image);

    return e_success;
}
