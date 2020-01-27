#include "replxx.hxx"
#include "util.h"
#include <regex>
#include <string>
#include <vector>
using Replxx = replxx::Replxx;
using namespace std;
void hook_color(string const &context, Replxx::colors_t &colors,
                vector<pair<string, Replxx::Color>> const &regex_color) {
  for (auto const &e : regex_color) {
    size_t pos{0};
    string str = context;
    smatch match;
    while (regex_search(str, match, regex(e.first))) {
      string c{match[0]};
      string prefix(match.prefix().str());
      pos += utf8str_codepoint_len(prefix.c_str(),
                                   static_cast<int>(prefix.length()));
      int len(utf8str_codepoint_len(c.c_str(), static_cast<int>(c.length())));
      for (int i = 0; i < len; ++i) {
        colors.at(pos + i) = e.second;
      }
      pos += len;
      str = match.suffix();
    }
  }
}
int main() {
  vector<pair<string, Replxx::Color>> regex_color{
      {"\\`", Replxx::Color::BRIGHTCYAN},
      {"\\'", Replxx::Color::BRIGHTBLUE},
      {"\\\"", Replxx::Color::BRIGHTBLUE},
      {"\\-", Replxx::Color::BRIGHTBLUE},
      {"\\+", Replxx::Color::BRIGHTBLUE},
      {"\\=", Replxx::Color::BRIGHTBLUE},
      {"\\/", Replxx::Color::BRIGHTBLUE},
      {"\\*", Replxx::Color::BRIGHTBLUE},
      {"\\^", Replxx::Color::BRIGHTBLUE},
      {"\\.", Replxx::Color::BRIGHTMAGENTA},
      {"\\(", Replxx::Color::BRIGHTMAGENTA},
      {"\\)", Replxx::Color::BRIGHTMAGENTA},
      {"\\[", Replxx::Color::BRIGHTMAGENTA},
      {"\\]", Replxx::Color::BRIGHTMAGENTA},
      {"\\{", Replxx::Color::BRIGHTMAGENTA},
      {"\\}", Replxx::Color::BRIGHTMAGENTA},
      {"[\\-|+]{0,1}[0-9]+", Replxx::Color::YELLOW},          // integers
      {"[\\-|+]{0,1}[0-9]*\\.[0-9]+", Replxx::Color::YELLOW}, // decimals
      {"[\\-|+]{0,1}[0-9]+e[\\-|+]{0,1}[0-9]+",
       Replxx::Color::YELLOW},                 // scientific notation
      {"\".*?\"", Replxx::Color::BRIGHTGREEN}, // double quotes
      {"\'.*?\'", Replxx::Color::BRIGHTGREEN}, // single quotes
  };
  Replxx rx;
  string history_file{"./replxx_history.txt"};
  rx.history_load(history_file);
  using namespace placeholders;
  rx.set_highlighter_callback(bind(&hook_color, _1, _2, cref(regex_color)));
  rx.set_word_break_characters(" \t.,-%!;:=*~^'\"/?<>|[](){}");
  string prompt{"\x1b[1;32mcshell\x1b[0m> "};
  while (true) {
    char const *cinput{nullptr};
    do {
      cinput = rx.input(prompt);
    } while ((cinput == nullptr) && (errno == EAGAIN));
    if (cinput == nullptr) {
      break;
    }
    string input{cinput};
    if (input.empty()) {
      continue;
    } else {
      rx.print("%s\n", input.c_str());
      rx.history_add(input);
      continue;
    }
  }
  rx.history_save(history_file);
  return 0;
}
