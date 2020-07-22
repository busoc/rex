#ifndef __REX_HRDL__
#define __REX_HRDL__

#include <ostream>
#include <vector>
#include <fstream>
#include <filesystem>
#include "util.h"

namespace hrdl {

  enum class hrdl_channel: unsigned char {
    vic1 = 1,
    vic2 = 2,
    lrsd = 3,
  };

  enum class hrdl_payload: unsigned char {
    fsl = 2,
  };

  enum class hrdl_error: unsigned short {
    hci_oor = 0x8000,
    seq_skip = 0x4000,
    seq_decr = 0x2000,
    seq_reset = 0x1000,
    wrong_sync = 0x0800,
    new_sync = 0x0400,
    missed_cadu = 0x0200,
    crc_error = 0x0100,
    unrecov = 0x0040,
    other = 0x003f,
  };

  std::ostream& operator<<(std::ostream& os, hrdl_channel chan);
  std::ostream& operator<<(std::ostream& os, hrdl_payload payload);
  std::ostream& operator<<(std::ostream& os, hrdl_error error);

  #pragma pack(push, 1)
  struct header {
    unsigned size;
    hrdl_error error;
    hrdl_payload payload;
    hrdl_channel channel;
    util::time::time5 acq;
    util::time::time5 rec;
    char syncword[4];
    unsigned length;
  };

  struct vmu_header {
    hrdl_channel channel;
    unsigned char source;
    unsigned short spare1;
    unsigned sequence;
    util::time::time6 time;
    unsigned short spare2;
  };

  struct data_header {
    char property;
    unsigned short streamseq;
    unsigned originseq;
    unsigned long long acqtime;
    unsigned long long auxtime;
    char origin;
  };
  #pragma pack(pop)

  namespace fs = std::filesystem;

  int read_file(std::string file);
  int list_files(std::vector<std::string> files);

  int merge_file(fs::path dir, fs::path file, bool keep = false);
  int merge_files(fs::path dir, std::vector<std::string> files, bool keep = false);

  struct packet_info {
    header archive;
    vmu_header vmu;
    data_header dat;

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
    char *buffer;
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
