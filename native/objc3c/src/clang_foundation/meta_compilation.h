#pragma once
#include "clang_foundation/meta_program.h"
#include <map>
#include <memory>
#include <set>
namespace clang {class ASTConsumer;}
namespace objc3::clang_foundation {
struct CompileRequest;
struct MetaClassExpansion {
  std::vector<MetaExpansion> entries;
  std::string ivars,declarations,implementations,input_model;
  std::vector<std::string> protocols,request_keys;
};
struct MetaCompilation {
  MetaPackageSet packages;
  bool collecting = true;
  bool failed = false;
  std::string error;
  std::map<std::string,MetaClassExpansion> classes;
};
std::unique_ptr<clang::ASTConsumer> MakeMetaConsumer(
    const CompileRequest &request);
} // namespace objc3::clang_foundation
