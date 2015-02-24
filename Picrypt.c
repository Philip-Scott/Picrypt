/*
Picrypt: Encrypt any file into an image 
By: Felipe Escoto

	Done! Add better en/de-cryption Keys
	DONE! .txt output & input
	DONE! CREATE BMP from scratch if one does NOT exist
	DONE! Add progress Bars 
	DONE! Create the bmp dimentions based on the inputfile size
	DONE! Allow any file as input!
*/

//To compile: cc     Picrypt.c   -o picrypt -std=c99 && ./picrypt

#include <stdio.h>
#include <stdlib.h>
#include "bmp.h" 

const int version = 0x15;
 
char ft[3];
char* fext;
char* infile;
char* outfile;
FILE* picfile;
FILE* txtfile;
BITMAPINFOHEADER bi;
BITMAPFILEHEADER bf;
int test, p = 19, option, key = 0;
int encrypt (), decrypt ();
int encheck(), decheck();
int help();
long  filesize, orfilesize, f = 0;


int main(int argc, char* argv[])   //USAGE!: programname imagefile txtfile
{
	setvbuf(stdout, NULL, _IONBF, 0);	
	if (argc != 3 && argc != 2) { // ensure proper argument usage
		help(argv[0]);
		return 0;
	}	
	char* fext = argv[1];	
	char* fext2 = argv[2];
	test = 0;	
	while (fext[test] != '.')
		{	
			test++;
		}
	test = test + 5 + 4;
	char fext3[test];
	test = 0;
	//getchar();
	int period = 0;
	
	switch (argc) {
	case 2: //One file mode!
		while (fext[test] != '\0')
		{
			if (fext[test] == '.')
			{
				period = test;
			}
			fext3[test] = fext[test];			
			test++;
		}
		test = period; //Go to the last "." to check the extention
		fext3[test] = fext[test];	
		test++;
		ft[0] = fext[test];
		ft[1] = fext[test + 1];
		ft[2] = fext[test + 2];
		if ( ft[0] != 'b' || ft[1] != 'm' || ft[2] != 'p') {//if image isn't bmp, end file
			test = test + 3;
			fext3[test] = '.';
			fext3[test + 1] = 'b';
			fext3[test + 2] = 'm';
			fext3[test + 3] = 'p';
			fext3[test + 4] = '\0';
			//printf("Picfile: %s  Infile: %s\n", fext3, argv[1]);
			encrypt (3 , fext3, argv[1]);			
		}
		else {
			test--;
			fext3[test] = '\0';
			//printf("Picfile: %s  Outfile: %s\n", argv[1], fext3);
			decrypt(3, argv[1], fext3);
		}
		return 0;
	case 3: //Advanced mode
		
		while (1) {
			printf("  Encrypt = 1\n  Decrypt = 2\n  Set Code = 3\n  Exit = 0\n\n What do you want to do: ");	
			scanf("%d", &option);

			switch (option) {
			case 1: encrypt (argc ,argv[1], argv[2] );
				exit(1);
			case 2: decrypt (argc ,argv[1], argv[2] );
				exit(1);
			case 4:
				printf("\n\n\nAbout:\n Made by: Felipe Escoto");
				printf("\n\nThis program is published under the terms of the gpl license\n it comes with ABSOLUTELY NO WARRANTY");
				printf("\nI am not responsable for any damage that could occour to your system");
			break;	
			case 3: 	
				printf("Enter digital keycode: ");
				scanf("%d",&key); 
				break;
			case 0: exit(1);
				default:
			printf("\n\nInvalid Option!\nTry again\n");
			}
		}
	} 
return 0;
}


