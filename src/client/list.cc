#include <iostream>
#include <exception>

#include "hrdl.h"
#include "pdh.h"
#include "pth.h"
#include "flag.h"


int main(int argc, char** argv) {
  std::string type = "";
  bool help = false;
  bool version = false;

  flag::Set parser("rtlist");
  parser.string_var(&type, "t", "type");
  parser.bool_var(&help, "h", "help");
  parser.bool_var(&version, "v", "version");
  try {
    parser.parse(argc, argv);
  } catch(std::exception &e) {
    std::cerr << e.what() << std::endl;
    std::cerr << parser.usage() << std::endl;
    return 1;
  } catch(...) {
    std::cerr << "unexpected error while parsing arguments" << std::endl;
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

  auto args = parser.arguments();
  int count = 0;
  if (type == "hrdl" || type == "vmu") {
    count = hrdl::list_files(args);
  } else if (type == "pth" || type == "pt" || type == "tm") {
    count = pth::list_files(args);
  } else if (type == "pdh" || type == "pd") {
    count = pdh::list_files(args);
  } else {
    std::cerr << "unsupported packet type " << type << std::endl;
    return 1;
  }
  std::cout << count << " packets" << std::endl;
}
