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
      "wiki/les_miserables.txt"
    };

    const string training_dir = "training/";

    const string training_prefix = "training_";

    string dictionary_fname;

    string training_fname;

    TrainingHandler();

    ~TrainingHandler();

    float get_pair_prob(const string& w1, const string& w2);

    float get_mono_prob(const string& w);

    vector<string> get_dictionary();

  private:
    const long long base_number = INT_MAX;

    typedef unordered_map<long long, float> bimap;

    vector<string> dictionary;
    
    unordered_map<string, int> dictionary_table;

    long long total_freq;

    bimap pair_prob;

    vector<float> mono_prob;

    bool valid_word(const string& word);

    long long pair_key(int i, int j);

    int word_to_index(const string& w);

    int register_to_dictionary(const string& w);

    void apply_words_frequency(const vector<string>& words,
        bimap* pair_freq, unordered_map<int, int>* mono_freq);
    
    void read_frequency_on_line(const string& line,
        bimap* pair_freq, unordered_map<int, int>* mono_freq);
   
    void read_frequency_in_file(const string& doc_filename,
        bimap* pair_freq, unordered_map<int, int>* mono_freq);
   
    void output_training_data();

    void restore_training_data();

    void dump();
    
    void training();
};