int encrypt (int argc, char* argv1, char* argv2) {
	// write outfile's BITMAPFILE and INFOHEADER
	encheck(argc, argv1, argv2 );
	
	fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, picfile);
	fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, picfile);

	char temp[3];
	RGBTRIPLE triple;
	//Set the amount of Bytes the original file had
	//Will use two pixels.... For consistency
	triple.rgbtBlue = version;	//Encryption version?	
	triple.rgbtRed = 0xfe;		//Erm.... my signature? sure, why not. had a byte left xD
	fwrite(&triple.rgbtBlue, 1, 1, picfile);
	fwrite(&triple.rgbtRed, 1, 1, picfile);	
	fwrite(&orfilesize, 4, 1, picfile);
	filesize = orfilesize;
	//	printf("\n\n\n");
	printf("\n\n Progress:                    :");
	printf("   \rProgress: ");
														
	for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++) // iterate over picfile's scanlines
	{
		for (int j = 0; j < bi.biWidth; j++) // iterate over pixels in line
		{
			for (int b = 0; b < 3; b++)  //reads the inputfile char by char- saves to temp
			{
				fread(&temp[b], 1, 1, txtfile);
				if ( feof(txtfile)) temp[b] = 0xff; //check if we reached the last byte of infile
			}
			triple.rgbtBlue = temp[0] + key; //saves each char to a "color"
			triple.rgbtGreen = temp[1] + key;	
			triple.rgbtRed = temp[2] + key;
				
			filesize = filesize	- 3;	
							
			test = filesize * 20 / orfilesize; 
			if (test <= p) {
				printf("*");
				p--;
			}
			fwrite(&triple, 3, 1, picfile); // write RGB triple to picfile
		}
	}
	// close files
	fclose(picfile);
	if (option == 1) fclose(txtfile);
	p--;
	while (p >= 0) {
		printf("*");	
		p--;	
	}
	printf("\n");
	return 0;
}

int decrypt (int argc, char* argv1, char* argv2 ) {
	decheck(argc, argv1, argv2 );
	int v;	
	
	RGBTRIPLE triple; // temporary storage

	fread(&triple.rgbtBlue, 1, 1, picfile); 
	fread(&triple.rgbtRed, 1, 1, picfile);	
	fread(&orfilesize, 4, 1, picfile);
	
	filesize = orfilesize;
	v = triple.rgbtBlue;
	//printf("The Filesize is: %ld\n Version is: %d\n" , orfilesize, v); DEBUG
	printf("\n\n Progress:                    :");
	printf("   \rProgress: ");
	for (int i = 0, biHeight = abs(bi.biHeight); i <= biHeight; i++)	 // iterate over picfile's scanlines
	{		
		for (int j = 0; j < bi.biWidth ; j++) // iterate over pixels in line
		{	//Decrypt! 		
			// read RGB triple from picfile 
			fread(&triple.rgbtBlue, 1, 1, picfile);
			fread(&triple.rgbtGreen, 1, 1, picfile);
			fread(&triple.rgbtRed, 1, 1, picfile);				
			//Show output in terminal (DEBUG)
			//if (orfilesize >= 1) printf("%c", triple.rgbtBlue - key);	
			//if (orfilesize >= 2) printf("%c", triple.rgbtGreen - key);
			//if (orfilesize >= 3) printf("%c", triple.rgbtRed - key);
					
			if (filesize >= 1) fprintf(txtfile, "%c", triple.rgbtBlue - key);	
			if (filesize >= 2) fprintf(txtfile, "%c", triple.rgbtGreen - key);
			if (filesize >= 3) fprintf(txtfile, "%c", triple.rgbtRed - key);
			filesize = filesize	- 3;	
						
			test = filesize * 20 / orfilesize; 
			if (test <= p) {
				printf("*");
				p--;
			}
		}
	}
	// close files
	fclose(picfile);
	if (option == 1) fclose(txtfile);
	
	while (p >= 0) {//If file was too short, print the rest of the '*'
		printf("*");	
		p--;	
	}
	printf("\n");
	return 0;
}


