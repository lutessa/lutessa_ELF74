|Thread Name|	Entry Function	|Stack size|	Priority|	Auto Start	|Time Slicing|
|:-----:|:----:|:---:|:------------------:|:-----:|:-----:|
|thread 0	|thread_0_entry|	1024|	1	|yes|	no|
|thread 1	|thread_1_entry	|1024	|16	|yes	|4|
|thread 2|	thread_2_entry	|1024|	16	|yes	|4|
|thread 3|	thread_3_and_4_entry|	1024|	8|	yes	|no|
|thread 4|	thread_3_and_4_entry|	1024|	8|	yes	|no|
|thread 5|	thread_5_entry	|1024	|4	|yes	|no|
|thread 6|	thread_6_and_7_entry|	1024|	8|	yes|	no|
|thread 7|  thread_6_and_7_entry|	1024|	8|	yes	|no|



|Name	|control| structure|	size|	location|
|------|------|-------|-------|----------------|
|byte pool 0	|byte_pool_0	|9120	|byte_pool_memory|
|mutex 0	|mutex_0|||		
|queue 0|	queue_0|	400|	pointer|
|semaphore 0|	semaphore_0|	||	
|block pool 0|	block_pool_0	|100|	pointer|
|event flags 0|	event_flags_0	|||	


![Test Image 1](https://github.com/lutessa/lutessa_ELF74/blob/master/Lab5/Diagram.jpg)