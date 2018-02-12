#include <vector>
#include <iostream>

using namespace std;

class Util {
  public:
    static string to_upper(const string& str);

    static string to_lower(const string& str);

    static string trim(const string& str);

    static string remove_chars(const string& str, const string& sep);

    static string remove_whitespaces(const string& str);

    static vector<string> split_by_separates(const string& str, const string& sep);

    template <class T>
    static inline T pick_last(const vector<T>& vec) {
      T last;
      if(vec.size() == 0)
        return last; // Returns default value
      return vec[vec.size() - 1];
    }

    template <class T>
    static inline void print_vec(const vector<T>& vec) {
      if(vec.size() == 0)
        return;
      for(int i = 0; i < vec.size(); i++)
        cout << vec[i] << " ";
      cout << endl;
    }

  private:
    static const string whitespaces;
};