int encheck( int argc, char* argv1, char* argv2 ) {
	int w = 0, h = 0, flag = 3;
	
	option = 0;
	switch (argc) { //Not needed anymore, always will encrypt from inputfile
	case 3: 
		infile = argv2; //txt
		txtfile = fopen(infile, "r");
		if (txtfile == NULL) {
			printf("Could not open %s\n", argv2);	
			exit(0);
		}
		
		fseek(txtfile, 0L, SEEK_END);		
		filesize = ftell(txtfile);
		fseek(txtfile, 0L, SEEK_SET);
		orfilesize = filesize;
		
	option = 1; //txt does exist
	case 2: 
		outfile = argv1; //pic
		picfile = fopen(outfile, "r");
		if (picfile == NULL) {
			picfile = fopen(outfile, "w+");//Crating BMP file
			if (picfile == NULL) {
				printf("Could not create %s\n", argv1);	
				exit(0); 
			}
			else 
			{ //Create BMP! Writes the headerfile bytes
				fprintf(picfile,"%c%c%c%c%c%c%c%c",0x42,0x4d,0x38,0x10,0x0e,0x00,0x00,0x00);
				fprintf(picfile,"%c%c%c%c%c%c%c%c",0x00,0x00,0x36,0x00,0x00,0x00,0x28,0x00);

				fprintf(picfile,"%c%c%c%c%c%c%c%c",0x00,0x00,0x04,0x00,0x00,0x00,0x04,0x00);
				fprintf(picfile,"%c%c%c%c%c%c%c%c",0x00,0x00,0x01,0x00,0x18,0x00,0x00,0x00);

				fprintf(picfile,"%c%c%c%c%c%c%c%c",0x00,0x00,0x02,0x10,0x0e,0x00,0x12,0x0b);
				fprintf(picfile,"%c%c%c%c%c%c%c%c",0x00,0x00,0x12,0x0b,0x00,0x00,0x00,0x00);

				fprintf(picfile,"%c%c%c%c%c%c",0x00,0x00,0x00,0x00,0x00,0x00);
				fclose(picfile);
				picfile = fopen(outfile, "r");				
			}
		}		
		else {
			printf("\n Are you sure you want to encrypt into %s?", argv1);
			printf("\n It will overwrite the file PERMANENTLY\n Type \"1\" If you are sure: ");
			scanf("%d",&option);
			if (option != 1) {
				printf("\nOperation Aborted\n"); 
				exit(0);
			}	
		}
	}
	// read infile's BITMAPFILEHEADER 
	fread(&bf, sizeof(BITMAPFILEHEADER), 1, picfile);
	fread(&bi, sizeof(BITMAPINFOHEADER), 1, picfile);
	
	//DONE Change image size:
	filesize = filesize + 6; //To keep in mind the extra 2 pixels needed (6 bytes)
	filesize += (filesize % 3);//Add bytes to store it all in the image (groups of 3)
	while (w * h <= (filesize / 3)) { 
		if (flag == 3) {
		w = w + 4;
		flag = 0;		
		}
		else if (flag < 3) {
		h = h + 1;
		flag++;
		}
	} 
	bi.biWidth = w;
	bi.biHeight = h;
	// ensure infile is a 24-bit uncompressed BMP 4.0
	if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
		bi.biBitCount != 24 || bi.biCompression != 0)
	{
		if (option = 1) fclose(txtfile);
		fclose(picfile);
		printf("Unsupported file format!\n");
		exit(0);
	}
	fclose(picfile);
	picfile = fopen(outfile, "w");
}


int decheck( int argc, char* argv1, char* argv2 ) {
	option = 0;
	switch (argc) { //is the input file here?
	case 3: 
		outfile = argv2;
		txtfile = fopen(outfile, "r");
		if (txtfile == NULL) {
			txtfile = fopen(outfile, "w+");
			if (txtfile == NULL) {
				printf("Could not create %s\n", argv2);	
				exit(0);
			}		
		}
		else {
			printf("\n Are you sure you want to decrypt into %s?", argv2);
			printf("\n It will overwrite the file PERMANENTLY\n Type \"1\" If you are sure: ");
			scanf("%d",&option);
			if (option != 1) {
				printf("\nOperation Aborted\n"); 
				exit(0);
			}	
		}
	fclose(txtfile);
	txtfile = fopen(outfile, "w");
	option = 1;	
	case 2:
		infile = argv1;
		picfile = fopen(infile, "r");
		if (picfile == NULL) {
			printf("Could not open picfile\n");
			if (option = 1) fclose(txtfile);
			exit(0); 
		}	
	}
	// read infile's BITMAPFILEHEADER 
	fread(&bf, sizeof(BITMAPFILEHEADER), 1, picfile);
	fread(&bi, sizeof(BITMAPINFOHEADER), 1, picfile);

	// ensure infile is a 24-bit uncompressed BMP 4.0 (FROM BMP.h)
	if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
		bi.biBitCount != 24 || bi.biCompression != 0)
	{
		fclose(picfile);
		if (option = 1) fclose(txtfile);
		printf("Unsupported file format!\n");
		exit(0);
	}
}

int help(char* argv)
{
	printf("Picrypt: Encrypt and Decrypt files\nBy: Felipe Escoto\n");
	printf("\nBasic usage:\n Encrypt: %s [FILE].xyz\n Decrypt: %s [IMAGE].xyz.bmp", argv, argv);
	printf("\nAdvanced usage:\n %s [IMAGE].bmp [FILE].xyz\n", argv);
	return 0;
}
