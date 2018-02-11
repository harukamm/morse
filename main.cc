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

template <class T>
void print_vec(const vector<T>& vec) {
  for(int i = 0; i < vec.size(); i++) {
    cout << vec[i] << " ";
  }
  cout << endl;
}

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
struct info_t {
  int word_count;
  int prev;
  int begin;
  int end;
  string word;
  float score;

  bool operator<(const info_t& a) const {
    return score == a.score ? (word_count > a.word_count) : score > a.score;
  }

  int node_id(int base) const {
    return end * base + (end - begin);
  }
};

vector<string> decode_sentence(const string& str, Trie* dict, TrainingHandler* training) {
  const vector<vector<pair<int, string> > >& matched =
      find_all_candidates(str, dict);
  map<int, info_t> visited_node; // info keyed by node_id
  vector<info_t> end_node; // node_id keyed by end-index
  queue<info_t> q;
  info_t init = { 0, -1, 0, 0, "", 0 };
  q.push(init);
  int beam = 5;
  while(!q.empty()) {
    info_t info = q.front();
    q.pop();
    int node_id = info.node_id(str.size());
    if(visited_node.find(node_id) != visited_node.end())
      continue;
    visited_node[node_id] = info;
    assert(info.end <= str.size());
    if(str.size() == info.end) {
      end_node.push_back(info);
      continue;
    }
    const vector<pair<int, string> >& ps = matched[info.end];
    vector<info_t> candidates(ps.size());
    for(int i = 0; i < ps.size(); i++) {
      const pair<int, string>& p = ps[i];
      int code_length = p.first;
      const string& word = p.second;
      float conn_prob = training->get_connection_prob(info.word, word);
      float freq_prob = training->get_freq_prob(word);
      float score = conn_prob + freq_prob / 1000;
      info_t info2 = { info.word_count + 1, node_id,
          info.end, info.end + code_length, word, score };
      candidates[i] = info2;
    }
    sort(candidates.begin(), candidates.end());
    if(beam < candidates.size())
      candidates.erase(candidates.begin() + beam, candidates.end());
    for(auto &cand : candidates)
      q.push(cand);
  }
  assert(end_node.size() != 0);
  vector<string> result;
  sort(end_node.begin(), end_node.end());
  info_t end = end_node[0];
  for(int i = 0; i < end_node.size(); i++) {
    float a = training->get_freq_prob(end.word);
    float b = training->get_freq_prob(end_node[i].word);
    if(a < b)
      end = end_node[i];
  }
  int id = end.node_id(str.size());
  while(id != -1) { // prev must be list !
    assert(visited_node.find(id) != visited_node.end());
    info_t info = visited_node[id];
    result.push_back(info.word);
    id = info.prev;
  }
  reverse(result.begin(), result.end());
  return result;
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
  print_vec(result);
  const string& encoded = encode_vec(result);
  assert(code == encoded);
  delete tr;
  delete training;
}
