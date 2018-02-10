#include "trie.h"
#include "util.h"

using namespace std;

Trie::Trie() {
}

Trie::~Trie() {
  for(auto &p : children) {
    delete p.second;
  }
}

bool Trie::terminal() {
  return is_terminal;
}

Trie* Trie::from_dictionary(const string& fname) {
  ifstream myfile(fname);
  assert(myfile.is_open());

  vector<string> words;
  string line;
  while (getline(myfile, line)) {
    const string& word = Util::trim(line);
    if(word.size() != 0) {
      words.push_back(word);
    }
  }
  myfile.close();
  return Trie::from_array(words);
}

Trie* Trie::from_array(const vector<string>& words) {
  Trie* root = new Trie();
  for(auto &word : words) {
    Trie* node = root;
    for(int i = 0; i < word.size(); i++) {
      char c = toupper(word[i]);
      Trie* child = node->get_child(c);
      if(child != nullptr) {
        node = child;
      } else {
        Trie* next_node = new Trie();
        node->children[c] = next_node;
        node = next_node;
      }
    }
    node->add_word(word);
  }
  return root;
}

Trie* Trie::get_child(char c) {
  return children[toupper(c)];
}

string Trie::get_word() {
  assert(terminal());
  return word;
}

bool Trie::exists(const string& str) {
  Trie* node = this;
  for(int i = 0; i < str.size(); i++) {
    if(node == nullptr)
      return false;
    node = node->get_child(str[i]);
  }
  return true;
}

void Trie::print_nodes() {
  print_nodes_(0);
}

void Trie::add_word(const string& word) {
  is_terminal = true;
  this->word = word;
}

void Trie::print_nodes_(int depth) {
  const string& space = string(depth, ' ');
  if(terminal())
    cout << space << "terminal" << endl;
  for(auto &p : children) {
    cout << space << p.first << "-child:" << endl;
    p.second->print_nodes_(depth + 1);
  }
}
