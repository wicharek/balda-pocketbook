#include "balda_dict.h"
#include <stdlib.h>
#include <assert.h>

void balda_dict_init(balda_dict_t* dict, balda_dict_data_t* data)
{
	dict->mem_data = data;
}

void balda_dict_free(balda_dict_t* dict)
{
	
}

#define balda_dict_chunk_is_plain(chunk) ((*((chunk)->p)) & BALDA_DICT_HEADER_PLAIN_FLAG)
#define balda_dict_chunk_is_last_chunk(chunk) ((*((chunk)->p)) & BALDA_DICT_HEADER_LAST_CHUNK_FLAG)
#define balda_dict_chunk_is_ends_here(chunk) ((*((chunk)->p)) & BALDA_DICT_HEADER_ENDS_HERE_FLAG)
#define balda_dict_chunk_is_single_char(chunk) ((*((chunk)->p)) & BALDA_DICT_HEADER_SINGLE_CHAR_FLAG)
#define balda_dict_chunk_get_header_count(chunk) (((*((chunk)->p)) & BALDA_DICT_HEADER_COUNT_MASK) >> 24)
#define balda_dict_chunk_get_header_char(chunk) ((((*((chunk)->p)) & BALDA_DICT_HEADER_CHAR_MASK) >> 24) + 1)
#define balda_dict_chunk_get_header_next_chunk(chunk) ((*((chunk)->p)) & BALDA_DICT_HEADER_NEXT_CHUNK_MASK)

#define balda_dict_chunk_get_entry_char(entry) ((((*(entry)) & BALDA_DICT_ENTRY_CHAR_MASK) >> 24) + 1)
#define balda_dict_chunk_is_entry_ends_here(entry) ((*(entry)) & BALDA_DICT_ENTRY_ENDS_HERE_FLAG)
#define balda_dict_chunk_get_entry_next_chunk(entry) ((*(entry)) & BALDA_DICT_ENTRY_NEXT_CHUNK_MASK)


balda_char balda_dict_chunk_get_plain_char(balda_dict_chunk_t* chunk)
{
	//unsigned int v = (*((chunk)->plain_p));

	if (((chunk)->plain_off + BALDA_DICT_CHAR_BIT_SIZE) <= BALDA_DICT_DATA_T_BIT_SIZE)
		return (((*((chunk)->plain_p)) >> (BALDA_DICT_DATA_T_BIT_SIZE - ((chunk)->plain_off + BALDA_DICT_CHAR_BIT_SIZE)))
			& BALDA_DICT_CHAR_MASK) + 1;
	else
	{
		int ls = (BALDA_DICT_DATA_T_BIT_SIZE - (chunk)->plain_off);
		int rs = (BALDA_DICT_CHAR_BIT_SIZE - ls);
		int left = (((*((chunk)->plain_p)) & (BALDA_DICT_CHAR_MASK >> rs)) << rs);
		int right = ((*((chunk)->plain_p+1)) >> (BALDA_DICT_DATA_T_BIT_SIZE - rs));
		return (left | right) + 1;
	}
}

#define balda_dict_chunk_entries_count(chunk) ( \
		(balda_dict_chunk_is_plain(chunk) || \
		balda_dict_chunk_is_single_char(chunk)) ? 1 : \
		balda_dict_chunk_get_header_count(chunk) \
	)

#define balda_dict_chunk_load(chunk) \
	if (balda_dict_chunk_is_plain(chunk)) \
	{ \
		(chunk)->plain_index = 0; \
		if (balda_dict_chunk_is_last_chunk(chunk)) \
		{ \
			(chunk)->plain_p = (chunk)->p; \
			(chunk)->plain_off = 8; \
		} \
		else \
		{ \
			(chunk)->plain_p = ((chunk)->p + 1); \
			(chunk)->plain_off = 0; \
		} \
	}

balda_bool balda_dict_contains_word(balda_dict_t* dict, const balda_char* word)
{
	assert(word);
	assert(*word);
	
	balda_dict_chunk_entry_t chunk_entry;
	balda_dict_get_root_chunk(dict, &chunk_entry.next_chunk);
	
	const balda_char* c=word;
	while (*c)
	{
		if (!balda_dict_chunk_is_valid(&chunk_entry.next_chunk))
		{
			return 0;
		}

		if (!balda_dict_chunk_get_entry(&chunk_entry.next_chunk, *c,
			&chunk_entry))
		{
			return 0;
		}
		++c;
	}
	
	return chunk_entry.ends_here;
}

