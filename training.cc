#include "training.h"
#include "util.h"

#define NOT_FOUND (-1)

using namespace std;

const string training_prefix = "training_";

TrainingHandler::TrainingHandler(const string& dict_fname) {
  dictionary_fname = dict_fname;
  training_fname = training_prefix + Util::replace_chars(dict_fname, "/", '_');
  
  init_dictionary(dict_fname);
  restore_training_data();
}

TrainingHandler::~TrainingHandler() {
}

long long TrainingHandler::bigram_key(int i, int j) {
  return 1LL * i * dictionary.size() + j;
}

int TrainingHandler::word_to_index(const string& w) {
  const string& word = Util::to_upper(w);
  if(valid_word(word)) {
    return dictionary_table[word];
  }
  return NOT_FOUND;
}

float TrainingHandler::get_connection_prob(const string& w1, const string& w2) {
  int i = word_to_index(w1);
  int j = word_to_index(w2);
  if(i == NOT_FOUND || j == NOT_FOUND)
    return 0.0;
  long long k = bigram_key(i, j);
  return connection_prob[k];
}

float TrainingHandler::get_freq_prob(const string& w) {
  int i = word_to_index(w);
  if(i == NOT_FOUND)
    return 0.0;
  return freq_prob[i];
}

void TrainingHandler::restore_training_data() {
  training();
  //output_training_data(training_data);
}

bool TrainingHandler::valid_word(const string& word) {
  return dictionary_table.find(word) != dictionary_table.end();
}

void TrainingHandler::init_dictionary(const string& fname) {
  ifstream myfile(fname);
  assert(myfile.is_open());

  string line;
  int i = 0;
  while(getline(myfile, line)) {
    const string& word = Util::to_upper(Util::trim(line));
    if(word.size() == 0)
      continue;
    if(dictionary_table.find(word) != dictionary_table.end())
      continue;
    dictionary.push_back(word);
    dictionary_table[word] = i;
    i++;
  }
  myfile.close();
}

void TrainingHandler::apply_words_frequency(const vector<string>& words,
    bimap* connection_freq, vector<int>* freq) {
  for(int i = 1; i < words.size(); i++) {
    const string& w1 = words[i - 1];
    const string& w2 = words[i];
    int a = word_to_index(w1);
    int b = word_to_index(w2);
    if(a != NOT_FOUND && b != NOT_FOUND) {
      long long k = bigram_key(a, b);
      (*connection_freq)[k]++;
    }
  }
  for(int i = 0; i < words.size(); i++) {
    const string& w = words[i];
    int a = word_to_index(w);
    if(a != NOT_FOUND) {
      (*freq)[a]++;
    }
  }
}

void TrainingHandler::read_frequency_on_line(const string& line,
    bimap* connection_freq, vector<int>* freq) {
  const vector<string>& sentences = Util::split_by_separates(line, "!?.,\t");
  for(auto &sentence : sentences) {
    const vector<string>& words = Util::split_by_separates(sentence, " ");
    apply_words_frequency(words, connection_freq, freq);
  }
}

void TrainingHandler::read_frequency_in_file(const string& doc_filename,
    bimap* connection_freq, vector<int>* freq) {
  ifstream myfile(doc_filename);
  assert(myfile.is_open());

  string sentence;
  string line;
  while(!myfile.eof()) {
    // Continue to read the line until "!?.," appears
    while(getline(myfile, line)) {
      sentence += line;
      if(line.find_first_of("!?.,") != string::npos)
        break;
    }
    read_frequency_on_line(sentence, connection_freq, freq);
    sentence.clear();
  }
  myfile.close();
}

void TrainingHandler::output_training_data() {
  ofstream output(training_fname);
  for(int i = 0; i < dictionary.size(); i++) {
    float freq = freq_prob[i];
    const string& word = dictionary[i];
    output << i << "," << word << "," << freq << endl;
  }
  for(auto &p : connection_prob) {
    int i = p.first / dictionary.size();
    int j = p.first % dictionary.size();
    output << i << "," << j << "," << p.second << endl;
  }
  output.close();
}

void TrainingHandler::dump() {
  vector<pair<float, int> > freq_copy(dictionary.size());
  for(int i = 0; i < dictionary.size(); i++) {
    freq_copy[i].first = freq_prob[i];
    freq_copy[i].second = i;
  }
  sort(freq_copy.begin(), freq_copy.end());
  for(int d = 0; d < 10; d++) {
    const pair<float, int>& p = freq_copy[freq_copy.size() - 1 - d];
    int i = p.second;
    cout << dictionary[i] << ": " << p.first << endl;
  }
  vector<pair<float, long long> > bi_copy(connection_prob.size());
  int x = 0;
  for(auto &p : connection_prob) {
    bi_copy[x].first = p.second;
    bi_copy[x].second = p.first;
    x++;
  }
  sort(bi_copy.begin(), bi_copy.end());
  for(int d = 0; d < 10; d++) {
    const pair<float, long long>& p = bi_copy[bi_copy.size() - 1 - d];
    int i = p.second / dictionary.size();
    int j = p.second % dictionary.size();
    cout << dictionary[i] << ", " << dictionary[j] << ": " << p.first << endl;
  }
}

void TrainingHandler::training() {
  int size = dictionary.size();
  vector<int> freq(size, 0);
  bimap connection_freq;
  for(int i = 0; i < training_sources.size(); i++) {
    read_frequency_in_file(training_sources[i], &connection_freq, &freq);
  }
  cout << "calc done!" << endl;
  freq_prob.resize(size, 0);
  long long freq_sum = 0LL;
  for(int i = 0; i < size; i++)
    freq_sum += freq[i];
  for(int i = 0; freq_sum != 0 && i < size; i++)
    freq_prob[i] = (float) freq[i] / freq_sum;
  cout << "freq done!" << endl;
  if(freq_sum < 2)
    return;
  for(auto &bifreq : connection_freq) {
    connection_prob[bifreq.first] = bifreq.second / (freq_sum - 1);
  }
  cout << "conn done!" << endl;
  //dump();
}
