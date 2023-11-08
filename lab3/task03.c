#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

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

status_codes validate_input(int argc, char** argv)
{
	if (argc != 4)
	{
		return INVALID_INPUT;
	}
	if ((argv[2][0] != '-' && argv[2][0] != '/') || argv[2][2] != '\0'
			|| (argv[2][1] != 'a' && argv[2][1] != 'd'))
	{
		return INVALID_FLAG;
	}
	return OK;
}

typedef struct
{
	ull id;
	char name[65];
	char surname[65];
	double salary;
} Employee;

int ascend_emp_comp(const void* l, const void* r)
{
	const Employee* emp_l = *((const Employee**) l);
	const Employee* emp_r = *((const Employee**) r);
	if (fabs(emp_l->salary - emp_r->salary) >= EPS)
	{
		return emp_l->salary < emp_r->salary ? -1 : 1;
	}
	ll name_cmp = strcmp(emp_l->name, emp_r->name);
	if (name_cmp)
	{
		return name_cmp;
	}
	ll surname_cmp = strcmp(emp_l->surname, emp_r->surname);
	if (surname_cmp)
	{
		return surname_cmp;
	}
	return emp_l->id < emp_r->id ? -1 : 1;
}

int descend_emp_comp(const void* l, const void* r)
{
	const Employee* emp_l = *((const Employee**) l);
	const Employee* emp_r = *((const Employee**) r);
	if (fabs(emp_l->salary - emp_r->salary) >= EPS)
	{
		return emp_l->salary > emp_r->salary ? -1 : 1;
	}
	ll name_cmp = strcmp(emp_l->name, emp_r->name);
	if (name_cmp)
	{
		return -name_cmp;
	}
	ll surname_cmp = strcmp(emp_l->surname, emp_r->surname);
	if (surname_cmp)
	{
		return -surname_cmp;
	}
	return emp_l->id > emp_r->id ? -1 : 1;
}

status_codes validate_name(const char* name)
{
	for (ull i = 0; name[i]; ++i)
	{
		if (!isalpha(name[i]))
		{
			return INVALID_INPUT;
		}
	}
	return OK;
}

status_codes read_employees(const char* path, ull* emp_cnt, Employee*** emps)
{
	if (path == NULL || emp_cnt == NULL || emps == NULL)
	{
		return INVALID_INPUT;
	}
	
	FILE* file = fopen(path, "r");
	if (file == NULL)
	{
		return FILE_OPENING_ERROR;
	}
	
	ull size = 2;
	ull cnt = 0;
	*emps = (Employee**) malloc(sizeof(Employee*) * size);
	if (*emps == NULL)
	{
		fclose(file);
		return BAD_ALLOC;
	}
	
	int error_flag = 0;
	status_codes error_code;
	while (!feof(file) && !error_flag)
	{
		ull id;
		char name[65];
		char surname[65];
		double salary;
		Employee* emp;
		
		ll read_cnt = fscanf(file, " %llu %64[^ ] %64[^ ] %lf", &id, name, surname, &salary);
		
		if (read_cnt != 4)
		{
			error_flag = 1;
			error_code = FILE_CONTENT_ERROR;
		}
		if (validate_name(name) || validate_name(surname))
		{
			return FILE_CONTENT_ERROR;
		}
		
		if (!error_flag)
		{
			emp = (Employee*) malloc(sizeof(Employee));
			if (emp == NULL)
			{
				error_flag = 1;
				error_code = BAD_ALLOC;
			}
			emp->id = id;
			strcpy(emp->name, name);
			strcpy(emp->surname, surname);
			emp->salary = salary;
		}
		
		if (!error_flag && cnt == size)
		{
			size *= 2;
			Employee** tmp = (Employee**) realloc(*emps, sizeof(Employee*) * size);
			if (tmp == NULL)
			{
				error_flag = 1;
				error_code = BAD_ALLOC;
			}
			else
			{
				*emps = tmp;
			}
		}
		
		if (!error_flag)
		{
			(*emps)[cnt++] = emp;
		}
	}
	
	fclose(file);
	
	if (!error_flag)
	{
		Employee** tmp = (Employee**) realloc(*emps, sizeof(Employee*) * cnt);
		if (tmp == NULL)
		{
			error_flag = 1;
			error_code = BAD_ALLOC;
		}
		else
		{
			*emps = tmp;
		}
	}
	
	if (error_flag)
	{
		for (ull i = 0; i < cnt; ++i)
		{
			free((*emps)[i]);
		}
		free(*emps);
		return error_code;
	}
	
	*emp_cnt = cnt;
	return OK;
}

status_codes write_employees(const char* path, ull emp_cnt, Employee** emps)
{
	if (path == NULL)
	{
		return INVALID_INPUT;
	}
	
	FILE* file = fopen(path, "w");
	if (file == NULL)
	{
		return FILE_OPENING_ERROR;
	}
	
	for (ull i = 0; i < emp_cnt; ++i)
	{
		const Employee* emp = emps[i];
		fprintf(file, "%llu %s %s %lf", emp->id, emp->name, emp->surname, emp->salary);
		if (i + 1 < emp_cnt)
		{
			fprintf(file, "\n");
		}
	}
	fclose(file);
	return OK;
}

int main(int argc, char** argv)
{
	if (argc == 1)
	{
		printf("Usage: <input> <flag> <output>\n");
		return OK;
	}
	
	status_codes validate_code = validate_input(argc, argv);
	if (validate_code != OK)
	{
		print_error(validate_code);
		return validate_code;
	}
	
	ull emp_cnt;
	Employee** emps;
	status_codes code = read_employees(argv[1], &emp_cnt, &emps);
	if (code)
	{
		print_error(code);
		return code;
	}
	
	char flag = argv[2][1];
	if (flag == 'a')
	{
		qsort(emps, emp_cnt, sizeof(Employee*), ascend_emp_comp);
	}
	else if (flag == 'd')
	{
		qsort(emps, emp_cnt, sizeof(Employee*), descend_emp_comp);
	}
	
	code = write_employees(argv[3], emp_cnt, emps);
	
	for (ull i = 0; i < emp_cnt; ++i)
	{
		free(emps[i]);
	}
	free(emps);
	
	if (code)
	{
		print_error(code);
		return code;
	}
}