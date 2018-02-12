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
      "wiki/metamorphosis.txt"
    };

    string dictionary_fname;

    string training_fname;

    TrainingHandler(const string& dict_fname);

    ~TrainingHandler();

    long long bigram_key(int i, int j);

    int word_to_index(const string& w);

    float get_connection_prob(const string& w1, const string& w2);

    float get_freq_prob(const string& w);

  private:
    typedef unordered_map<long long, float> bimap;

    const string training_prefix = "training_";
    
    vector<string> dictionary;
    
    unordered_map<string, int> dictionary_table;

    bimap connection_prob;

    vector<float> freq_prob;
    
    void restore_training_data();
    
    bool valid_word(const string& word);
    
    void init_dictionary(const string& fname);
    
    static vector<string> split_to_sentences(const string& str);

    static vector<string> split_to_words(const string& str);

    void apply_words_frequency(const vector<string>& words,
        bimap* connection_freq, vector<int>* freq);
    
    void read_frequency_on_line(const string& line,
        bimap* connection_freq, vector<int>* freq);
   
    void read_frequency_in_file(const string& doc_filename,
        bimap* connection_freq, vector<int>* freq);
   
    void output_training_data();

    void dump();
    
    void training();
};
