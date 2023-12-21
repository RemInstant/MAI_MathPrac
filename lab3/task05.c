#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#define EPS 1e-12

typedef long long ll;
typedef unsigned long long ull;

typedef enum
{
	OK,
	INVALID_INPUT,
	INVALID_FLAG,
	INVALID_NUMBER,
	FILE_OPENING_ERROR,
	FILE_CONTENT_ERROR,
	OVERFLOW,
	BAD_ALLOC
} status_codes;

void print_error(status_codes code)
{
	switch (code)
	{
		case OK:
			return;
		case INVALID_INPUT:
			printf("Invalid input\n");
			return;
		case INVALID_FLAG:
			printf("Invalid flag\n");
			return;
		case INVALID_NUMBER:
			printf("Invalid number\n");
			return;
		case FILE_OPENING_ERROR:
			printf("File cannot be opened\n");
			return;
		case FILE_CONTENT_ERROR:
			printf("Invalid content of file\n");
			return;
		case OVERFLOW:
			printf("An overflow occurred\n");
			return;
		case BAD_ALLOC:
			printf("Memory lack error occurred\n");
			return;
		default:
			printf("Unexpected error occurred\n");
			return;
	}
}

typedef struct
{
	ull id;
	char name[33];
	char surname[33];
	char group[17];
	unsigned short* marks;
} Student;

typedef enum
{
	NON_VOID_STRING,
	NON_VOID_ALPHA_STRING,
	UNSIGNED_STRING
} validation_type;

status_codes validate_string(char* str, validation_type type, ll char_n);
status_codes read_line(char** line);
status_codes read_students(FILE* file, ull* studs_cnt, Student** studs);

typedef enum
{
	STUD_NAME,
	STUD_SURNAME,
	STUD_GROUP
} search_type;

status_codes search_stud_by_id(ull studs_cnt, Student* studs, ull id, Student** found_stud);
status_codes search_stud_by_str(ull s_cnt, Student* studs, search_type type, const char* surname, ull* found_cnt, Student** found);

int stud_id_comp(const void* l, const void* r);
int stud_surname_comp(const void* l, const void* r);
int stud_name_comp(const void* l, const void* r);
int stud_group_comp(const void* l, const void* r);

status_codes print_studs(ull studs_cnt, const Student* studs);
status_codes fprint_stud_data(FILE* file, const Student* stud);
status_codes fprint_best_studs(FILE* file, ull studs_cnt, const Student* studs);

