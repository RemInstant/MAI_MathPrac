#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../utility.h"
#include "priority_queue.h"

#include "binomial_heap.h"

void free_req(request* req)
{
	free(req->txt);
	free(req);
}

int main()
{
	for (priority_queue_base base = PQB_BINARY; base <= PQB_TREAP; ++base)
	{
		if (base == PQB_BINARY) continue;
		if (base == PQB_LEFTIST) continue;
		if (base == PQB_SKEW) continue;
		if (base == PQB_FIB) continue;
		if (base == PQB_TREAP) continue;
		
		p_queue pq, pq1, pq2, pq3, pq4;
		size_t size;
		request* req;
		
		p_queue_set_null(&pq);
		p_queue_set_null(&pq1);
		p_queue_set_null(&pq2);
		p_queue_set_null(&pq3);
		p_queue_set_null(&pq4);
		
		assert(p_queue_init(&pq, base) == OK);
		assert(p_queue_init(&pq1, base) == OK);
		assert(p_queue_init(&pq2, base) == OK);
		assert(p_queue_init(&pq3, base) == OK);
		assert(p_queue_init(&pq4, base) == OK);
		
		assert(pq.set_null(pq.ds) == OK);
		assert(pq1.set_null(pq1.ds) == OK);
		assert(pq2.set_null(pq2.ds) == OK);
		assert(pq3.set_null(pq3.ds) == OK);
		assert(pq4.set_null(pq4.ds) == OK);
		
		// Empty pq
		assert(pq.destruct(pq.ds) == OK);
		assert(pq.construct(pq.ds, compare_request) == OK);
		assert(pq.destruct(pq.ds) == OK);
		
		assert(pq.construct(pq1.ds, compare_request) == OK);
		assert(pq.copy(pq2.ds, pq2.ds) == OK);
		assert(pq.meld(pq.ds, pq1.ds, pq2.ds) == OK);
		assert(pq.destruct(pq.ds) == OK);
		
		// One pq
		{
			unsigned prior[8] = { 5, 3, 6, 1, 7, 2, 8, 4 };
			unsigned vals[8] = { 5, 5, 6, 6, 7, 7, 8, 8 };
			
			assert(pq.construct(pq.ds, compare_request) == OK);
			assert(pq.size(pq.ds, &size) == OK);
			assert(size == 0);
			
			for (size_t i = 0; i < 8; ++i)
			{
				req = (request*) malloc(sizeof(request));
				assert(req != NULL);
				req->txt = (char*) calloc(2, sizeof(char));
				assert(req->txt != NULL);
				
				req->prior = prior[i];
				strcpy(req->time, "123");
				req->txt[0] = '0' + prior[i];
				
				assert(pq.insert(pq.ds, req) == OK);
				free_req(req);
				assert(pq.size(pq.ds, &size) == OK);
				assert(size == i+1);
				assert(pq.top(pq.ds, &req) == OK);
				assert(atoi(req->txt) == vals[i]);
				free_req(req);
			}
			
			for (size_t i = 8; i > 0; --i)
			{
				assert(pq.top(pq.ds, &req) == OK);
				assert(atoi(req->txt) == i);
				free_req(req);
				assert(pq.pop(pq.ds, &req) == OK);
				assert(atoi(req->txt) == i);
				free_req(req);
				assert(pq.size(pq.ds, &size) == OK);
				assert(size == i-1);
			}
			
			assert(pq.destruct(pq.ds) == OK);
		}
		// Copy
		{
			pair_prior_time pq_key_arr[36] = {
				(pair_prior_time) {1, "4"},
				(pair_prior_time) {1, "3"},
				(pair_prior_time) {3, "2"},
				(pair_prior_time) {3, "1"},
				(pair_prior_time) {4, "3"},
				(pair_prior_time) {3, "4"},
				(pair_prior_time) {3, "3"},
				(pair_prior_time) {5, "4"},
				(pair_prior_time) {6, "2"},
				
				(pair_prior_time) {7, "2"},
				(pair_prior_time) {5, "2"},
				(pair_prior_time) {2, "2"},
				(pair_prior_time) {6, "4"},
				(pair_prior_time) {4, "4"},
				(pair_prior_time) {5, "3"},
				(pair_prior_time) {7, "4"},
				(pair_prior_time) {7, "1"},
				(pair_prior_time) {1, "1"},
				
				(pair_prior_time) {8, "4"},
				(pair_prior_time) {5, "1"},
				(pair_prior_time) {6, "3"},
				(pair_prior_time) {8, "2"},
				(pair_prior_time) {9, "3"},
				(pair_prior_time) {6, "1"},
				(pair_prior_time) {8, "3"},
				(pair_prior_time) {9, "4"},
				(pair_prior_time) {1, "2"},
				
				(pair_prior_time) {2, "4"},
				(pair_prior_time) {2, "1"},
				(pair_prior_time) {2, "3"},
				(pair_prior_time) {4, "1"},
				(pair_prior_time) {4, "2"},
				(pair_prior_time) {9, "1"},
				(pair_prior_time) {9, "2"},
				(pair_prior_time) {8, "1"},
				(pair_prior_time) {7, "3"},
			};
			
			unsigned vals[] = { 
				14, 13, 32, 31, 43, 43, 43, 54, 62,
				72, 72, 72, 72, 72, 72, 72, 71, 71,
				84, 84, 84, 82, 93, 93, 93, 93, 93,
				93, 93, 93, 93, 93, 91, 91, 91, 91,
			};
			
			assert(pq.construct(pq.ds, compare_request) == OK);
			
			for (size_t i = 0; i < 36; ++i)
			{
				req = (request*) malloc(sizeof(request));
				assert(req != NULL);
				req->txt = (char*) calloc(3, sizeof(char));
				assert(req->txt != NULL);
				
				req->prior = pq_key_arr[i].prior;
				strcpy(req->time, pq_key_arr[i].time);
				req->txt[0] = '0' + pq_key_arr[i].prior;
				req->txt[1] = pq_key_arr[i].time[0];
				
				assert(pq.insert(pq.ds, req) == OK);
				free_req(req);
				assert(pq.top(pq.ds, &req) == OK);
				assert(atoi(req->txt) == vals[i]);
				free_req(req);
			}
			
			assert(pq1.copy(pq1.ds, pq.ds) == OK);
			
			for (size_t i = 36; i > 0; --i)
			{
				int num_val = (i+3) / 4 * 10 + (4 - ((i+3) % 4));
				assert(pq.top(pq.ds, &req) == OK);
				assert(atoi(req->txt) == num_val);
				free_req(req);
				assert(pq.pop(pq.ds, &req) == OK);
				assert(atoi(req->txt) == num_val);
				free(req);
				assert(pq1.pop(pq1.ds, &req) == OK);
				assert(atoi(req->txt) == num_val);
				free(req);
			}
			
			assert(pq.destruct(pq.ds) == OK);
			assert(pq1.destruct(pq.ds) == OK);
		}
		// meld
		{
			pair_prior_time pq1_key_arr[20] = {
				(pair_prior_time) {3, "4"},
				(pair_prior_time) {1, "1"},
				(pair_prior_time) {5, "2"},
				(pair_prior_time) {1, "4"},
				(pair_prior_time) {3, "3"},
				(pair_prior_time) {9, "3"},
				(pair_prior_time) {7, "2"},
				(pair_prior_time) {1, "2"},
				(pair_prior_time) {9, "2"},
				(pair_prior_time) {5, "1"},
				
				(pair_prior_time) {5, "4"},
				(pair_prior_time) {3, "1"},
				(pair_prior_time) {9, "4"},
				(pair_prior_time) {3, "2"},
				(pair_prior_time) {9, "1"},
				(pair_prior_time) {7, "1"},
				(pair_prior_time) {5, "3"},
				(pair_prior_time) {1, "3"},
				(pair_prior_time) {7, "3"},
				(pair_prior_time) {7, "4"},
			};
			
			unsigned vals1[] = { 
				34, 34, 52, 52, 52, 93, 93, 93, 92, 92,
				92, 92, 92, 92, 91, 91, 91, 91, 91, 91,
			};
			
			pair_prior_time pq2_key_arr[16] = {
				(pair_prior_time) {2, "2"},
				(pair_prior_time) {2, "3"},
				(pair_prior_time) {2, "1"},
				(pair_prior_time) {4, "3"},
				(pair_prior_time) {6, "4"},
				(pair_prior_time) {4, "2"},
				(pair_prior_time) {4, "1"},
				(pair_prior_time) {8, "3"},
				
				(pair_prior_time) {4, "4"},
				(pair_prior_time) {6, "3"},
				(pair_prior_time) {8, "1"},
				(pair_prior_time) {8, "4"},
				(pair_prior_time) {6, "2"},
				(pair_prior_time) {8, "2"},
				(pair_prior_time) {6, "1"},
				(pair_prior_time) {2, "4"},
			};
			
			unsigned vals2[] = { 
				22, 22, 21, 43, 64, 64, 64, 83,
				83, 83, 81, 81, 81, 81, 81, 81,
			};
			
			assert(pq1.construct(pq1.ds, compare_request) == OK);
			assert(pq2.construct(pq2.ds, compare_request) == OK);
			
			for (size_t i = 0; i < 20; ++i)
			{
				req = (request*) malloc(sizeof(request));
				assert(req != NULL);
				req->txt = (char*) calloc(3, sizeof(char));
				assert(req->txt != NULL);
				
				req->prior = pq1_key_arr[i].prior;
				strcpy(req->time, pq1_key_arr[i].time);
				req->txt[0] = '0' + pq1_key_arr[i].prior;
				req->txt[1] = pq1_key_arr[i].time[0];
				
				assert(pq1.insert(pq1.ds, req) == OK);
				free_req(req);
				assert(pq1.top(pq1.ds, &req) == OK);
				assert(atoi(req->txt) == vals1[i]);
				free_req(req);
			}
			
			for (size_t i = 0; i < 16; ++i)
			{
				req = (request*) malloc(sizeof(request));
				assert(req != NULL);
				req->txt = (char*) calloc(3, sizeof(char));
				assert(req->txt != NULL);
				
				req->prior = pq2_key_arr[i].prior;
				strcpy(req->time, pq2_key_arr[i].time);
				req->txt[0] = '0' + pq2_key_arr[i].prior;
				req->txt[1] = pq2_key_arr[i].time[0];
				
				assert(pq2.insert(pq2.ds, req) == OK);
				free_req(req);
				assert(pq2.top(pq2.ds, &req) == OK);
				assert(atoi(req->txt) == vals2[i]);
				free_req(req);
			}
			
			assert(pq1.size(pq1.ds, &size) == OK);
			assert(size == 20);
			assert(pq2.size(pq2.ds, &size) == OK);
			assert(size == 16);
			
			assert(pq3.copy_meld(pq3.ds, pq1.ds, pq2.ds) == OK);
			assert(pq4.copy_meld(pq4.ds, pq1.ds, pq2.ds) == OK);
			assert(pq.meld(pq.ds, pq1.ds, pq2.ds) == OK);
			
			assert(pq.size(pq.ds, &size) == OK);
			assert(size == 36);
			assert(pq4.size(pq4.ds, &size) == OK);
			assert(size == 36);
			
			for (size_t i = 36; i > 0; --i)
			{
				int num_val = (i+3) / 4 * 10 + (4 - ((i+3) % 4));
				assert(pq.top(pq.ds, &req) == OK);
				assert(atoi(req->txt) == num_val);
				free_req(req);
				assert(pq.pop(pq.ds, &req) == OK);
				assert(atoi(req->txt) == num_val);
				free_req(req);
				assert(pq3.pop(pq3.ds, &req) == OK);
				assert(atoi(req->txt) == num_val);
				free_req(req);
			}
			
			assert(pq.destruct(pq.ds) == OK);
			assert(pq.destruct(pq1.ds) == OK);
			assert(pq.destruct(pq2.ds) == OK);
			assert(pq.destruct(pq3.ds) == OK);
			assert(pq.destruct(pq4.ds) == OK);
		}
		
		assert(p_queue_destruct(&pq) == OK);
		assert(p_queue_destruct(&pq1) == OK);
		assert(p_queue_destruct(&pq2) == OK);
		assert(p_queue_destruct(&pq3) == OK);
		
		if (base == PQB_BINARY) 	printf("Binary heap passed the tests\n");
		if (base == PQB_LEFTIST) 	printf("Leftist heap passed the tests\n");
		if (base == PQB_SKEW) 		printf("Skew heap passed the tests\n");
		if (base == PQB_BINOM) 		printf("Binomial heap passed the tests\n");
		if (base == PQB_FIB) 		printf("Fibonacci heap passed the tests\n");
		if (base == PQB_TREAP) 		printf("Treap passed the tests\n");
	}
	printf("All tests have been passed\n");
}