balda_bool balda_dict_peek_word_5_callback(balda_dict_chunk_entry_t* entry, balda_char* out_word, int c)
{
	int n = balda_dict_chunk_entries_count(&entry->next_chunk);
	int i = ((n == 1) ? (0) : (rand() % n));
	balda_dict_chunk_entry_t next_entry;
	
	if (c == 4)
	{
		for (; i<n; ++i)
		{
			out_word[c] = balda_dict_chunk_get_entry_by_index(&entry->next_chunk, i, &next_entry);
			if (next_entry.ends_here)
			{
				out_word[c+1] = BALDA_CHAR_NONE;
				return 1;
			}
		}
	}
	else
	{
		for (; i<n; ++i)
		{
			out_word[c] = balda_dict_chunk_get_entry_by_index(&entry->next_chunk, i, &next_entry);
			if (balda_dict_chunk_is_valid(&next_entry.next_chunk))
			{
				if (balda_dict_peek_word_5_callback(&next_entry, out_word, c+1))
					return 1;
			}
		}
	}
	
	return 0;
}

balda_bool balda_dict_peek_word_5(balda_dict_t* dict, balda_char* out_word)
{
	balda_dict_chunk_entry_t chunk_entry;
	balda_dict_get_root_chunk(dict, &chunk_entry.next_chunk);
	
	return balda_dict_peek_word_5_callback(&chunk_entry, out_word, 0);
}

void balda_dict_get_root_chunk(balda_dict_t* dict, balda_dict_chunk_t* out_chunk)
{
	assert(out_chunk);
	
	out_chunk->dict = dict;
	out_chunk->p = (dict->mem_data + 1);
	balda_dict_chunk_load(out_chunk);
}

balda_char balda_dict_chunk_get_entry_by_index(balda_dict_chunk_t* chunk, int index, balda_dict_chunk_entry_t* out_entry)
{
	assert(chunk);
	assert(out_entry);
	
	if (balda_dict_chunk_is_plain(chunk))
	{
		if (index != 0)
		assert(index == 0); // only 1 entry if plain
		
		balda_char chunk_c = balda_dict_chunk_get_plain_char(chunk);
		
		int char_count = balda_dict_chunk_get_header_count(chunk);
		short next_index = chunk->plain_index + 1;
		
		if (next_index >= char_count)
		{
			out_entry->ends_here = balda_dict_chunk_is_ends_here(chunk);

			balda_dict_t* dict = chunk->dict;
			if (balda_dict_chunk_is_last_chunk(chunk))
			{
				out_entry->next_chunk.dict = dict;
				out_entry->next_chunk.p = 0;
			}
			else
			{
				int next_chunk_index = balda_dict_chunk_get_header_next_chunk(chunk);
				
				out_entry->next_chunk.dict = dict;
				out_entry->next_chunk.p = dict->mem_data + next_chunk_index;
				balda_dict_chunk_load(&out_entry->next_chunk);
			}
		}
		else
		{
			out_entry->ends_here = 0;

			balda_dict_t* dict = chunk->dict;
			balda_dict_data_t* chunk_p = chunk->p;
			balda_dict_data_t* chunk_plain_p = chunk->plain_p;
			short chunk_plain_off = chunk->plain_off;
			
			out_entry->next_chunk.dict = dict;
			out_entry->next_chunk.p = chunk_p;
			
			out_entry->next_chunk.plain_p = chunk_plain_p;
			out_entry->next_chunk.plain_off = chunk_plain_off + BALDA_DICT_CHAR_BIT_SIZE;
			out_entry->next_chunk.plain_index = next_index;
			
			if (out_entry->next_chunk.plain_off > BALDA_DICT_DATA_T_BIT_SIZE)
			{
				++out_entry->next_chunk.plain_p;
				out_entry->next_chunk.plain_off %= BALDA_DICT_DATA_T_BIT_SIZE;
			}
		}
		
		return chunk_c;
	}
	else
	{
		if (balda_dict_chunk_is_single_char(chunk))
		{
			assert(index == 0);
			
			balda_char chunk_c = balda_dict_chunk_get_header_char(chunk);
			
			out_entry->ends_here = balda_dict_chunk_is_ends_here(chunk);
			
			int next_chunk_index = balda_dict_chunk_get_header_next_chunk(chunk);
			balda_dict_t* dict = chunk->dict;
			
			if (next_chunk_index != 0)
			{
				out_entry->next_chunk.dict = dict;
				out_entry->next_chunk.p = dict->mem_data + next_chunk_index;
				balda_dict_chunk_load(&out_entry->next_chunk);
			}
			else
			{
				out_entry->next_chunk.p = 0;
			}
			
			return chunk_c;
		}
		else
		{
			int n = balda_dict_chunk_get_header_count(chunk);
			assert(index >= 0 && index < n);
			
			balda_dict_t* dict = chunk->dict;
			balda_dict_data_t* m = chunk->p + 1 + index;
			
			balda_char chunk_c = balda_dict_chunk_get_entry_char(m);
			out_entry->ends_here = balda_dict_chunk_is_entry_ends_here(m);
				
			int next_chunk_index = balda_dict_chunk_get_entry_next_chunk(m);
			if (next_chunk_index != 0)
			{
				out_entry->next_chunk.dict = dict;
				out_entry->next_chunk.p = dict->mem_data + next_chunk_index;
				balda_dict_chunk_load(&out_entry->next_chunk);
			}
			else
			{
				out_entry->next_chunk.p = 0;
			}
			
			return chunk_c;
		}
	}
}

