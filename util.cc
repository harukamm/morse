#include "util.h"

const string Util::whitespaces = " \t\f\v\n\r";

string Util::to_upper(const string& str) {
  char buf[str.size() + 1];
  for(int i = 0; i < str.size(); i++)
    buf[i] = toupper(str[i]);
  buf[str.size()] = '\0';
  return buf;
}

string Util::trim(const string& str) {
  int begin = str.find_first_not_of(Util::whitespaces);
  if(begin == string::npos) {
    return "";
  } else {
    int end = str.find_last_not_of(whitespaces) + 1;
    return str.substr(begin, end - begin);
  }
}

string Util::remove_chars(const string& str, const string& sep) {
  char buf[str.size()];
  copy_if(str.begin(), str.end(), buf,
      [](char c) { return c != ' '; });
  return buf;
}

string Util::remove_whitespaces(const string& str) {
  return Util::remove_chars(str, whitespaces);
}

vector<string> Util::split_by_separates(const string& str, const string& sep) {
  int begin = str.find_first_not_of(sep);
  vector<string> result;
  while(begin != string::npos) {
    int end = str.find_first_of(sep, begin);
    result.push_back(str.substr(begin, end - begin));
    begin = str.find_first_not_of(sep, end);
  }
  return result;
}
