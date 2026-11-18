#include "clang_foundation/meta_package.h"
#include "llvm/Support/JSON.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/raw_ostream.h"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <sys/stat.h>

using namespace objc3::clang_foundation;
namespace fs = std::filesystem;
namespace {
std::string JSON(const llvm::json::Value &value) {
  std::string result; llvm::raw_string_ostream stream(result); stream << value; stream.flush(); return result;
}
void Write(const fs::path &path, const std::string &bytes) {
  fs::create_directories(path.parent_path()); std::ofstream output(path, std::ios::binary); output << bytes; assert(output.good());
}
struct Fixture { fs::path path; std::string id, version, fingerprint; };
Fixture Package(const fs::path &path, std::string id, std::string version,
                bool portable, std::vector<std::string> requires_caps = {}) {
  llvm::json::Array entries;
  for (const char *name : portable ? std::vector<const char *>{"Equality", "Hash", "DebugDescription", "Codable"} : std::vector<const char *>{"Extra"}) {
    llvm::json::Array required; for (const auto &cap : requires_caps) required.push_back(cap);
    entries.push_back(llvm::json::Object{{"name", name}, {"kind", portable ? "derive" : "macro"},
        {"requires", std::move(required)}, {"declarations", llvm::json::Array{llvm::json::Object{{"text", "- (int)generated;"}}}},
        {"implementations", llvm::json::Array{llvm::json::Object{{"text", "- (int)generated { return 42; }"}}}}});
  }
  llvm::json::Value object(llvm::json::Object{{"contract_id", "objc3c.meta.package.v1"}, {"id", id}, {"version", version},
      {"requires", llvm::json::Object{{"meta_revision", 1}, {"extension_abi", 24}}},
      {"capabilities", llvm::json::Array{"vendor.base"}}, {"entrypoints", std::move(entries)}});
  std::string error; auto fingerprint = ComputeMetaPackageFingerprint(JSON(object), error); assert(fingerprint && error.empty());
  (*object.getAsObject())["fingerprint"] = *fingerprint;
  Write(path, JSON(object)); return {path, std::move(id), std::move(version), *fingerprint};
}
void Lock(const fs::path &path, const std::vector<Fixture> &packages) {
  llvm::json::Array rows;
  for (const auto &p : packages) rows.push_back(llvm::json::Object{{"id", p.id}, {"version", p.version}, {"fingerprint", p.fingerprint}});
  Write(path, JSON(llvm::json::Object{{"contract_id", "objc3c.meta.lock.v1"}, {"packages", std::move(rows)}}));
}
}
int main() {
  for(const char *value:{"none","actor","main","global","named:worker","named:main","named:global","actor@main","actor@global","actor@named:worker"})assert(IsMetaExecutorContract(value));
  for(const char *value:{"","worker","named:","actor@none","actor@actor","actor@named:","named:bad\nname"})assert(!IsMetaExecutorContract(value));

  llvm::SmallString<128> temporary; assert(!llvm::sys::fs::createUniqueDirectory("objc3-meta-packages", temporary));
  const fs::path root(temporary.c_str()); struct Cleanup { fs::path path; ~Cleanup(){fs::remove_all(path);} } cleanup{root};
  auto a = Package(root/"a/objc3-meta.json", "vendor.a", "1.2.3-rc.1+native.007", true);
  auto b = Package(root/"b/objc3-meta.json", "vendor.b", "2.0.0", false, {"vendor.base"});
  const auto lock = root/"lock.json"; Lock(lock, {a,b});
  std::string error;
  auto first = MetaPackageSet::Load({a.path.string(), b.path.parent_path().string()}, lock.string(), error);
  assert(first && error.empty() && first->packages.size() == 2 && first->HasPortableDerives() && first->VerifyInputs(error));
  auto reversed = MetaPackageSet::Load({b.path.string(), a.path.string()}, lock.string(), error);
  assert(reversed && first->fingerprint == reversed->fingerprint);
  const MetaPackage *owner = nullptr;
  assert(first->Resolve("Equality", MetaEntryKind::Derive, &owner, error) && owner->id == a.id);
  assert(first->Resolve("vendor.a::Equality", MetaEntryKind::Derive, &owner, error));
  assert(!first->Resolve("Missing", MetaEntryKind::Derive, &owner, error) && error.find("unresolved") != std::string::npos);
  assert(!MetaPackageSet::Load({a.path.string()}, lock.string(), error));
  assert(!MetaPackageSet::Load({a.path.string(),a.path.string()}, lock.string(), error));
  assert(!MetaPackageSet::Load({a.path.string()}, "", error));
  assert(!MetaPackageSet::Load({}, lock.string(), error));
  assert(MetaPackageSet::Load({}, "", error));
  { std::ofstream output(a.path, std::ios::app); output << ' '; }
  assert(!first->VerifyInputs(error) && error.find("changed") != std::string::npos);
  auto cosmetic = MetaPackageSet::Load({a.path.string(), b.path.string()}, lock.string(), error);
  assert(cosmetic && cosmetic->fingerprint == first->fingerprint);
  auto missing = Package(root/"missing/objc3-meta.json", "vendor.missing", "1.0.0", false, {"vendor.absent"}); Lock(lock, {missing});
  assert(!MetaPackageSet::Load({missing.path.string()}, lock.string(), error) && error.find("capability") != std::string::npos);
  auto duplicate = Package(root/"duplicate/objc3-meta.json", "vendor.duplicate", "1.0.0", true); Lock(lock, {a,duplicate});
  auto ambiguous = MetaPackageSet::Load({a.path.string(),duplicate.path.string()}, lock.string(), error);
  assert(ambiguous && !ambiguous->Resolve("Equality", MetaEntryKind::Derive, &owner, error) && !owner && error.find("ambiguous") != std::string::npos);
  assert(ambiguous->Resolve("vendor.a::Equality", MetaEntryKind::Derive, &owner, error) && owner->id == a.id);
  assert(!ambiguous->HasPortableDerives());
  for (const char *version : {"01.2.3", "1.2", "1.2.3.", "1.2.3-01", "1.2.3+", "1.2.3-alpha..beta"}) {
    auto invalid = Package(root/"invalid/objc3-meta.json", "vendor.invalid", version, true); Lock(lock, {invalid});
    assert(!MetaPackageSet::Load({invalid.path.string()}, lock.string(), error));
  }
  assert(!ComputeMetaPackageFingerprint(R"({"id":1,"\u0069d":2})", error) && error.find("duplicate") != std::string::npos);
  assert(!ComputeMetaPackageFingerprint(std::string(34,'[')+"0"+std::string(34,']'), error));
  std::string many = "["; for(unsigned i=0;i!=33000;++i) many += "0,"; many += "0]";
  assert(!ComputeMetaPackageFingerprint(many, error) && error.find("budget") != std::string::npos);
  auto fifo = root/"fifo/objc3-meta.json"; fs::create_directories(fifo.parent_path()); assert(!::mkfifo(fifo.c_str(),0600));
  assert(!MetaPackageSet::Load({fifo.string()}, lock.string(), error) && error.find("regular") != std::string::npos);
  Write(root/"large/objc3-meta.json", std::string(1024*1024+1,' '));
  assert(!MetaPackageSet::Load({(root/"large/objc3-meta.json").string()}, lock.string(), error));
  assert(ComputeMetaPackageFingerprint(R"({"b":2,"a":"x"})", error) == ComputeMetaPackageFingerprint("{ \"a\": \"x\", \"b\":2 }", error));
}