balda_bool balda_dict_chunk_get_entry(balda_dict_chunk_t* chunk, balda_char c, balda_dict_chunk_entry_t* out_entry)
{
	assert(chunk);
	assert(out_entry);
	
	//debug_printf(("balda_dict_chunk_get_entry(chunk: %d)", chunk->p));
	
	if (balda_dict_chunk_is_plain(chunk))
	{
		//debug_printf(("plain"));
		
		balda_char chunk_c = balda_dict_chunk_get_plain_char(chunk);
		if (chunk_c != c)
		{
			return 0;
		}
		
		int char_count = balda_dict_chunk_get_header_count(chunk);
		short next_index = chunk->plain_index + 1;
		if (next_index >= char_count)
		{
			out_entry->ends_here = balda_dict_chunk_is_ends_here(chunk);

			balda_dict_t* dict = chunk->dict;
			
			if (balda_dict_chunk_is_last_chunk(chunk))
			{
				out_entry->next_chunk.dict = dict;
				out_entry->next_chunk.p = 0;
			}
			else
			{
				int next_chunk_index = balda_dict_chunk_get_header_next_chunk(chunk);
				
				out_entry->next_chunk.dict = dict;
				out_entry->next_chunk.p = dict->mem_data + next_chunk_index;
				balda_dict_chunk_load(&out_entry->next_chunk);
			}
		}
		else
		{
			out_entry->ends_here = 0;

			balda_dict_t* dict = chunk->dict;
			balda_dict_data_t* chunk_p = chunk->p;
			balda_dict_data_t* chunk_plain_p = chunk->plain_p;
			short chunk_plain_off = chunk->plain_off;
			
			out_entry->next_chunk.dict = dict;
			out_entry->next_chunk.p = chunk_p;
			
			out_entry->next_chunk.plain_p = chunk_plain_p;
			out_entry->next_chunk.plain_off = chunk_plain_off + BALDA_DICT_CHAR_BIT_SIZE;
			out_entry->next_chunk.plain_index = next_index;
			
			if (out_entry->next_chunk.plain_off > BALDA_DICT_DATA_T_BIT_SIZE)
			{
				++out_entry->next_chunk.plain_p;
				out_entry->next_chunk.plain_off %= BALDA_DICT_DATA_T_BIT_SIZE;
			}
		}
		
		return 1;
	}
	else
	{
		if (balda_dict_chunk_is_single_char(chunk))
		{
			if (balda_dict_chunk_get_header_char(chunk) != c)
				return 0;
			
			out_entry->ends_here = balda_dict_chunk_is_ends_here(chunk);
			
			int next_chunk_index = balda_dict_chunk_get_header_next_chunk(chunk);
			balda_dict_t* dict = chunk->dict;
			
			if (next_chunk_index != 0)
			{
				out_entry->next_chunk.dict = dict;
				out_entry->next_chunk.p = dict->mem_data + next_chunk_index;
				balda_dict_chunk_load(&out_entry->next_chunk);
			}
			else
			{
				out_entry->next_chunk.p = 0;
			}
			
			return 1;
		}
		else
		{
			// binary search
			int n = balda_dict_chunk_get_header_count(chunk);
			balda_dict_data_t* l = chunk->p + 1;
			balda_dict_data_t* r = l + n - 1;
			balda_dict_data_t* m = 0;
			balda_char mc = 0;

			while (l <= r)
			{
				m = l + (r - l) / 2;

				if ((mc = balda_dict_chunk_get_entry_char(m)) == c)
					break;

				if (mc > c)
					r = m - 1;
				else
					l = m + 1;

				m = (l + n/2);
			}
			
			if (mc == c)
			{
				balda_dict_t* dict = chunk->dict;
				
				// found
				out_entry->ends_here = balda_dict_chunk_is_entry_ends_here(m);
					
				int next_chunk_index = balda_dict_chunk_get_entry_next_chunk(m);
				if (next_chunk_index != 0)
				{
					out_entry->next_chunk.dict = dict;
					out_entry->next_chunk.p = dict->mem_data + next_chunk_index;
					balda_dict_chunk_load(&out_entry->next_chunk);
				}
				else
				{
					out_entry->next_chunk.p = 0;
				}
				
				return 1;
			}
			
			// not found
			return 0;
		}
	}
	
	return 0;
}
