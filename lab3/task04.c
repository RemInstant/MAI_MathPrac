#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <time.h>

typedef long long ll;
typedef unsigned long long ull;

typedef enum
{
	OK,
	INVALID_INPUT,
	INVALID_FLAG,
	INVALID_NUMBER,
	UNAVAILABLE_ID,
	FILE_OPENING_ERROR,
	FILE_CONTENT_ERROR,
	OVERFLOW,
	BAD_ALLOC,
	BAD_SEARCH
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
		case UNAVAILABLE_ID:
			printf("Unavailable data access\n");
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
		case BAD_SEARCH:
			printf("No required data were found\n");
			return;
		default:
			printf("Unexpected error occurred\n");
			return;
	}
}

int is_separator(char ch)
{
	return ch == ' ' || ch == '\t' || ch == '\n';
}

int is_leap_year(int year)
{
	return year % 400 == 0 && year % 100 != 0 && year % 4 == 0;
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

typedef enum
{
	NON_VOID_STRING,
	DIGIT_STRING,
	INTEGER_STRING,
	UNSIGNED_STRING,
	FLOAT_STRING,
	LONG_TIME_STRING
} validation_type;

status_codes validate_string(const char* str, validation_type type, ll char_n)
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
		case DIGIT_STRING:
		{
			for (ull i = 0; str[i]; ++i)
			{
				if (!isdigit(str[i]))
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
		case INTEGER_STRING:
		{
			errno = 0;
			char* ptr;
			strtoll(str, &ptr, 10);
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
		case FLOAT_STRING:
		{
			errno = 0;
			char* ptr;
			strtod(str, &ptr);
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
		case LONG_TIME_STRING:
		{
			if (strlen(str) != 19)
			{
				return INVALID_INPUT;
			}
			for (ull i = 0; str[i]; ++i)
			{
				if (!isdigit(str[i]) && i != 2 && i != 5 && i != 10 && i != 13 && i != 16)
				{
					return INVALID_INPUT;
				}
			}
			if (str[2] != ':' || str[5] != ':' || str[10] != ' ' || str[13] != ':' || str[16] != ':')
			{
				return INVALID_INPUT;
			}
			
			char str_day[20], str_month[20], str_year[20], str_hour[20];
			strcpy(str_day, str);
			strcpy(str_month, str + 3);
			strcpy(str_year, str + 6);
			strcpy(str_hour, str + 11);
			str_day[2] = str_month[2] = str_year[4] = str_hour[2] = '\0';
			
			int day = atoi(str_day);
			int month = atoi(str_month);
			int year = atoi(str_year);
			int hour = atoi(str_hour);
			
			char month_content[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
			char leap_month_content[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
			
			if (year <= 1970 || year > 3000 || month > 11 || hour > 23 || str[14] > '5' || str[17] > '5')
			{
				return INVALID_INPUT;
			}
			if (is_leap_year(year) && day > leap_month_content[month])
			{
				return INVALID_INPUT;
			}
			if (!is_leap_year(year) && day > month_content[month])
			{
				return INVALID_INPUT;
			}
			return OK;
		}
	}
	return OK;
}

// PART 1

typedef struct
{
	char* data;
	int len;
} String;

status_codes construct_string(String* str, const char* other)
{
	if(str == NULL)
	{
		return INVALID_INPUT;
	}
	if (other == NULL)
	{
		str->len = 0;
		str->data = NULL;
		return OK;
	}
	str->len = strlen(other);
	str->data = (char*) malloc(sizeof(char) * (str->len + 1));
	if (str->data == NULL)
	{
		return BAD_ALLOC;
	}
	strcpy(str->data, other);
	return OK;
}

status_codes destruct_string(String* str)
{
	if (str == NULL)
	{
		return INVALID_INPUT;
	}
	free(str->data);
	str->data = NULL;
	str->len = 0;
	return OK;
}

int compare_string(const String left, const String right)
{
	if (left.len != right.len)
	{
		return left.len < right.len ? -1 : 1;
	}
	return strcmp(left.data, right.data);
}

int equal_string(const String left, const String right)
{
	if (left.len != right.len)
	{
		return 0;
	}
	return !strcmp(left.data, right.data);
}

status_codes copy_string(String* dest, const String src)
{
	if (dest == NULL)
	{
		return INVALID_INPUT;
	}
	if (dest->len < src.len)
	{
		char* tmp = (char*) realloc(dest->data, sizeof(char) * (src.len + 1));
		if (tmp == NULL)
		{
			return BAD_ALLOC;
		}
		dest->data = tmp;
	}
	strcpy(dest->data, src.data);
	dest->len = src.len;
	return OK;
}

// OR CONSTRUCT COPY????
status_codes dyn_copy_string(String** dest, const String src)
{
	if (dest == NULL)
	{
		return INVALID_INPUT;
	}
	*dest = (String*) malloc(sizeof(String));
	if (*dest == NULL)
	{
		return BAD_ALLOC;
	}
	(*dest)->len = src.len;
	(*dest)->data = (char*) malloc(sizeof(char) * (src.len + 1));
	if ((*dest)->data == NULL)
	{
		free(*dest);
		return BAD_ALLOC;
	}
	strcpy((*dest)->data, src.data);
	return OK;
}

status_codes concat_string(String* dest, const String src)
{
	char* tmp = (char*) realloc(dest->data, sizeof(char) * (dest->len + src.len + 1));
	if (tmp == NULL)
	{
		return BAD_ALLOC;
	}
	dest->len = dest->len + src.len;
	dest->data = tmp;
	strcat(dest->data, src.data);
	return OK;
}

status_codes read_string_line(String* str_line)
{
	if (str_line == NULL)
	{
		return INVALID_INPUT;
	}
	ull iter = 0;
	ull size = 2;
	char* line = (char*) malloc(sizeof(char) * size);
	if (line == NULL)
	{
		return BAD_ALLOC;
	}
	char ch = getchar();
	while (ch != '\n')
	{
		if (iter > size - 2)
		{
			size *= 2;
			char* temp_line = realloc(line, size);
			if (temp_line == NULL)
			{
				free(line);
				return BAD_ALLOC;
			}
			line = temp_line;
		}
		line[iter++] = ch;
		ch = getchar();
	}
	(line)[iter] = '\0';
	str_line->len = iter;
	str_line->data = line;
	return OK;
}

status_codes get_cur_long_time(String* str_time)
{
	char tmp[20];
	time_t cur_time = time(NULL);
	struct tm *cur_tm = localtime(&cur_time);
	sprintf(tmp, "%02d:%02d:%04d %02d:%02d:%02d", cur_tm->tm_mday, cur_tm->tm_mon + 1,
			cur_tm->tm_year + 1900, cur_tm->tm_hour, cur_tm->tm_min, cur_tm->tm_sec);
	return construct_string(str_time, tmp);
}

// PART 2

typedef struct
{
	String city;
	String street;
	ull house_number;
	String building;
	ull apartment_number;
	String receiver_id;
} Address;

typedef struct
{
	Address receiver_addr;
	double weight;
	String mail_id;
	String creation_time;
	String delivery_time;
	int is_delivered;
} Mail;

typedef struct
{
	Address office_addr;
	ull mail_size;
	ull mail_cnt;
	Mail* mail_arr;
} Post;

status_codes destruct_address(Address* addr)
{
	if (addr == NULL)
	{
		return INVALID_INPUT;
	}
	destruct_string(&(addr->city));
	destruct_string(&(addr->street));
	destruct_string(&(addr->building));
	destruct_string(&(addr->receiver_id));
	addr->house_number = 0;
	addr->apartment_number = 0;
	return OK;
}

status_codes read_address(Address* addr)
{
	if (addr == NULL)
	{
		return INVALID_INPUT;
	}
	
	status_codes err_code = OK;
	char* city = NULL;
	char* street = NULL;
	char* house_n_str = NULL;
	char* building = NULL;
	char* apartment_n_str = NULL;
	char* receiver_id = NULL;
	construct_string(&(addr->city), NULL);
	construct_string(&(addr->street), NULL);
	construct_string(&(addr->building), NULL);
	construct_string(&(addr->receiver_id), NULL);
	
	// READ CHAR*
	if (!err_code)
	{
		printf("Enter the city: ");
		err_code = read_line(&city);
	}
	if (!err_code)
	{
		err_code = validate_string(city, NON_VOID_STRING, -1);
	}
	if (!err_code)
	{
		printf("Enter the street: ");
		err_code = read_line(&street);
	}
	if (!err_code)
	{
		err_code = validate_string(street, NON_VOID_STRING, -1);
	}
	if (!err_code)
	{
		printf("Enter the number of the house: ");
		err_code = read_line(&house_n_str);
	}
	if (!err_code)
	{
		err_code = validate_string(house_n_str, UNSIGNED_STRING, -1);
	}
	if (!err_code && atoi(house_n_str) == 0)
	{
		err_code = INVALID_INPUT;
	}
	if (!err_code)
	{
		printf("Enter the building: ");
		err_code = read_line(&building);
	}
	if (!err_code)
	{
		printf("Enter the number of the apartment: ");
		err_code = read_line(&apartment_n_str);
	}
	if (!err_code)
	{
		err_code = validate_string(apartment_n_str, UNSIGNED_STRING, -1);
	}
	if (!err_code && atoi(apartment_n_str) == 0)
	{
		err_code = INVALID_INPUT;
	}
	if (!err_code)
	{
		printf("Enter receiver ID: ");
		err_code = read_line(&receiver_id);
	}
	if (!err_code)
	{
		err_code = validate_string(receiver_id, DIGIT_STRING, 6);
	}
	// CONSTRUCT STRINGS
	if (!err_code)
	{
		err_code = construct_string(&(addr->city), city);
	}
	if (!err_code)
	{
		err_code = construct_string(&(addr->street), street);
	}
	if (!err_code)
	{
		err_code = construct_string(&(addr->building), building);
	}
	if (!err_code)
	{
		err_code = construct_string(&(addr->receiver_id), receiver_id);
	}
	// SET NUMBERS OF ADDRESS
	if (!err_code)
	{
		addr->house_number = atoi(house_n_str);
		addr->apartment_number = atoi(apartment_n_str);
	}
	
	free(city);
	free(street);
	free(house_n_str);
	free(building);
	free(apartment_n_str);
	free(receiver_id);
	
	if (err_code)
	{
		destruct_address(addr);
		return err_code;
	}
	return OK;
}

status_codes destruct_mail(Mail* mail)
{
	if (mail == NULL)
	{
		return INVALID_INPUT;
	}
	destruct_address(&(mail->receiver_addr));
	destruct_string(&(mail->mail_id));
	destruct_string(&(mail->creation_time));
	destruct_string(&(mail->delivery_time));
	mail->weight = 0;
	mail->is_delivered = 0;
	return OK;
}

status_codes read_mail(Mail* mail)
{
	if (mail == NULL)
	{
		return INVALID_INPUT;
	}
	
	status_codes err_code = OK;
	char* weight_str = NULL;
	char* mail_id = NULL;
	char* delivery_time = NULL;
	construct_string(&(mail->mail_id), NULL);
	construct_string(&(mail->creation_time), NULL);
	construct_string(&(mail->delivery_time), NULL);

	// READ ADDRESS
	err_code = read_address(&(mail->receiver_addr));
	
	// READ CHAR*
	if (!err_code)
	{
		printf("Enter the package weight: ");
		err_code = read_line(&weight_str);
	}
	if (!err_code)
	{
		err_code = validate_string(weight_str, FLOAT_STRING, -1);
	}
	if (!err_code && atof(weight_str) < 0)
	{
		err_code = INVALID_INPUT;
	}
	if (!err_code)
	{
		printf("Enter the mail ID: ");
		err_code = read_line(&mail_id);
	}
	if (!err_code)
	{
		err_code = validate_string(mail_id, DIGIT_STRING, 14);
	}
	if (!err_code)
	{
		printf("Enter the time of delivery (dd:MM:yyyy hh:mm:ss): ");
		err_code = read_line(&delivery_time);
	}
	if (!err_code)
	{
		err_code = validate_string(delivery_time, LONG_TIME_STRING, -1);
	}
	// CONSTRUCT STRINGS
	if (!err_code)
	{
		err_code = construct_string(&(mail->mail_id), mail_id);
	}
	if (!err_code)
	{
		err_code = get_cur_long_time(&(mail->creation_time));
	}
	if (!err_code)
	{
		err_code = construct_string(&(mail->delivery_time), delivery_time);
	}
	// SET WEIGHT ANS STATUS
	if (!err_code)
	{
		mail->weight = atof(weight_str);
		mail->is_delivered = 0;
	}
	
	free(weight_str);
	free(mail_id);
	free(delivery_time);
	
	if (err_code)
	{
		destruct_mail(mail);
		return err_code;
	}
	return OK;
}

void print_mail(const Mail mail)
{
	const Address* ad = &(mail.receiver_addr);
	char* status = mail.is_delivered ? "Yes" : "No";
	printf("Receiver's ID: %s\n", ad->receiver_id.data);
	if (mail.receiver_addr.building.data[0] == '\0')
	{
		printf("Receiver's address: %s, %s St, %llu %llu\n",
				ad->city.data, ad->street.data, ad->house_number, ad->apartment_number);
	}
	else
	{
		printf("Receiver's address: %s, %s St, %llu-%s %llu\n",
				ad->city.data, ad->street.data, ad->house_number, ad->building.data, ad->apartment_number);
	}
	printf("Weight: %.3lf\n", mail.weight);
	printf("Time of creationg: %s\n", mail.creation_time.data);
	printf("Time of delivery: %s\n", mail.delivery_time.data);
	printf("Delivered: %s\n", status);
}

int compare_str_long_time(const String l, const String r)
{
	// validate with return 0
	// dd:MM:yyyy hh:mm:ss
	for (ull i = 6; i < 10; ++i)
	{
		if (l.data[i] != r.data[i])
		{
			return l.data[i] < r.data[i] ? -1 : 1;
		}
	}
	for (ull i = 3; i < 5; ++i)
	{
		if (l.data[i] != r.data[i])
		{
			return l.data[i] < r.data[i] ? -1 : 1;
		}
	}
	for (ull i = 0; i < 2; ++i)
	{
		if (l.data[i] != r.data[i])
		{
			return l.data[i] < r.data[i] ? -1 : 1;
		}
	}
	return strcmp(l.data, r.data);
}

int mail_time_comp(const void* l, const void* r)
{
	Mail* mail_l = (Mail*) l;
	Mail* mail_r = (Mail*) r;
	return compare_str_long_time(mail_l->creation_time, mail_r->creation_time);
}

int mail_id_comp(const void* l, const void* r)
{
	Mail* mail_l = (Mail*) l;
	Mail* mail_r = (Mail*) r;
	if (!equal_string(mail_l->receiver_addr.receiver_id, mail_r->receiver_addr.receiver_id))
	{
		return compare_string(mail_l->receiver_addr.receiver_id, mail_r->receiver_addr.receiver_id);
	}
	return compare_string(mail_l->mail_id, mail_r->mail_id);
}

status_codes construct_post(Post* post, const Address addr)
{
	if (post == NULL)
	{
		return INVALID_INPUT;
	}
	post->office_addr = addr;
	post->mail_size = 2;
	post->mail_cnt = 0;
	post->mail_arr = (Mail*) malloc(sizeof(Mail) * 2);
	if (post->mail_arr == NULL)
	{
		return BAD_ALLOC;
	}
	return OK;
}

status_codes destruct_post(Post* post)
{
	if (post == NULL)
	{
		return INVALID_INPUT;
	}
	for (ull i = 0; i < post->mail_cnt; ++i)
	{
		destruct_mail(&(post->mail_arr[i]));
	}
	free(post->mail_arr);
	destruct_address(&(post->office_addr));
	return OK;
}

void print_post_mail(const Post post)
{
	if (post.mail_cnt == 0)
	{
		printf("There is no mail\n");
		return;
	}
	printf("| ReceiverID |     MailID     | wght | address |  time of creation   |  time of delivery   | Delivered |\n");
	for (ull i = 0; i < post.mail_cnt; ++i)
	{
		Mail* m = &(post.mail_arr[i]);
		char* status = m->is_delivered ? "Yes" : "No";
		printf("| %10s | %s | % 3.1lf | no addr | %s | %s | %9s |\n",
				m->receiver_addr.receiver_id.data, m->mail_id.data, m->weight, m->creation_time.data, m->delivery_time.data, status);
	}
}

status_codes search_mail(const Post post, const String mailID, ull* ind)
{
	if (ind == NULL)
	{
		return INVALID_INPUT;
	}
	if (validate_string(mailID.data, DIGIT_STRING, 14))
	{
		return INVALID_INPUT;
	}
	// do you hear about binary search buddy?
	for (ull i = 0; i < post.mail_size; ++i)
	{
		if (equal_string(post.mail_arr[i].mail_id, mailID))
		{
			*ind = i;
			return OK;
		}
	}
	*ind = -1;
	return BAD_SEARCH;
}

status_codes insert_mail(Post* post, const Mail mail)
{
	if (post == NULL)
	{
		return INVALID_INPUT;
	}
	ull ind;
	if (search_mail(*post, mail.mail_id, &ind) == OK)
	{
		return UNAVAILABLE_ID;
	}
	if (post->mail_cnt == post->mail_size)
	{
		Mail* tmp = (Mail*) realloc(post->mail_arr, sizeof(Mail) * post->mail_size * 2);
		if (tmp == NULL)
		{
			return BAD_ALLOC;
		}
		post->mail_size *= 2;
		post->mail_arr = tmp;
	}
	post->mail_arr[post->mail_cnt++] = mail;
	qsort(post->mail_arr, post->mail_cnt, sizeof(Mail), mail_id_comp);
	return OK;
}

status_codes extract_mail(Post* post, const String mailID)
{
	if (post == NULL)
	{
		return INVALID_INPUT;
	}
	ull ind;
	status_codes code = search_mail(*post, mailID, &ind);
	if (code)
	{
		return code;
	}
	destruct_mail(&(post->mail_arr[ind]));
	for (ull i = ind + 1; i < post->mail_cnt; ++i)
	{
		post->mail_arr[i-1] = post->mail_arr[i];
	}
	post->mail_cnt--;
	return OK;
}

void print_delivered_post_mail(const Post post)
{
	int flag = 1;
	for (ull i = 0; i < post.mail_cnt; ++i)
	{
		Mail* m = &(post.mail_arr[i]);
		if (m->is_delivered)
		{
			if (flag)
			{
				printf("| ReceiverID |     MailID     | wght | address |  time of creation   |  time of delivery   |\n");
				flag = 0;
			}
			printf("| %10s | %s | % 3.1lf | no addr | %s | %s |\n",
				m->receiver_addr.receiver_id.data, m->mail_id.data, m->weight, m->creation_time.data, m->delivery_time.data);
		}
	}
	if (flag)
	{
		printf("There is no delivered mail\n");
	}
}

status_codes print_expired_post_mail(const Post post)
{
	String cur_time;
	status_codes code = get_cur_long_time(&cur_time);
	if (code)
	{
		return code;
	}
	int flag = 1;
	for (ull i = 0; i < post.mail_cnt; ++i)
	{
		Mail* m = &(post.mail_arr[i]);
		if (!m->is_delivered && compare_str_long_time(m->delivery_time, cur_time) < 0)
		{
			if (flag)
			{
				printf("| ReceiverID |     MailID     | wght | address |  time of creation   |  time of delivery   |\n");
				flag = 0;
			}
			printf("| %10s | %s | % 3.1lf | no addr | %s | %s |\n",
				m->receiver_addr.receiver_id.data, m->mail_id.data, m->weight, m->creation_time.data, m->delivery_time.data);
		}
	}
	if (flag)
	{
		printf("There is no expired mail\n");
	}
}

// TODO
// read_line -> read_string_line
// Интерактивный диалог (ого):
// 1. + Добавление посылки (by mail_id) + сортировка после каждого добавления.......
// 2. + Удаление посылки (by mail_id)
// 3. + Поиск посылки (by mail_id)
// 4. ? Поиск всех УЖЕ доставленных посылок (от старых к новым)
// 5. ? Поиск всех посылок, срок доставки которых истёк??? (от старых к новым)

int main(int argc, char** argv)
{
	// STRINGS TEST
	if (0)
	{
		String str1, str2;
		String* str3;
		construct_string(&str1, "aboba");
		construct_string(&str2, "ab");
		dyn_copy_string(&str3, str2);
		copy_string(&str2, str1);
		
		printf("%d %s\n", str1.len, str1.data);
		printf("%d %s\n", str2.len, str2.data);
		printf("%d %s\n", str3->len, str3->data);
		printf("%d\n", equal_string(str1, str2));
		printf("%d\n", compare_string(*str3, str1));
		
		concat_string(str3, str2);
		printf("%d %s\n", str3->len, str3->data);
		
		destruct_string(&str1);
		destruct_string(&str2);
		destruct_string(str3);
		return 0;
	}
	
	String null_str;
	construct_string(&null_str, NULL);
	
	Address office_addr = { .city = null_str, .street = null_str, .house_number = 0, .building = null_str,
							.apartment_number = 0, .receiver_id = null_str };
	
	Post post;
	construct_post(&post, office_addr);
	
	String cmd_exit, cmd_add, cmd_remove, cmd_print, cmd_search, cmd_deliver, cmd_print_delivered, cmd_print_expired;
	construct_string(&cmd_exit, "exit");
	construct_string(&cmd_add, "add");
	construct_string(&cmd_remove, "remove");
	construct_string(&cmd_print, "print");
	construct_string(&cmd_search, "search");
	construct_string(&cmd_deliver, "deliver");
	construct_string(&cmd_print_delivered, "delivered");
	construct_string(&cmd_print_expired, "expired");
	printf("cmds: exit add remove print search deliver delivered expired\n");
	
	status_codes err_code = OK;
	int run_flag = 1;
	while (run_flag && !err_code)
	{
		printf("cmd: ");
		// WRITE CMD
		String cmd;
		err_code = read_string_line(&cmd);	
		printf("\n");
		// EXECUTE CMD
		if (!err_code)
		{
			status_codes cmd_code = OK;
			// --- COMMAND EXIT ---
			if (equal_string(cmd, cmd_exit))
			{
				printf("hrrr mi mi mi hrrrr mi mi mi\n");
				run_flag = 0;
			}
			// --- COMMAND ADD MAIL ---
			else if (equal_string(cmd, cmd_add))
			{
				printf("Enter mail data\n");
				Mail mail;
				cmd_code = read_mail(&mail);
				if (!cmd_code)
				{
					cmd_code = insert_mail(&post, mail);
				}
				if (cmd_code == UNAVAILABLE_ID)
				{
					printf("Mail with this ID already exists\n");
					destruct_mail(&mail);
					cmd_code = OK;
				}
				else if (!cmd_code)
				{
					printf("Mail was added\n");
				}
			}
			// --- COMMAND REMOVE MAIL ---
			else if (equal_string(cmd, cmd_remove))
			{
				printf("Enter mail ID: ");
				String mailID;
				cmd_code = read_string_line(&mailID);
				if (!cmd_code)
				{
					cmd_code = extract_mail(&post, mailID);
				}
				if (cmd_code == BAD_SEARCH)
				{
					printf("This mail does not exist\n");
					cmd_code = OK;
				}
				else if (!cmd_code)
				{
					printf("Mail was removed\n");
				}
			}
			// --- COMMAND PRINT ALL MAIL (DELETE ME) ---
			else if (equal_string(cmd, cmd_print))
			{
				print_post_mail(post);
			}
			// --- COMMAND SEARCH MAIL ---
			else if (equal_string(cmd, cmd_search))
			{
				printf("Enter mail ID: ");
				String mailID;
				ull ind;
				cmd_code = read_string_line(&mailID);
				if (!cmd_code)
				{
					cmd_code = search_mail(post, mailID, &ind);
				}
				if (cmd_code == BAD_SEARCH)
				{
					printf("This mail does not exist\n");
					cmd_code = OK;
				}
				else if (!cmd_code)
				{
					print_mail(post.mail_arr[ind]);
				}
			}
			// --- COMMAND DELIVER MAIL ---
			else if (equal_string(cmd, cmd_deliver))
			{
				printf("Enter mail ID: ");
				String mailID;
				ull ind;
				cmd_code = read_string_line(&mailID);
				if (!cmd_code)
				{
					cmd_code = search_mail(post, mailID, &ind);
				}
				if (cmd_code == BAD_SEARCH)
				{
					printf("This mail does not exist\n");
					cmd_code = OK;
				}
				else if (!cmd_code)
				{
					post.mail_arr[ind].is_delivered = 1;
					printf("Status of mail was updated\n");
				}
			}
			// --- COMMAND PRINT DELIVERED MAIL ---
			else if (equal_string(cmd, cmd_print_delivered))
			{
				print_delivered_post_mail(post);
			}
			// --- COMMAND PRINT EXPIRED MAIL (DELETE ME) ---
			else if (equal_string(cmd, cmd_print))
			{
				err_code = print_expired_post_mail(post);
			}
			
			if (cmd_code)
			{
				print_error(cmd_code);
			}
			printf("\n");
		}
	}
	
	destruct_string(&cmd_exit);
	destruct_string(&cmd_add);
	destruct_string(&cmd_remove);
	destruct_string(&cmd_print);
	destruct_string(&cmd_search);
	
	destruct_post(&post);
}