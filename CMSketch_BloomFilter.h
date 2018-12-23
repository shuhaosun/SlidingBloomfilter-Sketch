#ifndef _CMSKETCH_H
#define _CMSKETCH_H

#include <algorithm>
#include <cstring>
#include <string.h>
#include "params.h"
#include "BOBHash32.h"
#include <iostream>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

using namespace std;

class CMSketch_BloomFilter
{	
private:
	BOBHash32 * bobhash32[MAX_HASH_NUM];
	BOBHash32 * bf_bobhash32_before[MAX_HASH_NUM][MAX_HASH_NUM];
	BOBHash32 * bf_bobhash32_after[MAX_HASH_NUM][MAX_HASH_NUM];
	int index[MAX_HASH_NUM];
	int bf_index[MAX_HASH_NUM];
	int *counter[MAX_HASH_NUM];
	int *bf_counter_before[MAX_HASH_NUM];
	int *bf_counter_after[MAX_HASH_NUM];
	int w, d, bf_w, bf_d;
	int MAX_CNT;
	int counter_index_size;
	uint64_t hash_value;
	

public:
	int mem_acc;
    int window_num = 1;
	CMSketch_BloomFilter(int _w, int _d, int bf__w, int bf__d)
	{
		mem_acc = 0;
		counter_index_size = 20;
		w = _w;
		d = _d;
		bf_w = bf__w;
		bf_d = bf__d;
		
		for(int i = 0; i < d; i++)	
		{
			counter[i] = new int[w];
			memset(counter[i], 0, sizeof(int) * w);

			bf_counter_before[i] = new int[bf_w];
			memset(bf_counter_before[i], 0, sizeof(int) * bf_w);

			bf_counter_after[i] = new int[bf_w];
			memset(bf_counter_after[i], 0, sizeof(int) * bf_w);

		}

		MAX_CNT = (1 << COUNTER_SIZE) - 1;

		for(int i = 0; i < d; i++)
		{
			bobhash32[i] = new BOBHash32(i + 1000);
		}

		for(int i = 0; i < d; i++)
		{
			for (int j = 0; j < bf_d; j++)
			{
				bf_bobhash32_before[i][j] = new BOBHash32(i * MAX_HASH_NUM + j + 500);
				bf_bobhash32_after[i][j] = new BOBHash32(i * MAX_HASH_NUM + j + 1000);
			}			
		}
	}
	int BF_Query2(const char * str, int i)
	{
		int sigal = 1;
		for (int j = 0; j < bf_d; j++)
		{
			bf_index[j] = (bf_bobhash32_before[i][j]->run(str, strlen(str))) % bf_w;
			if (bf_counter_before[i][bf_index[j]] == 0)
			{
				sigal = 0;
			}
		}
		return sigal;
	}
	int BF_Query1(const char * str, int i)
	{
		int sigal = 1;
		for (int j = 0; j < bf_d; j++)
		{
			bf_index[j] = (bf_bobhash32_after[i][j]->run(str, strlen(str))) % bf_w;
			if (bf_counter_after[i][bf_index[j]] == 0)
			{
				sigal = 0;
			}
		}
		return sigal;
	}

	void Insert(const char * str, int t)
	{
		for(int i = 0; i < d; i++)
		{
			index[i] = (bobhash32[i]->run(str, strlen(str))) % w;
			if(counter[i][index[i]] != MAX_CNT)
			{
				counter[i][index[i]]++;
				if(window_num == 1)
				{
					char bf_str[200];
					sprintf(bf_str,"%d",t);
					char bf_str1[200];
					sprintf(bf_str1,"%d",index[i]);
					char bf_str2[4] = "**";
					strcat(bf_str,bf_str2);
					strcat(bf_str,bf_str1);

					for (int j = 0; j < bf_d; j++)
					{
						bf_index[j] = (bf_bobhash32_before[i][j]->run(bf_str, strlen(bf_str))) % bf_w;
						bf_counter_before[i][bf_index[j]] = 1;
					}					
				}
				else if (window_num % 2 == 0)
				{
					char bf_str[200];
					sprintf(bf_str,"%d",t);
					char bf_str1[200];
					sprintf(bf_str1,"%d",index[i]);
					char bf_str2[4] = "**";
					strcat(bf_str,bf_str2);
					strcat(bf_str,bf_str1);	

					for (int j = 0; j < bf_d; j++)
					{
						bf_index[j] = (bf_bobhash32_after[i][j]->run(bf_str, strlen(bf_str))) % bf_w;
						bf_counter_after[i][bf_index[j]] = 1;
					}
					for (int z = 0; z < w; z++)
					{
						char bf_str[200];
						sprintf(bf_str,"%d",t - WINDOW_SIZE);
						char bf_str1[200];
						sprintf(bf_str1,"%d",z);
						char bf_str2[4] = "**";
					    strcat(bf_str,bf_str2);
						strcat(bf_str,bf_str1);	

						int tag;
						tag = BF_Query2(bf_str, i);
						if (tag == 1)
						{
							counter[i][z]--;
							if (counter[i][z] < 0)
							{
								counter[i][z] = 0;
							}
						}
					}
					if ((t + 1) % WINDOW_SIZE == 0)
					{
						memset(bf_counter_before[i], 0, sizeof(int) * bf_w);
					}					
				}
				else if (window_num % 2 == 1)
				{
					char bf_str[200];
					sprintf(bf_str,"%d",t);
					char bf_str1[200];
					sprintf(bf_str1,"%d",index[i]);
					char bf_str2[4] = "**";
					strcat(bf_str,bf_str2);
					strcat(bf_str,bf_str1);	

					for (int j = 0; j < bf_d; j++)
					{
						bf_index[j] = (bf_bobhash32_before[i][j]->run(bf_str, strlen(bf_str))) % bf_w;
						bf_counter_before[i][bf_index[j]] = 1;
					}
					for (int z = 0; z < w; z++)
					{
						char bf_str[200];
						sprintf(bf_str,"%d",t - WINDOW_SIZE);
						char bf_str1[200];
						sprintf(bf_str1,"%d",z);
						char bf_str2[4] = "**";
					    strcat(bf_str,bf_str2);
						strcat(bf_str,bf_str1);	

						int tag;
						tag = BF_Query1(bf_str, i);
						if (tag == 1)
						{
							counter[i][z]--;
							if (counter[i][z] < 0)
							{
								counter[i][z] = 0;
							}
						}
					}
					if ((t + 1) % WINDOW_SIZE == 0)
					{
						memset(bf_counter_after[i], 0, sizeof(int) * bf_w);
					}					
				}
			}

			mem_acc ++;
		}
		if ((t + 1) % WINDOW_SIZE == 0)
		{
			window_num++;
		}
	}
	int Query(const char *str)
	{
		int min_value = MAX_CNT;
		int temp;
		for(int i = 0; i < d; i++)
		{
			index[i] = (bobhash32[i]->run(str, strlen(str))) % w;
			temp = counter[i][index[i]];
			min_value = temp < min_value ? temp : min_value;
		}
		return min_value;
	}
	~CMSketch_BloomFilter()
	{
		for(int i = 0; i < d; i++)	
		{
			delete []counter[i];
		}


		for(int i = 0; i < d; i++)
		{
			delete bobhash32[i];
		}
	}
};
#endif//_CMSKETCH_H
