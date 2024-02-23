#include <stdlib.h>
#include <string.h>

#include "utility.h"
#include "department.h"

#include "priority_queue/binomial_heap.h"
#include "priority_queue/priority_queue.h"

#include "map/trie.h"
#include "map/map.h"

int main()
{
	Department dep;
	department_set_null(&dep);
	
	status_code code = OK;
	
	code = code ? code : department_construct(&dep, 1, PQB_BINOM, 5.0, 1e-9, 5, 5, compare_request);
	
	if (code)
	{
		return code;
	}
	
	char cur_time[21];
	strcpy(cur_time, "2000-01-01T10:00:00Z");
	
	
	char* txt = malloc(sizeof(char) * 4);
	strcpy(txt, "123");
	
	unsigned req_id = 1;
	
	size_t msg_cnt = 0;
	dep_msg* msgs = NULL;
	
	while (strcmp(cur_time, "2000-01-01T11:00:00Z"))
	{
		code = code ? code : department_handle_finishing(&dep, cur_time, &msg_cnt, &msgs);
		free(msgs);
		
		if (!strcmp(cur_time, "2000-01-01T10:01:00Z"))
		{
			request* req = (request*) malloc(sizeof(request));
			req->id = req_id++;
			req->prior = 2;
			strcpy(req->time, cur_time);
			req->txt = txt; 
			
			
			code = code ? code : department_add_request(&dep, req, &msg_cnt, &msgs);
			free(msgs);
		}
		
		iso_time_add(cur_time, 60, cur_time);
	}
}