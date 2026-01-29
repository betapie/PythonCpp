#pragma once
#ifndef PYCPP_SYS_H
#define PYCPP_SYS_H

#include <PythonCpp/Error.h>
#include <PythonCpp/Object.h>

#include <string>

/*
    Very high-level API to the sys module. This will only feature basic
    utilities that enable quick use of the Python API
*/

namespace pycpp {
class PYCPP_API Sys {
 public:
  static void AddToPath(const std::string& path);

  static void AddToPath(const char* path);

 private:
};
}  // namespace pycpp

#endif  // PYTHONSYS_H
