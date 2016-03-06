#include "config.hpp"
#include "setup.hpp"
#include "Key.hpp"
#include "Scheduler.hpp"
#include "CodeBreaker.hpp"
#include <sstream>
#include <algorithm>
#include <fstream>
#include <iostream>
using namespace std;
using namespace cipher;

default_random_engine engine(random_device{}());
vector<words> vocabularies = load_dictionaries(RESOURCE("plaintext_dictionary.txt"));

string pick(const words &vocabulary) {
  uniform_int_distribution<size_t> uni(0, vocabulary.size() - 1);
  return vocabulary[uni(engine)];
}

/**
 * double size the words and keep the distribution of
 * 1. word length distribution
 * 2. letter distribution (defined by 103 slots)
 * not varied much
 */
void double_size(words &vocabulary, words &initialVocabulary) {
	string dictionary = "";
	for(size_t i = 0; i < initialVocabulary.size(); i++){
		dictionary.append(initialVocabulary[i]); 
	}
	map<size_t, vector<int>> distribution;
	for(size_t i = 0; i < vocabulary.size(); i++){
		distribution[vocabulary[i].length()].push_back(i);
	}
	while(true){
		random_shuffle(dictionary.begin(),dictionary.end());
		string tempor = dictionary;
		bool failed = false;
		vector<string> newEntries;
		for(size_t i = 0; i < initialVocabulary.size(); i++){
			size_t x = initialVocabulary[i].length();
			string word = tempor.substr(0, x);
			tempor = tempor.substr(x);
			newEntries.push_back(word);
			for(size_t y = 0; y < distribution[x].size(); y++){
				if(word == vocabulary[distribution[x][y]]){
					failed = true;
				}			
			}
		}
		if(!failed){
			for(size_t i = 0; i < newEntries.size(); i++){
				vocabulary.push_back(newEntries[i]);
			}
			break;
		}
	}
}

float count_time_in_sec(const words &vocabulary, size_t word_num = 100) {
  words plains;
  for (size_t i = 0; i < word_num; i++) plains.push_back(pick(vocabulary));
  string ciphertext = uniform.encode(plains, Key());
  clock_t start = clock();
  CodeBreaker().solve(ciphertext);
  clock_t end = clock();
  return ((float)(end - start) / CLOCKS_PER_SEC);
}

int main(int, char **argv) {

  // do experiments on each original dictionary
  size_t dict_idx = 0;
  ofstream logFile;
  logFile.open("experimentslog.txt", ios::in | ios::out |ios::app);
  if(!logFile.is_open())
  { 
    cerr << "Bad File!" << endl;
    return -1;
  }

  logFile << "New Attempt\nTest 1!\n";
  for (words vocabulary : vocabularies) {
     words copyVocabulary = vocabulary; 
     words copyVocabularys = vocabulary;		
     size_t initSize = vocabulary.size();
    dict_idx++;

    // Experiment 1
    // little bit hack here
    float time_in_sec = 0;
    do {
      time_in_sec = count_time_in_sec(vocabulary);
      logFile << "Library " << dict_idx << ", " << time_in_sec << " seconds ," << vocabulary.size() << " words!" << endl;
      if(((vocabulary.size())/(initSize))+2 >= 26){
        break;
      }
	//cerr << "before" << endl;
      double_size(vocabulary, copyVocabulary);
      //cerr << "after" << endl;
    } while (time_in_sec < (60 * 2));

    // Experiment 2
    const size_t word_length = 10;
    string letters = "abcdefghijklmnopqrstuvwxyz";
    logFile << "Test 2\n";
	time_in_sec = count_time_in_sec(copyVocabulary);
	logFile << "Library " << dict_idx << ", " << time_in_sec << " seconds ," <<  " normal length words!" << endl;
    for(size_t i = 0; i < copyVocabulary.size(); i++){
	if(copyVocabulary[i].length() < word_length)
	{
		random_shuffle(letters.begin(), letters.end());
		copyVocabulary[i] += letters.substr(0, (word_length - copyVocabulary[i].length()));
	}
	else if(copyVocabulary[i].length() > word_length)
	{
		copyVocabulary[i] = copyVocabulary[i].substr(0, word_length);
	}
        
	
     }
	time_in_sec = count_time_in_sec(copyVocabulary);
        logFile << "Library " << dict_idx << ", " << time_in_sec << " seconds ," << word_length << " length words!" << endl;
	// Experiment 4
	logFile << "Test 4\n";
	int x = 1;
	do {
      time_in_sec = count_time_in_sec(copyVocabularys, 100*x);
      logFile << "Library " << dict_idx << ", " << time_in_sec << " seconds ," << 100*x << " words!" << endl;
      x++;
	if(x >= 100){break;}
      }while (time_in_sec < (60 * 2));
    } 
    fstream pinnapples;
    pinnapples.open("dictionary.txt");
    string nextword;
    words thedictionary;
    while(getline(pinnapples, nextword)){
		thedictionary.push_back(nextword);
	}
	for(words vocabulary:vocabularies)
	{
		float time_in_sec = count_time_in_sec(vocabulary);
		logFile << "Librarys, " << time_in_sec << " seconds ," << vocabulary.size() << " words!" << endl;
	}
	float time_in_sec = count_time_in_sec(thedictionary);
	logFile << "Dictionary, " << time_in_sec << " seconds ," << thedictionary.size() << " words!" << endl;
 logFile.close();
}