int main(int argc, char** argv)
{
	if (argc == 1)
	{
		printf("Usage: cmd_path <input> <output>\n");
		return OK;
	}
	
	if (argc != 3 || !strcmp(argv[1], argv[2]))
	{
		print_error(INVALID_INPUT);
		return INVALID_INPUT;
	}
	
	FILE* input = fopen(argv[1], "r");
	if (input == NULL)
	{
		print_error(FILE_OPENING_ERROR);
		return FILE_OPENING_ERROR;
	}
	
	FILE* output = fopen(argv[2], "w");
	if (output == NULL)
	{
		fclose(input);
		print_error(FILE_OPENING_ERROR);
		return FILE_OPENING_ERROR;
	}
	
	status_codes code;
	ull studs_cnt;
	Student* studs;
	
	code = read_students(input, &studs_cnt, &studs);
	fclose(input);
	if (code)
	{
		fclose(output);
		print_error(code);
		return code;
	}
	
	int run_flag = 1;
	printf("Cmds: exit sort_id sort_name sort_surname sort_group print_id print_name print_surname print_group fprint_stud fprint_best\n");
	while (run_flag && !code)
	{
		status_codes cmd_code = OK;
		ull studs_print_cnt = 0;
		Student* studs_print = NULL;
		Student* stud_print = NULL;
		char* cmd = NULL;
		printf("Cmd: ");
		code = read_line(&cmd);
		
		if (!code)
		{
			// --- COMMAND EXIT ---
			if (!strcmp(cmd, "exit"))
			{
				run_flag = 0;
				printf("Program is closed\n");
			}
			// --- COMMAND SORT BY ID ---
			else if (!strcmp(cmd, "sort_id"))
			{
				qsort(studs, studs_cnt, sizeof(Student), stud_id_comp);
				printf("Students are sorted\n");
			}
			// --- COMMAND SORT BY NAME ---
			else if (!strcmp(cmd, "sort_name"))
			{
				qsort(studs, studs_cnt, sizeof(Student), stud_name_comp);
				printf("Students are sorted\n");
			}
			// --- COMMAND SORT BY SURNAME ---
			else if (!strcmp(cmd, "sort_surname"))
			{
				qsort(studs, studs_cnt, sizeof(Student), stud_surname_comp);
				printf("Students are sorted\n");
			}
			// --- COMMAND SORT BY GROUP ---
			else if (!strcmp(cmd, "sort_group"))
			{
				qsort(studs, studs_cnt, sizeof(Student), stud_group_comp);
				printf("Students are sorted\n");
			}
			// --- COMMAND PRINT STUDS BY ID ---
			else if (!strcmp(cmd, "print_id"))
			{
				char* id_str = NULL;
				ull id = 0;
				printf("Enter id: ");
				cmd_code = read_line(&id_str);
				cmd_code = cmd_code ? cmd_code : validate_string(id_str, UNSIGNED_STRING, -1);
				id = cmd_code ? 0 : strtoull(id_str, NULL, 10);
				cmd_code = cmd_code ? cmd_code : search_stud_by_id(studs_cnt, studs, id, &stud_print);
				cmd_code = cmd_code ? cmd_code : print_studs(1, stud_print);
				free(id_str);
			}
			// --- COMMAND PRINT STUDS BY NAME ---
			else if (!strcmp(cmd, "print_name"))
			{
				char* name = NULL;
				printf("Enter name: ");
				cmd_code = read_line(&name);
				cmd_code = cmd_code ? cmd_code : validate_string(name, NON_VOID_ALPHA_STRING, -1);
				cmd_code = cmd_code ? cmd_code :
						search_stud_by_str(studs_cnt, studs, STUD_NAME, name, &studs_print_cnt, &studs_print);
				cmd_code = cmd_code ? cmd_code : print_studs(studs_print_cnt, studs_print);
				free(name);
			}
			// --- COMMAND PRINT STUDS BY SURNAME ---
			else if (!strcmp(cmd, "print_surname"))
			{
				char* surname = NULL;
				printf("Enter surname: ");
				cmd_code = read_line(&surname);
				cmd_code = cmd_code ? cmd_code : validate_string(surname, NON_VOID_ALPHA_STRING, -1);
				cmd_code = cmd_code ? cmd_code :
						search_stud_by_str(studs_cnt, studs, STUD_SURNAME, surname, &studs_print_cnt, &studs_print);
				cmd_code = cmd_code ? cmd_code : print_studs(studs_print_cnt, studs_print);
				free(surname);
			}
			// --- COMMAND PRINT STUDS BY GROUP ---
			else if (!strcmp(cmd, "print_group"))
			{
				char* group = NULL;
				printf("Enter group: ");
				cmd_code = read_line(&group);
				cmd_code = cmd_code ? cmd_code : validate_string(group, NON_VOID_STRING, -1);
				cmd_code = cmd_code ? cmd_code :
						search_stud_by_str(studs_cnt, studs, STUD_GROUP, group, &studs_print_cnt, &studs_print);
				cmd_code = cmd_code ? cmd_code : print_studs(studs_print_cnt, studs_print);
				free(group);
			}
			// --- COMMAND FPRINT STUD BY ID ---
			else if (!strcmp(cmd, "fprint_stud"))
			{
				char* id_str = NULL;
				ull id = 0;
				printf("Enter id: ");
				cmd_code = read_line(&id_str);
				cmd_code = cmd_code ? cmd_code : validate_string(id_str, UNSIGNED_STRING, -1);
				id = cmd_code ? 0 : strtoull(id_str, NULL, 10);
				cmd_code = cmd_code ? cmd_code : search_stud_by_id(studs_cnt, studs, id, &stud_print);
				if (stud_print == NULL)
				{
					printf("There is no student with this ID\n");
				}
				else
				{
					cmd_code = cmd_code ? cmd_code : fprint_stud_data(output, stud_print);
					if (!cmd_code)
					{
						printf("Stud is printed into file\n");
					}
				}
				free(id_str);
			}
			// --- COMMAND FPRINT BEST STUDS ---
			else if (!strcmp(cmd, "fprint_best"))
			{
				cmd_code = cmd_code ? cmd_code : fprint_best_studs(output, studs_cnt, studs);
				if (!cmd_code)
				{
					printf("Studs are printed into file\n");
				}
			}
			// --- INVALID COMMAND ---
			else
			{
				printf("Invalid command\n");
			}
			
			if (cmd_code)
			{
				print_error(cmd_code);
			}
		}
		free(cmd);
		free(studs_print);
		printf("\n");
	}
	
	fclose(output);
	for (ull i = 0; i < studs_cnt; ++i)
	{
		free(studs[i].marks);
	}
	free(studs);
	if (code)
	{
		printf("CRITICAL ERROR OCCURRED\n");
		print_error(code);
		return code;
	}
}

