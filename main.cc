#include <assert.h>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <utility>
#include <vector>
#include <map>
#include "trie.h"
#include "training.h"
#include "util.h"

using namespace std;

const string dictionary_filename = "/usr/share/dict/words";
//const string dictionary_filename = "dictionary/dictionary.txt";
//const string dictionary_filename = "dictionary/dictionary_from_book.txt";

unordered_map<string, char> decode_table;
unordered_map<char, string> encode_table;
int max_morse_length = 4;

void init_table() {
  decode_table[".-"  ] = 'A';
  decode_table["-..."] = 'B';
  decode_table["-.-."] = 'C';
  decode_table["-.." ] = 'D';
  decode_table["."   ] = 'E';
  decode_table["..-."] = 'F';
  decode_table["--." ] = 'G';
  decode_table["...."] = 'H';
  decode_table[".."  ] = 'I';
  decode_table[".---"] = 'J';
  decode_table["-.-" ] = 'K';
  decode_table[".-.."] = 'L';
  decode_table["--"  ] = 'M';
  decode_table["-."  ] = 'N';
  decode_table["---" ] = 'O';
  decode_table[".--."] = 'P';
  decode_table["--.-"] = 'Q';
  decode_table[".-." ] = 'R';
  decode_table["..." ] = 'S';
  decode_table["-"   ] = 'T';
  decode_table["..-" ] = 'U';
  decode_table["...-"] = 'V';
  decode_table[".--" ] = 'W';
  decode_table["-..-"] = 'X';
  decode_table["-.--"] = 'Y';
  decode_table["--.."] = 'Z';

  encode_table['A'] = ".-"  ;
  encode_table['B'] = "-...";
  encode_table['C'] = "-.-.";
  encode_table['D'] = "-.." ;
  encode_table['E'] = "."   ;
  encode_table['F'] = "..-.";
  encode_table['G'] = "--." ;
  encode_table['H'] = "....";
  encode_table['I'] = ".."  ;
  encode_table['J'] = ".---";
  encode_table['K'] = "-.-" ;
  encode_table['L'] = ".-..";
  encode_table['M'] = "--"  ;
  encode_table['N'] = "-."  ;
  encode_table['O'] = "---" ;
  encode_table['P'] = ".--.";
  encode_table['Q'] = "--.-";
  encode_table['R'] = ".-." ;
  encode_table['S'] = "..." ;
  encode_table['T'] = "-"   ;
  encode_table['U'] = "..-" ;
  encode_table['V'] = "...-";
  encode_table['W'] = ".--" ;
  encode_table['X'] = "-..-";
  encode_table['Y'] = "-.--";
  encode_table['Z'] = "--..";
}

// - decode ---------------------------
bool decodable1(const string& s) {
  return decode_table.find(s) != decode_table.end();
}

char decode1(const string& s) {
  if(decodable1(s))
    return decode_table[s];
  assert(false);
}

int min(int a, int b) {
  return a < b ? a : b;
}

vector<pair<int, string> > decode_prefix(const string& str, Trie* tr) {
  vector<pair<int, string> > result;
  queue<pair<int, Trie*> > q;
  q.push(make_pair(0, tr));
  while(!q.empty()) {
    const pair<int, Trie*>& p = q.front();
    int index = p.first;
    Trie* node = p.second;
    q.pop();
    if(node->terminal())
      result.push_back(make_pair(index, node->get_word()));
    if(str.size() <= index)
      continue;

    int max_length = min(max_morse_length, str.size() - index);
    for(int len = 1; len <= max_length; len++) {
      const string& code = str.substr(index, len);
      if(decodable1(code)) {
        Trie* next_node = node->get_child(decode1(code));
        if(next_node != nullptr) {
          q.push(make_pair(index + len, next_node));
        }
      }
    }
  }
  return result;
}

