#pragma once

#include "clang_foundation/interface_codec.h"

namespace objc3::clang_foundation {

inline std::string CanonicalInterfaceIncludeGuard(llvm::StringRef body) {
  return "__OBJC3_INTERFACE_"+ComputeInterfaceSourceSha256(body);
}

inline std::string WrapCanonicalInterfaceSource(llvm::StringRef body) {
  const auto guard=CanonicalInterfaceIncludeGuard(body);
  return "/* Generated canonical Clang interface. */\n#pragma once\n#ifndef "+
      guard+"\n#define "+guard+"\n"+body.str()+"#endif // "+guard+"\n";
}

// Only a complete, content-bound canonical wrapper can suppress reparsing.
// Imported source bytes have already passed source/declaration replay checks.
inline std::string ReadCanonicalInterfaceIncludeGuard(llvm::StringRef source) {
  if(!source.consume_front("/* Generated canonical Clang interface. */\n#pragma once\n#ifndef "))return {};
  const auto line=source.find('\n');if(line==llvm::StringRef::npos)return {};
  const auto guard=source.take_front(line).str();source=source.drop_front(line+1);
  if(!source.consume_front("#define "+guard+"\n") ||
      !source.consume_back("#endif // "+guard+"\n") ||
      guard!=CanonicalInterfaceIncludeGuard(source))return {};
  return guard;
}

} // namespace objc3::clang_foundation
