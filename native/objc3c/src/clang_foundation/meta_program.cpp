#include "clang_foundation/meta_program.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Lex/Lexer.h"
#include "clang/Lex/Token.h"
#include "llvm/ADT/StringRef.h"
#include <algorithm>
#include <map>

namespace objc3::clang_foundation {
namespace {
constexpr size_t MaxOutput = 1024 * 1024;
class Interpreter {
  const MetaPackage &package_;
  const MetaEntrypoint &entry_;
  const MetaClass &target_;
  const MetaExpansionContext &context_;
  std::string &error_;
  unsigned work_ = 65536;
  size_t bytes_ = 0;
  unsigned tokens_ = 0;
  bool Fail(std::string message) {
    error_ = package_.id + "::" + entry_.name + " on " + target_.name + ": " + std::move(message); return false;
  }
  bool Charge() { if (!work_) return Fail("expansion work budget exhausted"); --work_; return true; }
  bool Append(std::string &out, llvm::StringRef value) {
    if (value.size() > MaxOutput - bytes_) return Fail("expansion exceeds 1 MiB");
    bytes_ += value.size(); out.append(value.data(), value.size()); return true;
  }
  static std::string CString(llvm::StringRef value) {
    std::string result = "\"";
    for (unsigned char c : value) {
      if (c == '\\' || c == '"') {result += '\\'; result += static_cast<char>(c);}
      else if (c >= 32 && c < 127) result += static_cast<char>(c);
      else {result += '\\'; result += char('0'+(c>>6)); result += char('0'+((c>>3)&7)); result += char('0'+(c&7));}
    }
    return result + '"';
  }
  std::optional<std::string> Query(llvm::StringRef key, const MetaMember *member) {
    if (key == "class.name") return target_.name;
    if (key == "class.superclass") return target_.superclass;
    if (key == "entry.name") return entry_.name;
    if (key == "package.id") return package_.id;
    if (key == "package.version") return package_.version;
    if (key == "package.fingerprint") return package_.fingerprint;
    if (member) {
      if (key == "member.name") return member->name;
      if (key == "member.type") return member->type;
      if (key == "member.getter") return member->getter;
      if (key == "member.setter") return member->setter;
    }
    Fail("unknown or out-of-scope template query: " + key.str()); return std::nullopt;
  }
  bool Text(llvm::StringRef text, const MetaMember *member, std::string &out) {
    while (!text.empty()) {
      if (!Charge()) return false;
      const auto next = text.find('$');
      if (next == llvm::StringRef::npos) return Append(out,text);
      if (!Append(out,text.take_front(next))) return false;
      text = text.drop_front(next+1);
      if (text.consume_front("$")) {if (!Append(out,"$")) return false; continue;}
      if (!text.consume_front("{")) return Fail("template '$' must use '$$' or '${query}'");
      const auto end = text.find('}');
      if (end == llvm::StringRef::npos) return Fail("unterminated template query");
      auto query = text.take_front(end); const bool string = query.consume_back(":string");
      auto value = Query(query,member); if (!value) return false;
      if (!Append(out,string ? CString(*value) : *value)) return false;
      text = text.drop_front(end+1);
    }
    return true;
  }
  std::optional<bool> Predicate(llvm::StringRef predicate, const MetaMember *member) {
    if (predicate == "class.has_superclass") return !target_.superclass.empty();
    if (predicate == "class.has_properties") return !target_.properties.empty();
    if (predicate == "class.has_ivars") return !target_.ivars.empty();
    if (predicate == "language.cxx") return context_.cxx;
    if (predicate == "ownership.arc") return context_.arc;
    if (predicate == "profile.strict") return context_.strict;
    if (member) {
      if (predicate == "member.writable") return member->writable;
      if (predicate == "member.borrowed") return member->borrowed;
      if (predicate == "member.resource") return member->resource;
      if (predicate == "member.weak") return member->weak;
      if (predicate == "member.atomic") return member->atomic;
      if (predicate == "member.class") return member->is_class;
      if (predicate == "member.integer") return member->kind == MetaMemberKind::Integer;
      if (predicate == "member.floating") return member->kind == MetaMemberKind::Floating;
      if (predicate == "member.object") return member->kind == MetaMemberKind::Object;
      if (predicate == "member.block") return member->kind == MetaMemberKind::Block;
      if (predicate == "member.pointer") return member->kind == MetaMemberKind::Pointer;
      if (predicate == "member.record") return member->kind == MetaMemberKind::Record;
    }
    Fail("unknown or out-of-scope predicate: " + predicate.str()); return std::nullopt;
  }
  bool Run(const std::vector<MetaInstruction> &program, const MetaMember *member,
           std::string &out, unsigned depth) {
    if (depth > 8) return Fail("expansion nesting exceeds eight levels");
    for (const auto &instruction : program) {
      if (!Charge()) return false;
      switch (instruction.operation) {
      case MetaInstruction::Operation::Text:
        if (!Text(instruction.text,member,out)) return false;
        break;
      case MetaInstruction::Operation::Error: {
        std::string message; if (!Text(instruction.text,member,message)) return false;
        return Fail("requirements not satisfied: " + message);
      }
      case MetaInstruction::Operation::Conditional: {
        auto condition = Predicate(instruction.predicate,member);
        if (!condition || !Run(*condition ? instruction.body : instruction.alternative,member,out,depth+1)) return false;
        break;
      }
      case MetaInstruction::Operation::Properties:
      case MetaInstruction::Operation::Ivars: {
        const auto &members = instruction.operation == MetaInstruction::Operation::Properties ? target_.properties : target_.ivars;
        if (members.size() > 4096) return Fail("class member inventory exceeds 4096 entries");
        bool first = true;
        for (const auto &item : members) {
          if (!Charge()) return false;
          if ((instruction.member_kind && *instruction.member_kind != item.kind) ||
              (instruction.writable && *instruction.writable != item.writable)) continue;
          if (!first && !Text(instruction.separator,&item,out)) return false;
          first = false;
          if (!Run(instruction.body,&item,out,depth+1)) return false;
        }
        break;
      }
      }
    }
    return true;
  }
  bool Tokens(const std::string &text) {
    if (text.find('\0') != std::string::npos) return Fail("generated source contains a NUL byte");
    clang::LangOptions language; language.CPlusPlus = true; language.CPlusPlus11 = true; language.ObjC = true; language.Digraphs = true; language.Trigraphs = true;
    clang::Lexer lexer(clang::SourceLocation(),language,text.data(),text.data(),text.data()+text.size());
    clang::Token token; bool at = false;
    for (;;) {
      lexer.LexFromRawLexer(token);
      if (token.is(clang::tok::eof)) break;
      if (!Charge()) return false;
      if (!token.is(clang::tok::comment)) ++tokens_;
      if (token.is(clang::tok::hash) || token.is(clang::tok::hashhash)) return Fail("generated preprocessing directives are not permitted");
      if (token.is(clang::tok::raw_identifier)) {
        const auto name = token.getRawIdentifier();
        if (name == "_Pragma" || name == "__pragma" || name == "__DATE__" || name == "__TIME__" || name == "__TIMESTAMP__" ||
            name == "asm" || name == "__asm" || name == "__asm__") return Fail("nondeterministic or external-input compiler operation is not permitted: " + name.str());
        if (at && (name == "end" || name == "interface" || name == "implementation" || name == "import" || name == "class"))
          return Fail("expansion cannot escape its declaration container");
        if (name == "objc_derive" || name == "objc_expand" || name == "objc_property_behavior")
          return Fail("recursive metaprogramming requests are not permitted in this expansion contract");
      }
      at = token.is(clang::tok::at);
    }
    return true;
  }
public:
  Interpreter(const MetaPackage &package,const MetaEntrypoint &entry,const MetaClass &target,
              const MetaExpansionContext &context,std::string &error)
      :package_(package),entry_(entry),target_(target),context_(context),error_(error) {}
  std::optional<MetaExpansion> Execute() {
    MetaExpansion result; result.package_id=package_.id; result.package_version=package_.version;
    result.package_fingerprint=package_.fingerprint; result.entrypoint=entry_.name; result.kind=entry_.kind; result.protocols=entry_.protocols;
    if (!Run(entry_.declarations,nullptr,result.declarations,0) || !Run(entry_.implementations,nullptr,result.implementations,0) ||
        !Tokens(result.declarations) || !Tokens(result.implementations)) return std::nullopt;
    if (!tokens_ && result.protocols.empty() && entry_.kind != MetaEntryKind::Macro) {
      Fail("program produced no declarations or protocol conformances"); return std::nullopt;
    }
    return result;
  }
};
}  // namespace
std::optional<MetaExpansion> ExpandMetaProgram(const MetaPackage &package,const MetaEntrypoint &entry,
    const MetaClass &target,const MetaExpansionContext &context,std::string &error) {
  error.clear(); return Interpreter(package,entry,target,context,error).Execute();
}
}  // namespace objc3::clang_foundation
