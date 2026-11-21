#include "native_target_test_support.h"
#include "clang_foundation/compile_session.h"
#include "clang_foundation/interface_codec.h"
#include "clang_foundation/frontend_api.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/JSON.h"
#include "llvm/Support/Program.h"
#include <algorithm>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
using namespace objc3::clang_foundation;
namespace fs=std::filesystem;
namespace {
CompileRequest Request(const fs::path &path,const std::string &source,BaseLanguage language,const char *standard) {
 CompileRequest request;request.input_path=path.string();request.source_text=source;
 request.language=language;request.arguments={std::string("-std=")+standard};
 request.target_triple=OBJC3_TEST_NATIVE_LINUX_TARGET;return request;
}
std::string Apply(const CompileResult &result,const std::string &source,const fs::path &path) {
 assert(!result.success);std::vector<DiagnosticEdit> edits;
 for(const auto &diagnostic:result.diagnostics) {
  assert(diagnostic.edits.empty() || diagnostic.edits_unavailable_reason.empty());
  for(const auto &edit:diagnostic.edits) {
   if(edit.file!=path.string())std::cerr<<"Edit file: ["<<edit.file<<"] expected ["<<path.string()<<"]\n";assert(edit.file==path.string());assert(edit.source_sha256==ComputeInterfaceSourceSha256(source));
   assert(edit.source_sha256!=ComputeInterfaceSourceSha256(source+" "));
   assert(edit.begin_offset<=edit.end_offset && edit.end_offset<=source.size());
   assert(edit.begin.file==edit.file && edit.end.file==edit.file);
   edits.push_back(edit);
  }
 }
 assert(!edits.empty());
 // Fixtures have disjoint edits. Descending physical byte offsets preserve the
 // original source positions even with a multibyte prefix or #line directives.
 std::stable_sort(edits.begin(),edits.end(),[](const auto &a,const auto &b){return a.begin_offset>b.begin_offset;});
 std::string fixed=source;std::uint64_t previous=source.size();
 for(const auto &edit:edits){assert(edit.end_offset<=previous);fixed.replace(edit.begin_offset,edit.end_offset-edit.begin_offset,edit.replacement);previous=edit.begin_offset;}
 return fixed;
}
void Check(const fs::path &path,const std::string &source,BaseLanguage language,const char *standard) {
 auto request=Request(path,source,language,standard);auto broken=Compile(request);
 if(broken.success)std::cerr<<"Expected diagnostic for "<<source<<'\n';assert(!broken.success);
 auto json=llvm::json::parse(EncodeCompileDiagnostics(broken));assert(json);
 assert(json->getAsObject()->getString("contract_id")=="objc3c.native-diagnostics.v1");
 auto &rows=*json->getAsObject()->getArray("diagnostics");assert(rows.size()==broken.diagnostics.size());
 for(size_t i=0;i<rows.size();++i) {
  auto *serialized=rows[i].getAsObject()->getArray("edits");assert(serialized);
  assert(serialized->size()==broken.diagnostics[i].edits.size());
  for(size_t j=0;j<serialized->size();++j) {
   auto *row=(*serialized)[j].getAsObject();const auto &edit=broken.diagnostics[i].edits[j];
   assert(row->getString("file_bytes_hex")==llvm::toHex(edit.file,true));
   assert(row->getString("replacement_bytes_hex")==llvm::toHex(edit.replacement,true));
   assert(row->getInteger("begin_offset")==static_cast<int64_t>(edit.begin_offset));
  }
 }
 request.source_text=Apply(broken,source,path);auto repaired=Compile(request);
 if(!repaired.success)for(const auto &d:repaired.diagnostics)std::cerr<<d.message<<'\n';assert(repaired.success);
 assert(llvm::json::parse(EncodeCompileDiagnostics(repaired)));
}
}
int main() {
 llvm::SmallString<128> temporary;assert(!llvm::sys::fs::createUniqueDirectory("objc3-diagnostic-edits",temporary));
 fs::path root(temporary.c_str());struct Cleanup{fs::path path;~Cleanup(){fs::remove_all(path);}}cleanup{root};
 for(const char *standard:{"c17","c23"})
  Check(root/"source.c","/* π */ int answer(void){return 42}",BaseLanguage::C,standard);
 for(const char *standard:{"c++20","c++23"}) {
  Check(root/"source.cpp","struct Value{int member;}\n",BaseLanguage::Cxx,standard);
  Check(root/"source.cpp","struct Value{int member;};int answer(Value *value){return value.member;}",BaseLanguage::Cxx,standard);
  Check(root/"source.cpp","struct Value{int member;};int answer(){Value value{42};return value->member;}",BaseLanguage::Cxx,standard);
  Check(root/"source.cpp","/* π */ int answer(){return 42} int other(){return 43}",BaseLanguage::Cxx,standard);
  const std::string line="#line 900 \"logical.cpp\"\nint answer(){return 42}";
  auto request=Request(root/"physical.cpp",line,BaseLanguage::Cxx,standard);auto broken=Compile(request);assert(!broken.success);
  bool mapped=false;for(const auto &d:broken.diagnostics)for(const auto &edit:d.edits) {
   assert(d.expansion.file=="logical.cpp");assert(edit.file==(root/"physical.cpp").string());assert(edit.begin.line==2);mapped=true;
  }assert(mapped);request.source_text=Apply(broken,line,root/"physical.cpp");assert(Compile(request).success);
 }
 // Relative virtual inputs use the request filesystem's working directory,
 // rather than the host process directory or an anonymous MemoryBuffer name.
 auto relative=Request("relative.c","int answer(void){return 42}",BaseLanguage::C,"c17");
 relative.working_directory=root.string();auto relative_result=Compile(relative);assert(!relative_result.success);
 relative.source_text=Apply(relative_result,*relative.source_text,root/"relative.c");assert(Compile(relative).success);
 // A malformed return inside a macro body must not become a guessed edit of
 // an invocation or compiler scratch buffer.
 auto macro=Request(root/"macro.c","#define BAD int answer(void){return 42}\nBAD\n",BaseLanguage::C,"c17");
 auto macro_result=Compile(macro);assert(!macro_result.success);
 for(const auto &d:macro_result.diagnostics)assert(d.edits.empty());
 // Interface errors retain their machine-readable code/stage rather than
 // becoming a failure string with an empty diagnostic catalogue.
 auto missing=Request(root/"missing.cpp","int value;",BaseLanguage::Cxx,"c++20");
 missing.interface_search_paths={root.string()};InterfaceModuleRequest module;
 module.canonical_name="Missing";module.locked_identity.canonical_name="Missing";
 module.locked_identity.semantic_fingerprint=std::string(64,'0');module.locked_identity.package_identity="missing-v1";
 missing.module_imports={module};auto absent=Compile(missing);assert(!absent.success && !absent.interface_import_diagnostics.empty());
 auto imported=llvm::json::parse(EncodeCompileDiagnostics(absent));assert(imported);
 auto *contracts=imported->getAsObject()->getArray("interface_diagnostics");assert(contracts && !contracts->empty());
 for(const auto &row:*contracts)assert(row.getAsObject()->getString("stage")=="import");
 auto publication=Request(root/"publication.cpp","struct __attribute__((annotate(\"unmodeled\"))) Value{};",BaseLanguage::Cxx,"c++20");
 publication.emit_semantic_interface=true;publication.module_name="test.Diagnostic";publication.package_identity="diagnostics-v1";
 auto unpublished=Compile(publication);assert(!unpublished.success && unpublished.interface_artifact);
 auto emitted=llvm::json::parse(EncodeCompileDiagnostics(unpublished));assert(emitted);
 auto *failures=emitted->getAsObject()->getArray("interface_diagnostics");assert(failures && !failures->empty());
 for(const auto &row:*failures)assert(row.getAsObject()->getString("stage")=="publication");
 // The public C result retains diagnostics and edits after input storage dies.
 assert(!objc3c_frontend_v3_result_diagnostics_json(nullptr));
 objc3c_frontend_v2_result *result=nullptr;
 {
  std::string source="int answer(void){return 42}";const char *args[]={"-std=c17"};
  const auto path=(root/"embedded.c").string();
  objc3c_frontend_v2_options options{};options.struct_size=sizeof(options);options.abi_version=OBJC3C_FRONTEND_V2_ABI;
  options.input_path=path.c_str();options.source=source.data();options.source_size=source.size();
  options.target_triple=OBJC3_TEST_NATIVE_LINUX_TARGET;options.arguments=args;options.argument_count=1;
  assert(objc3c_frontend_v2_compile(&options,&result)==1 && result);
 }
 auto embedded=llvm::json::parse(objc3c_frontend_v3_result_diagnostics_json(result));assert(embedded);
 bool insertion=false;for(auto &row:*embedded->getAsObject()->getArray("diagnostics"))
  for(auto &edit:*row.getAsObject()->getArray("edits"))insertion|=edit.getAsObject()->getString("replacement_bytes_hex")=="3b";
 assert(insertion);objc3c_frontend_v2_result_destroy(result);
 // Raw byte spellings remain exact even when a display string needs repair.
 CompileResult raw;CompileDiagnostic diagnostic;DiagnosticEdit edit;
 edit.file=std::string("invalid-")+char(0xff);edit.replacement=std::string("x\0",2)+char(0xff);
 diagnostic.edits.push_back(edit);raw.diagnostics.push_back(diagnostic);
 auto bytes=llvm::json::parse(EncodeCompileDiagnostics(raw));assert(bytes);
 auto *encoded=(*(*bytes->getAsObject()->getArray("diagnostics"))[0].getAsObject()->getArray("edits"))[0].getAsObject();
 assert(encoded->getString("replacement_bytes_hex")=="7800ff");
}
