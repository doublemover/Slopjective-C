#pragma once
#include "clang_foundation/compile_session.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Program.h"
#include "llvm/Support/StringSaver.h"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace objc3::clang_foundation::standard_library_test {
namespace fs=std::filesystem;
inline void Success(const CompileResult &result) {
  if(!result.success) {
    std::cerr<<result.failure<<'\n';
    for(const auto &d:result.diagnostics)std::cerr<<d.spelling.file<<':'<<d.spelling.line<<": "<<d.message<<'\n';
    for(const auto &d:result.interface_import_diagnostics)std::cerr<<d.code<<": "<<d.message<<'\n';
    if(result.interface_artifact)for(const auto &d:result.interface_artifact->diagnostics)std::cerr<<d.code<<": "<<d.message<<'\n';
  }
  assert(result.success);
}
inline void Write(const fs::path &path,const std::string &text) {
  std::ofstream file(path);file<<text;assert(file.good());
}
inline void Execute(const std::vector<std::string> &owned) {
  std::vector<llvm::StringRef> argv(owned.begin(),owned.end());
  std::string error;bool failed=false;
  const auto code=llvm::sys::ExecuteAndWait(owned.front(),argv,std::nullopt,{},60,0,&error,&failed);
  if(code || failed)std::cerr<<owned.front()<<": "<<error<<" ("<<code<<")\n";
  assert(!failed && code==0);
}
inline std::vector<std::string> Instrumentation() {
  llvm::BumpPtrAllocator allocator;llvm::StringSaver saver(allocator);llvm::SmallVector<const char *,16> flags;
  llvm::cl::TokenizeGNUCommandLine(OBJC3_STANDARD_LIBRARY_TEST_LINK_FLAGS,saver,flags);
  std::vector<std::string> result;
  for(const char *flag:flags) {
    const std::string value(flag);
    if(value.starts_with("-fsanitize=") || value.starts_with("-fno-sanitize") ||
        value=="-fno-omit-frame-pointer")result.push_back(value);
  }
  return result;
}

inline CompileRequest Request(const fs::path &input,const fs::path &output,
    const char *standard,int ownership,const char *optimization,
    const std::vector<std::string> &library_arguments) {
  const bool objc=ownership!=0;
  CompileRequest value;value.input_path=input.string();value.output_path=output.string();
  value.emit=EmitKind::Object;value.target_triple="x86_64-unknown-linux-gnu";
  value.language=objc?BaseLanguage::ObjectiveCxx:BaseLanguage::Cxx;
  value.objc3=objc;value.arc=ownership==2;if(objc)value.objc_runtime="gnustep-2.2";
  value.arguments={std::string("-std=")+standard,optimization,"-resource-dir=" OBJC3_STANDARD_LIBRARY_TEST_RESOURCE_DIR};
  auto instrumentation=Instrumentation();value.arguments.insert(value.arguments.end(),instrumentation.begin(),instrumentation.end());
  value.arguments.insert(value.arguments.end(),library_arguments.begin(),library_arguments.end());
  return value;
}

inline void LinkAndExecute(const fs::path &root,const fs::path &main_object,
                           bool objc,const std::vector<std::string> &library_arguments) {
  std::vector<std::string> link={OBJC3_STANDARD_LIBRARY_TEST_LINKER,"-fuse-ld=lld",
      (root/"provider.o").string(),(root/"a.o").string(),(root/"b.o").string(),main_object.string(),
      "-o",(root/"witness").string()};
  link.insert(link.end(),library_arguments.begin(),library_arguments.end());
  llvm::BumpPtrAllocator allocator;llvm::StringSaver saver(allocator);llvm::SmallVector<const char *,16> flags;
  llvm::cl::TokenizeGNUCommandLine(OBJC3_STANDARD_LIBRARY_TEST_LINK_FLAGS,saver,flags);
  for(const char *flag:flags)link.emplace_back(flag);
  if(objc) {
    const fs::path runtime=OBJC3_STANDARD_LIBRARY_TEST_OBJC_LIBRARY;
    link.push_back(runtime.string());link.push_back("-Wl,-rpath,"+runtime.parent_path().string());
  }
  Execute(link);Execute({(root/"witness").string()});
}
} // namespace objc3::clang_foundation::standard_library_test
