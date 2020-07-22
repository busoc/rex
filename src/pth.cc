#include <unordered_map>
#include <sstream>
#include <iomanip>
#include "pth.h"
#include "util.h"
#include "rt.h"
#include "walk.h"

namespace fs = std::filesystem;

int pth::ccsds_header::apid() const {
  return util::endian::swap_bytes(pid) & 0x07FF;
}

int pth::ccsds_header::segmentation() const {
  return util::endian::swap_bytes(segment) >> 14;
}

int pth::ccsds_header::sequence() const {
  return util::endian::swap_bytes(segment) & 0x3FFF;
}

int pth::ccsds_header::size() const {
  return util::endian::swap_bytes(length)+1;
}

int pth::read_file(std::string file) {
  reader in{file};
  std::unordered_map<int, int> missing;
  int count = 0;
  while (auto p = in.next()) {
    auto apid = p->ccsds.apid();
    int delta = 0;
    if (missing.find(apid) != missing.end()) {
      delta = p->ccsds.sequence() - missing[apid];
    }
    missing[apid] = p->ccsds.sequence();

    ever::instant rec = p->archive.time.as_instant().to_gps();
    ever::instant acq = p->esa.time.as_instant();
    std::cout << std::setw(4) << p->archive.size
    << " | " << acq.to_string()
    << " | " << rec.to_string()
    << " | " << std::setw(4) <<  p->ccsds.apid()
    << " | " << std::setw(1) << p->ccsds.segmentation()
    << " | " << std::setw(6) << p->ccsds.sequence()
    << " | " << std::setw(5) << delta
    << " | " << std::setw(4) << p->ccsds.size()
    << " | " << util::endian::swap_bytes(p->esa.sid)
    << " | " << rec.diff_millis(acq)
    << std::endl;
    count++;

    in.skip(p->remain());
  }
  return count;
}

int pth::list_files(std::vector<std::string> files) {
  int count = 0;
  for(auto file: files) {
    count += read_file(file);
  }
  return count;
}

bool pth::packet_info::operator==(const packet_info &other) const {
  return equal(other);
}

bool pth::packet_info::operator!=(const packet_info &other) const {
  return !equal(other);
}

bool pth::packet_info::operator<(const packet_info &other) const {
  return less(other);
}

bool pth::packet_info::operator<=(const packet_info &other) const {
  return less(other) || equal(other);
}

bool pth::packet_info::operator>(const packet_info &other) const {
  return !less(other) && !equal(other);
}

bool pth::packet_info::operator>=(const packet_info &other) const {
  return !less(other) || equal(other);
}

bool pth::packet_info::equal(const packet_info &other) const {
  auto t1 = util::time::gps2unix(esa.time.as_instant(true));
  auto t2 = util::time::gps2unix(other.esa.time.as_instant(true));

  return t1 == t2
    && ccsds.apid() == other.ccsds.apid()
    && ccsds.sequence() == other.ccsds.sequence()
    && ccsds.size() == other.ccsds.size();
}

bool pth::packet_info::less(const packet_info &other) const {
  auto t1 = util::time::gps2unix(esa.time.as_instant(true));
  auto t2 = util::time::gps2unix(other.esa.time.as_instant(true));

  if (t1 < t2) {
    return true;
  }
  if (t1 == t2) {
    if (ccsds.apid() < other.ccsds.apid()) {
      return true;
    }
    if (ccsds.apid() == other.ccsds.apid()) {
      return ccsds.sequence() < other.ccsds.sequence();
    }
  }
  return false;
}

fs::path pth::packet_info::location() const {
  auto it = util::time::gps2unix(esa.time.as_instant(true));

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

int pth::packet_info::remain() const {
  return (archive.size) - sizeof(archive) - sizeof(ccsds) - sizeof(esa);
}

pth::reader::reader(fs::path file): in(file, std::ios::binary) {}

pth::reader::~reader() {
  in.close();
}

std::optional<pth::packet_info> pth::reader::next() {
  if(!in) {
    return {};
  }
  packet_info p;
  in.read(reinterpret_cast<char*>(&p.archive), sizeof(p.archive));
  in.read(reinterpret_cast<char*>(&p.ccsds), sizeof(p.ccsds));
  in.read(reinterpret_cast<char*>(&p.esa), sizeof(p.esa));
  if (!in) {
    return {};
  }

  p.archive.size += sizeof(p.archive.size);
  return p;
}

void pth::reader::skip(int size) {
  in.seekg(size, std::ios::cur);
  if (!in) {

  }
}

void pth::reader::copy_to(std::ofstream &out, int size) {
  if (size <= 0) {
    return;
  }
  if (size > 4096) {
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

pth::merger::merger(fs::path file, bool keep): source(file), keepall(keep) {}

int pth::merger::merge(fs::path dir) {
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

void pth::merger::merge(fs::path file, const packet_info &curr) {
  fs::path tmp = fs::temp_directory_path();
  tmp /= "pth-merge.dat.tmp";
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

void pth::merger::copy_packet(std::ofstream &out, packet_info curr) {
  curr.archive.size -= sizeof(curr.archive.size);
  out.write(reinterpret_cast<char*>(&curr.archive), sizeof(curr.archive));
  out.write(reinterpret_cast<char*>(&curr.ccsds), sizeof(curr.ccsds));
  out.write(reinterpret_cast<char*>(&curr.esa), sizeof(curr.esa));
  if (!out) {
    // throw
  }
}

int pth::merge_file(fs::path dir, fs::path file, bool keep) {
  merger m{file, keep};
  return m.merge(dir);
}

int pth::merge_files(fs::path dir, std::vector<std::string> files, bool keep) {
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

void pth::check_file(std::string file, bool dry) {
  walk::walk_files(file, [](fs::path p, fs::file_status s) {
    rt::coze z{.file = p};
    reader in{p};
    while(auto p = in.next()) {
      int size = p->archive.size - sizeof(p->archive) - sizeof(p->ccsds);
      if (size != p->ccsds.size()) {
        z.invalid++;
        break;
      }
      z.total++;
      in.skip(p->ccsds.size() - sizeof(p->esa));
    }
    rt::print_coze(z);
  });
}

void pth::check_files(std::vector<std::string> files, bool dry) {
  for (auto f: files) {
    check_file(f, dry);
  }
}
