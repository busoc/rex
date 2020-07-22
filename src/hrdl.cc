#include <unordered_map>
#include <sstream>
#include <iomanip>
#include <optional>
#include <filesystem>
#include "hrdl.h"
#include "rt.h"
#include "walk.h"

namespace fs = std::filesystem;

std::ostream& hrdl::operator<<(std::ostream& os, hrdl_error error) {
  switch(error) {
    case hrdl_error::hci_oor:
    os << "oor";
    break;
    case hrdl_error::seq_skip:
    os << "skip";
    break;
    case hrdl_error::seq_decr:
    os << "decr";
    break;
    case hrdl_error::seq_reset:
    os << "reset";
    break;
    case hrdl_error::wrong_sync:
    os << "wrong";
    break;
    case hrdl_error::new_sync:
    os << "sync";
    break;
    case hrdl_error::missed_cadu:
    os << "cadu";
    break;
    case hrdl_error::crc_error:
    os << "crc";
    break;
    case hrdl_error::unrecov:
    os << "unrecov";
    break;
    case hrdl_error::other:
    os << "other";
    break;
    default:
    os << "ok";
  }
  return os;
}

std::ostream& hrdl::operator<<(std::ostream& os, hrdl_channel chan) {
  switch (chan) {
    case hrdl_channel::vic1:
    os << "vic1";
    break;
    case hrdl_channel::vic2:
    os << "vic2";
    break;
    case hrdl_channel::lrsd:
    os << "lrsd";
    break;
    default:
    os << "???";
  }
  return os;
}

std::ostream& hrdl::operator<<(std::ostream& os, hrdl_payload payload) {
  switch (payload) {
    case hrdl_payload::fsl:
    os << "FSL";
    break;
    default:
    os << "???";
  }
  return os;
}

int hrdl::read_file(std::string file) {
  reader in{file};
  std::unordered_map<hrdl_channel, unsigned> missing;
  int count = 0;
  while (auto p = in.next()) {
    int delta = 0;
    if (missing.find(p->vmu.channel) != missing.end()) {
      delta = p->vmu.sequence - missing[p->vmu.channel];
    }
    missing[p->vmu.channel] = p->vmu.sequence;

    auto acq = p->archive.acq.as_instant(true).add(util::time::delta);
    auto rec = p->archive.rec.as_instant();
    auto tim = p->vmu.time.as_instant();
    std::cout << std::setw(8) << p->archive.size
      << " | " << acq.to_string()
      << " | " << rec.to_string()
      << " | " << std::setw(6) << p->archive.error
      << " | " << std::setw(3) << p->archive.payload
      << " | " << std::setw(4) << p->archive.channel
      << " | " << util::hex::to_hex(p->archive.syncword, 4)
      << " | " << std::setw(8) << p->archive.length
      << " | " << std::setw(5) << std::hex << int(p->vmu.source) << std::dec
      << " | " << std::setw(8) << p->vmu.sequence
      << " | " << std::setw(8) << delta
      << " | " << tim.to_string()
      << " | " << rec.diff_millis(acq)
      << std::endl;

    in.skip(p->remain());
    count++;
  }
  return count;
}

int hrdl::list_files(std::vector<std::string> files) {
  int count = 0;
  for(auto file: files) {
    count += read_file(file);
  }
  return count;
}

bool hrdl::packet_info::operator<(const packet_info &other) const {
  return less(other);
}

bool hrdl::packet_info::operator<=(const packet_info &other) const {
  return less(other) || equal(other);
}

bool hrdl::packet_info::operator>(const packet_info &other) const {
  return !less(other) && !equal(other);
}

bool hrdl::packet_info::operator>=(const packet_info &other) const {
  return !less(other) || equal(other);
}

bool hrdl::packet_info::operator==(const packet_info &other) const {
  return equal(other);
}

bool hrdl::packet_info::operator!=(const packet_info &other) const {
  return !equal(other);
}

bool hrdl::packet_info::equal(const packet_info &other) const {
  auto t1 = vmu.time.as_instant(true);
  auto t2 = other.vmu.time.as_instant(true);

  return t1 == t2
    && vmu.channel == other.vmu.channel
    && vmu.sequence == other.vmu.sequence;
}

bool hrdl::packet_info::less(const packet_info &other) const {
  auto t1 = vmu.time.as_instant(true);
  auto t2 = other.vmu.time.as_instant(true);
  if (t1 < t2) {
    return true;
  }
  if (t1 == t2) {
    if (vmu.channel > other.vmu.channel) {
      return true;
    }
    if (vmu.channel == other.vmu.channel) {
      return vmu.sequence < other.vmu.sequence;
    }
  }
  return false;
}

