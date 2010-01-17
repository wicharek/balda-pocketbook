#include <string>
#include <fstream>
#include <iostream>

extern "C"
{
#include <balda_dict.h>
}

extern balda_dict_data_t balda_dict_data[];

using namespace std;

int main(int argc, char** argv)
{
	balda_dict_t dict;
	balda_dict_init(&dict, balda_dict_data);
	
	balda_char word[128];
	string line;
	
	ifstream correct_file("correct_words.txt");
	cout << "Checking CORRECT words from \"correct_words.txt\"" << endl;
	int total_correct = 0;
	int total_correct_success = 0;
	while (getline(correct_file, line))
	{
		balda_char_from_utf8(line.c_str(), word, 64);
		++total_correct;
		cout << "Checking \"" << line << "\": ";
		if (!balda_dict_contains_word(&dict, word))
		{
			cout << "false (FAILED)" << endl;
		}
		else
		{
			++total_correct_success;
			cout << "true (SUCCESS)" << endl;
		}
	}
	cout << endl;

	ifstream incorrect_file("incorrect_words.txt");
	cout << "Checking INCORRECT words from \"incorrect_words.txt\"" << endl;
	int total_incorrect = 0;
	int total_incorrect_success = 0;
	while (getline(incorrect_file, line))
	{
		balda_char_from_utf8(line.c_str(), word, 64);
		++total_incorrect;
		cout << "Checking \"" << line << "\": ";
		if (!balda_dict_contains_word(&dict, word))
		{
			++total_incorrect_success;
			cout << "false (SUCCESS)" << endl;
		}
		else
		{
			cout << "true (FAILED)" << endl;
		}
	}
	cout << endl;


	cout << "Peeking random words with 5 letters" << endl;
	char utf8_word[16];

	for (int i=0; i<20; ++i)
	{
		balda_bool ret = balda_dict_peek_word_5(&dict, word);
		if (ret)
		{
			balda_char_to_utf8(word, utf8_word, 16);
			cout << "[" << i << "]: \"" << utf8_word << "\"" << endl;
		}
		else
			cout << "[" << i << "]: FAILED" << endl;
	}

	balda_dict_free(&dict);

	cout << "Total:" << endl;
	cout << " correct words:   " << total_correct_success << " of " << total_correct << endl;
	cout << " incorrect words: " << total_incorrect_success << " of " << total_incorrect << endl;

	if (total_correct_success < total_correct || total_incorrect_success < total_incorrect)
	{
		cout << "Result: FAILED";
		return 1;
	}

	cout << "Result: PASSED";
	return 0;
}
