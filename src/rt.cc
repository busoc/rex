#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

#include "rt.h"
#include "walk.h"

void rt::print_coze(const coze &z) {
  std::cout << std::setw(6) << z.total
    << " | " << std::setw(12) << z.invalid
    << " | " << std::setw(12) << fs::file_size(z.file)
    << " | " << z.file.string()
    << std::endl;
}

rt::splitter::splitter(fs::path dir, unsigned count, std::string prefix):
  distrib(0, count-1),
  rng(std::chrono::steady_clock::now().time_since_epoch().count()) {
  if (count <= 1) {
    throw "split to at least 2 files";
  }
  fs::create_directories(dir);

  if (prefix == "") {
    prefix = "file";
  }
  for (int i = 0; i < count; i++) {
    std::ostringstream os;
    os << prefix << "_" << std::setw(6) << std::setfill('0') << i+1 << ".dat";
    fs::path p = dir;
    p /= os.str();

    files.emplace_back(std::ofstream{p, std::ios::binary});
  }
}

rt::splitter::~splitter() {
  for (auto &f: files) {
    f.close();
  }
}

void rt::splitter::split(std::string file) {
  char* buffer = new char[8<<20];
  walk::walk_files(file, [this, &buffer](fs::path p, fs::file_status s) {
    std::ifstream in{p, std::ios::binary};
    while (in) {
      unsigned size = 0;
      in.read(reinterpret_cast<char*>(&size), sizeof(size));
      if (!in) {
        break;
      }
      in.read(buffer, size);
      if (!in) {
        break;
      }

      int n = distrib(rng);
      files.at(n).write(reinterpret_cast<char*>(&size), sizeof(size));
      files.at(n).write(buffer, in.gcount());
      if (!files.at(n)) {
        break;
      }
    }
    in.close();
  });
  delete[] buffer;
}

void rt::splitter::split(std::vector<std::string> args) {
  for (auto a: args) {
    split(a);
  }
}

rt::shuffler::shuffler(fs::path dir):
  dir(dir),
  rng(std::chrono::steady_clock::now().time_since_epoch().count()) {
  fs::create_directories(dir);
};

void rt::shuffler::shuffle(fs::path file) {
  std::ifstream in{file, std::ios::binary};
  if (!in) {
    return ;
  }
  auto pos = index(in);

  fs::path loc = dir;
  file.replace_extension(".dat.shuf");
  loc /= file.filename();
  std::ofstream out{loc, std::ios::binary};
  if (!out) {
    return ;
  }
  shuffle(out, in, pos);

  in.close();
  out.close();
}

std::vector<rt::poze> rt::shuffler::index(std::ifstream &in) {
  std::vector<rt::poze> pos;
  while (!in.eof()) {
    rt::poze z;
    z.pos = in.tellg();
    in.read(reinterpret_cast<char*>(&z.siz), sizeof(z.siz));
    if (!in) {
      break;
    }
    in.seekg(z.siz, std::ios::cur);
    pos.push_back(z);
  }
  in.clear();
  in.seekg(0, std::ios::beg);

  return pos;
}

void rt::shuffler::shuffle(std::ofstream &out, std::ifstream &in, std::vector<poze> pos) {
  std::shuffle(pos.begin(), pos.end(), rng);

  char* buffer = new char[8<<20];
  int total = 0;
  for (auto z: pos) {
    in.seekg(z.pos, std::ios::beg);
    if (!in) {
      break;
    }
    in.read(buffer, z.siz+sizeof(z.siz));
    if (!in) {
      break;
    }
    out.write(buffer, in.gcount());
    if (!out) {
      break;
    }
    total++;
  }
  delete[] buffer;
}
