#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

int main(int argc, char** argv) {
	if (argc == 1) {
		// TODO
		printf("Usage: command_name <flag> <input file>\n");
		printf("flags:\n");
		printf("-[n]d  -  exclude Arabic numerals from file\n");
		printf("-[n]i  -  count occurrence of Latin letters for each line\n");
		printf("-[n]s  -  count occurrence of characters other than Latin letters,");
		printf(" Arabic numerals and spaces for each line\n");
		printf("-[n]a  -  change characters other than Arabic numerals with their hexadecimal ASCII code\n");
		printf("The result is written to a file.\n");
		printf("If flag -n defined the output file name must be the third argument of command.");
		printf(" Otherwise, its name is concatenation of prefix \"out_\" and input file name\n");
		return 0;
	}
	
	char* flag = argv[1];
	char main_flag = ' ';
	int output_flag = 0;
	
	if (flag[0] == '-' || flag[0] == '/') {
		if (flag[2] == '\0') {
			main_flag = flag[1];
		} else if (flag[3] == '\0' && flag[1] == 'n') {
			main_flag = flag[2];
			output_flag = 1;
		}
	}
	
	if (argc != (3 + output_flag)) {
		printf("Invalid input\n");
		return 1;
	}
	
	if (main_flag != 'd' && main_flag != 'i' && main_flag != 's' && main_flag != 'a') {
		printf("Invalid flag\n");
		return 2;
	}
	
	char* input_file_name = argv[2];
	char* output_file_name;
	
	if(output_flag) {
		output_file_name = argv[3];
	} else {
		int len = strlen(input_file_name);
		output_file_name = malloc(sizeof(char) * (len + 4));
		sprintf(output_file_name, "%s%s", "out_", input_file_name);
	}
	
	FILE* input = fopen(input_file_name, "r");
	FILE* output = fopen(output_file_name, "w");
	
	switch (main_flag) {
		case 'd': {
			char ch;
			while ((ch = fgetc(input)) != EOF) {
				if (isdigit(ch)) continue;
				fputc(ch, output);
			}
			break;
		}
		
		case 'i': {
			char ch;
			unsigned int counter = 0;
			
			while (1) {
				ch = fgetc(input);
				
				if (ch == '\n') {
					fprintf(output, "%u\n", counter);
					counter = 0;
					continue;
				}
				
				if (ch == EOF) {
					fprintf(output, "%u", counter);
					break;
				}
				
				if (isalpha(ch)) {

					if (counter > UINT_MAX - 1) {
						fprintf(output, "overflow\n");
						counter = 0;
						
						while (ch != EOF && ch != '\n')
							ch = fgetc(input);
						ch = fgetc(input);
						
						continue;
					}
					
					++counter;
				}
			}
			break;
		}
		
		case 's': {
			char ch;
			unsigned int counter = 0;
			
			while (1) {
				ch = fgetc(input);
				
				if (ch == '\n') {
					fprintf(output, "%u\n", counter);
					counter = 0;
					continue;
				}
				
				if (ch == EOF) {
					fprintf(output, "%u", counter);
					break;
				}
				
				if (isalpha(ch) || isdigit(ch) || ch == ' ') continue;
					
				if (counter > UINT_MAX - 1) {
					fprintf(output, "overflow\n");
					counter = 0;
					
					while (ch != EOF && ch != '\n')
						ch = fgetc(input);
					ch = fgetc(input);
					
					continue;
				}
				
				++counter;
			}
			break;
		}
		
		case 'a': {
			char ch;
			while ((ch = fgetc(input)) != EOF) {
				if (isdigit(ch)) {
					fputc(ch, output);
					continue;
				}
				printf("%X", ch);
				fprintf(output, "%X", ch);
			}
			break;
		}
	}
	
	fclose(input);
	fclose(output);
	
	if(!output_flag) {
		free(output_file_name);
	}
}