status_codes validate_string(char* str, validation_type type, ll char_n)
{
	if (str == NULL)
	{
		return INVALID_INPUT;
	}
	if (char_n != -1 && strlen(str) != char_n)
	{
		return INVALID_INPUT;
	}
	switch (type)
	{
		case NON_VOID_STRING:
		{
			return str[0] ? OK : INVALID_INPUT;
		}
		case NON_VOID_ALPHA_STRING:
		{
			if (!str[0])
			{
				return INVALID_INPUT;
			}
			for (ull i = 0; str[i]; ++i)
			{
				if (!isalpha(str[i]))
				{
					return INVALID_INPUT;
				}
			}
			return OK;
		}
		case UNSIGNED_STRING:
		{
			errno = 0;
			char* ptr;
			strtoull(str, &ptr, 10);
			if (errno == ERANGE)
			{
				return OVERFLOW;
			}
			if (*ptr != '\0')
			{
				return INVALID_INPUT;
			}
			return OK;
		}
	}
	return OK;
}

status_codes read_line(char** line)
{
	if (line == NULL)
	{
		return INVALID_INPUT;
	}
	ull iter = 0;
	ull size = 2;
	*line = (char*) malloc(sizeof(char) * size);
	if (*line == NULL)
	{
		return BAD_ALLOC;
	}
	char ch = getchar();
	while (ch != '\n')
	{
		if (iter > size - 2)
		{
			size *= 2;
			char* temp_line = realloc(*line, size);
			if (temp_line == NULL)
			{
				free(*line);
				return BAD_ALLOC;
			}
			*line = temp_line;
		}
		(*line)[iter++] = ch;
		ch = getchar();
	}
	(*line)[iter] = '\0';
	return OK;
}

status_codes read_students(FILE* file, ull* studs_cnt, Student** studs)
{
	if (file == NULL || studs_cnt == NULL || studs == NULL)
	{
		return INVALID_INPUT;
	}
	
	ull size = 2;
	ull cnt = 0;
	Student* studs_tmp = (Student*) malloc(sizeof(Student) * 2);
	if (studs == NULL)
	{
		return BAD_ALLOC;
	}
	
	status_codes code = OK;
	while (!code && !feof(file))
	{
		Student stud;
		stud.marks = (unsigned short*) malloc(sizeof(unsigned short) * 5);
		if (stud.marks == NULL)
		{
			code = BAD_ALLOC;
		}
		// READ
		if (!code)
		{
			if (fscanf(file, "%llu %32[^ ] %32[^ ] %16[^ ] %hu %hu %hu %hu %hu\n",
					&(stud.id), stud.name, stud.surname, stud.group, &(stud.marks[0]),
					&(stud.marks[1]), &(stud.marks[2]), &(stud.marks[3]), &(stud.marks[4])) != 9)
			{
				code = FILE_CONTENT_ERROR;
			}
			code = code ? code : validate_string(stud.name, NON_VOID_ALPHA_STRING, -1);
			code = code ? code : validate_string(stud.surname, NON_VOID_ALPHA_STRING, -1);
			code = code ? code : validate_string(stud.group, NON_VOID_STRING, -1);
		}
		// REALLOC
		if (!code)
		{
			if (cnt == size)
			{
				size *= 2;
				Student* tmp = (Student*) realloc(studs_tmp, sizeof(Student) * size);
				if (tmp == NULL)
				{
					code = BAD_ALLOC;
				}
				else
				{
					studs_tmp = tmp;
				}
			}
		}
		// INSERT
		if (!code)
		{
			studs_tmp[cnt++] = stud;
		}
	}
	if (code)
	{
		for (ull i = 0; i < cnt; ++i)
		{
			free(studs_tmp[i].marks);
		}
		free(studs_tmp);
		return code;
	}
	*studs_cnt = cnt;
	*studs = studs_tmp;
	return OK;
}

status_codes search_stud_by_id(ull studs_cnt, Student* studs, ull id, Student** found_stud)
{
	if (studs == NULL || found_stud == NULL)
	{
		return INVALID_INPUT;
	}
	for (ull i = 0; i < studs_cnt; ++i)
	{
		if (studs[i].id == id)
		{
			*found_stud = &(studs[i]);
			return OK;
		}
	}
	*found_stud = NULL;
	return OK;
}

