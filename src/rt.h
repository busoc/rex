#ifndef __REX_RT__
#define __REX_RT__

#include <iomanip>
#include <fstream>
#include <random>
#include <vector>
#include <filesystem>
#include "ever.h"

namespace fs = std::filesystem;

namespace rt {
  struct poze {
    unsigned siz{0};
    unsigned pos{0};
  };

  struct coze {
    unsigned total{0};
    unsigned invalid{0};
    unsigned size{0};
    fs::path file;

    bool is_valid() {
      return invalid == 0;
    }
  };

  void print_coze(const coze &z);

  class splitter {
  public:
    splitter(fs::path dir, unsigned count, std::string prefix = "file");
    ~splitter();
    void split(std::vector<std::string> args);
    void split(std::string file);
  private:
    std::vector<std::ofstream> files;
    std::mt19937 rng;
    std::uniform_int_distribution<> distrib;
  };

  class shuffler {
  public:
    shuffler(fs::path dir);
    void shuffle(fs::path file);
  private:
    std::mt19937 rng;
    fs::path dir;

    std::vector<poze> index(std::ifstream &in);
    void shuffle(std::ofstream &out, std::ifstream &in, std::vector<poze> pos);
  };
}

#endif
