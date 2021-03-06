/*
 * Copyright (c) 2016 Catalysts GmbH
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <fstream>
#include <sstream>
#include <unistd.h>

#include "common.h"

namespace ebpf {

std::vector<int> read_cpu_range(std::string path) {
  std::ifstream cpus_range_stream { path };
  std::vector<int> cpus;
  std::string cpu_range;

  while (std::getline(cpus_range_stream, cpu_range, ',')) {
    std::size_t rangeop = cpu_range.find('-');
    if (rangeop == std::string::npos) {
      cpus.push_back(std::stoi(cpu_range));
    }
    else {
      int start = std::stoi(cpu_range.substr(0, rangeop));
      int end = std::stoi(cpu_range.substr(rangeop + 1));
      for (int i = start; i <= end; i++)
        cpus.push_back(i);
    }
  }
  return cpus;
}

std::vector<int> get_online_cpus() {
  return read_cpu_range("/sys/devices/system/cpu/online");
}

std::vector<int> get_possible_cpus() {
  return read_cpu_range("/sys/devices/system/cpu/possible");
}

FileDesc::FileDesc(int fd) : fd_(fd) {}

FileDesc::FileDesc(FileDesc &&that) : fd_(-1) { *this = std::move(that); }

FileDesc::~FileDesc() {
  if (fd_ >= 0)
    ::close(fd_);
}

FileDesc &FileDesc::operator=(int fd) {
  if (fd_ >= 0)
    ::close(fd_);
  fd_ = fd;
  return *this;
}

FileDesc &FileDesc::operator=(FileDesc &&that) {
  if (fd_ >= 0)
    ::close(fd_);
  fd_ = that.fd_;
  that.fd_ = -1;
  return *this;
}

FileDesc FileDesc::dup() const {
  int dup_fd = ::dup(fd_);
  return FileDesc(dup_fd);
}

FileDesc::operator int() { return fd_; }

FileDesc::operator int() const { return fd_; }

} // namespace ebpf