status_codes search_stud_by_str(ull s_cnt, Student* studs, search_type type, const char* str, ull* found_cnt, Student** found)
{
	if (studs == NULL || found == NULL)
	{
		return INVALID_INPUT;
	}
	ull size = 2;
	ull cnt = 0;
	Student* studs_tmp = (Student*) malloc(sizeof(Student) * 2);
	if (studs_tmp == NULL)
	{
		return BAD_ALLOC;
	}
	for (ull i = 0; i < s_cnt; ++i)
	{
		if ((type == STUD_NAME && !strcmp(studs[i].name, str))
				|| (type == STUD_SURNAME && !strcmp(studs[i].surname, str))
				|| (type == STUD_GROUP && !strcmp(studs[i].group, str)))
		{
			if (cnt == size)
			{
				size *= 2;
				Student* tmp = (Student*) realloc(studs_tmp, sizeof(Student) * size);
				if (tmp == NULL)
				{
					free(studs_tmp);
					return BAD_ALLOC;
				}
				studs_tmp = tmp;
			}
			studs_tmp[cnt++] = studs[i];
		}
	}
	*found_cnt = cnt;
	*found = studs_tmp;
	return OK;
}

int stud_id_comp(const void* l, const void* r)
{
	const Student* stud_l = (const Student*) l;
	const Student* stud_r = (const Student*) r;
	if (stud_l->id == stud_r->id)
	{
		return 0;
	}
	return stud_l->id < stud_r->id ? -1 : 1;
}

int stud_surname_comp(const void* l, const void* r)
{
	const Student* stud_l = (const Student*) l;
	const Student* stud_r = (const Student*) r;
	return strcmp(stud_l->surname, stud_r->surname);
}

int stud_name_comp(const void* l, const void* r)
{
	const Student* stud_l = (const Student*) l;
	const Student* stud_r = (const Student*) r;
	return strcmp(stud_l->name, stud_r->name);
}

int stud_group_comp(const void* l, const void* r)
{
	const Student* stud_l = (const Student*) l;
	const Student* stud_r = (const Student*) r;
	return strcmp(stud_l->group, stud_r->group);
}

status_codes print_studs(ull studs_cnt, const Student* studs)
{
	if (studs == NULL)
	{
		return INVALID_INPUT;
	}
	
	if (studs_cnt == 0)
	{
		printf("No students are found\n");
		return OK;
	}
	
	printf("|   ID   |               NAME               |              SURNAME             |       GROUP      |   MARKS   |\n");
	for (ull i = 0; i < studs_cnt; ++i)
	{
		const Student* s = &(studs[i]);
		printf("| %-6llu | %-32s | %-32s | %-16s | %d %d %d %d %d |\n", s->id, s->name, s->surname, s->group,
				s->marks[0], s->marks[1], s->marks[2], s->marks[3], s->marks[4]);
	}
	return OK;
}

status_codes fprint_stud_data(FILE* file, const Student* stud)
{
	if (file == NULL || stud == NULL)
	{
		return INVALID_INPUT;
	}
	double avg = (stud->marks[0] + stud->marks[1] + stud->marks[2] + stud->marks[3] + stud->marks[4]) / 5.0;
	fprintf(file, "Executed cmd: fprint_stud\n");
	fprintf(file, "ID: %llu\n", stud->id);
	fprintf(file, "Name: %s\n", stud->name);
	fprintf(file, "Surname: %s\n", stud->surname);
	fprintf(file, "Group: %s\n", stud->group);
	fprintf(file, "Avg mark: %.2lf\n\n", avg);
	fflush(file);
	return OK;
}

status_codes fprint_best_studs(FILE* file, ull studs_cnt, const Student* studs)
{
	if (file == NULL || studs == NULL)
	{
		return INVALID_INPUT;
	}
	
	double all_avg = 0;
	for (ull i = 0; i < studs_cnt; ++i)
	{
		for(ull j = 0; j < 5; ++j)
		{
			all_avg += studs[i].marks[j];
		}
	}
	all_avg /= studs_cnt * 5;
	
	fprintf(file, "Executed cmd: fprint_best\n");
	fprintf(file, "|               NAME               |              SURNAME             |\n");
	for (ull i = 0; i < studs_cnt; ++i)
	{
		const Student* s = &(studs[i]);
		double avg = 0;
		for(ull j = 0; j < 5; ++j)
		{
			avg += s->marks[j];
		}
		avg /= 5;
		if (avg - all_avg >= EPS)
		{
			fprintf(file, "| %-32s | %-32s |\n", s->name, s->surname);
		}
	}
	fprintf(file, "\n");
	fflush(file);
	return OK;
}