#pragma once
#include "clang_foundation/meta_package.h"
#include <string>
#include <vector>

namespace objc3::clang_foundation {
struct MetaMember {
  std::string name, type, getter, setter;
  MetaMemberKind kind = MetaMemberKind::Record;
  bool writable = false;
  bool borrowed = false;
  bool resource = false;
  bool weak = false;
  bool atomic = false;
  bool is_class = false;
};
struct MetaClass {
  std::string name, superclass;
  std::vector<MetaMember> properties, ivars;
};
struct MetaExpansionContext {
  bool cxx = false;
  bool arc = false;
  bool strict = false;
};
struct MetaExpansion {
  std::string declarations, implementations;
  std::vector<std::string> protocols;
  std::string package_id, package_version, package_fingerprint, entrypoint;
  MetaEntryKind kind = MetaEntryKind::Derive;
};
std::optional<MetaExpansion> ExpandMetaProgram(
    const MetaPackage &package, const MetaEntrypoint &entry,
    const MetaClass &target, const MetaExpansionContext &context,
    std::string &error);
}  // namespace objc3::clang_foundation
