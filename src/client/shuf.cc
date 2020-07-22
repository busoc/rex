#include <iostream>
#include <random>

#include "rt.h"
#include "flag.h"

int main(int argc, char** argv) {
  std::string dir = "";
  bool help = false;
  bool version = false;

  flag::Set parser("rexshuf");
  parser.string_var(&dir, "d", "directory");
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
  rt::shuffler shuf{dir};
  for (auto a: parser.arguments()) {
    shuf.shuffle(a);
  }
}
