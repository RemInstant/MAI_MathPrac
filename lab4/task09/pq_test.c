#include <stdlib.h>
#include <assert.h>

#include "utility.h"
#include "priority_queue.h"

#include "binomial_heap.h"

int main()
{
	for (priority_queue_base base = PQB_BINARY; base <= PQB_TREAP; ++base)
	{
		base = PQB_BINOM; // the only finished heap
		
		p_queue pq, pq1, pq2, pq3, pq4;
		char* val;
		
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
		
		assert(pq.set_null(pq.heap) == OK);
		assert(pq1.set_null(pq1.heap) == OK);
		assert(pq2.set_null(pq2.heap) == OK);
		assert(pq3.set_null(pq3.heap) == OK);
		assert(pq4.set_null(pq4.heap) == OK);
		
		// Empty pq
		assert(pq.destruct(pq.heap) == OK);
		assert(pq.construct(pq.heap, compare_pq_key) == OK);
		assert(pq.destruct(pq.heap) == OK);
		
		assert(pq.construct(pq1.heap, compare_pq_key) == OK);
		assert(pq.copy(pq2.heap, pq2.heap) == OK);
		assert(pq.meld(pq.heap, pq1.heap, pq2.heap) == OK);
		assert(pq.destruct(pq.heap) == OK);
		
		// One pq
		{
			pair_prior_time pq_key_arr[8] = {
				(pair_prior_time) {5, "123"},
				(pair_prior_time) {3, "123"},
				(pair_prior_time) {6, "123"},
				(pair_prior_time) {1, "123"},
				(pair_prior_time) {7, "123"},
				(pair_prior_time) {2, "123"},
				(pair_prior_time) {8, "123"},
				(pair_prior_time) {4, "123"},
			};
			
			unsigned vals[] = { 5, 5, 6, 6, 7, 7, 8, 8 };
			
			assert(pq.construct(pq.heap, compare_pq_key) == OK);	
			
			for (size_t i = 0; i < 8; ++i)
			{
				val = (char*) calloc(2, sizeof(char));
				assert(val != NULL);
				val[0] = '0' + pq_key_arr[i].prior;
				assert(pq.insert(pq.heap, pq_key_arr[i], val) == OK);
				free(val);
				assert(pq.top(pq.heap, &val) == OK);
				assert(atoi(val) == vals[i]);
			}
			
			for (size_t i = 8; i > 0; --i)
			{
				//bm_heap bmh = *((bm_heap*)(pq.heap));
				assert(pq.top(pq.heap, &val) == OK);
				assert(atoi(val) == i);
				free(val);
				assert(pq.pop(pq.heap, &val) == OK);
				assert(atoi(val) == i);
				free(val);
			}
			
			assert(pq.destruct(pq.heap) == OK);
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
			
			assert(pq.construct(pq.heap, compare_pq_key) == OK);
			
			for (size_t i = 0; i < 36; ++i)
			{
				val = (char*) calloc(3, sizeof(char));
				assert(val != NULL);
				val[0] = '0' + pq_key_arr[i].prior;
				val[1] = pq_key_arr[i].time[0];
				assert(pq.insert(pq.heap, pq_key_arr[i], val) == OK);
				free(val);
				assert(pq.top(pq.heap, &val) == OK);
				assert(atoi(val) == vals[i]);
			}
			
			assert(pq1.copy(pq1.heap, pq.heap) == OK);
			
			for (size_t i = 36; i > 0; --i)
			{
				int num_val = (i+3) / 4 * 10 + (4 - ((i+3) % 4));
				assert(pq.top(pq.heap, &val) == OK);
				assert(atoi(val) == num_val);
				free(val);
				assert(pq.pop(pq.heap, &val) == OK);
				assert(atoi(val) == num_val);
				free(val);
				assert(pq1.pop(pq1.heap, &val) == OK);
				assert(atoi(val) == num_val);
				free(val);
			}
			
			assert(pq.destruct(pq.heap) == OK);
			assert(pq1.destruct(pq.heap) == OK);
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
			
			assert(pq1.construct(pq1.heap, compare_pq_key) == OK);
			assert(pq2.construct(pq2.heap, compare_pq_key) == OK);	
			
			for (size_t i = 0; i < 20; ++i)
			{
				val = (char*) calloc(3, sizeof(char));
				assert(val != NULL);
				val[0] = '0' + pq1_key_arr[i].prior;
				val[1] = pq1_key_arr[i].time[0];
				assert(pq1.insert(pq1.heap, pq1_key_arr[i], val) == OK);
				free(val);
				assert(pq1.top(pq1.heap, &val) == OK);
				assert(atoi(val) == vals1[i]);
			}
			
			for (size_t i = 0; i < 16; ++i)
			{
				val = (char*) calloc(3, sizeof(char));
				assert(val != NULL);
				val[0] = '0' + pq2_key_arr[i].prior;
				val[1] = pq2_key_arr[i].time[0];
				assert(pq2.insert(pq2.heap, pq2_key_arr[i], val) == OK);
				free(val);
				assert(pq2.top(pq2.heap, &val) == OK);
				assert(atoi(val) == vals2[i]);
			}
			
			assert(pq3.copy_meld(pq3.heap, pq1.heap, pq2.heap) == OK);
			assert(pq4.copy_meld(pq4.heap, pq1.heap, pq2.heap) == OK);
			assert(pq.meld(pq.heap, pq1.heap, pq2.heap) == OK);
			
			for (size_t i = 36; i > 0; --i)
			{
				int num_val = (i+3) / 4 * 10 + (4 - ((i+3) % 4));
				
				bm_heap bmh = *((bm_heap*)(pq.heap));
				
				assert(pq.top(pq.heap, &val) == OK);
				assert(atoi(val) == num_val);
				free(val);
				assert(pq.pop(pq.heap, &val) == OK);
				assert(atoi(val) == num_val);
				free(val);
				assert(pq3.pop(pq3.heap, &val) == OK);
				assert(atoi(val) == num_val);
				free(val);
			}
			
			assert(pq.destruct(pq.heap) == OK);
			assert(pq.destruct(pq1.heap) == OK);
			assert(pq.destruct(pq2.heap) == OK);
			assert(pq.destruct(pq3.heap) == OK);
			assert(pq.destruct(pq4.heap) == OK);
		}
		
		assert(p_queue_destruct(&pq) == OK);
		assert(p_queue_destruct(&pq1) == OK);
		assert(p_queue_destruct(&pq2) == OK);
		assert(p_queue_destruct(&pq3) == OK);
		
		break;
	}
	
	printf("TESTS HAVE BEEN PASSED\n");
}