#ifndef __REX_PDH__
#define __REX_PDH__

#include <ostream>
#include <fstream>
#include <vector>
#include <optional>
#include <filesystem>
#include "util.h"

namespace pdh {

  namespace fs = std::filesystem;

  enum class value_type: unsigned char {
    int32 = 1,
    float64 = 2,
    binary8 = 3,
    reference = 4,
    string8 = 5,
    int64 = 6,
    decimal = 7,
    real = 8,
    time = 9,
    datetime = 10,
    varchar = 11,
    binary = 12,
    bit = 13,
  };

  enum class value_state: unsigned char {
    empty = 0,
    same = 1,
    newv = 2,
    latest = 3,
    error = 4,

  };

  std::ostream& operator<<(std::ostream& os, value_state state);
  std::ostream& operator<<(std::ostream& os, value_type type);

  #pragma pack(push, 1)
  struct umi_header {
    unsigned size;
    value_state state;
    unsigned orbit;
    char code[6];
    value_type type;
    unsigned short unit;
    util::time::time5 time;
    unsigned short length;

    char source() const;
    unsigned long long uniq_id() const;

    bool operator<(const umi_header &other) const;
    bool operator<=(const umi_header &other) const;
    bool operator>(const umi_header &other) const;
    bool operator>=(const umi_header &other) const;
    bool operator==(const umi_header &other) const;
    bool operator!=(const umi_header &other) const;

    bool less(const umi_header &other) const;
    bool equal(const umi_header &other) const;

    int remain() const;
    fs::path location() const;
  };
  #pragma pack(pop)

  int read_file(std::string file);
  int list_files(std::vector<std::string> files);
  int merge_file(fs::path dir, fs::path file, bool keep = false);
  int merge_files(fs::path dir, std::vector<std::string> files, bool keep = false);

  class reader {
  public:
    reader(fs::path file);
    ~reader();

    std::optional<umi_header> next();
    void skip(int size);
    void copy_to(std::ofstream &out, int size);
  private:
    std::ifstream in;
    char buffer[4096];
  };

  class merger {
  public:
    merger(fs::path file, bool keep = false);
    int merge(fs::path dir);
  private:
    reader source;
    bool keepall;

    void merge(fs::path file, const umi_header &curr);
    void copy_packet(std::ofstream &out, umi_header curr);
  };
}

#endif
