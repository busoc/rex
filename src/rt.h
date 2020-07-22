#ifndef __REX_RT__
#define __REX_RT__

#include <fstream>
#include <random>
#include <vector>
#include <filesystem>
#include "ever.h"

namespace fs = std::filesystem;

namespace rt {
  struct poze {
    unsigned siz;
    unsigned pos;

    ever::instant time;
    unsigned id;
    unsigned seq;
  };

  // class packet_info {
  // public:
  //   virtual bool operator<(const packet_info &other) const = 0;
  //   virtual bool operator==(const packet_info &other) const = 0;
  //
  //   virtual bool less(const packet_info &other) const = 0;
  //   virtual bool equal(const packet_info &other) const = 0;
  //
  //   virtual fs::path location() const = 0;
  //   virtual int remain() const = 0;
  // };

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
