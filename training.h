#include <assert.h>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>

using namespace std;

class TrainingHandler {
  public:
    const vector<string> training_sources {
      "wiki/tom_sawyer.txt",
      "wiki/wizard_oz.txt",
      "wiki/metamorphosis.txt",
      "wiki/les_miserable.txt"
    };

    const string training_dir = "training/";

    const string training_prefix = "training_";

    string dictionary_fname;

    string training_fname;

    TrainingHandler();

    ~TrainingHandler();

    long long bigram_key(int i, int j);

    int word_to_index(const string& w);

    float get_connection_prob(const string& w1, const string& w2);

    float get_freq_prob(const string& w);

    vector<string> get_dictionary();

  private:
    const long long base_number = INT_MAX;

    typedef unordered_map<long long, float> bimap;

    vector<string> dictionary;
    
    unordered_map<string, int> dictionary_table;

    bimap connection_prob;

    vector<float> freq_prob;

    bool valid_word(const string& word);

    int register_to_dictionary(const string& w);

    void apply_words_frequency(const vector<string>& words,
        bimap* connection_freq, unordered_map<int, int>* freq);
    
    void read_frequency_on_line(const string& line,
        bimap* connection_freq, unordered_map<int, int>* freq);
   
    void read_frequency_in_file(const string& doc_filename,
        bimap* connection_freq, unordered_map<int, int>* freq);
   
    void output_training_data();

    void restore_training_data();

    void dump();
    
    void training();
};
