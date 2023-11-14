#include <stdio.h>
#include "decode.h"
#include "types.h"
#include <string.h>
#include <stdlib.h>

/* staring the function and performing the decoding operation while using the condition and function calls */
Status do_decoding(char *argv[],DecodeInfo *decInfo)
{
    if(open_file(decInfo) == e_success)
    {
	printf("Source file open Successfully\n");
	if(check_magic_string(decInfo) == e_success)
	{
	    printf("Magic String Found\n");
	    if(decode_file_size(decInfo) == e_success)
	    {
		printf("File size checking completed\n");
		if(decode_secret_file_extn(decInfo->file_size,decInfo) == e_success)
		{
		    printf("File Extension Found\n");
		    if(open_file_secret(argv,decInfo) == e_success)
		    {
			printf("Secret file open successfully\n"); 
	                if(decode_file_size(decInfo) == e_success)
			{
			    printf("File Size Completed\n");
		            if(decode_secret_file_extn(decInfo->file_size,decInfo) == e_success)
			    {
				printf("Secret data found\n");
				if(copy_secret_data_file(decInfo) == e_success)
				{
				    printf("Secret data copied Successfully\n");
                                    if(close_files_decode(decInfo) == e_success)
				    {
					printf("File closed file\n");
					return e_success;
				    }
				}
			    }
			}

		    }
		}
	    }
	}
	else
	    printf("Magic string not Found\n");
    }
}

/* copying the the secret data from source file to another file */
Status copy_secret_data_file(DecodeInfo *decInfo)
{
    fwrite(decInfo->data,1,decInfo->file_size,decInfo->fptr_secret);
    return e_success;
}

/* opening the secret data file as per the extension given in src image */
Status open_file_secret(char *argv[],DecodeInfo *decInfo)
{
    if(!strstr(argv[3],"."))
    {
	decInfo->secret_fname = strcat(argv[3],decInfo->data);
    }
    if(strcmp(strstr(argv[3],"."),decInfo->data) == 0)
    {
	decInfo->secret_fname = argv[3];
    }
    else if(strcmp(strstr(argv[3],"."),decInfo->data)!=0)
    {
	decInfo->secret_fname = strcat(strtok(argv[3],"."),decInfo->data);
    }

    decInfo->fptr_secret = fopen(decInfo->secret_fname,"w");
    if(decInfo->fptr_secret == NULL)
    {
	perror("fopen");
	return e_failure;
    }
    free(decInfo->data);
    return e_success;
}

/* decoding the secret file extension and file data*/
Status decode_secret_file_extn(uint file_size, DecodeInfo *decInfo)
{
    decInfo->data = (char *)calloc(file_size,sizeof(char));
    char ch[8];
    for(int i=0; i<file_size; i++)
    {
	fread(ch,1,8,decInfo->fptr_image);
	decode_byte_to_lsb(ch,decInfo,i); 
    }
    return e_success;
}

/* decoding the file size and extn size */
Status decode_file_size_by_lsb(char *ch,DecodeInfo *decInfo)
{
    for(int i=0; i<32; i++)
    {
	decInfo->file_size = (ch[i] & 1)<<i | decInfo->file_size;
    }
}

/* calling the function to decode the file size and extn size */
Status decode_file_size(DecodeInfo *decInfo)
{
    decInfo->file_size = 0;
    char buffer[32];
    fread(buffer,1,32,decInfo->fptr_image);
    decode_file_size_by_lsb(buffer, decInfo);
    return e_success;
}

/* decoding the character from lsb */
Status decode_byte_to_lsb(char *image_buffer,DecodeInfo *decInfo, uint i)
{
    for(int j=0; j<8; j++)
    {
	decInfo->data[i] = (image_buffer[j] & 1)<<j | decInfo->data[i];
    }
}

/* checking whether the magic string is there or not */
Status check_magic_string(DecodeInfo *decInfo)
{
    decInfo->data=(char *)malloc(2);
    char ch[8];
    fseek(decInfo->fptr_image,54,SEEK_SET);
    for(int i=0; i<2; i++)
    {
	fread(ch,1,8,decInfo->fptr_image);
	decode_byte_to_lsb(ch,decInfo,i); 
    }
    if(strcmp(decInfo->data,MAGIC_STRING)==0)
    {
	free(decInfo->data);
	return e_success;
    }
}

/* checking whether user has passed for decoding or not */
OperationType check_operation(char *argv[])
{
    if(strcmp(argv[1],"-d") == 0 )
    {
	return e_decode;
    }
    else
	return e_unsupported;
}

/* checking the command line arguments */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if(strcmp(strstr(argv[2],"."),".bmp") == 0)
    {
	decInfo->image_fname = argv[2];
	return e_success;
    }
    else
	return e_failure;
}

/* opening the src file */
Status open_file(DecodeInfo *decInfo)
{
    decInfo->fptr_image=fopen(decInfo->image_fname,"r");

    if(decInfo->fptr_image == NULL)
    {
	perror("");
	return e_failure;
    }
    return e_success;
}

/* closing the file*/
Status close_files_decode(DecodeInfo *decInfo)
{
    fclose(decInfo->fptr_image);
    fclose(decInfo->fptr_secret);
    return e_success;
}
