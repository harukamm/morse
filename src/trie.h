#include <assert.h>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>

using namespace std;

class Trie {
  public:
    Trie();

    ~Trie();

    bool terminal();

    static Trie* from_dictionary(const string& fname);

    static Trie* from_array(const vector<string>& words);

    Trie* get_child(char c);

    string get_word();

    bool exists(const string& str);

    void print_nodes();

  private:
    bool is_terminal;

    unordered_map<char, Trie*> children;

    string word;

    void add_word(const string& word);

    void print_nodes_(int depth);
};

