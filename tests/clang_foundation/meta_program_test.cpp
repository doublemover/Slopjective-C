#include "clang_foundation/meta_program.h"
#include <cassert>
using namespace objc3::clang_foundation;
namespace {
MetaInstruction Text(std::string value){MetaInstruction i;i.text=std::move(value);return i;}
MetaInstruction Conditional(std::string condition,std::vector<MetaInstruction> yes,std::vector<MetaInstruction> no={}){
 MetaInstruction i;i.operation=MetaInstruction::Operation::Conditional;i.predicate=std::move(condition);i.body=std::move(yes);i.alternative=std::move(no);return i;
}
}
int main(){
 MetaPackage package;package.id="example.portable";package.version="1.0.0";package.fingerprint=std::string(64,'a');
 MetaClass target;target.name="Counter";target.properties.push_back({"value","int","value","setValue:",MetaMemberKind::Integer,true});
 target.properties.push_back({"other","id","other","",MetaMemberKind::Object,false});
 MetaEntrypoint entry;entry.name="Equality";entry.declarations={Text("- (int)equal:(id)other;")};
 MetaInstruction loop;loop.operation=MetaInstruction::Operation::Properties;loop.member_kind=MetaMemberKind::Integer;
 loop.body={Text("self.${member.name} == other.${member.getter}")};loop.separator=" && ";
 entry.implementations={Text("- (int)equal:(id)other { return "),loop,Text("; }")};
 std::string error;
 auto first=ExpandMetaProgram(package,entry,target,{},error);
 assert(first && error.empty() && first->implementations=="- (int)equal:(id)other { return self.value == other.value; }");
 auto second=ExpandMetaProgram(package,entry,target,{},error);assert(second && second->declarations==first->declarations && second->implementations==first->implementations);
 entry.implementations={Conditional("ownership.arc",{Text("- (int)mode { return 1; }")},{Text("- (int)mode { return 0; }")})};
 MetaExpansionContext arc;arc.arc=true;
 assert(ExpandMetaProgram(package,entry,target,arc,error)->implementations.find("return 1")!=std::string::npos);
 assert(ExpandMetaProgram(package,entry,target,{},error)->implementations.find("return 0")!=std::string::npos);
 entry.implementations={Text("- (const char *)name {return ${class.name:string};}")};
 target.name="Odd\"\\\nName";
 auto escaped=ExpandMetaProgram(package,entry,target,{},error);assert(escaped && escaped->implementations.find("Odd\\\"\\\\\\012Name")!=std::string::npos);
 target.name="Counter";
 entry.ivars={Text("int stored;")};
 auto storage=ExpandMetaProgram(package,entry,target,{},error);assert(storage && storage->ivars=="int stored;");
 for(const char *source:{"}","int a[2);","int f; } @public int escaped; {","int a[1;"}) {
  entry.ivars={Text(source)};assert(!ExpandMetaProgram(package,entry,target,{},error) && error.find("delimiter")!=std::string::npos);
 }
 entry.ivars.clear();
 entry.implementations={Text("- (const char *)literal {return \"#include <x> __DATE__ asm\";}")};
 assert(ExpandMetaProgram(package,entry,target,{},error));
 for(const char *source:{"__DATE__","__TIME__ ","__TIMESTAMP__","#include <x>","%:include <x>","asm(\"x\");","_Pragma(\"x\")","@end","@import Bad;","objc_derive(\"Hash\")"}){
  entry.implementations={Text(source)};assert(!ExpandMetaProgram(package,entry,target,{},error));
 }
 entry.implementations={Text("${unknown}")};assert(!ExpandMetaProgram(package,entry,target,{},error) && error.find("query")!=std::string::npos);
 entry.implementations={Conditional("unknown",{Text(";")})};assert(!ExpandMetaProgram(package,entry,target,{},error) && error.find("predicate")!=std::string::npos);
 entry.implementations={Text("${member.name}")};assert(!ExpandMetaProgram(package,entry,target,{},error));
 MetaInstruction failure;failure.operation=MetaInstruction::Operation::Error;failure.text="missing serializer for ${class.name}";
 entry.implementations={failure};assert(!ExpandMetaProgram(package,entry,target,{},error) && error.find("Counter")!=std::string::npos && error.find("requirements")!=std::string::npos);
 entry.declarations={Text("// no declaration\n")};entry.implementations={};assert(!ExpandMetaProgram(package,entry,target,{},error));
 entry.kind=MetaEntryKind::Macro;assert(ExpandMetaProgram(package,entry,target,{},error));
 entry.kind=MetaEntryKind::Derive;entry.declarations={Text(std::string(1024*1024+1,' '))};assert(!ExpandMetaProgram(package,entry,target,{},error));
 target.properties.resize(4096, target.properties.front());
 MetaInstruction inner;inner.operation=MetaInstruction::Operation::Properties;inner.body={Text(";")};
 MetaInstruction outer;outer.operation=MetaInstruction::Operation::Properties;outer.body={inner};
 entry.declarations={outer};assert(!ExpandMetaProgram(package,entry,target,{},error) && error.find("work budget")!=std::string::npos);
}
