#include <iostream>
#include <exception>

#include "rt.h"
#include "flag.h"

int main(int argc, char** argv) {
  unsigned number = 2;
  std::string dir = "";
  std::string prefix = "file";
  bool help = false;
  bool version = false;

  flag::Set parser("rexsplit");
  parser.uint_var(&number, "c", "number");
  parser.string_var(&dir, "d", "directory");
  parser.string_var(&prefix, "p", "prefix");
  parser.bool_var(&help, "h", "help");
  parser.bool_var(&version, "v", "version");
  try {
    parser.parse(argc, argv);
  } catch(std::exception &e) {
    std::cerr << e.what() << std::endl;
    std::cerr << parser.usage() << std::endl;
    return 1;
  }

  if (help) {
    std::cout << parser.help();
    std::cout << std::endl;
    return 0;
  }
  if (version) {
    std::cout << parser.program() << " " << "0.1" << std::endl;
    return 0;
  }
  try {
    rt::splitter split{dir, number, prefix};
    split.split(parser.arguments());
  } catch (std::exception &e) {
    std::cerr << "unexpected error: " << e.what() << std::endl;
    return 2;
  }
}