fs::path hrdl::packet_info::location() const {
  auto it = util::time::gps2unix(vmu.time.as_instant(true));

  int min = it.minutes();
  min = min - (min%5);
  std::ostringstream os;
  os << "rt_"
    << std::setw(2) << std::setfill('0') << min
    << "_"
    << std::setw(2) << std::setfill('0') << min+4 << ".dat";

  fs::path base = it.format("%Y/%j/%h");
  return base / os.str();
}

int hrdl::packet_info::remain() const {
  return (archive.size) - sizeof(archive) - sizeof(vmu) - sizeof(dat);;
}

hrdl::reader::reader(fs::path file): in{file, std::ios::binary} {
  buffer = new char[8<<20];
}

hrdl::reader::~reader() {
  delete[] buffer;
}

std::optional<hrdl::packet_info> hrdl::reader::next() {
  if (!in) {
    return {};
  }
  packet_info p;
  in.read(reinterpret_cast<char*>(&p.archive), sizeof(p.archive));
  in.read(reinterpret_cast<char*>(&p.vmu), sizeof(p.vmu));
  in.read(reinterpret_cast<char*>(&p.dat), sizeof(p.dat));
  if (!in) {
    return {};
  }
  p.archive.size += sizeof(p.archive.size);
  return p;
}

void hrdl::reader::skip(int size) {
  in.seekg(size, std::ios::cur);
  if (!in) {

  }
}

void hrdl::reader::copy_to(std::ofstream &out, int size) {
  if (size <= 0) {
    return;
  }
  if (size > 8<<20) {
    return ;
  }
  in.read(buffer, size);
  if (!in || in.gcount() < size) {
    return ;
  }
  out.write(buffer, in.gcount());
  if (!out) {

  }
}

hrdl::merger::merger(fs::path file, bool keep): source(file), keepall(keep) {}

int hrdl::merger::merge(fs::path dir) {
  int count = 0;
  while(true) {
    auto curr = source.next();
    if (!curr) {
      break;
    }
    fs::path loc = dir / curr->location();
    loc.make_preferred();
    if (fs::exists(loc)) {
      merge(loc, *curr);
    } else {
      fs::create_directories(loc.parent_path());

      std::ofstream out{loc, std::ios::binary};
      copy_packet(out, *curr);
      source.copy_to(out, curr->remain());

      out.close();
    }
    count++;
  }
  return count;
}

void hrdl::merger::merge(fs::path file, const packet_info &curr) {
  fs::path tmp = fs::temp_directory_path();
  tmp /= "hrdl-merge.dat.tmp";
  tmp.make_preferred();

  {
    std::ofstream out{tmp, std::ios::binary};
    bool done = false;
    reader in{file};
    while(true) {
      auto other = in.next();
      if (!other) {
        break;
      }
      if (!done && curr == *other) {
        done = true;
        if (!keepall) {
          source.skip(curr.remain());
        } else {
          copy_packet(out, curr);
          source.copy_to(out, curr.remain());
        }
      }
      if (!done && curr < *other) {
        copy_packet(out, curr);
        source.copy_to(out, curr.remain());
        done = true;
      }
      copy_packet(out, *other);
      in.copy_to(out, other->remain());
    }
    if (!done) {
      copy_packet(out, curr);
      source.copy_to(out, curr.remain());
    }
    out.close();
  }

  fs::remove(file);
  fs::rename(tmp, file);
  fs::remove(tmp);
}

void hrdl::merger::copy_packet(std::ofstream &out, packet_info curr) {
  curr.archive.size -= sizeof(curr.archive.size);
  out.write(reinterpret_cast<char*>(&curr.archive), sizeof(curr.archive));
  out.write(reinterpret_cast<char*>(&curr.vmu), sizeof(curr.vmu));
  out.write(reinterpret_cast<char*>(&curr.dat), sizeof(curr.dat));
  if (!out) {
    // throw
  }
}

int hrdl::merge_file(fs::path dir, fs::path file, bool keep) {
  merger m{file};
  return m.merge(dir);
}

int hrdl::merge_files(fs::path dir, std::vector<std::string> files, bool keep) {
  if (!fs::is_directory(dir)) {
    fs::create_directories(dir);
  }
  int total = 0;
  for (auto f: files) {
    if (!fs::is_regular_file(f)) {
      continue;
    }
    total += merge_file(dir, f, keep);
  }
  return total;
}

void hrdl::check_file(std::string file, bool dry) {
  walk::walk_files(file, [](fs::path p, fs::file_status s) {
    rt::coze z{.file = p};
    reader in{p};
    while(auto p = in.next()) {
      int size = p->archive.size - sizeof(p->archive);
      if (size != p->archive.length+4) {
        z.invalid++;
      }
      in.skip(p->remain());
      z.total++;
    }
    rt::print_coze(z);
  });
}

void hrdl::check_files(std::vector<std::string> files, bool dry) {
  for (auto f: files) {
    check_file(f, dry);
  }
}