vector<vector<pair<int, string> > > find_all_candidates(const string& str, Trie* dict) {
  vector<vector<pair<int, string> > > result(str.size());
  vector<bool> visited(str.size(), false);
  queue<int> q;
  q.push(0);
  while(!q.empty()) {
    int i = q.front();
    q.pop();
    if(str.size() <= i || visited[i])
      continue;
    const vector<pair<int, string> >& ps = decode_prefix(str.substr(i), dict);
    for(auto p : ps) {
      int code_length = p.first;
      q.push(i + code_length);
    }
    visited[i] = true;
    result[i] = ps;
  }
  return result;
}

// - decode a sentence ----------------
struct node_t {
  float score;
  int word_count;
  pair<int, int> prev;
  string word;

  bool operator<(const node_t& a) const {
    return score == a.score ? (word_count < a.word_count) : score > a.score;
  }
};

vector<string> decode_sentence(const string& str, Trie* dict, TrainingHandler* training) {
  const vector<vector<pair<int, string> > >& cand_words =
      find_all_candidates(str, dict);
  vector<vector<node_t> > dp(str.size() + 1);
  node_t init = { 0.0, 0, make_pair(-1, -1), "" };
  dp[0].push_back(init);

  for(int i = 0; i < str.size(); i++) {
    sort(dp[i].begin(), dp[i].end());
    for(int k = 0; k < 1000; k++) {
      if(dp[i].size() <= k)
        break;
      const node_t& node = dp[i][k];
      const string& prev_word = node.word;
      for(auto &p : cand_words[i]) {
        int code_length = p.first;
        const string& word = p.second;
        float freq_prob = training->get_freq_prob(word);
        float conn_prob = training->get_connection_prob(prev_word, word);
        float score;
        if(prev_word != "" && conn_prob == 0)
          score = -1;
        else
          score = conn_prob; // * freq_prob;
        node_t next_node = {
            node.score + score, node.word_count + 1,
            make_pair(i, k), word };
        dp[i + code_length].push_back(next_node);
      }
    }
  }
  sort(dp[str.size()].begin(), dp[str.size()].end());
  vector<vector<string> > best_sentences;
  for(int k = 0; k < 10; k++) {
    if(dp[str.size()].size() <= k)
      break;
    vector<string> sentence;
    node_t node = dp[str.size()][k];
    while(node.prev.first != -1) {
      sentence.push_back(node.word);
      node = dp[node.prev.first][node.prev.second];
    }
    reverse(sentence.begin(), sentence.end());
    cout << (k + 1) << "-the (";
    cout << dp[str.size()][k].score << "): ";
    Util::print_vec(sentence);
    best_sentences.push_back(sentence);
  }
  assert(best_sentences.size() != 0);
  return best_sentences[0];
}

// - encode ---------------------------
string encode1(char c) {
  c = toupper(c);
  if(encode_table.find(c) != encode_table.end())
    return encode_table[c];
  assert(false);
}

string encode_str(const string& s) {
  string result;
  for(int i = 0; i < s.size(); i++)
    result += encode1(s[i]);
  return result;
}

string encode_vec(const vector<string>& vec) {
  string result;
  for(int i = 0; i < vec.size(); i++) {
    result += encode_str(vec[i]);
  }
  return result;
}

void test_all() {
  assert(false);
}
// ------------------------------------

int main() {
  init_table();
  Trie* tr = Trie::from_dictionary(dictionary_filename);
  cout << "trie done" << endl;
  TrainingHandler* training = new TrainingHandler(dictionary_filename);
  cout << "training done" << endl;
//  string input = "sheissoangryyouhavetobeaway";
//  string input = "hesaidtohimselfthathewouldnotspeak";
//  string sentence = "I want to assign this to a string";
//  string sentence = "Tom pried his mouth open";
//  string sentence = "Then he went tearing around the house again";
  string sentence = "I really like you";
  string input = Util::remove_whitespaces(sentence);
  const string& code = encode_str(input);
  cout << code << endl;
  const vector<string>& result = decode_sentence(code, tr, training);
  Util::print_vec(result);
  const string& encoded = encode_vec(result);
  assert(code == encoded);
  delete tr;
  delete training;
}
