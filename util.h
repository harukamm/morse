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

  private:
    static const string whitespaces;
};
