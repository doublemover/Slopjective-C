#include "clang_foundation/compile_session.h"
#include "clang/AST/ASTImporter.h"
#include "clang/AST/Decl.h"
#include "clang/AST/ObjC3Borrowed.h"
#include "clang/AST/ODRHash.h"
#include "clang/Frontend/ASTUnit.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/Error.h"
#include <cassert>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>
using namespace objc3::clang_foundation;
namespace fs = std::filesystem;
namespace {
const std::string Header = R"(
typedef borrowed const char *BorrowedBytes;
BorrowedBytes bytes(id owner) __attribute__((objc_returns_borrowed(owner_index=0)));
void use(borrowed const char *pointer);
void use_noescape(const char *pointer __attribute__((noescape)));
void escape(const char *pointer);
void escape_address(const char **pointer);
void pair_escape(const char *first, const char *second);
void invoke(void (^work)(void) __attribute__((noescape)));
void launch(void (^work)(void));
id make_owner(void);
const char *global_pointer;
void (^global_block)(void);
struct Holder { const char *pointer; };
struct Pair { struct Holder left, right; };
struct AddressHolder { const char **pointer; };
)";
CompileRequest Request(const fs::path &Root, const std::string &Source,
                       BaseLanguage Language, const std::string &Standard,
                       bool ARC) {
  CompileRequest R;
  R.input_path = (Root / (Language == BaseLanguage::ObjectiveC ? "input.m" : "input.mm")).string();
  R.source_text = Source;
  R.language = Language;
  R.profile = SemanticProfile::StrictSystem;
  R.objc3 = true;
  R.arc = ARC;
  R.objc_runtime = "gnustep-2.2";
  R.target_triple = "x86_64-unknown-linux-gnu";
  R.arguments = {"-std=" + Standard, "-fblocks"};
  return R;
}
void Expect(const CompileRequest &R, bool Success,
            const std::string &Diagnostic = {}) {
  CompileResult Result = Compile(R);
  if (Result.success != Success) {
    std::cerr << "unexpected borrowed-pointer result for:\n" << *R.source_text << '\n'
              << Result.failure << '\n';
    for (const auto &D : Result.interface_import_diagnostics)
      std::cerr << D.code << ": " << D.message << '\n';
    for (const auto &D : Result.diagnostics)
      std::cerr << D.message << '\n';
  }
  assert(Result.success == Success);
  if (!Diagnostic.empty()) {
    bool Found = false;
    for (const auto &D : Result.diagnostics)
      Found |= D.message.find(Diagnostic) != std::string::npos;
    if (!Found) {
      std::cerr << "expected borrowed-pointer diagnostic containing: "
                << Diagnostic << "\nfor:\n" << *R.source_text << '\n';
      for (const auto &D : Result.diagnostics)
        std::cerr << D.message << '\n';
    }
    assert(Found && "rejection must identify its borrowed operation/contract");
  }
}
void SemanticCases(const fs::path &Root, BaseLanguage Language,
                    const std::string &Standard, bool ARC) {
  const std::string FillHeader=Header+
      "typedef borrowed const char *FillBytes;\n"
      "FillBytes fill_bytes(void*) __attribute__((objc_returns_borrowed(owner_index=0)));\n";
  for(const std::string &Code:{
      "void f(void*o){struct H{const char*p;};struct H h={fill_bytes(o)};__builtin_memset(&h,0,sizeof h);escape(h.p);}",
      "void f(void*o){const char*p=fill_bytes(o);const char**q=(const char**)__builtin_memset(&p,0,sizeof p);escape(*q);}"})
    Expect(Request(Root,FillHeader+Code,Language,Standard,ARC),true);
  for(const std::string &Code:{
      "void f(void*o){struct H{const char*p;};struct H h={fill_bytes(o)};__builtin_memset(&h,0,1);escape(h.p);}",
      "void f(void*o,int i){const char*a[2]={0,0};a[i]=fill_bytes(o);__builtin_memset(&a[1],0,sizeof a[1]);escape(a[0]);}"})
    Expect(Request(Root,FillHeader+Code,Language,Standard,ARC),false,"argument without a non-escaping lifetime contract");
  const std::string CallableHeader =
      "typedef id (*RetainedFactory)(void) __attribute__((ns_returns_retained));\n"
      "typedef void (*ConsumedCallback)(id __attribute__((ns_consumed)));\n"
      "typedef void (*PlainCallback)(id);\n"
      "typedef void (*NoEscapeCallback)(id __attribute__((noescape)));\n"
      "typedef void (*NestedConsumed)(ConsumedCallback);\n"
      "typedef void (*NestedPlain)(PlainCallback);\n";
  for (const std::string &Case : {
           "void f(ConsumedCallback p){(void)(PlainCallback)p;}",
           "void f(PlainCallback p){(void)(ConsumedCallback)p;}",
           "void f(RetainedFactory p){(void)(id (*)(void))p;}",
           "void f(PlainCallback p){(void)(NoEscapeCallback)p;}",
           "void f(NestedConsumed p){(void)(NestedPlain)p;}"})
    Expect(Request(Root,CallableHeader+Case,Language,Standard,ARC),false,
           "callable cast changes an ownership");
  Expect(Request(Root,CallableHeader+
      "void f(ConsumedCallback p,NoEscapeCallback n){ConsumedCallback same=(ConsumedCallback)p;PlainCallback weaker=(PlainCallback)n;(void)same;(void)weaker;}",
      Language,Standard,ARC),true);
  const std::string ForeignHeader =
      "typedef struct ForeignOwner *ForeignRef;\n"
      "typedef borrowed const char *ForeignBytes;\n"
      "ForeignBytes foreign_bytes(ForeignRef owner) __attribute__((objc_returns_borrowed(owner_index=0)));\n"
      "void foreign_use(const char *pointer __attribute__((noescape)));\n"
      "void foreign_consume(ForeignRef owner __attribute__((cf_consumed)),int unused);\n"
      "void foreign_pair(ForeignRef owner __attribute__((cf_consumed)),const char *pointer __attribute__((noescape)));\n"
      "typedef void (*ForeignCallback)(ForeignRef owner __attribute__((cf_consumed)),int unused);\n"
      "typedef void (*ForeignPlain)(ForeignRef,int);\n"
      "typedef void (^OwnerBlock)(void);\n"
      "ForeignBytes block_bytes(OwnerBlock owner) __attribute__((objc_returns_borrowed(owner_index=0)));\n"
      "void consume_block_owner(OwnerBlock owner __attribute__((ns_consumed)));\n";
  for (const std::string &Case : {
           "void f(ForeignRef o){const char*p=foreign_bytes(o);foreign_consume(o,0);foreign_use(p);}",
           "void f(ForeignRef o,ForeignCallback callback){const char*p=foreign_bytes(o);callback(o,0);foreign_use(p);}",
           "void f(ForeignRef o){ForeignRef alias=o;const char*p=foreign_bytes(alias);foreign_consume(o,0);foreign_use(p);}",
           "void f(ForeignRef o,int c){const char*p=foreign_bytes(o);if(c)foreign_consume(o,0);foreign_use(p);}",
           "void f(ForeignRef o){foreign_pair(o,foreign_bytes(o));}"})
    Expect(Request(Root,ForeignHeader+Case,Language,Standard,ARC),false,
           "borrowed pointer escapes");
  Expect(Request(Root,ForeignHeader+
      "void f(ForeignCallback callback){(void)(ForeignPlain)callback;}",
      Language,Standard,ARC),false,"callable cast changes an ownership");
  Expect(Request(Root,ForeignHeader+
      "void f(ForeignRef o,ForeignRef r){foreign_consume(o,0);o=r;foreign_use(foreign_bytes(o));}",
      Language,Standard,ARC),true);
  Expect(Request(Root,ForeignHeader+
      "void f(OwnerBlock o){const char*p=block_bytes(o);consume_block_owner(o);foreign_use(p);}",
      Language,Standard,ARC),ARC,ARC ? "" : "after its owner lifetime ended");
  Expect(Request(Root,ForeignHeader+
      "void f(OwnerBlock o){keepAlive(o);const char*p=block_bytes(o);consume_block_owner(o);foreign_use(p);}",
      Language,Standard,ARC),true);
  Expect(Request(Root,ForeignHeader+
      "void f(void){ForeignCallback callbacks[2]={0,0};(void)(ForeignPlain *)callbacks;}",
      Language,Standard,ARC),false,"callable cast changes an ownership");
  Expect(Request(Root,ForeignHeader+
      "void f(ForeignRef o){void(^work)(void)=^{foreign_use(foreign_bytes(o));};foreign_consume(o,0);work();}",
      Language,Standard,ARC),false,"captured owner lifetime ended");
  for(const std::string &Case:{
      "void f(ForeignRef owner){const char*p=foreign_bytes(owner);void(^work)(void)=^{foreign_consume(owner,0);};work();foreign_use(p);}",
      "void f(ForeignRef owner){void(^work)(void)=^{foreign_use(foreign_bytes(owner));foreign_consume(owner,0);};work();work();}",
      "void f(ForeignRef owner){const char*p=foreign_bytes(owner);void(^inner)(void)=^{foreign_consume(owner,0);};void(^outer)(void)=^{inner();};outer();foreign_use(p);}"})
    Expect(Request(Root,ForeignHeader+Case,Language,Standard,ARC),false,"borrowed pointer escapes");
  Expect(Request(Root,ForeignHeader+
      "void f(ForeignRef owner){const char*p=foreign_bytes(owner);void(^inner)(void)=^{foreign_consume(owner,0);};void(^unused)(void)=^{(void)inner;};unused();foreign_use(p);}",
      Language,Standard,ARC),true);
  if (Language == BaseLanguage::ObjectiveCxx) {
    for(const std::string &Case:{
        "void f(ForeignRef o){auto work=[o]{foreign_use(foreign_bytes(o));};foreign_consume(o,0);work();}",
        "void f(ForeignRef o){auto work=[copy=o]{foreign_use(foreign_bytes(copy));};foreign_consume(o,0);work();}",
        "void f(ForeignRef o){auto work=[o]{ForeignRef alias=o;foreign_use(foreign_bytes(alias));};foreign_consume(o,0);work();}",
        "void f(ForeignRef o){auto work=[o]{auto nested=[o]{foreign_use(foreign_bytes(o));};nested();};foreign_consume(o,0);work();}"})
      Expect(Request(Root,ForeignHeader+Case,Language,Standard,ARC),false,
             "after its owner lifetime ended");
    for(const std::string &Case:{
        "void f(ForeignRef o){auto work=[o]{foreign_use(foreign_bytes(o));};auto copy=work;copy();foreign_consume(o,0);}",
        "void f(ForeignRef a,ForeignRef b){auto work=[a,b]{ForeignRef alias=a;alias=b;foreign_use(foreign_bytes(alias));};foreign_consume(a,0);work();}"})
      Expect(Request(Root,ForeignHeader+Case,Language,Standard,ARC),true);
    Expect(Request(Root,Header+
        "void f(id o){auto work=[o]{use_noescape(bytes(o));};o=0;work();}",
        Language,Standard,ARC),ARC,ARC ? "" : "after its owner lifetime ended");
    for(const std::string &Case:{
        "void f(ForeignRef owner){const char*p=foreign_bytes(owner);auto work=[owner]{foreign_consume(owner,0);};auto copy=work;foreign_use(p);copy();foreign_use(p);}",
        "void f(ForeignRef owner){const char*p=foreign_bytes(owner);auto inner=[owner]{foreign_consume(owner,0);};auto outer=[inner]{inner();};outer();foreign_use(p);}"})
      Expect(Request(Root,ForeignHeader+Case,Language,Standard,ARC),false,"after its owner lifetime ended");
    Expect(Request(Root,ForeignHeader+
        "void f(ForeignRef owner){const char*p=foreign_bytes(owner);auto inner=[owner]{foreign_consume(owner,0);};auto unused=[inner]{(void)inner;};unused();foreign_use(p);}",
        Language,Standard,ARC),true);
    const std::string GetterHeader=ForeignHeader+
        "ForeignBytes getter(ForeignRef owner,int unused) __attribute__((objc_returns_borrowed(owner_index=0)));\n";
    for(const std::string &Case:{
        "void f(ForeignRef o,ForeignRef r){const char*p=getter(o,(o=r,0));foreign_use(p);}",
        "void f(ForeignRef o){const char*p=getter(o,(foreign_consume(o,0),0));foreign_use(p);}",
        "void f(ForeignRef o,ForeignRef r){ForeignRef old=o;const char*p=getter(o,(o=r,o=old,0));foreign_use(p);}"})
      Expect(Request(Root,GetterHeader+Case,Language,Standard,ARC),false,
             "storage of a value whose owner lifetime has ended");
    Expect(Request(Root,GetterHeader+
        "void f(ForeignRef o,ForeignRef other){const char*p=getter(true?o:other,0);foreign_consume(other,0);foreign_use(p);}",
        Language,Standard,ARC),true);
    const auto BlockExceptions = ForeignHeader+
        "void consume_block_noexcept(OwnerBlock __attribute__((ns_consumed))) noexcept;\n"
        "void consume_foreign_block_noexcept(ForeignRef __attribute__((cf_consumed)),OwnerBlock __attribute__((ns_consumed))) noexcept;\n";
    Expect(Request(Root,BlockExceptions+
        "void f(OwnerBlock o){try{keepAlive(o);consume_block_noexcept(o);}catch(...){foreign_use(block_bytes(o));}}",
        Language,Standard,ARC),true);
    Expect(Request(Root,BlockExceptions+
        "void f(ForeignRef o,OwnerBlock b){const char*p=foreign_bytes(o);try{consume_foreign_block_noexcept(o,b);}catch(...){foreign_use(p);}}",
        Language,Standard,ARC),true);
    const auto Exceptions = ForeignHeader+
        "void consume_foreign_noexcept(ForeignRef __attribute__((cf_consumed)),int) noexcept;\n"
        "int foreign_argument_that_may_throw();\n";
    Expect(Request(Root,Exceptions+
        "void f(ForeignRef o){const char*p=foreign_bytes(o);try{consume_foreign_noexcept(o,foreign_argument_that_may_throw());}catch(...){foreign_use(p);}}",
        Language,Standard,ARC),true);
    Expect(Request(Root,ForeignHeader+
        "void f(ForeignRef o){const char*p=foreign_bytes(o);try{foreign_consume(o,0);}catch(...){foreign_use(p);}}",
        Language,Standard,ARC),false,"after its owner lifetime ended");
  }
  const std::vector<std::string> Accepted = {
      "void f(id o) { borrowed const char *p = bytes(o); use(p); }",
      "void f(id o) { const char *p = bytes(o); void *q = (void*)p; use_noescape((const char*)q); }",
      "void f(id o) { const char *p = bytes(o); p = 0; escape(p); }",
      "void f(id o, int b) { const char *p = bytes(o); if(b) p=0; else p=\"owned\"; escape(p); }",
      "void f(id o, int b) { const char *p = bytes(o); while(b--) { use(p); } }",
      "void f(id o) { const char *p = bytes(o); invoke(^{ use(p); }); }",
      "void f(id o) { const char *p = bytes(o); (^{ use(p); })(); }",
      "void f(id o) { const char *p = bytes(o); void (^b)(void)=^{ use(p); }; b(); }",
      "int f(id o) { const char *p=bytes(o); char out[4]; __builtin_memcpy(out,p,4); return out[0]+p[0]; }",
      "void f(id o) { withLifetime(o) { const char *p=bytes(o); use(p); } }",
      "void f(id o, id replacement) { keepAlive(o); const char *p=bytes(o); o=replacement; use(p); }",
      "void f(id o, id replacement) { const char *p=bytes(o); keepAlive(o); o=replacement; use(p); }",
      "void f(id o) { const char *p=bytes(o); withLifetime(o) { use(p); } use(p); }",
      "void f(id o, id replacement) { id alias=o; const char *p=bytes(o); keepAlive(alias); o=replacement; use(p); }",
      "void f(id a,id b,int choose) { use(bytes(choose?a:b)); }",
      "void f(id o) { const char *p=bytes(o); if(!p) escape(p); else use(p); }",
      "void f(id o) { const char *p=bytes(o); if(p==0) global_pointer=p; }",
      "void f(id o) { const char *p=bytes(o); const char **alias=&p; *alias=0; escape(p); }",
      "void f(id o) { const char *p=bytes(o); const char **alias; alias=&p; use(*alias); }",
      "void f(id o) { struct Holder h={bytes(o)}; struct Holder *alias=&h; alias->pointer=0; escape(h.pointer); }",
      "void f(id o) { const char *p[2]={bytes(o),0}; const char **alias=p; *alias=0; escape(p[0]); }",
      "void f(id o) { const char *p=bytes(o); invoke(^[move p]{use(p);}); escape(p); }",
      "void f(id o) { const char *p=bytes(o); invoke(^[move o,p]{(void)o;use(p);}); }",
      "void f(id o) { Optional(BorrowedBytes) p=Some(BorrowedBytes,bytes(o)); invoke(^{(void)p;}); }",
      "void f(const char *p __attribute__((noescape))) { use_noescape(p); }",
      "char f(const char *p __attribute__((noescape))) { return p[0]; }",
      "void f(id o,int choose) { const char *p=bytes(o),*q=0; const char **a; if(choose)a=&p;else a=&q; p=0;escape_address(a); }",
      "void f(id o) { const char *p=bytes(o); struct AddressHolder h={&p}; p=0;escape_address(h.pointer); }",
      "void f(id o) { struct Pair h={{bytes(o)},{0}}; escape(h.right.pointer);h.left.pointer=0;escape(h.left.pointer); }",
      "void f(id o) { struct Pair source={{bytes(o)},{0}}; struct Pair copy=source;use(copy.left.pointer);escape(copy.right.pointer);copy.left.pointer=0;escape(copy.left.pointer); }",
      "void f(id o) { struct Holder h[2]={{bytes(o)},{0}};escape(h[1].pointer);h[0].pointer=0;escape(h[0].pointer); }",
      "void f(id o) { struct Holder h[2][2]={{{bytes(o)},{0}},{{0},{0}}};escape(h[0][1].pointer);h[0][0].pointer=0;escape(h[0][0].pointer); }",
      "void f(id o) { struct Holder source={bytes(o)},copy={0};__builtin_memcpy(&copy,&source,sizeof copy);use(copy.pointer);copy.pointer=0;escape(copy.pointer); }",
      "void f(id o,void *heap) { const char *p=bytes(o);__builtin_memcpy(heap,&p,0); }",
      "typedef BorrowedBytes (*Provider)(id); Provider provider=bytes; void f(id o) {use(provider(o));}",
      "BorrowedBytes f(id o) __attribute__((objc_returns_borrowed(owner_index=0))) { Optional(BorrowedBytes) s=Some(BorrowedBytes,bytes(o));guard let p=s else{return 0;}return p; }",
      "BorrowedBytes f(id o) __attribute__((objc_returns_borrowed(owner_index=0))) { Optional(BorrowedBytes) s=Some(BorrowedBytes,bytes(o));if let p=s {return p;}return 0; }",
      "BorrowedBytes f(id o) __attribute__((objc_returns_borrowed(owner_index=0))) { Optional(BorrowedBytes) s=Some(BorrowedBytes,bytes(o));return match(s){case __objc3_case_some(let p)=>p;case __objc3_case_none=>(BorrowedBytes)0;}; }",
      "void f(id o, id replacement) { const char *p=bytes(o); o=replacement; p=bytes(o); use(p); }",
      "BorrowedBytes f(id o) __attribute__((objc_returns_borrowed(owner_index=0))) { return bytes(o); }",
      "BorrowedBytes f(id o) __attribute__((objc_returns_borrowed(owner_index=0))) { id alias=o; return bytes(alias); }",
      "void f(id o) { const char *p[2]={bytes(o),0}; escape(p[1]); p[0]=0; escape(p[0]); }",
      "void f(void) { borrowed const char *p=0; escape(p); }",
      "long f(id o) { const char *p=bytes(o); return (p+2)-p; }",
      "BorrowedBytes f(id a,id o) __attribute__((objc_returns_borrowed(owner_index=1))) { return bytes(o); }",
      "@returns_borrowed(owner: 0) borrowed const char *f(id o) { return bytes(o); }",
      "int borrowed(int v) { return v; } int f(void) { return borrowed(3); }",
      "void f(id o) { const char *p; { id q=make_owner(); p=bytes(q); use(p); p=0; } escape(p); }",
      "void f(id o) { struct Holder h={bytes(o)}; use(h.pointer); h.pointer=0; escape(h.pointer); }",
  };
  for (const auto &Code : Accepted)
    Expect(Request(Root, Header + Code, Language, Standard, ARC), true);
  const std::vector<std::string> Escapes = {
      "void f(id o) { global_pointer=bytes(o); }",
      "void f(id o) { static const char *p; p=bytes(o); }",
      "void f(id o) { static __thread const char *p; p=bytes(o); }",
      "void f(id o, struct Holder *h) { h->pointer=bytes(o); }",
      "void f(id o) { escape(bytes(o)); }",
      "void f(id o) { const char *p=bytes(o); const char **alias=&p; escape_address(alias); }",
      "void f(id o) { const char *p=bytes(o); const char **alias=&p; invoke(^{escape(*alias);}); }",
      "void f(id o) { struct Holder h={bytes(o)}; struct Holder *alias=&h; escape(alias->pointer); }",
      "void f(id o) { const char *p[2]={bytes(o),0}; const char **alias=p; escape(*alias); }",
      "void f(id o) { const char *p=bytes(o); launch(^[move p]{use(p);}); }",
      "void f(id o) { const char *p=bytes(o); invoke(^[move o,p]{(void)o;use(p);}); use(p); }",
      "void f(id o) { Optional(BorrowedBytes) p=Some(BorrowedBytes,bytes(o)); launch(^{(void)p;}); }",
      "void f(const char *p __attribute__((noescape))) { global_pointer=p; }",
      "const char *f(const char *p __attribute__((noescape))) { return p; }",
      "void f(id o,int choose) { const char *p=0,*q=0;const char **a;if(choose)a=&p;else a=&q;p=bytes(o);escape_address(a); }",
      "void f(id o) { const char *p=0;struct AddressHolder h={&p};p=bytes(o);escape_address(h.pointer); }",
      "void f(id o) { struct Pair h={{bytes(o)},{0}};h.right.pointer=0;escape(h.left.pointer); }",
      "void f(id o) { struct Holder h[2]={{0},{0}};h[0].pointer=bytes(o);h[1].pointer=0;escape(h[0].pointer); }",
      "void f(id o,int index) { struct Holder h[2]={{0},{0}};h[index].pointer=bytes(o);h[index].pointer=0;escape(h[0].pointer); }",
      "void f(id o) { struct Pair source={{bytes(o)},{0}},copy=source;escape(copy.left.pointer); }",
      "void f(id o,void *heap) { struct Pair h={{bytes(o)},{0}};__builtin_memcpy(heap,&h,sizeof h); }",
      "void f(id o) { struct Holder source={bytes(o)},copy={0};__builtin_memmove(&copy,&source,sizeof copy);escape(copy.pointer); }",
      "void f(id o) { union Overlap{const char *a;const char *b;};union Overlap u={0};u.a=bytes(o);escape(u.b); }",
      "void f(id o) { Optional(BorrowedBytes) s=Some(BorrowedBytes,bytes(o));guard let p=s else{return;}escape(p); }",
      "void f(id o) { const char *p=bytes(o); launch(^{ use(p); }); }",
      "void f(id o) { const char *p=bytes(o); invoke(^{ escape(p); }); }",
      "void f(id o) { const char *p[2]={bytes(o),0}; escape(p[0]); }",
      "void f(id o, char *owned) { const char *p=bytes(o); global_pointer=(const char*)__builtin_memcpy((void*)p,owned,1); }",
      "void f(id o) { const char *p=bytes(o); global_block=^{ use(p); }; }",
      "const char *f(id o) { return bytes(o); }",
      "BorrowedBytes f(id a,id b) __attribute__((objc_returns_borrowed(owner_index=0))) { return bytes(b); }",
      "unsigned long f(id o) { return (unsigned long)bytes(o); }",
      "void f(id o) { void *p=(void*)bytes(o); global_pointer=(const char*)p; }",
      "void f(id o, int b) { const char *p=0; if(b) p=bytes(o); escape(p); }",
      "void f(id o, id replacement) { const char *p=bytes(o); o=replacement; use(p); }",
      "void f(void) { const char *p; { id o=make_owner(); p=bytes(o); } use(p); }",
      "void f(id o, void *heap) { const char *p=bytes(o); __builtin_memcpy(heap,&p,sizeof p); }",
      "void f(id o) { const char *p=bytes(o); struct Holder h={p}; global_pointer=h.pointer; }",
  };
  for (const auto &Code : Escapes)
    Expect(Request(Root, Header + Code, Language, Standard, ARC), false,
           "borrowed pointer escapes");
  Expect(Request(Root, Header + "void f(void) { use(bytes(make_owner())); }",
                 Language, Standard, ARC), false, "non-stable owner");
  for (const auto &Code : std::vector<std::string>{
           "borrowed int bad;",
           "borrowed int (*bad)(void);",
           "const char *f(id o) __attribute__((objc_returns_borrowed(owner_index=0)));",
           "BorrowedBytes f(id o) __attribute__((objc_returns_borrowed(owner_index=1)));",
           "BorrowedBytes f(id o) __attribute__((objc_returns_borrowed(owner_index=-1)));",
           "BorrowedBytes f(int o) __attribute__((objc_returns_borrowed(owner_index=0)));",
           "BorrowedBytes f(id o) __attribute__((objc_returns_borrowed(0)));"})
    Expect(Request(Root, Header + Code, Language, Standard, ARC), false);
  for (const auto &Code : std::vector<std::string>{
           "void f(borrowed const char*); void f(const char*);",
           "BorrowedBytes f(id); const char *f(id);",
           "typedef BorrowedBytes Local; typedef const char *Local;",
           "extern BorrowedBytes slot; extern const char *slot;",
           "void f(Optional(BorrowedBytes)); void f(Optional(const char *));",
           "BorrowedBytes f(id a,id b) __attribute__((objc_returns_borrowed(owner_index=0))); BorrowedBytes f(id a,id b) __attribute__((objc_returns_borrowed(owner_index=1)));"})
    Expect(Request(Root, Header + Code, Language, Standard, ARC), false,
           Code.starts_with("typedef") ? "borrowed pointer typedef contract"
           : Code.starts_with("extern") ? "borrowed pointer variable contract"
                                       : "borrowed pointer declaration contract");
  if (Language == BaseLanguage::ObjectiveCxx) {
    for (const auto &Code : std::vector<std::string>{
             "void f(id o) { Holder a={bytes(o)},b={0};b=a;use(b.pointer);b.pointer=0;escape(b.pointer); }",
             "struct Scalar{int number;const char *pointer;};int f(const Scalar& s __attribute__((noescape))){return s.number;}",
             "struct Copied{char value;Copied(const char *p __attribute__((noescape))):value(p[0]){}};void f(id o){Copied value(bytes(o));}"})
      Expect(Request(Root, Header + Code, Language, Standard, ARC), true);
    for (const auto &Code : std::vector<std::string>{
             "void f(id o) { Holder a={bytes(o)},b={0};b=a;escape(b.pointer); }",
             "struct Saved{const char *pointer;Saved(const char *p):pointer(p){}};void f(id o){Saved value(bytes(o));}",
             "struct Saved{const char *pointer;Saved(const char *p __attribute__((noescape))):pointer(p){}};",
             "void f(id o){(void)new Holder{bytes(o)};}"})
      Expect(Request(Root, Header + Code, Language, Standard, ARC), false,
             "borrowed pointer escapes");
    const std::string LambdaHeader=Header+
        "template<class Work> void sync(const Work &work __attribute__((noescape))) {work();}\n"
        "template<class Work> void sink(Work) {}\n";
    for (const auto &Code : std::vector<std::string>{
             "void f(id o){const char *p=bytes(o);auto work=[p]{use_noescape(p);};p=0;auto copy=work;copy();work.operator()();sync(copy);}",
             "void f(id o){auto work=[p=bytes(o)]{use_noescape(p);};work();}",
             "void f(id o){const char *p=bytes(o);auto work=[p](auto){use_noescape(p);};work(0);}",
             "void f(id o){Holder h{bytes(o)};auto work=[h]{use_noescape(h.pointer);};h.pointer=0;work();}",
             "void f(id o){const char *p=bytes(o);auto work=[p]{auto nested=[p]{use_noescape(p);};nested();};work();}"})
      Expect(Request(Root,LambdaHeader+Code,Language,Standard,ARC),true);
    for (const auto &Code : std::vector<std::string>{
             "void f(id o){const char *p=bytes(o);auto work=[p]{escape(p);};work();}",
             "void f(id o){auto work=[p=bytes(o)]{escape(p);};work();}",
             "void f(id o){const char *p=bytes(o);auto work=[p]{};sink(work);}",
             "auto f(id o){const char *p=bytes(o);return [p]{};}",
             "void f(id o){const char *p=bytes(o);auto work=[&p]{use_noescape(p);};work();}",
             "void f(id o){const char *p=0;auto work=[&p]{escape(p);};p=bytes(o);work();}",
             "void f(id o){const char *p=0;AddressHolder h{&p};auto work=[h]{escape(*h.pointer);};p=bytes(o);work();}",
             "void f(id o,int choose){const char *p=0;if(choose){auto work=[&p]{escape(p);};sink(work);}p=bytes(o);}",
             "void f(id o){auto work=[o,p=(const char*)0]()mutable{p=bytes(o);};work();}",
             "void f(id o){struct V{mutable const char *p;};auto work=[o,v=V{0}]{v.p=bytes(o);};work();}"})
      Expect(Request(Root,LambdaHeader+Code,Language,Standard,ARC),false,"borrowed pointer escapes");
    for(const std::string &Code:{
        "void f(id o){const char*p=bytes(o);auto work=[p,count=0]()mutable{++count;use_noescape(p);};work();work();}",
        "void f(id o,int &count){const char*p=bytes(o);auto work=[p,&count]()mutable{++count;use_noescape(p);};work();}"})
      Expect(Request(Root,LambdaHeader+Code,Language,Standard,ARC),true);
    Expect(Request(Root,LambdaHeader+
        "void f(id o){const char*p=bytes(o);auto work=[p]()mutable{use_noescape(p);p=nullptr;};work();}",
        Language,Standard,ARC),false,"mutable or generic C++ lambda capture");
    const std::string PathHeader=LambdaHeader+
        "typedef borrowed const char *PathBytes;\n"
        "PathBytes path_bytes(void*) __attribute__((objc_returns_borrowed(owner_index=0)));\n"
        "void path_consume(void* __attribute__((cf_consumed)));\n";
    for(const std::string &Code:{
        "void f(void*o){const char*p=path_bytes(o);auto inner=[o]{path_consume(o);};struct H{decltype(inner) callback;};H h{inner};auto work=[h]{h.callback();};work();use_noescape(p);}",
        "void f(void*a,void*b){const char*p=path_bytes(b);void(^callbacks[2])()={^{path_consume(a);},^{path_consume(b);}};auto work=[callbacks]{callbacks[1]();};work();use_noescape(p);}"})
      Expect(Request(Root,PathHeader+Code,Language,Standard,ARC),false,"after its owner lifetime ended");
    for(const std::string &Code:{
        "void f(void*a,void*b){const char*p=path_bytes(b);auto one=[a]{path_consume(a);};auto two=[b]{path_consume(b);};struct H{decltype(one) a;decltype(two) b;};H h{one,two};auto work=[h]{h.a();};work();use_noescape(p);}",
        "void f(void*a,void*b){const char*p=path_bytes(a);void(^callbacks[2])()={^{path_consume(a);},^{path_consume(b);}};auto work=[callbacks]{callbacks[1]();};work();use_noescape(p);}"})
      Expect(Request(Root,PathHeader+Code,Language,Standard,ARC),true);
    std::string LargeArray=PathHeader+
        "void f(void*o,int index){const char*p=path_bytes(o);void(^callbacks[128])()={";
    for(int i=0;i!=127;++i)LargeArray+="^{},";
    LargeArray+="^{path_consume(o);}};auto work=[callbacks,index]{callbacks[index]();};work();use_noescape(p);}";
    Expect(Request(Root,LargeArray,Language,Standard,ARC),false,"after its owner lifetime ended");
    if (ARC) {
      Expect(Request(Root,LambdaHeader+
          "void f(id o){const char*p=bytes(o);auto work=[o,p]()mutable{use_noescape(p);o=nullptr;};work();}",
          Language,Standard,ARC),false,"mutable or generic C++ lambda capture");
      Expect(Request(Root,LambdaHeader+
          "void f(id o){const char *p=bytes(o);auto work=[o,p]{(void)o;use_noescape(p);};o=0;work();}",
          Language,Standard,ARC),true);
      Expect(Request(Root,LambdaHeader+
          "void f(id o){const char *p=bytes(o);auto work=[copy=o,p]{(void)copy;use_noescape(p);};o=0;work();}",
          Language,Standard,ARC),true);
      for (const auto &Code : std::vector<std::string>{
               "void f(id o){const char *p=bytes(o);auto work=[p]{use_noescape(p);};o=0;work();}",
               "void f(id o){const char *p=bytes(o);auto work=[p]{use_noescape(p);};o=0;work.operator()();}"})
        Expect(Request(Root,LambdaHeader+Code,Language,Standard,ARC),false,"owner lifetime ended");
    }
    // Objective-C++ dynamic initialization must receive the mandatory check,
    // including aggregate initialization and out-of-line static members.
    for (const auto &Code : std::vector<std::string>{
             "extern id owner; const char *leak=bytes(owner);",
             "extern id owner; Holder leak={bytes(owner)};",
             "extern id owner; struct Static { static const char *pointer; }; const char *Static::pointer=bytes(owner);",
             // Argument values are captured when evaluated. Clearing a later
             // argument cannot erase an earlier argument's obligation.
             "void f(id o) { const char *p=0; pair_escape((p=bytes(o),p),p=0); }"})
      Expect(Request(Root, Header + Code, Language, Standard, ARC), false,
             "borrowed pointer escapes");
  }
  const std::string ConsumedHeader=Header+
      "void consume_claim(id owner __attribute__((ns_consumed)),int unused);\n"
      "void consume_pair(id owner __attribute__((ns_consumed)),const char *p __attribute__((noescape)));\n"
      "typedef void (*OwnerCallback)(id owner __attribute__((ns_consumed)),int);\n"
      "typedef void (^OwnerBlock)(id owner __attribute__((ns_consumed)),int);\n"
      "__attribute__((objc_root_class)) @interface ConsumedOwner\n"
      "- (void)consume __attribute__((ns_consumes_self));\n"
      "- (void)consumeAfter:(int)unused __attribute__((ns_consumes_self));\n@end\n";
  for (const auto &Code : std::vector<std::string>{
           "void f(ConsumedOwner *_Nonnull o){keepAlive(o);const char*p=bytes(o);consume_pair(o,p);use_noescape(p);}",
           "void f(ConsumedOwner *_Nonnull o){keepAlive(o);const char*p=bytes(o);[o consume];use_noescape(p);}"})
    Expect(Request(Root,ConsumedHeader+Code,Language,Standard,ARC),true);
  for (const auto &Code : std::vector<std::string>{
           "void f(ConsumedOwner *_Nonnull o){const char*p=bytes(o);consume_claim(o,0);use_noescape(p);}",
           "void f(ConsumedOwner *_Nonnull o){const char*p=bytes(o);[o consume];use_noescape(p);}",
           "void f(ConsumedOwner *_Nonnull o,OwnerCallback callback){const char*p=bytes(o);callback(o,0);use_noescape(p);}",
           "void f(ConsumedOwner *_Nonnull o,OwnerBlock callback){const char*p=bytes(o);callback(o,0);use_noescape(p);}",
           "void f(ConsumedOwner *_Nonnull o){const char*p=bytes(o);consume_pair(o,p);}"})
    Expect(Request(Root,ConsumedHeader+Code,Language,Standard,ARC),ARC,
           ARC ? "" : "borrowed pointer escapes");
  if (Language==BaseLanguage::ObjectiveCxx) {
    Expect(Request(Root,ConsumedHeader+
        "void f(ConsumedOwner *_Nonnull o,ConsumedOwner *_Nonnull r){ConsumedOwner *_Nonnull old=o;const char*p=bytes(old);consume_claim(o,(o=r,0));use_noescape(p);}",
        Language,Standard,ARC),ARC,ARC ? "" : "borrowed pointer escapes");
    Expect(Request(Root,ConsumedHeader+
        "struct Sink{Sink(id owner __attribute__((ns_consumed)));};void f(ConsumedOwner *_Nonnull o){const char*p=bytes(o);Sink sink(o);use_noescape(p);}",
        Language,Standard,ARC),ARC,ARC ? "" : "borrowed pointer escapes");
    const std::string ExceptionHeader=ConsumedHeader+
        "void consume_noexcept(id owner __attribute__((ns_consumed)),int unused) noexcept;\n"
        "int argument_that_may_throw();\n";
    auto BeforeEntry=Request(Root,ExceptionHeader+
        "void f(ConsumedOwner *_Nonnull o){const char*p=bytes(o);try{consume_noexcept(o,argument_that_may_throw());}catch(...){use_noescape(p);}}",
        Language,Standard,ARC);
    BeforeEntry.arguments.push_back("-fexceptions");
    Expect(BeforeEntry,true);
    auto AfterEntry=Request(Root,ExceptionHeader+
        "void f(ConsumedOwner *_Nonnull o){const char*p=bytes(o);try{consume_claim(o,0);}catch(...){use_noescape(p);}}",
        Language,Standard,ARC);
    AfterEntry.arguments.push_back("-fexceptions");
    Expect(AfterEntry,ARC,ARC ? "" : "borrowed pointer escapes");
  }
  const std::string SelfHeader=Header+
      "__attribute__((objc_root_class)) @interface BorrowedSelf\n"
      "- (void)usePointer;\n@end\n@implementation BorrowedSelf\n";
  Expect(Request(Root,SelfHeader+
      "- (void)usePointer {const char *p=bytes(self);use_noescape(p);}@end",
      Language,Standard,ARC),true);
  Expect(Request(Root,SelfHeader+
      "- (void)usePointer {escape(bytes(self));}@end",
      Language,Standard,ARC),false,"borrowed pointer escapes");
  if (!ARC) {
    const std::string ManualHeader=Header+
        "__attribute__((objc_root_class)) @interface ManualOwner\n"
        "- (oneway void)release;\n@end\n";
    for (const auto &Code : std::vector<std::string>{
             "void f(ManualOwner *_Nonnull o){const char*p=bytes(o);use_noescape(p);[o release];}",
             "void f(ManualOwner *_Nonnull o){keepAlive(o);const char*p=bytes(o);[o release];use_noescape(p);use_noescape(bytes(o));}",
             "void f(ManualOwner *_Nonnull o){ManualOwner *_Nonnull a=o;const char*p=bytes(o);withLifetime(o){[a release];use_noescape(p);use_noescape(bytes(o));}}",
             "void f(ManualOwner *_Nonnull o,ManualOwner *_Nonnull r){[o release];o=r;use_noescape(bytes(o));}"})
      Expect(Request(Root,ManualHeader+Code,Language,Standard,ARC),true);
    for (const auto &Code : std::vector<std::string>{
             "void f(ManualOwner *_Nonnull o){const char*p=bytes(o);[o release];use_noescape(p);}",
             "void f(ManualOwner *_Nonnull o){ManualOwner *_Nonnull a=o;const char*p=bytes(o);[a release];use_noescape(p);}",
             "void f(ManualOwner *_Nonnull o){ManualOwner *_Nonnull a=o;const char*p=bytes(a);[o release];use_noescape(p);}",
             "void f(ManualOwner *_Nonnull o,int c){const char*p=bytes(o);if(c)[o release];use_noescape(p);}",
             "void f(ManualOwner *_Nonnull o){[o release];use_noescape(bytes(o));}",
             "int f(ManualOwner *_Nonnull o){[o release];return bytes(o)[0];}",
             "void f(ManualOwner *_Nonnull o){[o release];ManualOwner *_Nonnull a=o;use_noescape(bytes(a));}",
             "void f(ManualOwner *_Nonnull o){[o release];keepAlive(o);use_noescape(bytes(o));}",
             "void f(ManualOwner *_Nonnull o){const char*p=bytes(o);withLifetime(o){[o release];use_noescape(p);}use_noescape(p);}",
             "void f(ManualOwner *_Nonnull o){[o release];void(^work)(void)=^{use_noescape(bytes(o));};work();}"})
      Expect(Request(Root,ManualHeader+Code,Language,Standard,ARC),false,"borrowed pointer escapes");
    if (Language==BaseLanguage::ObjectiveCxx)
      for (const auto &Code : std::vector<std::string>{
               "void f(ManualOwner *_Nonnull o){[o release];auto work=[o]{use_noescape(bytes(o));};work();}",
               "void f(ManualOwner *_Nonnull o){[o release];auto work=[copy=o]{use_noescape(bytes(copy));};work();}"})
        Expect(Request(Root,ManualHeader+Code,Language,Standard,ARC),false,"borrowed pointer escapes");
  }
  auto Compatibility = Request(Root, Header + "void f(id o) { global_pointer=bytes(o); }",
                                Language, Standard, ARC);
  Compatibility.profile = SemanticProfile::Compatibility;
  Expect(Compatibility, true);
}
std::unique_ptr<clang::ASTUnit> Parse(const std::string &Source,
                                     const std::string &Filename,
                                     const std::vector<std::string> &Extra = {}) {
  std::vector<std::string> Arguments = {"-x", "objective-c++", "-std=c++20",
      "-fobjc-version=3", "-fobjc3-profile=strict-system", "-fblocks",
      "-fobjc-runtime=gnustep-2.2", "--target=x86_64-unknown-linux-gnu"};
  Arguments.insert(Arguments.end(), Extra.begin(), Extra.end());
  auto AST = clang::tooling::buildASTFromCodeWithArgs(Source, Arguments, Filename);
  assert(AST && !AST->getDiagnostics().hasErrorOccurred());
  return AST;
}
clang::FunctionDecl *Function(clang::ASTContext &Context, const std::string &Name) {
  for (clang::Decl *D : Context.getTranslationUnitDecl()->decls())
    if (auto *F = llvm::dyn_cast<clang::FunctionDecl>(D))
      if (F->getNameAsString() == Name)
        return F;
  assert(false && "expected borrowed contract function");
  return nullptr;
}
void PersistenceCases(const fs::path &Root) {
  const std::string Provider =
      "typedef borrowed const char *BorrowedBytes;\n"
      "BorrowedBytes exported(id a,id b) __attribute__((objc_returns_borrowed(owner_index=1)));\n"
      "void noescape_api(const char* p __attribute__((noescape)));\n"
      "typedef void (*ConsumedOwnerCallback)(id owner __attribute__((ns_consumed)));\n"
      "typedef id (*RetainedOwnerFactory)(void) __attribute__((ns_returns_retained));\n"
      "using RetainedOwnerAlias = id (__attribute__((ns_returns_retained)) *)(void);\n"
      "typedef id (__attribute__((ns_returns_retained)) *(*HigherOwnerFactory)(void))(void);\n";
  const auto HeaderPath = Root / "provider.h";
  { std::ofstream Out(HeaderPath); Out << Provider; }
  auto From = Parse(Provider, HeaderPath.string());
  auto To = Parse("", (Root / "consumer.mm").string());
  auto *F = Function(From->getASTContext(), "exported");
  assert(clang::isObjC3BorrowedType(From->getASTContext(), F->getReturnType()));
  assert(clang::getObjC3BorrowedReturnContract(F)->getOwnerIndex() == 1);
  clang::ASTImporter Importer(To->getASTContext(), To->getFileManager(),
      From->getASTContext(), From->getFileManager(), false);
  auto Imported = Importer.Import(F);
  assert(Imported);
  auto *ImportedFunction = llvm::cast<clang::FunctionDecl>(*Imported);
  assert(clang::isObjC3BorrowedType(To->getASTContext(), ImportedFunction->getReturnType()));
  assert(clang::getObjC3BorrowedReturnContract(ImportedFunction)->getOwnerIndex() == 1);
  auto Conflict = Parse("const char *exported(id,id);", (Root / "conflict.mm").string());
  clang::ASTImporter ConflictImporter(Conflict->getASTContext(), Conflict->getFileManager(),
      From->getASTContext(), From->getFileManager(), false);
  auto Failed = ConflictImporter.Import(F);
  assert(!Failed && "AST import must reject erased borrowed contract identity");
  llvm::consumeError(Failed.takeError());
  const auto PCHPath = Root / "borrowed.pch";
  assert(!From->Save(PCHPath.string()));
  auto PCHClient = Parse(
      "BorrowedBytes forward(id a,id b) __attribute__((objc_returns_borrowed(owner_index=1))) { return exported(a,b); }",
      (Root / "pch-client.mm").string(), {"-include-pch", PCHPath.string()});
  auto *Forward = Function(PCHClient->getASTContext(), "forward");
  assert(clang::isObjC3BorrowedType(PCHClient->getASTContext(), Forward->getReturnType()));
  // Exercise the maintained source-free interface path as well as native
  // AST serialization: both the typedef marker and named owner must survive.
  auto InterfaceProvider = Request(Root, Provider, BaseLanguage::ObjectiveCxx,
                                    "c++20", false);
  InterfaceProvider.emit_semantic_interface = true;
  InterfaceProvider.module_name = "Borrowed.Contracts";
  InterfaceProvider.package_identity = "borrowed-contracts-v1";
  InterfaceProvider.interface_output_path = (Root / "Borrowed.objc3i").string();
  auto Emitted = Compile(InterfaceProvider);
  if (!Emitted.success)
    for (const auto &D : Emitted.diagnostics)
      std::cerr << D.message << '\n';
  assert(Emitted.success && Emitted.interface_artifact);
  assert(Emitted.interface_artifact->source_bytes.find("owner_index=1") != std::string::npos);
  assert(Emitted.interface_artifact->sidecar_json.find(
             "type.pointee:ns-returns-retained") != std::string::npos &&
         "retained callback results require ownership identity");
  auto InterfaceClient = Request(Root,
      "BorrowedBytes imported_forward(id a,id b) __attribute__((objc_returns_borrowed(owner_index=1))) { noescape_api(exported(a,b)); return exported(a,b); }",
      BaseLanguage::ObjectiveCxx, "c++20", false);
  InterfaceImportInput Input;
  Input.source_path = InterfaceProvider.interface_output_path;
  Input.locked_identity = Emitted.interface_artifact->identity;
  InterfaceClient.imported_interfaces = {Input};
  Expect(InterfaceClient, true);
  InterfaceClient.source_text = "const char *imported_escape(id a,id b) { return exported(a,b); }";
  Expect(InterfaceClient, false, "borrowed pointer escapes");
  InterfaceClient.source_text=
      "void imported_consume(id owner,ConsumedOwnerCallback callback){const char *p=exported(owner,owner);callback(owner);noescape_api(p);}";
  Expect(InterfaceClient,false,"borrowed pointer escapes");
  InterfaceClient.source_text =
      "void imported_erasure(ConsumedOwnerCallback callback){(void)(void (*)(id))callback;}";
  Expect(InterfaceClient,false,"callable cast changes an ownership");
  InterfaceClient.source_text =
      "void imported_result(RetainedOwnerFactory factory){(void)(id (*)(void))factory;}";
  Expect(InterfaceClient,false,"callable cast changes an ownership");
  InterfaceClient.source_text =
      "void imported_preserved(RetainedOwnerFactory factory,RetainedOwnerAlias alias,HigherOwnerFactory higher){RetainedOwnerFactory copy=(RetainedOwnerFactory)factory;copy=alias;copy=higher();(void)copy;}";
  Expect(InterfaceClient,true);
  auto OtherOwner = Parse(
      "typedef borrowed const char *BorrowedBytes; BorrowedBytes exported(id a,id b) __attribute__((objc_returns_borrowed(owner_index=0)));",
      (Root / "other.mm").string());
  clang::ODRHash First, Second;
  First.AddFunctionDecl(F);
  Second.AddFunctionDecl(Function(OtherOwner->getASTContext(), "exported"));
  assert(First.CalculateHash() != Second.CalculateHash());
}
void RetainablePersistenceCases(const fs::path &Root) {
  const std::string Provider =
      "typedef struct FamilyObject{int marker;} *FamilyRef __attribute__((objc_retainable_family(Example)));\n"
      "typedef FamilyRef FamilyAlias;\n"
      "FamilyRef family_hold(FamilyRef) __attribute__((objc_family_retain(Example)));\n"
      "void family_drop(FamilyAlias) __attribute__((objc_family_release(Example)));\n"
      "void family_drop(FamilyAlias);\n"
      "FamilyRef family_schedule(FamilyRef) __attribute__((objc_family_autorelease(Example)));\n"
      "typedef borrowed const char *FamilyBytes;\n"
      "FamilyBytes family_bytes(FamilyRef owner) __attribute__((objc_returns_borrowed(owner_index=0)));\n"
      "void family_use(const char *pointer __attribute__((noescape)));\n"
      "typedef struct IntegratedObject{int marker;} *IntegratedRef __attribute__((NSObject,objc_retainable_family(Integrated)));\n"
      "void integrated_drop(IntegratedRef) __attribute__((objc_family_release(Integrated)));\n";
  for(bool ARC:{false,true}) {
    auto Emission=Request(Root,Provider,BaseLanguage::ObjectiveCxx,"c++23",ARC);
    Emission.emit_semantic_interface=true;
    Emission.module_name=ARC ? "Family.ARC" : "Family.MRC";
    Emission.package_identity="canonical-retainable-family-v1";
    Emission.interface_output_path=(Root/(ARC ? "FamilyARC.objc3i" : "FamilyMRC.objc3i")).string();
    auto Emitted=Compile(Emission);
    if(!Emitted.success) {
      std::cerr << Emitted.failure << '\n';
      for(const auto &D:Emitted.diagnostics) std::cerr << D.message << '\n';
      if(Emitted.interface_artifact)
        for(const auto &D:Emitted.interface_artifact->diagnostics) std::cerr << D.message << '\n';
    }
    assert(Emitted.success && Emitted.interface_artifact);
    for(const char *Marker:{"retainable-family","family-retain","family-release","family-autorelease","objc-integrated"})
      assert(Emitted.interface_artifact->sidecar_json.find(Marker)!=std::string::npos);
    InterfaceImportInput Input;
    Input.source_path=Emission.interface_output_path;
    Input.locked_identity=Emitted.interface_artifact->identity;
    auto Client=Request(Root,"void accepted(FamilyRef owner){FamilyRef held=family_hold(owner);family_drop(family_schedule(held));}",
        BaseLanguage::ObjectiveCxx,"c++23",ARC);
    Client.imported_interfaces={Input};
    Expect(Client,true);
    Client.source_text="void expired(FamilyRef owner){const char*p=family_bytes(owner);family_drop(owner);family_use(p);}";
    Expect(Client,false,"after its owner lifetime ended");
    Client.source_text="void integrated(IntegratedRef owner){integrated_drop(owner);}";
    Expect(Client,!ARC,ARC ? "direct retainable family operation" : "");
  }
}
void ForeignPersistenceCases(const fs::path &Root) {
  const std::string Provider =
      "typedef struct ForeignOwner{int marker;} *ForeignRef;\n"
      "typedef borrowed const char *ForeignBytes;\n"
      "ForeignBytes foreign_bytes(ForeignRef owner) __attribute__((objc_returns_borrowed(owner_index=0)));\n"
      "void foreign_use(const char *pointer __attribute__((noescape)));\n"
      "void foreign_escape(const char *pointer);\n"
      "inline void foreign_clear(ForeignRef owner){struct H{const char*p;};H h{foreign_bytes(owner)};__builtin_memset(&h,0,sizeof h);foreign_escape(h.p);}\n"
      "void foreign_consume(ForeignRef owner __attribute__((cf_consumed)));\n"
      "typedef void (*ForeignCallback)(ForeignRef owner __attribute__((cf_consumed)));\n"
      "void dispatch_foreign(ForeignCallback);void dispatch_foreign(void (*)(ForeignRef));\n"
      "inline void foreign_scoped(ForeignRef owner){auto work=[owner,count=0]()mutable{++count;foreign_use(foreign_bytes(owner));};work();work();}\n"
      "inline void foreign_release_locally(ForeignRef owner __attribute__((cf_consumed))){const char*p=foreign_bytes(owner);auto inner=[owner]{foreign_consume(owner);};struct H{decltype(inner) callback;};H holder{inner};auto work=[holder]{holder.callback();};foreign_use(p);work();}\n"
      "__attribute__((objc_root_class)) @interface ForeignContract\n"
      "- (void)dropForeign:(ForeignRef __attribute__((cf_consumed)))owner;\n"
      "@end\n";
  for (bool ARC : {false,true}) {
    auto Emission = Request(Root,Provider,BaseLanguage::ObjectiveCxx,"c++23",ARC);
    Emission.emit_semantic_interface=true;
    Emission.module_name=ARC ? "Foreign.ARC" : "Foreign.MRC";
    Emission.package_identity="foreign-owner-contracts-v1";
    Emission.interface_output_path=(Root/(ARC ? "ForeignARC.objc3i" : "ForeignMRC.objc3i")).string();
    auto Emitted=Compile(Emission);
    if (!Emitted.success) {
      std::cerr << Emitted.failure << '\n';
      for(const auto &D:Emitted.diagnostics) std::cerr << D.message << '\n';
      if(Emitted.interface_artifact)
        for(const auto &D:Emitted.interface_artifact->diagnostics) std::cerr << D.message << '\n';
    }
    assert(Emitted.success && Emitted.interface_artifact);
    assert(Emitted.interface_artifact->sidecar_json.find("cf-consumed")!=std::string::npos);
    InterfaceImportInput Input;
    Input.source_path=Emission.interface_output_path;
    Input.locked_identity=Emitted.interface_artifact->identity;
    auto Client=Request(Root,
        "void preserved(ForeignCallback callback){ForeignCallback copy=(ForeignCallback)callback;(void)copy;}",
        BaseLanguage::ObjectiveCxx,"c++23",ARC);
    Client.imported_interfaces={Input};
    Expect(Client,true);
    Client.source_text="void overloads(ForeignRef owner,ForeignCallback callback,void(*plain)(ForeignRef)){foreign_scoped(owner);foreign_release_locally(owner);foreign_clear(owner);dispatch_foreign(callback);dispatch_foreign(plain);}";
    Expect(Client,true);
    Client.source_text="void expired(ForeignRef owner){const char*p=foreign_bytes(owner);foreign_consume(owner);foreign_use(p);}";
    Expect(Client,false,"after its owner lifetime ended");
    Client.source_text="void expired_callback(ForeignRef owner,ForeignCallback callback){const char*p=foreign_bytes(owner);callback(owner);foreign_use(p);}";
    Expect(Client,false,"after its owner lifetime ended");
    Client.source_text="void expired_message(ForeignContract *_Nonnull receiver,ForeignRef owner){const char*p=foreign_bytes(owner);[receiver dropForeign:owner];foreign_use(p);}";
    Expect(Client,false,"after its owner lifetime ended");
    Client.source_text="void erased(ForeignCallback callback){(void)(void(*)(ForeignRef))callback;}";
    Expect(Client,false,"callable cast changes an ownership");
  }
}
} // namespace
int main() {
  const auto Stamp = std::chrono::steady_clock::now().time_since_epoch().count();
  const auto Root = fs::temp_directory_path() / ("objc3-borrowed-" + std::to_string(Stamp));
  fs::create_directories(Root);
  for (bool ARC : {false, true}) {
    SemanticCases(Root, BaseLanguage::ObjectiveC, "c17", ARC);
    SemanticCases(Root, BaseLanguage::ObjectiveCxx, "c++20", ARC);
    SemanticCases(Root, BaseLanguage::ObjectiveCxx, "c++23", ARC);
  }
  PersistenceCases(Root);
  ForeignPersistenceCases(Root);
  RetainablePersistenceCases(Root);
  fs::remove_all(Root);
}
