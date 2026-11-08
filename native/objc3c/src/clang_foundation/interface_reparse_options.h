#pragma once

#include "clang/Basic/FileManager.h"
#include "clang/Basic/Sanitizers.h"
#include "clang/Lex/DirectoryLookup.h"
#include "clang/Lex/HeaderSearch.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Lex/PreprocessorOptions.h"
#include "llvm/ADT/SmallString.h"
#include <string>
#include <vector>

namespace objc3::clang_foundation {
// Physical lookup inputs are local to a reparse. Interface identities bind
// their contents and native definitions, rather than installation paths.
inline void AppendInterfaceHeaderSearchArguments(const clang::Preprocessor &preprocessor,
                                                std::vector<std::string> &arguments) {
  const auto &search=preprocessor.getHeaderSearchInfo();
  arguments.push_back("-nostdinc");
  bool quoted=true;
  for(auto entry=search.search_dir_begin();entry!=search.search_dir_end();++entry) {
    if(entry==search.angled_dir_begin())quoted=false;
    llvm::SmallString<256> directory(entry->getName());
    (void)preprocessor.getFileManager().makeAbsolutePath(directory);
    const auto name=directory.str().str();
    if(quoted)arguments.insert(arguments.end(),{"-iquote",name});
    else if(entry->isFramework())arguments.insert(arguments.end(),{entry->isSystemHeaderDirectory()?"-iframework":"-F",name});
    else if(entry->getDirCharacteristic()==clang::SrcMgr::C_ExternCSystem)
      arguments.insert(arguments.end(),{"-Xclang","-internal-externc-isystem","-Xclang",name});
    else arguments.insert(arguments.end(),{entry->isSystemHeaderDirectory()?"-isystem":"-I",name});
  }
  for(const auto &[macro,undefine]:preprocessor.getPreprocessorOpts().Macros)
    arguments.push_back(std::string(undefine?"-U":"-D")+macro);
  const auto &predefines=preprocessor.getPredefines();
  if(predefines.find("#define __OPTIMIZE_SIZE__ ")!=std::string::npos)arguments.push_back("-Os");
  else arguments.push_back(predefines.find("#define __OPTIMIZE__ ")!=std::string::npos?"-O2":"-O0");
  if(predefines.find("#define __NO_INLINE__ ")!=std::string::npos)arguments.push_back("-fno-inline");
  const auto &language=preprocessor.getLangOpts();
  if(language.PICLevel)arguments.push_back(language.PIE
      ? (language.PICLevel==2?"-fPIE":"-fpie") : (language.PICLevel==2?"-fPIC":"-fpic"));
  else arguments.insert(arguments.end(),{"-fno-pic","-fno-pie"});
  // These language options also select native-header definitions through
  // __has_feature/__SANITIZE_* even for a syntax-only canonical reparse.
  llvm::SmallVector<llvm::StringRef,16> sanitizers;
  clang::serializeSanitizerSet(preprocessor.getLangOpts().Sanitize,sanitizers);
  if(!sanitizers.empty()) {
    std::string option="-fsanitize=";
    for(auto name:sanitizers){if(option.back()!='=')option+=',';option+=name.str();}
    // Retain semantic feature state without imposing the producer's link/LTO
    // requirements on a syntax-only reparse (for example CFI).
    arguments.insert(arguments.end(),{"-Xclang",std::move(option)});
  }
  if(preprocessor.getLangOpts().CPlusPlus)
    switch(preprocessor.getPreprocessorOpts().ObjCXXARCStandardLibrary) {
    case clang::ARCXX_libcxx:arguments.push_back("-stdlib=libc++");break;
    case clang::ARCXX_libstdcxx:arguments.push_back("-stdlib=libstdc++");break;
    case clang::ARCXX_nolib:break;
    }
}
} // namespace objc3::clang_foundation
