#include <iostream>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include "pdh.h"

namespace fs = std::filesystem;

std::ostream& pdh::operator<<(std::ostream& os, value_state state) {
  switch (state) {
    case value_state::empty:
    os << "empty";
    break;
    case value_state::same:
    os << "same";
    break;
    case value_state::newv:
    os << "new";
    break;
    case value_state::latest:
    os << "latest";
    break;
    case value_state::error:
    os << "error";
    break;
    default:
    os << "???";
  }
  return os;
}

std::ostream& pdh::operator<<(std::ostream& os, value_type type) {
  switch (type) {
    case value_type::int32:
    os << "long";
    break;
    case value_type::float64:
    os << "float";
    break;
    case value_type::binary8:
    os << "bin";
    break;
    case value_type::reference:
    os << "ref";
    break;
    case value_type::string8:
    os << "string";
    break;
    case value_type::int64:
    os << "long";
    break;
    case value_type::decimal:
    os << "float";
    break;
    case value_type::real:
    os << "float";
    break;
    case value_type::time:
    os << "time";
    break;
    case value_type::datetime:
    os << "time";
    break;
    case value_type::varchar:
    os << "string";
    break;
    case value_type::binary:
    os << "bin";
    break;
    case value_type::bit:
    os << "bit";
    break;
    default:
    os << "???";
  }
  return os;
}

char pdh::umi_header::source() const {
  return code[0];
}

unsigned long long pdh::umi_header::uniq_id() const {
  unsigned long long umi = 0;
  unsigned long long tmp = 0;
  for (int i = 1; i < 6; i++) {
    tmp = (unsigned long long)(code[i]) & 0xFF;
    umi |= tmp << ((5 - i) * 8);
  }
  return umi;
}

bool pdh::umi_header::operator<(const umi_header &other) const {
  return less(other);
}

bool pdh::umi_header::operator<=(const umi_header &other) const {
  return less(other) || equal(other);
}

bool pdh::umi_header::operator>(const umi_header &other) const {
  return !less(other) && !equal(other);
}

bool pdh::umi_header::operator>=(const umi_header &other) const {
  return !less(other) || equal(other);
}

bool pdh::umi_header::operator==(const umi_header &other) const {
  return equal(other);
}

bool pdh::umi_header::operator!=(const umi_header &other) const {
  return !equal(other);
}

bool pdh::umi_header::less(const umi_header &other) const {
  auto t1 = util::time::gps2unix(time.as_instant(true));
  auto t2 = util::time::gps2unix(other.time.as_instant(true));
  if (t1 < t2) {
    return true;
  }
  if (t1 == t2) {
    if (source() < other.source()) {
      return true;
    }
    if (source() == other.source()) {
      auto uid1 = uniq_id();
      auto uid2 = other.uniq_id();
      if (uid1 < uid2) {
        return true;
      }
      if (uid1 == uid2) {
        return orbit < other.orbit;
      }
    }
  }
  return false;
}

bool pdh::umi_header::equal(const umi_header &other) const {
  auto t1 = util::time::gps2unix(time.as_instant(true));
  auto t2 = util::time::gps2unix(other.time.as_instant(true));

  bool same = false;
  for (int i = 0; i < 6; i++) {
    same = code[i] == other.code[i];
    if (!same) {
      return same;
    }
  }
  return t1 == t2 && same && orbit == other.orbit;
}


int pdh::umi_header::remain() const {
  return size - sizeof(umi_header);
}

fs::path pdh::umi_header::location() const {
  auto it = util::time::gps2unix(time.as_instant(true));

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

int pdh::read_file(std::string file) {
  reader in{file};
  int count = 0;
  while (auto p = in.next()) {
    in.skip(p->remain());
    auto it = util::time::gps2unix(p->time.as_instant(true));
    std::cout << std::setw(6) << p->size
      << " | " << it.to_string()
      << " | " << std::setw(6) << p->state
      << " | " << util::hex::to_hex(p->code, 6)
      << " | " << std::setw(6) << p->uniq_id()
      << " | " << std::setw(8) << p->orbit
      << " | " << std::setw(8) << p->type
      << " | " << std::setw(8) << util::endian::swap_bytes(p->unit)
      << " | " << util::endian::swap_bytes(p->length)
      << std::endl;
    count++;
  }
  return count;
}

int pdh::list_files(std::vector<std::string> files) {
  int count = 0;
  for(auto file: files) {
    count += read_file(file);
  }
  return count;
}

pdh::reader::reader(fs::path file): in{file, std::ios::binary} {}

pdh::reader::~reader() {
  in.close();
}

std::optional<pdh::umi_header> pdh::reader::next() {
  if (!in) {
    return {};
  }
  umi_header p;
  in.read(reinterpret_cast<char*>(&p), sizeof(p));
  if (!in) {
    return {};
  }
  p.size += sizeof(p.size);
  return p;
}

void pdh::reader::skip(int size) {
  in.seekg(size, std::ios::cur);
  if (!in) {

  }
}

void pdh::reader::copy_to(std::ofstream &out, int size) {
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

pdh::merger::merger(fs::path file, bool keep): source(file), keepall(keep) {}

int pdh::merger::merge(fs::path dir) {
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

void pdh::merger::merge(fs::path file, const umi_header &curr) {
  fs::path tmp = fs::temp_directory_path();
  tmp /= "pdh-merge.dat.tmp";
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

void pdh::merger::copy_packet(std::ofstream &out, umi_header curr) {
  curr.size -= sizeof(curr.size);
  out.write(reinterpret_cast<char*>(&curr), sizeof(curr));
  if (!out) {
    // throw
  }
}

int pdh::merge_file(fs::path dir, fs::path file, bool keep) {
  merger m{file, keep};
  return m.merge(dir);
}

int pdh::merge_files(fs::path dir, std::vector<std::string> files, bool keep) {
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
