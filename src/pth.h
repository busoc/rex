#ifndef __REX_PTH__
#define __REX_PTH__

#include <vector>
#include <filesystem>
#include <fstream>
#include "util.h"

namespace pth {
  #pragma pack(push, 1)
  struct header {
    unsigned size;
    unsigned char flag;
    util::time::time5 time;
  };

  struct esa_header {
    util::time::time5 time;
    unsigned char flag;
    unsigned sid;
  };

  struct ccsds_header {
    unsigned short pid;
    unsigned short segment;
    unsigned short length;

    int apid() const;
    int segmentation() const;
    int sequence() const;
    int size() const;
  };
  #pragma pack(pop)

  namespace fs = std::filesystem;

  int read_file(std::string);
  int list_files(std::vector<std::string> files);

  int merge_file(fs::path dir, fs::path file, bool keep = false);
  int merge_files(fs::path dir, std::vector<std::string> files, bool keep = false);

  struct packet_info {
    header archive;
    ccsds_header ccsds;
    esa_header esa;

    bool operator<(const packet_info &other) const;
    bool operator<=(const packet_info &other) const;
    bool operator>(const packet_info &other) const;
    bool operator>=(const packet_info &other) const;
    bool operator==(const packet_info &other) const;
    bool operator!=(const packet_info &other) const;

    bool less(const packet_info &other) const;
    bool equal(const packet_info &other) const;

    fs::path location() const;
    int remain() const;
  };

  class reader {
  public:
    reader(fs::path file);
    ~reader();

    std::optional<packet_info> next();
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

    void merge(fs::path file, const packet_info &curr);
    void copy_packet(std::ofstream &out, packet_info curr);
  };
}

#endif
