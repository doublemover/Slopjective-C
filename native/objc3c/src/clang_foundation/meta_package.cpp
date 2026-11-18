#include "clang_foundation/meta_package.h"
#include "clang_foundation/interface_codec.h"
#include "llvm/Support/JSON.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/raw_ostream.h"
#include <algorithm>
#include <array>
#include <cerrno>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <tuple>
#include <cctype>
#include <filesystem>
#include <map>
#include <set>

namespace objc3::clang_foundation {
namespace {
constexpr size_t MaxDocumentBytes = 1024 * 1024;
constexpr unsigned MaxJSONNodes = 32768;
constexpr unsigned MaxProgramNodes = 4096;

bool Fail(std::string &error, std::string message) {
  error = std::move(message);
  return false;
}

// LLVM's object representation cannot retain duplicate keys. Reject them before
// constructing that representation, including equivalent escaped spellings.
class JSONShape {
  llvm::StringRef bytes_;
  size_t offset_ = 0;
  unsigned nodes_ = MaxJSONNodes;
  std::string &error_;
  void Space() {
    while (offset_ < bytes_.size() &&
           (bytes_[offset_] == ' ' || bytes_[offset_] == '\n' ||
            bytes_[offset_] == '\r' || bytes_[offset_] == '\t')) ++offset_;
  }
  bool Take(char c) {
    Space();
    if (offset_ == bytes_.size() || bytes_[offset_] != c) return false;
    ++offset_; return true;
  }
  std::optional<std::string> String() {
    Space(); const auto begin = offset_;
    if (offset_ == bytes_.size() || bytes_[offset_++] != '"') return std::nullopt;
    while (offset_ < bytes_.size()) {
      const auto c = bytes_[offset_++];
      if (c == '\\') { if (offset_ == bytes_.size()) return std::nullopt; ++offset_; }
      else if (c == '"') {
        auto value = llvm::json::parse(bytes_.slice(begin, offset_));
        if (!value) { llvm::consumeError(value.takeError()); return std::nullopt; }
        if (auto text = value->getAsString()) return text->str();
        return std::nullopt;
      }
    }
    return std::nullopt;
  }
  bool Value(unsigned depth) {
    if (!nodes_ || depth > 32) return Fail(error_, "metaprogramming JSON exceeds its structural budget");
    --nodes_; Space(); if (offset_ == bytes_.size()) return false;
    if (bytes_[offset_] == '{') {
      ++offset_; std::set<std::string> keys;
      if (Take('}')) return true;
      do {
        auto key = String();
        if (!key || !Take(':')) return false;
        if (!keys.insert(*key).second) return Fail(error_, "duplicate metaprogramming JSON key: " + *key);
        if (!Value(depth + 1)) return false;
        if (Take('}')) return true;
      } while (Take(','));
      return false;
    }
    if (bytes_[offset_] == '[') {
      ++offset_; if (Take(']')) return true;
      do { if (!Value(depth + 1)) return false; if (Take(']')) return true; } while (Take(','));
      return false;
    }
    if (bytes_[offset_] == '"') return String().has_value();
    const auto begin = offset_;
    while (offset_ < bytes_.size() && bytes_[offset_] != ',' &&
           bytes_[offset_] != ']' && bytes_[offset_] != '}' &&
           !std::isspace(static_cast<unsigned char>(bytes_[offset_]))) ++offset_;
    return offset_ != begin;
  }
public:
  JSONShape(llvm::StringRef bytes, std::string &error): bytes_(bytes), error_(error) {}
  bool Check() { if (!Value(0)) return false; Space(); return offset_ == bytes_.size(); }
};

std::optional<llvm::json::Object> Document(std::string_view text, std::string &error) {
  if (text.size() > MaxDocumentBytes) { Fail(error, "metaprogramming document exceeds 1 MiB"); return std::nullopt; }
  if (!JSONShape(llvm::StringRef(text), error).Check()) {
    if (error.empty()) Fail(error, "invalid metaprogramming JSON structure");
    return std::nullopt;
  }
  auto parsed = llvm::json::parse(llvm::StringRef(text));
  if (!parsed) { Fail(error, llvm::toString(parsed.takeError())); return std::nullopt; }
  auto *object = parsed->getAsObject();
  if (!object) { Fail(error, "metaprogramming document must be an object"); return std::nullopt; }
  return std::move(*object);
}

std::string CanonicalObject(const llvm::json::Object &object);
std::string Canonical(const llvm::json::Value &value) {
  if (auto *object = value.getAsObject()) return CanonicalObject(*object);
  if (auto *array = value.getAsArray()) {
    std::string result = "[";
    for (const auto &item : *array) { if (result.size() != 1) result += ','; result += Canonical(item); }
    return result + ']';
  }
  std::string result; llvm::raw_string_ostream stream(result); stream << value; stream.flush(); return result;
}

std::string CanonicalObject(const llvm::json::Object &object) {
    std::vector<std::string> keys;
    for (const auto &[key, item] : object) { (void)item; keys.push_back(key.str()); }
    std::sort(keys.begin(), keys.end());
    std::string result = "{";
    for (const auto &key : keys) {
      if (result.size() != 1) result += ',';
      result += Canonical(llvm::json::Value(key)) + ':' + Canonical(*object.get(key));
    }
    return result + '}';
  }

bool Keys(const llvm::json::Object &object, std::initializer_list<llvm::StringRef> allowed,
          std::string &error) {
  for (const auto &[key, value] : object) {
    (void)value;
    if (std::find(allowed.begin(), allowed.end(), key.str()) == allowed.end())
      return Fail(error, "unknown metaprogramming field: " + key.str());
  }
  return true;
}

bool Name(llvm::StringRef value) {
  const auto alpha=[](unsigned char c){return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';};
  if (value.empty() || value.size() > 192 || !alpha(value.front())) return false;
  return std::all_of(value.begin(), value.end(), [&](unsigned char c) {
    return alpha(c) || (c >= '0' && c <= '9') || c == '.' || c == '-' || c == ':';
  });
}
bool Hex(llvm::StringRef value) {
  return value.size() == 64 && std::all_of(value.begin(), value.end(), [](char c) {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
  });
}
bool Semver(llvm::StringRef value) {
  if (value.empty() || value.size() > 256) return false;
  auto identifiers = [](llvm::StringRef list, bool prerelease) {
    if (list.empty()) return false;
    while (!list.empty()) {
      const bool separated = list.contains('.'); auto pair = list.split('.'); auto part = pair.first;
      if (part.empty() || (separated && pair.second.empty()) ||
          !std::all_of(part.begin(), part.end(), [](unsigned char c) {
            return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '-';
          })) return false;
      const bool numeric = std::all_of(part.begin(), part.end(), [](char c){return c >= '0' && c <= '9';});
      if (prerelease && numeric && part.size() > 1 && part.front() == '0') return false;
      list = pair.second;
    }
    return true;
  };
  if (value.contains('+')) {
    auto pair = value.split('+'); if (!identifiers(pair.second, false)) return false; value = pair.first;
  }
  if (value.contains('-')) {
    auto pair = value.split('-'); if (!identifiers(pair.second, true)) return false; value = pair.first;
  }
  unsigned count = 0;
  while (!value.empty()) {
    const bool separated = value.contains('.'); auto pair = value.split('.'); auto part = pair.first;
    if (part.empty() || (separated && pair.second.empty()) || (part.size() > 1 && part.front() == '0') ||
        !std::all_of(part.begin(), part.end(), [](char c){return c >= '0' && c <= '9';})) return false;
    ++count; value = pair.second;
  }
  return count == 3;
}

bool Strings(const llvm::json::Object &object, llvm::StringRef field,
             std::vector<std::string> &output, std::string &error, bool required = true) {
  const auto *array = object.getArray(field);
  if (!array) return !required && !object.get(field) ? true : Fail(error, "expected string array: " + field.str());
  if (array->size() > 256) return Fail(error, "metaprogramming string array exceeds 256 entries");
  std::set<std::string> seen;
  for (const auto &value : *array) {
    auto text = value.getAsString();
    if (!text || !Name(*text) || !seen.insert(text->str()).second) return Fail(error, "invalid or duplicate name in " + field.str());
    output.push_back(text->str());
  }
  std::sort(output.begin(), output.end());
  return true;
}

std::optional<MetaMemberKind> MemberKind(llvm::StringRef name) {
  if (name == "integer") return MetaMemberKind::Integer;
  if (name == "floating") return MetaMemberKind::Floating;
  if (name == "object") return MetaMemberKind::Object;
  if (name == "block") return MetaMemberKind::Block;
  if (name == "pointer") return MetaMemberKind::Pointer;
  if (name == "record") return MetaMemberKind::Record;
  return std::nullopt;
}

bool Program(const llvm::json::Array &array, std::vector<MetaInstruction> &output,
             unsigned &budget, unsigned depth, std::string &error) {
  if (depth > 8) return Fail(error, "metaprogramming template nesting exceeds eight levels");
  for (const auto &value : array) {
    if (!budget--) return Fail(error, "metaprogramming program exceeds 4096 instructions");
    const auto *object = value.getAsObject();
    if (!object || !Keys(*object, {"text", "for_each", "body", "separator", "where", "if", "then", "else", "error"}, error)) return Fail(error, "invalid metaprogramming instruction");
    MetaInstruction instruction;
    if (auto message = object->getString("error")) {
      if (object->size() != 1 || message->empty() || message->size() > 4096 || message->contains('\0')) return Fail(error, "invalid metaprogramming error instruction");
      instruction.operation = MetaInstruction::Operation::Error; instruction.text = message->str();
    } else if (auto predicate = object->getString("if")) {
      if (!Keys(*object, {"if", "then", "else"}, error)) return false;
      const auto *body = object->getArray("then"); const auto *alternative = object->getArray("else");
      if (!Name(*predicate) || !body || (object->get("else") && !alternative)) return Fail(error, "invalid metaprogramming conditional");
      instruction.operation = MetaInstruction::Operation::Conditional; instruction.predicate = predicate->str();
      if (!Program(*body, instruction.body, budget, depth + 1, error) ||
          (alternative && !Program(*alternative, instruction.alternative, budget, depth + 1, error))) return false;
    } else if (auto text = object->getString("text")) {
      if (object->size() != 1 || text->size() > 65536 || text->contains('\0')) return Fail(error, "invalid metaprogramming text instruction");
      instruction.text = text->str();
    } else {
      if (object->get("text") || object->get("if") || object->get("error") ||
          !Keys(*object, {"for_each", "body", "separator", "where"}, error))
        return Fail(error, "invalid metaprogramming instruction variant");
      auto loop = object->getString("for_each"); const auto *body = object->getArray("body");
      if (!loop || !body || (*loop != "properties" && *loop != "ivars")) return Fail(error, "invalid metaprogramming member iteration");
      instruction.operation = *loop == "properties" ? MetaInstruction::Operation::Properties : MetaInstruction::Operation::Ivars;
      if (auto separator = object->getString("separator")) instruction.separator = separator->str();
      else if (object->get("separator")) return Fail(error, "metaprogramming separator must be a string");
      if (instruction.separator.size() > 4096 || instruction.separator.find('\0') != std::string::npos) return Fail(error, "invalid metaprogramming separator");
      if (auto *where = object->getObject("where")) {
        if (!Keys(*where, {"kind", "writable"}, error)) return false;
        if (auto kind = where->getString("kind")) {
          instruction.member_kind = MemberKind(*kind);
          if (!instruction.member_kind) return Fail(error, "unknown metaprogramming member kind");
        } else if (where->get("kind")) return Fail(error, "member kind must be a string");
        if (auto writable = where->getBoolean("writable")) instruction.writable = *writable;
        else if (where->get("writable")) return Fail(error, "member writable predicate must be Boolean");
      } else if (object->get("where")) return Fail(error, "metaprogramming member predicate must be an object");
      if (!Program(*body, instruction.body, budget, depth + 1, error)) return false;
    }
    output.push_back(std::move(instruction));
  }
  return true;
}

std::optional<MetaPackage> Package(llvm::json::Object object, std::string &error) {
  if (!Keys(object, {"contract_id", "id", "version", "fingerprint", "requires", "capabilities", "entrypoints"}, error)) return std::nullopt;
  auto contract = object.getString("contract_id"), id = object.getString("id"), version = object.getString("version"), fingerprint = object.getString("fingerprint");
  if (contract != "objc3c.meta.package.v1" || !id || !Name(*id) || id->contains(':') || !version || !Semver(*version) || !fingerprint || !Hex(*fingerprint)) {
    Fail(error, "invalid metaprogramming package identity"); return std::nullopt;
  }
  const auto expected = fingerprint->str(); object.erase("fingerprint");
  const auto actual = ComputeInterfaceSourceSha256("objc3c.meta.package.v1\n" + CanonicalObject(object));
  if (actual != expected) { Fail(error, "metaprogramming package fingerprint mismatch: " + id->str()); return std::nullopt; }
  MetaPackage result; result.id = id->str(); result.version = version->str(); result.fingerprint = actual;
  auto *constraints = object.getObject("requires");
  if (!constraints || !Keys(*constraints, {"meta_revision", "extension_abi"}, error) ||
      constraints->getInteger("meta_revision") != 1 || constraints->getInteger("extension_abi") != 24) {
    Fail(error, "metaprogramming package requires an unsupported toolchain revision/ABI"); return std::nullopt;
  }
  if (!Strings(object, "capabilities", result.capabilities, error)) return std::nullopt;
  auto *entries = object.getArray("entrypoints");
  if (!entries || entries->empty() || entries->size() > 256) { Fail(error, "package requires 1..256 entrypoints"); return std::nullopt; }
  std::set<std::pair<std::string, MetaEntryKind>> seen;
  unsigned budget = MaxProgramNodes;
  for (const auto &value : *entries) {
    const auto *entry = value.getAsObject();
    if (!entry || !Keys(*entry, {"name", "kind", "requires", "protocols", "declarations", "implementations", "dispatch", "executor"}, error)) return std::nullopt;
    auto name = entry->getString("name"), kind = entry->getString("kind");
    if (!name || !Name(*name) || !kind) { Fail(error, "invalid metaprogramming entrypoint identity"); return std::nullopt; }
    MetaEntrypoint item; item.name = name->str();
    if (*kind == "derive") item.kind = MetaEntryKind::Derive;
    else if (*kind == "macro") item.kind = MetaEntryKind::Macro;
    else if (*kind == "property-behavior") item.kind = MetaEntryKind::PropertyBehavior;
    else { Fail(error, "unknown metaprogramming entrypoint kind"); return std::nullopt; }
    if (!seen.insert({item.name, item.kind}).second) { Fail(error, "duplicate metaprogramming entrypoint: " + item.name); return std::nullopt; }
    if (!Strings(*entry, "requires", item.required_capabilities, error) ||
        !Strings(*entry, "protocols", item.protocols, error, false)) return std::nullopt;
    auto *declarations = entry->getArray("declarations"), *implementations = entry->getArray("implementations");
    if (!declarations || !implementations || (declarations->empty() && implementations->empty()) ||
        !Program(*declarations, item.declarations, budget, 0, error) || !Program(*implementations, item.implementations, budget, 0, error)) {
      if (error.empty()) Fail(error, "entrypoint requires a nonempty expansion program"); return std::nullopt;
    }
    if (item.kind == MetaEntryKind::PropertyBehavior) {
      auto dispatch = entry->getString("dispatch"), executor = entry->getString("executor");
      if (!dispatch || (*dispatch != "direct" && *dispatch != "dynamic") || !executor || executor->empty() || executor->size() > 192) {
        Fail(error, "property behavior requires explicit dispatch and executor contracts"); return std::nullopt;
      }
      item.dispatch = dispatch->str(); item.executor = executor->str();
    } else if (entry->get("dispatch") || entry->get("executor")) {
      Fail(error, "dispatch/executor contracts belong to property behaviors"); return std::nullopt;
    }
    result.entrypoints.push_back(std::move(item));
  }
  std::sort(result.entrypoints.begin(), result.entrypoints.end(), [](const auto &a, const auto &b){ return std::tie(a.kind, a.name) < std::tie(b.kind, b.name); });
  return result;
}

std::optional<std::string> Read(const std::string &path, std::string &error) {
  const int descriptor = ::open(path.c_str(), O_RDONLY | O_CLOEXEC | O_NONBLOCK);
  if (descriptor < 0) { Fail(error, "cannot open metaprogramming input: " + path); return std::nullopt; }
  struct Close { int descriptor; ~Close(){::close(descriptor);} } close{descriptor};
  struct stat status{};
  if (::fstat(descriptor, &status) || !S_ISREG(status.st_mode) || status.st_size < 0 ||
      static_cast<uint64_t>(status.st_size) > MaxDocumentBytes) {
    Fail(error, "metaprogramming input must be a bounded regular file: " + path); return std::nullopt;
  }
  std::string bytes; std::array<char, 8192> buffer;
  for (;;) {
    const auto count = ::read(descriptor, buffer.data(), buffer.size());
    if (count < 0 && errno == EINTR) continue;
    if (count < 0 || bytes.size() + static_cast<size_t>(count) > MaxDocumentBytes) {
      Fail(error, "cannot read bounded metaprogramming input: " + path); return std::nullopt;
    }
    if (!count) return bytes;
    bytes.append(buffer.data(), static_cast<size_t>(count));
  }
}
}  // namespace

std::optional<std::string> ComputeMetaPackageFingerprintFromFile(const std::string &path,std::string &error) {
  auto document=Read(path,error);if(!document)return std::nullopt;
  return ComputeMetaPackageFingerprint(*document,error);
}

std::optional<std::string> ComputeMetaPackageFingerprint(std::string_view document, std::string &error) {
  error.clear(); auto object = Document(document, error); if (!object) return std::nullopt;
  object->erase("fingerprint");
  return ComputeInterfaceSourceSha256("objc3c.meta.package.v1\n" + Canonical(llvm::json::Value(std::move(*object))));
}

std::optional<MetaPackageSet> MetaPackageSet::Load(const std::vector<std::string> &paths, const std::string &lock_path, std::string &error) {
  error.clear(); MetaPackageSet result;
  if (paths.empty()) {
    if (!lock_path.empty()) { Fail(error, "metaprogramming lock has no explicit package inputs"); return std::nullopt; }
    result.fingerprint = ComputeInterfaceSourceSha256("objc3c.meta.package-set.v1\n"); return result;
  }
  if (paths.size() > 64 || lock_path.empty()) { Fail(error, "explicit metaprogramming packages require a lock and at most 64 inputs"); return std::nullopt; }
  std::set<std::string> ids, physical_paths;
  for (const auto &path : paths) {
    auto file = std::filesystem::path(path); std::error_code ec;
    if (std::filesystem::is_directory(file, ec)) file /= "objc3-meta.json";
    file = std::filesystem::absolute(file, ec).lexically_normal();
    if (ec || file.filename() != "objc3-meta.json" || !physical_paths.insert(file.string()).second) { Fail(error, "invalid/duplicate explicit metaprogramming manifest path"); return std::nullopt; }
    auto bytes = Read(file.string(), error); if (!bytes) return std::nullopt;
    auto object = Document(*bytes, error); if (!object) return std::nullopt;
    auto package = Package(std::move(*object), error); if (!package) return std::nullopt;
    if (!ids.insert(package->id).second) { Fail(error, "duplicate metaprogramming package identity: " + package->id); return std::nullopt; }
    package->manifest_path = file.string(); result.inputs.push_back({file.string(), ComputeInterfaceSourceSha256(*bytes)});
    result.packages.push_back(std::move(*package));
  }
  auto locked_bytes = Read(lock_path, error); if (!locked_bytes) return std::nullopt;
  auto lock = Document(*locked_bytes, error); if (!lock) return std::nullopt;
  if (!Keys(*lock, {"contract_id", "packages"}, error) || lock->getString("contract_id") != "objc3c.meta.lock.v1") { Fail(error, "invalid metaprogramming lock contract"); return std::nullopt; }
  const auto *rows = lock->getArray("packages");
  if (!rows || rows->size() != result.packages.size()) { Fail(error, "lock does not match the complete explicit package set"); return std::nullopt; }
  std::map<std::string, std::pair<std::string, std::string>> locked;
  for (const auto &value : *rows) {
    const auto *row = value.getAsObject();
    if (!row || !Keys(*row, {"id", "version", "fingerprint"}, error)) return std::nullopt;
    auto id = row->getString("id"), version = row->getString("version"), fingerprint = row->getString("fingerprint");
    if (!id || !version || !fingerprint || !locked.emplace(id->str(), std::make_pair(version->str(), fingerprint->str())).second) { Fail(error, "invalid/duplicate metaprogramming lock entry"); return std::nullopt; }
  }
  std::sort(result.packages.begin(), result.packages.end(), [](const auto &a, const auto &b){return a.id < b.id;});
  std::set<std::string> capabilities{"objc3.meta.templates.v1", "objc3.meta.ast-query.v1"};
  std::string identity = "objc3c.meta.package-set.v1\n";
  for (const auto &package : result.packages) {
    auto found = locked.find(package.id);
    if (found == locked.end() || found->second != std::make_pair(package.version, package.fingerprint)) { Fail(error, "metaprogramming lock mismatch: " + package.id); return std::nullopt; }
    identity += package.id + "=" + package.version + "=" + package.fingerprint + "\n";
    capabilities.insert(package.capabilities.begin(), package.capabilities.end());
  }
  for (const auto &package : result.packages) for (const auto &entry : package.entrypoints)
    for (const auto &required : entry.required_capabilities) if (!capabilities.contains(required)) { Fail(error, "missing metaprogramming capability: " + required); return std::nullopt; }
  std::error_code ec; const auto absolute_lock = std::filesystem::absolute(lock_path, ec).lexically_normal();
  if (ec) { Fail(error, "cannot resolve metaprogramming lock path"); return std::nullopt; }
  result.inputs.push_back({absolute_lock.string(), ComputeInterfaceSourceSha256(*locked_bytes)});
  result.fingerprint = ComputeInterfaceSourceSha256(identity);
  return result;
}

const MetaEntrypoint *MetaPackageSet::Resolve(std::string_view name, MetaEntryKind kind, const MetaPackage **owner, std::string &error) const {
  const MetaEntrypoint *result = nullptr; if (owner) *owner = nullptr;
  const MetaPackage *qualified = nullptr;
  for (const auto &package : packages) if (name.starts_with(package.id + "::")) { qualified = &package; break; }
  for (const auto &package : packages) for (const auto &entry : package.entrypoints)
    if (entry.kind == kind && (qualified ? qualified == &package && package.id + "::" + entry.name == name : entry.name == name)) {
      if (result) { if (owner) *owner = nullptr; Fail(error, "ambiguous metaprogramming entrypoint: " + std::string(name)); return nullptr; }
      result = &entry; if (owner) *owner = &package;
    }
  if (!result) Fail(error, "unresolved metaprogramming entrypoint: " + std::string(name));
  return result;
}

bool MetaPackageSet::HasPortableDerives() const {
  for (const char *name : {"Equality", "Hash", "DebugDescription", "Codable"}) {
    std::string error; if (!Resolve(name, MetaEntryKind::Derive, nullptr, error)) return false;
  }
  return true;
}

bool MetaPackageSet::VerifyInputs(std::string &error) const {
  for (const auto &input : inputs) {
    auto bytes = Read(input.path, error);
    if (!bytes || ComputeInterfaceSourceSha256(*bytes) != input.sha256) return Fail(error, "metaprogramming input changed during compilation: " + input.path);
  }
  return true;
}
}  // namespace objc3::clang_foundation
