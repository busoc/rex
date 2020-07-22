#include <iostream>
#include <filesystem>
#include "flag.h"
#include "hrdl.h"
#include "pth.h"
#include "pdh.h"

namespace fs = std::filesystem;

int main(int argc, char** argv) {
  std::string type = "";
  bool dry = false;
  bool keepall = false;
  bool help = false;
  bool version = false;

  flag::Set parser("rexmerge");
  parser.string_var(&type, "t", "type");
  parser.bool_var(&dry, "n", "dry-run");
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

  auto args = parser.arguments();
  if (type == "hrdl" || type == "vmu") {
    hrdl::check_files(args, dry);
  } else if (type == "pth" || type == "pt" || type == "tm") {
    pth::check_files(args, dry);
  } else if (type == "pdh" || type == "pd") {
    // pdh::check_files(args, dry);
  } else {
    std::cerr << "unsupported packet type " << type << std::endl;
    return 1;
  }
}
