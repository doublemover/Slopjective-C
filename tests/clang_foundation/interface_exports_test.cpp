#include "clang_foundation/compile_session.h"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

using namespace objc3::clang_foundation;
namespace fs = std::filesystem;
namespace {
void Write(const fs::path &path, const std::string &value) {
  std::ofstream(path, std::ios::binary | std::ios::trunc) << value;
}
std::string Read(const fs::path &path) {
  std::ifstream stream(path, std::ios::binary);
  return {std::istreambuf_iterator<char>(stream), {}};
}
CompileRequest Request(const fs::path &path, BaseLanguage language,
                       const std::string &standard, bool arc,
                       std::string source) {
  CompileRequest r;
  r.input_path = path.string();
  r.source_text = std::move(source);
  r.language = language;
  r.objc3 = language == BaseLanguage::ObjectiveC ||
            language == BaseLanguage::ObjectiveCxx;
  r.arc = arc;
  if (r.objc3)
    r.objc_runtime = "gnustep-2.2";
  r.target_triple = "x86_64-unknown-linux-gnu";
  r.arguments = {"-std=" + standard, "-fblocks"};
  return r;
}
void Surface(const fs::path &root, BaseLanguage language,
             const std::string &standard, bool arc) {
  bool cpp =
      language == BaseLanguage::Cxx || language == BaseLanguage::ObjectiveCxx;
  const std::string implementation =
      "typedef struct API { int value; } API;\n"
      "int answer(struct API input) { struct BodyOnly { int x; }; "
      "struct BodyOnly local = {0}; return input.value + local.x + 42; }\n"
      "int hidden(void) { return 1; }\n" +
      (cpp ? std::string(
                 "class Private { int value; public: Private():value(1){} };\n"
                 "template<class T> T private_template(T value) { return "
                 "value; }\n")
           : std::string("struct Private { int value; };\n"));
  auto r = Request(root / (cpp ? "surface.mm" : "surface.m"), language,
                   standard, arc, implementation);
  r.emit_semantic_interface = true;
  r.module_name = "Surface";
  r.package_identity = "surface-v1";
  r.interface_exports = {"API", "answer"};
  r.interface_output_path = (root / "Surface.objc3i").string();
  r.emit = EmitKind::Object;
  r.output_path = (root / "surface.o").string();
  auto built = Compile(r);
  assert(built.success);
  const auto &first = *built.interface_artifact;
  assert(first.source_bytes.find("hidden") == std::string::npos);
  assert(first.sidecar_json.find("Private") == std::string::npos);
  assert(first.sidecar_json.find("BodyOnly") == std::string::npos);
  assert(first.sidecar_json.find("private_template") == std::string::npos);
  auto edited = r;
  *edited.source_text += "int another_private_function(void) { return 100; }\n";
  auto changed = Compile(edited);
  assert(changed.success &&
         changed.interface_artifact->sidecar_json == first.sidecar_json);
  auto client = Request(
      root / "client.m", language, standard, arc,
      "int client(void) { struct API input = {0}; return answer(input); }");
  InterfaceImportInput input;
  input.source_path = r.interface_output_path;
  input.locked_identity = first.identity;
  client.imported_interfaces = {input};
  assert(Compile(client).success);
  client.source_text = "int client(void) { return hidden(); }";
  assert(!Compile(client).success);
  client.source_text = "int client(void) { return sizeof(struct Private); }";
  assert(!Compile(client).success);
  for (const auto &selection :
       std::vector<std::vector<std::string>>{{"missing"},
                                             {"API", "missing"},
                                             {"*", "answer"},
                                             {"answer", "answer"},
                                             {"answer"},
                                             {"hidden", ""}}) {
    Write(r.output_path, "prior object");
    Write(r.interface_output_path, "prior interface");
    Write(r.interface_output_path + ".json", "prior sidecar");
    r.interface_exports = selection;
    assert(!Compile(r).success);
    assert(Read(r.output_path) == "prior object");
    assert(Read(r.interface_output_path) == "prior interface");
    assert(Read(r.interface_output_path + ".json") == "prior sidecar");
  }
  r.source_text = "typedef int HiddenAlias; HiddenAlias public_alias(void);";
  r.interface_exports = {"public_alias"};
  assert(!Compile(r).success);
  r.interface_exports = {"HiddenAlias", "public_alias"};
  assert(Compile(r).success);
  r.interface_exports = {"*"};
  assert(Compile(r).success);
}
void CompilerBuiltins(const fs::path &root, const std::string &standard, bool arc) {
  auto r=Request(root/"builtins.mm",BaseLanguage::ObjectiveCxx,standard,arc,
      "inline int public_zero(void *storage){__builtin_memset(storage,0,8);return __builtin_popcount(42);}");
  r.emit_semantic_interface=true;r.module_name="Builtins";r.package_identity="builtins-v1";
  r.interface_exports={"public_zero"};r.interface_output_path=(root/"Builtins.objc3i").string();
  r.emit=EmitKind::Object;r.output_path=(root/"builtins.o").string();
  auto built=Compile(r);assert(built.success && built.interface_artifact);
  InterfaceImportInput input;input.source_path=r.interface_output_path;
  input.locked_identity=built.interface_artifact->identity;
  auto client=Request(root/"builtin-client.mm",BaseLanguage::ObjectiveCxx,standard,arc,
      "int client(){unsigned long long value=~0ULL;return public_zero(&value);}");
  client.imported_interfaces={input};client.emit=EmitKind::Object;
  client.output_path=(root/"builtin-client.o").string();assert(Compile(client).success);
  for(const std::string &name:{"__builtin_vendor_copy","memset"}) {
    r.source_text="extern \"C\" void *"+name+"(void*,int,__SIZE_TYPE__);"
        "inline int public_zero(void *storage){"+name+"(storage,0,8);return 0;}";
    auto rejected=Compile(r);assert(!rejected.success && rejected.interface_artifact);
    bool excluded=false;
    for(const auto &diagnostic:rejected.interface_artifact->diagnostics)
      excluded|=diagnostic.code=="O3IFC156";
    assert(excluded);
  }
}
void KeyPathSubscriptBodies(const fs::path &root,const std::string &standard,bool arc) {
  auto r=Request(root/"indexed-provider.mm",BaseLanguage::ObjectiveCxx,standard,arc,R"(
      __attribute__((objc_root_class)) @interface InlinePathRoot @end
      inline int indexed_roundtrip(InlinePathRoot *_Nonnull root,
          WritableKeyPath<InlinePathRoot*,int> path,int value) {
        int assigned=(root[path]=value);int updated=(root[path]+=2);
        return assigned+updated+root[path]++ + ++root[path];
      }
  )");
  r.emit_semantic_interface=true;r.module_name="IndexedBodies";r.package_identity="indexed-bodies-v1";
  r.interface_exports={"InlinePathRoot","indexed_roundtrip"};
  r.interface_output_path=(root/"IndexedBodies.objc3i").string();
  r.emit=EmitKind::Object;r.output_path=(root/"indexed-provider.o").string();
  auto built=Compile(r);assert(built.success && built.interface_artifact);
  assert(built.interface_artifact->sidecar_json.find("objc3.keypath-subscripts.v1")!=std::string::npos);
  InterfaceImportInput input;input.source_path=r.interface_output_path;
  input.locked_identity=built.interface_artifact->identity;
  auto client=Request(root/"indexed-client.mm",BaseLanguage::ObjectiveCxx,standard,arc,R"(
      int client(InlinePathRoot *_Nonnull root,WritableKeyPath<InlinePathRoot*,int> path) {
        const objc3_keypath_descriptor_v2 *metadata=KeyPathMetadata(path);
        return indexed_roundtrip(root,path,10)+(objc3_keypath_validate_v2(metadata)==0);
      }
  )");
  client.imported_interfaces={input};client.emit=EmitKind::Object;
  client.output_path=(root/"indexed-client.o").string();
  assert(Compile(client).success);
}

void UnnamedLocalBodies(const fs::path &root,const std::string &standard) {
  auto r=Request(root/"unnamed-provider.cpp",BaseLanguage::Cxx,standard,false,R"(
      inline int unnamed_inline(int n) {
        enum {delta=2};
        struct {struct {int n;} nested;int get() const {return nested.n+delta;}} value{{n}};
        using Copy=decltype(value);Copy copied=value;return copied.get();
      }
  )");
  r.emit_semantic_interface=true;r.module_name="UnnamedBodies";r.package_identity="unnamed-bodies-v1";
  r.interface_exports={"unnamed_inline"};r.interface_output_path=(root/"UnnamedBodies.objc3i").string();
  r.emit=EmitKind::Object;r.output_path=(root/"unnamed-provider.o").string();
  auto built=Compile(r);assert(built.success && built.interface_artifact);
  assert(built.interface_artifact->sidecar_json.find("objc3.cxx-unnamed-local-types.v1")!=std::string::npos);
  InterfaceImportInput input;input.source_path=r.interface_output_path;
  input.locked_identity=built.interface_artifact->identity;
  auto client=Request(root/"unnamed-client.cpp",BaseLanguage::Cxx,standard,false,
      "int client(){return unnamed_inline(40);}");
  client.imported_interfaces={input};client.emit=EmitKind::Object;
  client.output_path=(root/"unnamed-client.o").string();assert(Compile(client).success);
}

void GenericLocalBodies(const fs::path &root,const std::string &standard) {
  auto r=Request(root/"generic-provider.cpp",BaseLanguage::Cxx,standard,false,R"(
      inline constexpr int generic_value(int n) {
        auto sum=[]<class... T>(T... values){return (0+...+values);};
        auto defaults=[]<class T=int>(T value=42){return value;};
        int (*callback)(int)=[](auto value){return value+2;};
        return sum(n,2)+sum()+defaults.template operator()<>()+callback(n);
      }
  )");
  r.emit_semantic_interface=true;r.module_name="GenericBodies";r.package_identity="generic-bodies-v1";
  r.interface_exports={"generic_value"};r.interface_output_path=(root/"GenericBodies.objc3i").string();
  r.emit=EmitKind::Object;r.output_path=(root/"generic-provider.o").string();
  auto built=Compile(r);assert(built.success && built.interface_artifact);
  InterfaceImportInput input;input.source_path=r.interface_output_path;
  input.locked_identity=built.interface_artifact->identity;
  auto client=Request(root/"generic-client.cpp",BaseLanguage::Cxx,standard,false,
      "static_assert(generic_value(40)==126);int client(){return generic_value(40);}");
  client.imported_interfaces={input};client.emit=EmitKind::Object;
  client.output_path=(root/"generic-client.o").string();assert(Compile(client).success);
}

void GenericLocalContracts(const fs::path &root,const std::string &standard) {
  auto r=Request(root/"generic-contracts-provider.cpp",BaseLanguage::Cxx,standard,false,R"(
      struct GenericBase {int bias;};
      struct GenericConfig : GenericBase {int values[2];double scale;};
      int generic_probe(int); long generic_probe(long);
      inline constexpr int generic_contracts() {
        auto checked=[]<class T>(T value) noexcept(noexcept(value+2))
            requires requires(T item) {generic_probe(item);item+2;} {return value+2;};
        auto head=[]<class T> requires (sizeof(T)>0) (T value) {return value+2;};
        auto structural=[]<GenericConfig C>() {
          return C.bias+C.values[0]+C.values[1]+int(C.scale);
        };
        auto floating=[]<double D>() {return int(D);};
        static_assert(noexcept(checked(40)));
        return checked(40)+head(40)+
            structural.template operator()<GenericConfig{{2},{20,18},2.0}>()+
            floating.template operator()<42.0>();
      }
  )");
  r.emit_semantic_interface=true;r.module_name="GenericContracts";r.package_identity="generic-contracts-v2";
  r.interface_exports={"GenericBase","GenericConfig","generic_probe","generic_contracts"};
  r.interface_output_path=(root/"GenericContracts.objc3i").string();
  r.emit=EmitKind::Object;r.output_path=(root/"generic-contracts-provider.o").string();
  auto built=Compile(r);assert(built.success && built.interface_artifact);
  InterfaceImportInput input;input.source_path=r.interface_output_path;
  input.locked_identity=built.interface_artifact->identity;
  auto client=Request(root/"generic-contracts-client.cpp",BaseLanguage::Cxx,standard,false,
      "static_assert(generic_contracts()==168);int client(){return generic_contracts();}");
  client.imported_interfaces={input};client.emit=EmitKind::Object;
  client.output_path=(root/"generic-contracts-client.o").string();
  auto imported=Compile(client);
  for (const auto &diagnostic:imported.diagnostics)
    std::fprintf(stderr,"%s\n",diagnostic.message.c_str());
  for (const auto &diagnostic:imported.interface_import_diagnostics)
    std::fprintf(stderr,"%s: %s (%s)\n",diagnostic.code.c_str(),diagnostic.message.c_str(),diagnostic.subject.c_str());
  assert(imported.success);
}

void UnionBodies(const fs::path &root,const std::string &standard) {
  auto r=Request(root/"union-provider.cpp",BaseLanguage::Cxx,standard,false,R"(
      union Choice {int left;int right;};
      struct Member {int n;constexpr Member(int value):n(value){} constexpr ~Member() {}};
      union Managed {
        Member member;long other;
        constexpr Managed(int n):member(n){}
        constexpr Managed(const Managed &source):member(source.member.n){}
        constexpr ~Managed() {member.~Member();}
        constexpr int number() const {return member.n;}
      };
      inline constexpr int union_value() {
        Managed first(40);Managed copy=first;
        auto use=[]<Choice C>() {return 42;};
        return copy.number()+2+use.template operator()<Choice{.left=42}>()+
               use.template operator()<Choice{.right=42}>();
      }
  )");
  r.emit_semantic_interface=true;r.module_name="UnionBodies";r.package_identity="union-bodies-v1";
  r.interface_exports={"Choice","Member","Managed","union_value"};
  r.interface_output_path=(root/"UnionBodies.objc3i").string();
  r.emit=EmitKind::Object;r.output_path=(root/"union-provider.o").string();
  auto built=Compile(r);assert(built.success && built.interface_artifact);
  InterfaceImportInput input;input.source_path=r.interface_output_path;
  input.locked_identity=built.interface_artifact->identity;
  auto client=Request(root/"union-client.cpp",BaseLanguage::Cxx,standard,false,
      "static_assert(union_value()==126);int client(){return union_value();}");
  client.imported_interfaces={input};client.emit=EmitKind::Object;
  client.output_path=(root/"union-client.o").string();
  auto imported=Compile(client);
  for (const auto &diagnostic:imported.diagnostics)
    std::fprintf(stderr,"%s\n",diagnostic.message.c_str());
  assert(imported.success);
}

void ConceptBodies(const fs::path &root,const std::string &standard) {
  auto r=Request(root/"concept-provider.cpp",BaseLanguage::Cxx,standard,false,R"(
      template<class T> concept Number=requires(T n){n+2;};
      template<class T> concept Wide=Number<T> && (sizeof(T)>=sizeof(int));
      template<int N> concept Positive=N>0;
      template<class T> concept Compound=Number<T> && requires(T n){requires sizeof(T)>0;n+2;};
      inline constexpr int concept_value() {return []<Wide T>(T n){return n+2;}(40);}
  )");
  r.emit_semantic_interface=true;r.module_name="ConceptBodies";r.package_identity="concept-bodies-v1";
  r.interface_exports={"Number","Wide","Positive","Compound","concept_value"};
  r.interface_output_path=(root/"ConceptBodies.objc3i").string();
  r.emit=EmitKind::Object;r.output_path=(root/"concept-provider.o").string();
  auto built=Compile(r);
  for (const auto &diagnostic:built.diagnostics) std::fprintf(stderr,"%s\n",diagnostic.message.c_str());
  assert(built.success && built.interface_artifact);
  InterfaceImportInput input;input.source_path=r.interface_output_path;
  input.locked_identity=built.interface_artifact->identity;
  auto client=Request(root/"concept-client.cpp",BaseLanguage::Cxx,standard,false,R"(
      template<class T> requires Number<T> constexpr int score(T) {return 40;}
      template<class T> requires Wide<T> constexpr int score(T) {return 42;}
      static_assert(score(1)==42 && concept_value()==42 && Positive<2> && !Positive<-1> && Compound<int>);
      int client(){return score(1);}
  )");
  client.imported_interfaces={input};client.emit=EmitKind::Object;
  client.output_path=(root/"concept-client.o").string();
  auto imported=Compile(client);
  for (const auto &diagnostic:imported.diagnostics) std::fprintf(stderr,"%s\n",diagnostic.message.c_str());
  assert(imported.success);
}

void FunctionTemplateBodies(const fs::path &root,const std::string &standard) {
  auto r=Request(root/"template-provider.cpp",BaseLanguage::Cxx,standard,false,R"(
      namespace Generic {
        template<class T> concept Number=requires(T n){n+2;};
        template<class T,int Bias=2> constexpr T add(T n){return n+Bias;}
        template<class... T> constexpr int sum(T... n){return (0+...+n);}
        template<class T> requires Number<T> constexpr int rank(T){return 40;}
        template<class T> requires (Number<T> && sizeof(T)>=sizeof(int)) constexpr int rank(T){return 42;}
        template<Number T> constexpr T identity(T n) noexcept(noexcept(n+2)){return n;}
        template<int N> int count(){return N;}
      }
      int producer(){return Generic::add(40)+Generic::count<7>();}
  )");
  r.emit_semantic_interface=true;r.module_name="FunctionTemplates";r.package_identity="function-templates-v1";
  r.module_owned_names=true;
  r.interface_exports={"Generic","producer"};
  r.interface_output_path=(root/"FunctionTemplates.objc3i").string();
  r.emit=EmitKind::Object;r.output_path=(root/"template-provider.o").string();
  auto built=Compile(r);
  for (const auto &d:built.diagnostics) std::fprintf(stderr,"%s\n",d.message.c_str());
  for (const auto &d:built.interface_import_diagnostics) std::fprintf(stderr,"%s: %s\n",d.code.c_str(),d.message.c_str());
  assert(built.success && built.interface_artifact);
  InterfaceImportInput input;input.source_path=r.interface_output_path;input.locked_identity=built.interface_artifact->identity;
  auto client=Request(root/"template-client.cpp",BaseLanguage::Cxx,standard,false,R"(
      static_assert(Generic::add(40)==42 && Generic::add<long,3>(39)==42);
      static_assert(Generic::sum(20,22)==42 && Generic::rank(1)==42);
      static_assert(Generic::identity(42)==42 && noexcept(Generic::identity(42)));
      int client(){return Generic::count<42>()+Generic::count<7>()+Generic::add(40);}
  )");
  client.imported_interfaces={input};client.emit=EmitKind::Object;
  client.output_path=(root/"template-client.o").string();
  auto imported=Compile(client);
  for (const auto &d:imported.diagnostics) std::fprintf(stderr,"%s\n",d.message.c_str());
  for (const auto &d:imported.interface_import_diagnostics) std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
  assert(imported.success);
}

void AliasTemplateBodies(const fs::path &root,const std::string &standard) {
  auto r=Request(root/"alias-provider.cpp",BaseLanguage::Cxx,standard,false,R"(
      namespace Types {
        template<class T> concept Number=requires(T n){n+2;};
        template<class T=int> using Pointer=T*;
        template<class... T> using Callback=int(*)(T...);
        template<class T> using Element=typename T::element;
        template<Number T> using NumericPointer=T*;
        template<class T> requires(sizeof(T)>0) using SizedPointer=T*;
        struct Box{using element=int;};
      }
  )");
  r.emit_semantic_interface=true;r.module_name="AliasTemplates";r.package_identity="alias-templates-v1";
  r.module_owned_names=true;r.interface_exports={"Types"};
  r.interface_output_path=(root/"AliasTemplates.objc3i").string();
  r.emit=EmitKind::Object;r.output_path=(root/"alias-provider.o").string();
  auto built=Compile(r);
  for (const auto &d:built.diagnostics) std::fprintf(stderr,"%s\n",d.message.c_str());
  assert(built.success && built.interface_artifact);
  InterfaceImportInput input;input.source_path=r.interface_output_path;input.locked_identity=built.interface_artifact->identity;
  auto client=Request(root/"alias-client.cpp",BaseLanguage::Cxx,standard,false,R"(
      static_assert(__is_same(Types::Pointer<>,int*));
      static_assert(__is_same(Types::Element<Types::Box>,int));
      static_assert(__is_same(Types::NumericPointer<long>,long*));
      static_assert(__is_same(Types::SizedPointer<double>,double*));
      int sum(int a,int b){return a+b;}
      Types::Callback<int,int> callback=sum;
      int client(){Types::Pointer<> n=nullptr;return n ? 0 : callback(20,22);}
  )");
  client.imported_interfaces={input};client.emit=EmitKind::Object;
  client.output_path=(root/"alias-client.o").string();
  auto imported=Compile(client);
  for (const auto &d:imported.diagnostics) std::fprintf(stderr,"%s\n",d.message.c_str());
  for (const auto &d:imported.interface_import_diagnostics) std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
  assert(imported.success);
}

void NestedTemplateTypes(const fs::path &root,const std::string &standard) {
  auto r=Request(root/"nested-types-provider.cpp",BaseLanguage::Cxx,standard,false,R"(

    template<class T> class NestedFamily {
    public:
      using element=T;
      struct Detail {using element=T;T value;explicit Detail(T n):value(n){} T number()const{return value;}};
      template<class U=T,int Bias=2> class Box {
        U value;
      public:
        using element=U;
        template<class V=U> using Pointer=V*;
        inline static int live=0;
        explicit Box(U n=U{40}):value(n){++live;}
        Box(const Box &other):value(other.value){++live;}
        virtual ~Box(){--live;}
        virtual U number()const{return value+Bias;}
        template<class V> auto add(V n)const{return number()+n;}
      };
      template<class U=int> inline static U shared=U{40};
      template<class U=int> inline static thread_local U local=U{40};
      template<int N> inline static constexpr int answer=N+2;
    };
    NestedFamily<int>::Box<int> *make_nested(){return new NestedFamily<int>::Box<int>;}
    int nested_state(){return ++NestedFamily<int>::shared<> + ++NestedFamily<int>::local<>;}
    template<double D> struct FloatingFamily {
      template<class T> struct Inner {static constexpr int answer(){return 42;}};
    };
    inline constexpr double nested_argument=__builtin_bit_cast(double,0x0000000000000000ULL);
    inline constexpr int nested_argument_probe(){return FloatingFamily<nested_argument>::Inner<int>::answer();}

  )");
  r.emit_semantic_interface=true;r.module_name="NestedTypes";r.package_identity="nested-types-v1";
  r.module_owned_names=true;r.interface_exports={"NestedFamily","make_nested","nested_state","FloatingFamily","nested_argument","nested_argument_probe"};
  r.interface_output_path=(root/"NestedTypes.objc3i").string();
  r.emit=EmitKind::Object;r.output_path=(root/"nested-types-provider.o").string();
  auto built=Compile(r);
  for(const auto &d:built.diagnostics)std::fprintf(stderr,"%s\n",d.message.c_str());
  assert(built.success && built.interface_artifact);
  InterfaceImportInput input;input.source_path=r.interface_output_path;input.locked_identity=built.interface_artifact->identity;
  fs::remove(r.input_path);
  auto client=Request(root/"nested-types-client.cpp",BaseLanguage::Cxx,standard,false,R"(
    static_assert(NestedFamily<long>::answer<40> ==42 && nested_argument_probe()==42);
    int client(){
      auto *value=make_nested();auto result=value->number();delete value;
      NestedFamily<long>::Box<long,3> local(39);
      NestedFamily<long>::Detail detail(42);
      NestedFamily<long>::Box<long>::Pointer<> pointer=nullptr;
      return result+local.add(0)+detail.number()+NestedFamily<int>::shared<> +
        NestedFamily<int>::local<> + (pointer==nullptr);
    }
  )");
  client.imported_interfaces={input};client.emit=EmitKind::Object;
  client.output_path=(root/"nested-types-client.o").string();
  auto imported=Compile(client);
  for(const auto &d:imported.diagnostics)std::fprintf(stderr,"%s\n",d.message.c_str());
  for(const auto &d:imported.interface_import_diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
  assert(imported.success);
}

void ClassMemberAliasTemplates(const fs::path &root,const std::string &standard) {
  auto r=Request(root/"member-aliases-provider.cpp",BaseLanguage::Cxx,standard,false,R"(

    template<class T> struct AliasValue {using element=T;T value;};
    template<class T> struct MemberAliases {
      template<class U=T> using Pointer=U*;
      template<class... U> using Callback=T(*)(U...);
      template<int N> using Array=T[N];
      template<class U=T> using Rebind=AliasValue<U>;
      template<class U> requires requires{typename U::element;}
      using Element=typename U::element;
      template<class U> static constexpr Element<Rebind<U>> number(U n){return n;}
    };
    inline constexpr int alias_member_probe(){
      MemberAliases<int>::Array<2> values={40,2};
      MemberAliases<int>::Rebind<long> value{values[0]};
      return MemberAliases<int>::number(value.value)+values[1];
    }

  )");
  r.emit_semantic_interface=true;r.module_name="MemberAliases";r.package_identity="member-aliases-v1";
  r.module_owned_names=true;r.interface_exports={"AliasValue","MemberAliases","alias_member_probe"};
  r.interface_output_path=(root/"MemberAliases.objc3i").string();
  r.emit=EmitKind::Object;r.output_path=(root/"member-aliases-provider.o").string();
  auto built=Compile(r);
  for(const auto &d:built.diagnostics)std::fprintf(stderr,"%s\n",d.message.c_str());
  assert(built.success && built.interface_artifact);
  InterfaceImportInput input;input.source_path=r.interface_output_path;input.locked_identity=built.interface_artifact->identity;
  fs::remove(r.input_path);
  auto client=Request(root/"member-aliases-client.cpp",BaseLanguage::Cxx,standard,false,R"(
    static_assert(alias_member_probe()==42);
    static_assert(MemberAliases<long>::number(42)==42);
    int client(){
      MemberAliases<long>::Pointer<> pointer=nullptr;
      MemberAliases<long>::Array<2> values={40,2};
      MemberAliases<long>::Element<AliasValue<int>> result=42;
      MemberAliases<long>::Callback<short,int> callback=[](short a,int b)->long{return a+b;};
      return pointer==nullptr ? callback(values[0],values[1])+result : 0;
    }
  )");
  client.imported_interfaces={input};client.emit=EmitKind::Object;
  client.output_path=(root/"member-aliases-client.o").string();
  auto imported=Compile(client);
  for(const auto &d:imported.diagnostics)std::fprintf(stderr,"%s\n",d.message.c_str());
  for(const auto &d:imported.interface_import_diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
  assert(imported.success);
}

void ClassMemberTemplates(const fs::path &root,const std::string &standard) {
  auto r=Request(root/"members-provider.cpp",BaseLanguage::Cxx,standard,false,R"(

    template<class T> class MemberBox {
      T value;
    public:
      constexpr MemberBox(T n):value(n){}
      template<class U> requires requires(U n){static_cast<T>(n);}
      constexpr explicit(sizeof(U)>0) MemberBox(U n):value(static_cast<T>(n)){}
      template<class U=T,int Delta=2>
      constexpr auto add(U n=U{2})const noexcept(noexcept(value+n)){return value+n+Delta;}
      template<class U=T> constexpr decltype(auto) reference(){return (value);}
      template<class U> constexpr auto reader(U offset)const {
        return [n=value,offset](int extra){return n+offset+extra;};
      }
      template<class... U> constexpr auto sum(U... n)const{return (value+...+n);}
      template<class U> constexpr auto operator()(U n)const{return value+n;}
      template<class U> constexpr explicit operator U()const{return static_cast<U>(value);}
      template<class U=int> static int next(){static int calls=0;return ++calls;}
    };
    constexpr int member_probe(){MemberBox<int> box(40L);return box.add(0);}
    int provider(){return MemberBox<int>::next<>();}

  )");
  r.emit_semantic_interface=true;r.module_name="ClassMembers";r.package_identity="class-members-v1";
  r.module_owned_names=true;r.interface_exports={"MemberBox","member_probe","provider"};
  r.interface_output_path=(root/"ClassMembers.objc3i").string();
  r.emit=EmitKind::Object;r.output_path=(root/"members-provider.o").string();
  auto built=Compile(r);
  for(const auto &d:built.diagnostics)std::fprintf(stderr,"%s\n",d.message.c_str());
  assert(built.success && built.interface_artifact);
  InterfaceImportInput input;input.source_path=r.interface_output_path;input.locked_identity=built.interface_artifact->identity;
  fs::remove(r.input_path);
  auto client=Request(root/"members-client.cpp",BaseLanguage::Cxx,standard,false,R"(
    static_assert(member_probe()==42);
    static_assert(MemberBox<long>{40}.add(0L)==42);
    static_assert(MemberBox<long>{40}.sum(1,1L)==42);
    static_assert(MemberBox<long>{40}.reader(1L)(1)==42);
    static_assert(MemberBox<long>{42}(0)==42);
    static_assert(static_cast<long>(MemberBox<int>{42})==42);
    int client(){MemberBox<long> box(40);++box.reference<>();
      return box.add(0)+MemberBox<int>::next<>()+MemberBox<long>::next<long>();}
  )");
  client.imported_interfaces={input};client.emit=EmitKind::Object;
  client.output_path=(root/"members-client.o").string();
  auto imported=Compile(client);
  for(const auto &d:imported.diagnostics)std::fprintf(stderr,"%s\n",d.message.c_str());
  for(const auto &d:imported.interface_import_diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
  assert(imported.success);
}

void TemplateDeduction(const fs::path &root,const std::string &standard) {
  auto r=Request(root/"deduction-provider.cpp",BaseLanguage::Cxx,standard,false,R"(
    template<class T> constexpr auto add(T n){return n+2;}
    template<class T> constexpr decltype(auto) borrow(T &n){return (n);}
    template<class T> constexpr auto make_reader(T n){return [n](int d){return n+d;};}
    template<class T> class Holder {
      T value;
    public:
      constexpr Holder(T n):value(n){}
      constexpr auto number()const{return value+2;}
      constexpr auto reader()const{return [n=value](int d){return n+d;};}
      constexpr decltype(auto) reference(){return (value);}
    };
    template<class T=int> inline constexpr auto answer=T{42};
    template<class T> inline auto reader=[n=T{40}](int d){return n+d;};
    int provider(){return make_reader(40)(2)+reader<int>(2);}
  )");
  r.emit_semantic_interface=true;r.module_name="TemplateDeduction";r.package_identity="template-deduction-v1";
  r.module_owned_names=true;r.interface_exports={"add","borrow","make_reader","Holder","answer","reader","provider"};
  r.interface_output_path=(root/"TemplateDeduction.objc3i").string();
  r.emit=EmitKind::Object;r.output_path=(root/"deduction-provider.o").string();
  auto built=Compile(r);
  for(const auto &d:built.diagnostics)std::fprintf(stderr,"%s\n",d.message.c_str());
  assert(built.success && built.interface_artifact);
  InterfaceImportInput input;input.source_path=r.interface_output_path;input.locked_identity=built.interface_artifact->identity;
  auto client=Request(root/"deduction-client.cpp",BaseLanguage::Cxx,standard,false,R"(
    static_assert(add(40L)==42 && make_reader(40L)(2)==42 && answer<long> ==42);
    static_assert(Holder<long>{40}.number()==42 && Holder<long>{40}.reader()(2)==42);
    int client(){int value=40;++borrow(value);Holder<int> holder{40};++holder.reference();
      return value+holder.number()+reader<long>(2);}
  )");
  client.imported_interfaces={input};client.emit=EmitKind::Object;
  client.output_path=(root/"deduction-client.o").string();
  auto imported=Compile(client);
  for(const auto &d:imported.diagnostics)std::fprintf(stderr,"%s\n",d.message.c_str());
  for(const auto &d:imported.interface_import_diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
  assert(imported.success);
}

void TemplateStructure(const fs::path &root,const std::string &standard) {
  auto r=Request(root/"structure-provider.cpp",BaseLanguage::Cxx,standard,false,R"(
    template<class T=int,int N=sizeof(T)> extern T count;
    template<class U,int M> U count=M;
    template<class T=int,int Bias=2> class Root;
    template<class U,int Delta> class Root {
      U value;
    public:
      explicit Root(U n=U{40}):value(n){}
      virtual ~Root()=default;
      virtual int number()const{return int(value)+Delta;}
    };
    template<class T=int> struct Left:virtual Root<T>{Left():Root<T>(T{40}){}};
    template<class T=int> struct Right:virtual Root<T>{Right():Root<T>(T{40}){}};
    template<class T=int> struct Diamond:Left<T>,Right<T>{
      Diamond():Root<T>(T{40}){}
      int number()const override{return Root<T>::number();}
    };
    Root<int> *provider(){return new Diamond<>;}
  )");
  r.emit_semantic_interface=true;r.module_name="TemplateStructure";r.package_identity="template-structure-v1";
  r.module_owned_names=true;r.interface_exports={"Root","Left","Right","Diamond","count","provider"};
  r.interface_output_path=(root/"TemplateStructure.objc3i").string();
  r.emit=EmitKind::Object;r.output_path=(root/"structure-provider.o").string();
  auto built=Compile(r);
  for(const auto &d:built.diagnostics)std::fprintf(stderr,"%s\n",d.message.c_str());
  assert(built.success && built.interface_artifact);
  InterfaceImportInput input;input.source_path=r.interface_output_path;input.locked_identity=built.interface_artifact->identity;
  auto client=Request(root/"structure-client.cpp",BaseLanguage::Cxx,standard,false,R"(
    int client(){Diamond<long> value;Root<long> *root=&value;
      auto *left=dynamic_cast<Left<long> *>(root);auto *right=dynamic_cast<Right<long> *>(root);
      return left && right ? root->number()+int(count<long>) : 0;}
  )");
  client.imported_interfaces={input};client.emit=EmitKind::Object;
  client.output_path=(root/"structure-client.o").string();
  auto imported=Compile(client);
  for(const auto &d:imported.diagnostics)std::fprintf(stderr,"%s\n",d.message.c_str());
  for(const auto &d:imported.interface_import_diagnostics)std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
  assert(imported.success);
}

void ClassTemplateBodies(const fs::path &root, const std::string &standard) {
  auto r=Request(root/"class-template-provider.cpp",BaseLanguage::Cxx,standard,false,R"(
    namespace Types {
      template<class T> concept Number=requires(T n){n+2;};
      template<Number T=int,int Bias=2> class Box {
        T value;
      public:
        using element=T;
        inline static int visits=0;
        constexpr Box(T n=T{40}):value(n){}
        constexpr Box(const Box&)=default;
        constexpr T number()const noexcept{return value+Bias;}
        static int next(){return ++visits;}
      };
      template<class... T> struct Arity {static constexpr unsigned count=sizeof...(T);};
    }
    int provider(){return Types::Box<>::next();}
  )");
  r.emit_semantic_interface=true;r.module_name="ClassTemplates";r.package_identity="class-templates-v1";
  r.module_owned_names=true;r.interface_exports={"Types","provider"};
  r.interface_output_path=(root/"ClassTemplates.objc3i").string();
  r.emit=EmitKind::Object;r.output_path=(root/"class-template-provider.o").string();
  auto built=Compile(r);
  for(const auto &d:built.diagnostics) std::fprintf(stderr,"%s\n",d.message.c_str());
  for(const auto &d:built.interface_import_diagnostics) std::fprintf(stderr,"%s: %s\n",d.code.c_str(),d.message.c_str());
  assert(built.success && built.interface_artifact);
  InterfaceImportInput input;input.source_path=r.interface_output_path;input.locked_identity=built.interface_artifact->identity;
  auto client=Request(root/"class-template-client.cpp",BaseLanguage::Cxx,standard,false,R"(
    static_assert(Types::Box<>{}.number()==42 && Types::Box<long,3>(39).number()==42);
    static_assert(__is_same(Types::Box<long>::element,long));
    static_assert(Types::Arity<int,long,double>::count==3);
    int client(){Types::Box<long> n(40);auto copy=n;return copy.number()+Types::Box<>::next();}
  )");
  client.imported_interfaces={input};client.emit=EmitKind::Object;
  client.output_path=(root/"class-template-client.o").string();
  auto imported=Compile(client);
  for(const auto &d:imported.diagnostics) std::fprintf(stderr,"%s\n",d.message.c_str());
  for(const auto &d:imported.interface_import_diagnostics) std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
  assert(imported.success);
}

void VariableTemplateBodies(const fs::path &root, const std::string &standard) {
  auto r=Request(root/"variable-provider.cpp",BaseLanguage::Cxx,standard,false,R"(
    namespace Values {
      template<class T=int,int N=40> inline constexpr T answer=T{N+2};
      template<class T=int> inline T shared=40;
      template<class T=int> inline thread_local T local=40;
      template<class... T> inline constexpr unsigned count=sizeof...(T);
      template<class T> requires(sizeof(T)>0) inline constexpr unsigned width=sizeof(T);
      template<class T> T zero;
    }
    int provider(){return ++Values::shared<> + ++Values::local<>;}
  )");
  r.emit_semantic_interface=true;r.module_name="VariableTemplates";r.package_identity="variable-templates-v1";
  r.module_owned_names=true;r.interface_exports={"Values","provider"};
  r.interface_output_path=(root/"VariableTemplates.objc3i").string();
  r.emit=EmitKind::Object;r.output_path=(root/"variable-provider.o").string();
  auto built=Compile(r);
  for (const auto &d:built.diagnostics) std::fprintf(stderr,"%s\n",d.message.c_str());
  for (const auto &d:built.interface_import_diagnostics) std::fprintf(stderr,"%s: %s\n",d.code.c_str(),d.message.c_str());
  assert(built.success && built.interface_artifact);
  InterfaceImportInput input;input.source_path=r.interface_output_path;input.locked_identity=built.interface_artifact->identity;
  auto client=Request(root/"variable-client.cpp",BaseLanguage::Cxx,standard,false,R"(
    static_assert(Values::answer<> ==42 && Values::answer<long,39> ==41);
    static_assert(Values::count<int,long,double> ==3 && Values::width<char> ==1);
    int client(){return ++Values::shared<> + ++Values::local<> + Values::zero<long>;}
  )");
  client.imported_interfaces={input};client.emit=EmitKind::Object;
  client.output_path=(root/"variable-client.o").string();
  auto imported=Compile(client);
  for (const auto &d:imported.diagnostics) std::fprintf(stderr,"%s\n",d.message.c_str());
  for (const auto &d:imported.interface_import_diagnostics) std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
  assert(imported.success);
}

void FunctionTemplateDeclarations(const fs::path &root,const std::string &standard) {
  auto r=Request(root/"declaration-provider.cpp",BaseLanguage::Cxx,standard,false,R"(
      template<class T=int,int Bias=2> constexpr T forwarded(T n=T{40});
      template<class U,int Delta> constexpr U forwarded(U value){return value+Delta;}
      template<class T,int N> constexpr T late_default(T n);
      template<class U,int M=2> constexpr U late_default(U value){return value+M;}
      template<class T=int,int N=2> constexpr T tail_decl(T n=T{40}){return n+N;}
      template<class U,int M> constexpr U tail_decl(U value);
      template<int N=40> requires(N>0) constexpr int constrained_default(int n=N);
      template<int M> requires(M>0) constexpr int constrained_default(int value){return value+2;}
      template<class T=int> constexpr T generic_default(T n=T{40}){return n+2;}
  )");
  r.emit_semantic_interface=true;r.module_name="TemplateDeclarations";r.package_identity="template-declarations-v2";
  r.module_owned_names=true;r.interface_exports={"forwarded","late_default","tail_decl","constrained_default","generic_default"};
  r.interface_output_path=(root/"TemplateDeclarations.objc3i").string();
  r.emit=EmitKind::Object;r.output_path=(root/"declaration-provider.o").string();
  auto built=Compile(r);
  for (const auto &d:built.diagnostics) std::fprintf(stderr,"%s\n",d.message.c_str());
  assert(built.success && built.interface_artifact);
  InterfaceImportInput input;input.source_path=r.interface_output_path;input.locked_identity=built.interface_artifact->identity;
  auto client=Request(root/"declaration-client.cpp",BaseLanguage::Cxx,standard,false,R"(
      static_assert(forwarded()==42 && forwarded<long,3>(39)==42);
      static_assert(late_default(40)==42 && tail_decl()==42);
      static_assert(constrained_default()==42 && generic_default()==42);
      int client(){return forwarded()+late_default(40)+tail_decl()+constrained_default()+generic_default();}
  )");
  client.imported_interfaces={input};client.emit=EmitKind::Object;
  client.output_path=(root/"declaration-client.o").string();
  auto imported=Compile(client);
  for (const auto &d:imported.diagnostics) std::fprintf(stderr,"%s\n",d.message.c_str());
  for (const auto &d:imported.interface_import_diagnostics) std::fprintf(stderr,"%s: %s (%s)\n",d.code.c_str(),d.message.c_str(),d.subject.c_str());
  assert(imported.success);
}

void Projection(const fs::path &root, bool producer_arc) {
  auto r = Request(
      root / "projected.m", BaseLanguage::ObjectiveC, "c17", producer_arc,
      "id factory(void) __attribute__((ns_returns_retained));\n"
      "struct Private { id value; }; int hidden(struct Private value);\n");
  r.emit_semantic_interface = true;
  r.projected_arc = !producer_arc;
  r.module_name = "Projected";
  r.package_identity = "projection-v1";
  r.interface_exports = {"factory"};
  auto result = Compile(r);
  assert(result.success && result.interface_artifact->source_bytes.find(
                               "hidden") == std::string::npos);
}
} // namespace
int main() {
  const auto root = fs::temp_directory_path() / "objc3 public export selection";
  fs::remove_all(root);
  fs::create_directories(root);
  Surface(root, BaseLanguage::C, "c17", false);
  for (const auto &standard : {"c++20", "c++23"}) {
    Surface(root, BaseLanguage::Cxx, standard, false);UnnamedLocalBodies(root,standard);
    GenericLocalBodies(root,standard);
    GenericLocalContracts(root,standard);
    UnionBodies(root,standard);
    ConceptBodies(root,standard);
    FunctionTemplateBodies(root,standard);
    AliasTemplateBodies(root,standard);
    FunctionTemplateDeclarations(root,standard);
    VariableTemplateBodies(root,standard);
    ClassTemplateBodies(root,standard);
    TemplateStructure(root,standard);
    TemplateDeduction(root,standard);
    ClassMemberTemplates(root,standard);
    ClassMemberAliasTemplates(root,standard);
    NestedTemplateTypes(root,standard);
  }
  for (bool arc : {false, true}) {
    for(const auto &standard:{"c++20","c++23"}) {
      CompilerBuiltins(root,standard,arc);KeyPathSubscriptBodies(root,standard,arc);
    }
    Surface(root, BaseLanguage::ObjectiveC, "c17", arc);
    for (const auto &standard : {"c++20", "c++23"})
      Surface(root, BaseLanguage::ObjectiveCxx, standard, arc);
    Projection(root, arc);
  }
  fs::remove_all(root);
}
