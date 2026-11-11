#include "CodecTarget.h"
#include "clang_foundation/interface_codec.h"
#include "clang_foundation/interface_emitter.h"
#include "clang_foundation/module_names.h"
#include "clang_foundation/runtime_interface_types.h"
#include "clang/Frontend/ASTUnit.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/Error.h"
#include <algorithm>
#include <cassert>
#include <charconv>
#include <memory>
#include <string>
#include <vector>

using namespace objc3::clang_foundation;

int main(int argc,char **argv) {
  assert(argc==1 || argc==2);
  unsigned first=0,last=38;
  if(argc==2) {
    const char *end=argv[1]+std::char_traits<char>::length(argv[1]);
    const auto parsed=std::from_chars(argv[1],end,first);
    assert(parsed.ec==std::errc{} && parsed.ptr==end && first<last);
    last=first+1;
  }
  for (unsigned family=first;family!=last;++family) {
    const bool owning_closures=family==37,mrc_records=family==36,union_bytes=family==35,deduced_groups=family==34,emission_policies=family==33,static_proofs=family==32,union_carriers=family==31,unnamed_enums=family==30,storage_lifetime=family==29,managed_union_paths=family==28,atomic_managed=family==27,implicit_checked=family==26,arc_records=family==25,carrier_lambdas=family==24,mixed_storage=family==23,byref_cpp=family==21,byref_c=family==22,aligned_c=family==20,aligned_blocks=family==19,cpp_values=family==18,mixed_calls=family==17,cpp_runtime=family==16,registers=family==15,storage=family==14,messages=family==13,composition=family==12,carrier_calls=family==11;
    const bool resource_paths=family==9,union_paths=family==10;
    const bool results=family==8,properties=family>=6 && family<8,cxx=(family&1) || results || union_paths || composition || messages || storage || registers || cpp_runtime || mixed_calls || cpp_values || aligned_blocks || byref_cpp || mixed_storage || carrier_lambdas || arc_records || implicit_checked || atomic_managed || managed_union_paths || storage_lifetime || unnamed_enums || union_carriers || static_proofs || emission_policies || deduced_groups || union_bytes || mrc_records || owning_closures;
    const bool blocks=family>=2 && family<6,producers=family>=4 && family<6;
    std::string source = owning_closures ? R"(
      namespace CodecClosure {
        void *acquire(void *);void release(void *);
        struct Pin {void *value;explicit Pin(void *value):value(acquire(value)){}Pin(const Pin &other):value(acquire(other.value)){}Pin(Pin &&other)noexcept:value(other.value){other.value=nullptr;}~Pin()noexcept{release(value);}};
        inline auto make(void *value){return [pin=Pin(value)]{return pin.value;};}
        template<class Work>inline void *invoke(Work work){return work();}
        inline void *use(void *value){return invoke(make(value));}
      }
    )" : mrc_records ? R"(
      namespace CodecMRC {
        id hold(id);void drop(id);
        struct Owner {id value;explicit Owner(id value):value(hold(value)){}Owner(const Owner &other):value(hold(other.value)){}~Owner()noexcept{drop(value);}};
        using Maybe=__objc3_optional(Owner);
        inline Maybe make(id value){return __objc3_some(Owner,Owner(value));}
        inline int capture(id value){auto owned=make(value);auto callback=[owned]{return __objc3_has_value(owned);};return callback();}
      }
    )" : union_bytes ? R"(
      typedef borrowed const char *CodecUnionLoan;
      CodecUnionLoan codec_union_bytes(void *owner) __attribute__((objc_returns_borrowed(owner_index=0)));
      union CodecByteUnion {const char *first;const char *second;};
      __attribute__((objc_returns_borrowed(owner_index=1)))
      inline CodecUnionLoan codec_union_rebind(void *old_owner,void *new_owner) {
        CodecByteUnion value={codec_union_bytes(old_owner)};
        char *bytes=(char *)&value;*(const char **)bytes=codec_union_bytes(new_owner);return value.second;
      }
    )" : deduced_groups ? R"(
      #include <objc3c/runtime/objc3_task.hpp>
      namespace CodecGroup {
        inline objc3::task<int> callback(objc3::task_group_handle){co_return 42;}
        inline auto make(){return objc3::with_task_group(callback);}
      }
    )" : emission_policies ? R"(
      extern int codec_policy_data __attribute__((section(".data.objc3.codec"),weak));
      inline __attribute__((used,retain,section(".text.objc3.codec"),no_sanitize("address","undefined"),disable_sanitizer_instrumentation,no_profile_instrument_function,no_instrument_function,no_stack_protector,nodebug)) int codec_policy_read(const int *value){return *value+codec_policy_data;}
    )" : static_proofs ? R"(
      typedef borrowed const char *CodecStaticLoan;
      union CodecStaticUnion {char bytes[8];long other;};
      extern const CodecStaticUnion codec_static_union;
      const CodecStaticUnion codec_static_union={{'s','t','a','t','i','c',0}};
      inline const char *codec_static_source(){return codec_static_union.bytes;}
      inline const char *codec_static_offset(int offset){if(offset)return codec_static_source()+offset;return codec_static_union.bytes;}
      inline const char *codec_static_local(int offset){const char *const value=codec_static_offset(offset);return value;}
      __attribute__((objc_returns_borrowed(owner_index=0)))
      inline CodecStaticLoan codec_static_view(void *owner,int offset){return codec_static_local(offset);}
    )" : union_carriers ? R"(
      namespace CatalogUnionCarrier {
        struct Arm {int tag;id __strong owner;id __weak observer;};
        union Value;union Value {Arm owned;int scalar;
          Value();Value(const Value &);Value(Value &&);~Value() noexcept;};
        using Maybe=__objc3_optional(Value);
        inline Maybe make(){return __objc3_some(Value,Value());}
        inline Maybe copy(const Maybe &value){return __objc3_copy(value);}
        inline Maybe implicit_result(){Maybe value=make();return value;}
      }
    )" : unnamed_enums ? R"(
      namespace CatalogUnnamedEnum {
        enum {First=201,Second=203};enum {Other=17,Last=19};
        using Value=decltype(First);
        inline constexpr Value echo(Value value){return value;}
        void initialize() __attribute__((constructor(First)));
        inline int use(){return int(echo(First))+int(Other);}
      }
    )" : storage_lifetime ? R"(
      namespace CatalogStorage {
        struct Owner {int value;explicit Owner(int value):value(value){}~Owner() noexcept {}};
        void initialize() __attribute__((constructor(201)));
        void finalize() __attribute__((destructor(203)));
        inline constinit int ready=42;
        inline __attribute__((no_destroy)) Owner retained(43);
        inline __attribute__((init_priority(202),always_destroy)) Owner ordered(44);
        inline Owner &local(){static __attribute__((no_destroy)) Owner value(45);return value;}
      }
    )" : managed_union_paths ? R"(
      struct CatalogManagedArm {int tag;id owner;id __weak observer;int value;};
      union CatalogManagedUnion {
        CatalogManagedArm owned;int plain;CatalogManagedUnion();CatalogManagedUnion(const CatalogManagedUnion &);
        CatalogManagedUnion(CatalogManagedUnion &&) noexcept;~CatalogManagedUnion();
      };
      __attribute__((objc_root_class)) @interface CatalogManagedUnionRoot
      @property(nonatomic) CatalogManagedUnion active;
      @end
      inline int catalog_managed_union_field(CatalogManagedUnionRoot *_Nonnull root,int value) {
        root[@keypath(CatalogManagedUnionRoot,active.owned.value)]=value;
        return root[@keypath(CatalogManagedUnionRoot,active.owned.value)];
      }
    )" : atomic_managed ? R"(
      typedef __objc3_optional(id) CatalogAtomicMaybe;
      __attribute__((objc_root_class)) @interface CatalogAtomicHost
      @property(atomic) CatalogAtomicMaybe value;
      @end
      inline int catalog_atomic_read(CatalogAtomicHost *_Nonnull host){return __objc3_has_value(host.value);}
    )" : implicit_checked ? R"(
      @protocol Error @end
      namespace CatalogImplicitChecked {
        struct Leaf {int value;~Leaf() noexcept;};struct Owner {Leaf value;};
        Owner produce() __objc3_throws(id<Error>);
        inline int use() __objc3_throws(id<Error>){auto value=__objc3_try(produce());return value.value.value;}
        struct ARC {id __strong object;id __weak observer;};ARC arc() __objc3_throws(id<Error>);
      }
    )" : arc_records ? R"(
      namespace CatalogARCRecord {
        typedef id __strong Strong;typedef id __weak Weak;
        struct Owner {Strong object;Weak observer;};typedef __objc3_optional(Owner) Maybe;
        inline Maybe make(Strong object){return __objc3_some(Owner,Owner{object,object});}
        inline int use(Strong object){auto value=make(object);auto copy=__objc3_copy(value);return __objc3_has_value(copy);}
      }
    )" : carrier_lambdas ? R"(
      namespace CatalogCarrierLambda {
        struct Leaf {int value;explicit Leaf(int);Leaf(const Leaf &);Leaf(Leaf &&);~Leaf() noexcept;};
        typedef id __strong Strong;typedef __objc3_result(Leaf,Strong) Mixed;
        inline int use(const Mixed &source){auto value=__objc3_copy(source);auto callback=[value]{return __objc3_is_ok(value);};auto copied=callback;return copied();}
        template<class T>inline int capture(const T &value){auto callback=[value]{return __objc3_is_ok(value);};return callback();}
      }
    )" : mixed_storage ? R"(
      namespace CatalogMixedStorage {
        struct Leaf {int value;explicit Leaf(int);Leaf(const Leaf &);Leaf(Leaf &&);~Leaf() noexcept;};
        typedef id __strong Strong;typedef __objc3_result(Leaf,Strong) Mixed;
        struct Record {Mixed values[2][2];Record(const Record &)=default;Record(Record &&)=default;~Record() noexcept=default;};
        inline int use(const Record &source){Record copied(source),moved(static_cast<Record &&>(copied));return __objc3_is_ok(moved.values[1][1]);}
        typedef __objc3_optional(Record) Outer;
        inline int nested(const Outer &source){auto copied=__objc3_copy(source);auto moved=__objc3_move(copied);return __objc3_has_value(moved);}

      }
    )" : byref_cpp ? R"(
      namespace CatalogByref {
        struct alignas(128) Value {int value;explicit Value(int);Value(const Value &);Value(Value &&);~Value() noexcept;};
        typedef __objc3_optional(Value) Maybe;
        inline int capture(int seed){__block Maybe value=__objc3_some(Value,Value(seed));auto block=^int{return __objc3_has_value(value);};return block();}
        template<class T>inline int generic(int seed){__block __objc3_optional(T) value=__objc3_some(T,T(seed));auto block=^int{return __objc3_has_value(value);};return block();}
      }
    )" : byref_c ? R"(
      inline int catalog_c_byref(int seed){__block int value __attribute__((aligned(64)))=seed;int (^block)(void)=^int{return ++value;};return block();}
    )" : aligned_c ? R"(
      struct __attribute__((aligned(64))) CatalogCAlignedValue {int value;};
      inline int catalog_c_aligned_capture(int seed){struct CatalogCAlignedValue value={seed};int (^block)(void)=^int{return value.value;};return block();}
    )" : aligned_blocks ? R"(
      namespace CatalogAlignedBlock {
        struct alignas(128) Value {int value;explicit Value(int);Value(const Value &);~Value() noexcept;};
        inline int capture(int seed){Value value(seed);auto block=^int{return value.value;};return block();}
        template<class T>inline int dependent(T value){auto block=^int{(void)value;return 1;};return block();}
      }
    )" : cpp_values ? R"(
      namespace CatalogCppValue {
        struct Value {int value;explicit Value(int);Value(const Value &);Value(Value &&);};
        struct Plain{int value;};typedef __objc3_optional(Value) Maybe;typedef __objc3_optional(Plain) PlainMaybe;
        typedef Maybe (^Callback)(Maybe);
        inline Maybe invoke(Callback _Nonnull callback,const Maybe &value){return callback(value);}
      }
      __attribute__((objc_root_class)) @interface CatalogCppValueHost
      - (CatalogCppValue::Maybe)echo:(CatalogCppValue::Maybe)value;
      - (CatalogCppValue::PlainMaybe)plain:(CatalogCppValue::PlainMaybe)value;
      @end
      inline int catalog_cpp_value(CatalogCppValueHost *_Nonnull object,const CatalogCppValue::Maybe &value){auto result=[object echo:value];return __objc3_has_value(result);}
    )" : mixed_calls ? R"(
      namespace CatalogMixedCall {
        struct Value {int value;explicit Value(int);Value(const Value &);Value(Value &&);~Value() noexcept;};
        typedef id __strong Strong;typedef __objc3_result(Value,Strong) Mixed;
        typedef Mixed (^Callback)(Mixed);
        inline Mixed invoke(Callback _Nonnull callback,const Mixed &value){return callback(value);}
      }
      __attribute__((objc_root_class)) @interface CatalogMixedCallHost
      - (CatalogMixedCall::Mixed)echo:(CatalogMixedCall::Mixed)value;
      @end
      inline int catalog_mixed_message(CatalogMixedCallHost *_Nonnull object,const CatalogMixedCall::Mixed &value){auto result=[object echo:value];return __objc3_is_ok(result);}
    )" : cpp_runtime ? R"(
      namespace CatalogCppRuntime {
        struct Value {int value;explicit Value(int);Value(const Value &);Value(Value &&);~Value() noexcept;};
        typedef __objc3_optional(Value) Maybe;typedef Maybe (^Callback)(Maybe);
        inline Maybe invoke(Callback _Nonnull callback){return callback(__objc3_some(Value,Value(7)));}
      }
      __attribute__((objc_root_class)) @interface CatalogCppRuntimeHost
      - (CatalogCppRuntime::Maybe)echo:(CatalogCppRuntime::Maybe)value;
      @end
      inline int catalog_cpp_message(CatalogCppRuntimeHost *_Nonnull object){auto original=__objc3_some(CatalogCppRuntime::Value,CatalogCppRuntime::Value(7));auto value=[object echo:original];return __objc3_has_value(value);}
    )" : registers ? R"(
      @protocol Error @end
      namespace CatalogCheckedRegister {
        struct Small {int value;};struct Large {long value,other[4];};
        struct [[clang::trivial_abi]] Owner {long value;explicit Owner(int);Owner(const Owner &);Owner(Owner &&);~Owner() noexcept;};
        Small source(int) __objc3_throws(id<Error>);Owner owned(int) __objc3_throws(id<Error>);
        inline Small relay(int mode) __objc3_throws(id<Error>){return __objc3_try(source(mode));}
        inline int use(int mode){auto small=__objc3_try_optional(source(mode));auto value=__objc3_try_optional(owned(mode));return __objc3_has_value(small)+__objc3_has_value(value);}
      }
    )" : storage ? R"(
      namespace CatalogCarrierDefaults {
        struct Value {int value;Value(int);Value(const Value &);Value(Value &&);~Value() noexcept;};
        typedef __objc3_optional(Value) Maybe;
        struct Fields {Maybe value{};Maybe matrix[2][2]{};};
        inline int local(){Maybe value{};Fields fields;return __objc3_has_value(value)+__objc3_has_value(fields.matrix[1][1]);}
        inline void dynamic(int count){auto *values=new Maybe[count]{__objc3_some(Value,Value(7))};delete[] values;}
      }
    )" : messages ? R"(
      @protocol Error @end
      struct CatalogMessageValue {int value;CatalogMessageValue(const CatalogMessageValue &);~CatalogMessageValue() noexcept;};
      __attribute__((objc_root_class)) @interface CatalogMessageHost
      @property(readonly)CatalogMessageValue value;
      - (CatalogMessageValue)value __objc3_throws(id<Error>);
      @end
      inline int catalog_checked_property(CatalogMessageHost *_Nonnull object) {
        auto value=__objc3_try_optional(object.value);return __objc3_has_value(value);
      }
    )" : composition ? R"(
      namespace CatalogCarrierComposition {
        struct Leaf {int value;explicit Leaf(int);Leaf(const Leaf &);Leaf(Leaf &&);~Leaf() noexcept;};
        typedef __objc3_optional(Leaf) Maybe;
        struct Array {Maybe values[2];explicit Array(int v):values{__objc3_some(Leaf,Leaf(v)),__objc3_none(Leaf)} {}
          Array(const Array &)=default;Array(Array &&)=default;~Array() noexcept=default;};
        typedef __objc3_optional(Array) Outer;
        inline Outer make(){return __objc3_some(Array,Array(7));}
        inline int use(){auto value=make();auto copy=__objc3_copy(value);return __objc3_has_value(copy);}
      }
    )" : carrier_calls ? R"(
      @protocol Error @end
      namespace CatalogCarrierCall {
        struct Value {int value;explicit Value(int);Value(const Value &);Value(Value &&);~Value() noexcept;};
        typedef __objc3_optional(Value) Maybe;
        Maybe produce(bool);int consume(Maybe);
        inline Maybe relay(bool some){return produce(some);}
        Maybe checked(int) __objc3_throws(id<Error>);
        inline Maybe checked_relay(int mode) __objc3_throws(id<Error>) {return __objc3_try(checked(mode));}
        inline int checked_optional(int mode){auto value=__objc3_try_optional(checked(mode));return __objc3_has_value(value);}
        inline int use(bool some){auto value=relay(some);return consume(__objc3_copy(value));}
      }
    )" : union_paths ? R"(
      union CatalogUnion {
        int value;double other;
        explicit CatalogUnion(int);CatalogUnion(const CatalogUnion &);
        CatalogUnion(CatalogUnion &&) noexcept;~CatalogUnion() noexcept;
      };
      __attribute__((objc_root_class)) @interface CatalogUnionPath
      @property(nonatomic) CatalogUnion active;
      @end
      KeyPath<CatalogUnionPath *,CatalogUnion> catalog_union_signature();
      inline CatalogUnion catalog_union_value(CatalogUnionPath *_Nonnull object) {
        return KeyPathGet(@keypath(CatalogUnionPath,active),object);
      }
    )" : resource_paths ? R"(
      void catalog_close(int) noexcept;
      typedef int CatalogHandle __attribute__((objc_requires_cleanup(catalog_close),objc_resource_family(CatalogHandles),objc_invalid(-1)));
      struct CatalogOwner {
        CatalogHandle handle;int value;
        explicit CatalogOwner(int);CatalogOwner(const CatalogOwner &);
        CatalogOwner(CatalogOwner &&) noexcept;~CatalogOwner() noexcept;
      };
      __attribute__((objc_root_class)) @interface CatalogResourcePath
      @property(nonatomic) CatalogOwner owner;
      @end
      inline int catalog_resource_field(CatalogResourcePath *_Nonnull object,int value) {
        object[@keypath(CatalogResourcePath,owner.value)]=value;
        return object[@keypath(CatalogResourcePath,owner.value)];
      }
    )" : results ? R"(
      @protocol Error @end
      namespace Catalog {
        struct Value {
          int value;explicit Value(int) noexcept;
          Value(const Value &) noexcept;Value(Value &&) noexcept;~Value() noexcept;
        };
        Value source(int) __objc3_throws(id<Error>);
        inline Value forward(int mode) __objc3_throws(id<Error>) {return __objc3_try(source(mode));}
        inline int optional_result(int mode,bool selected) {
          auto value=selected?__objc3_try_optional(source(0)):__objc3_try_optional(source(mode));
          const auto &reference=__objc3_try_optional(source(mode));
          auto copy=__objc3_copy(value);
          __objc3_try_optional(source(mode));
          return __objc3_has_value(value)+__objc3_has_value(reference)+__objc3_has_value(copy);
        }
        int integer(int) __objc3_throws(id<Error>);
        struct Aggregate {Value first,second;int tail;};
        struct References {const Value &first;int tail;};
        inline int regions(bool selected,int mode) __objc3_throws(id<Error>) {
          Value conditional=selected?__objc3_try(source(mode)):__objc3_try(source(mode+1));
          Aggregate value={__objc3_try(source(0)),__objc3_try(source(mode)),__objc3_try(integer(mode))};
          References references={__objc3_try(source(0)),__objc3_try(integer(mode))};
          return conditional.value+value.tail+references.first.value;
        }

        inline int use(int mode) __objc3_throws(id<Error>) {
          Value (^factory)(int) __objc3_throws(id<Error>)=^Value(int input) __objc3_throws(id<Error>) {return __objc3_try(source(input));};
          Value value=__objc3_try(factory(mode));return value.value;
        }
      }
    )" : properties ? R"(
      #include <objc3c/runtime/objc3_runtime_services.h>
      @protocol Sendable @end
      @protocol Error <Sendable> @end
      __attribute__((objc_root_class)) @interface CatalogProperty <Sendable>
      @property(nonatomic) int value;
      - (int)value __objc3_async_frame __objc3_throws(id<Error>) __attribute__((nothrow,objc_executor(main)));
      - (void)setValue:(int)value __objc3_async_frame __objc3_throws(id<Error>) __attribute__((nothrow,objc_executor(main)));
      @end
      inline int catalog_property(CatalogProperty *object,int value) __objc3_async_frame __objc3_throws(id<Error>) {
        __objc3_try(__objc3_await (object.value=value));
        return __objc3_try(__objc3_await (object.value+=1));
      }
    )" : producers ? R"(
      typedef void (^CatalogCallback)(void);
      struct CatalogSlots {CatalogCallback slots[2];};
      CatalogCallback catalog_producer(int) __attribute__((ns_returns_retained));
      CatalogCallback catalog_shared(int);
      int catalog_step(void);void catalog_checkpoint(void) __attribute__((nothrow));
      void catalog_consume(CatalogCallback __attribute__((ns_consumed))) __attribute__((nothrow));
      inline void catalog_production(struct CatalogSlots *store) {
        __objc3_defer {catalog_checkpoint();}
        CatalogCallback local=catalog_producer(1),shared=catalog_shared(2);
        store->slots[catalog_step()]=catalog_producer(3);
        catalog_consume(catalog_producer(4));local();shared();
      }
    )" : blocks ? R"(
      typedef void (^CatalogCallback)(void);
      struct CatalogSlots {CatalogCallback slots[2];};
      int catalog_step(void);void catalog_checkpoint(void) __attribute__((nothrow));
      inline void catalog_mutation(struct CatalogSlots *store,CatalogCallback value) {
        __objc3_defer {catalog_checkpoint();}
        store->slots[catalog_step()]=(catalog_step(),value);
      }
    )" : cxx ? R"(
      namespace Catalog {
        template<class T=int>struct Opaque;
        template<class T>struct Opaque<T*>;
        template<>struct Opaque<char>;
        template<class T=int>extern T value;
        template<class T>extern T *value<T*>;
        template<class T=int>extern thread_local T local;
        template<class T>extern thread_local T *local<T*>;
        Opaque<> *pointer();
      }
    )" : "inline int catalog_answer(void){return 42;}\n";
    if(properties && cxx)source+=R"(
      template<class T> struct CatalogDependentInitializer {
        T value;explicit CatalogDependentInitializer(T input):value(input) {}
      };
      template<class T> int catalog_property_template(T object) __objc3_async_frame __objc3_throws(id<Error>) {
        return __objc3_try(__objc3_await (object.value+=1));
      }
    )";
    std::vector<std::string> args={cxx?(blocks || properties || results || resource_paths || union_paths || carrier_calls || composition || messages || storage || registers || cpp_runtime || mixed_calls || cpp_values || aligned_blocks || byref_cpp || mixed_storage || carrier_lambdas || arc_records || implicit_checked || atomic_managed || managed_union_paths || storage_lifetime || unnamed_enums || union_carriers || static_proofs || emission_policies || deduced_groups || union_bytes || mrc_records || owning_closures?"-xobjective-c++":"-xc++"):(blocks || properties || aligned_c || byref_c?"-xobjective-c":"-xc"),cxx?"-std=c++23":"-std=c17"};
    if(blocks || properties || results || resource_paths || union_paths || carrier_calls || composition || messages || storage || registers || cpp_runtime || mixed_calls || cpp_values || aligned_blocks || aligned_c || byref_cpp || byref_c || mixed_storage || carrier_lambdas || arc_records || implicit_checked || atomic_managed || managed_union_paths || storage_lifetime || unnamed_enums || union_carriers || static_proofs || emission_policies || deduced_groups || union_bytes || mrc_records || owning_closures)args.insert(args.end(),{"-fobjc-version=3","-fobjc-runtime=gnustep-2.2","-fobjc3-profile=strict-system","-fobjc-arc","-fblocks","-fexceptions","-fobjc-exceptions","--target=x86_64-unknown-linux-gnu"});
    if(mrc_records)std::erase(args,std::string("-fobjc-arc"));
    clang::tooling::FileContentMappings files;
    if(deduced_groups){args.push_back("-resource-dir=" OBJC3_CODEC_TEST_RESOURCE_DIR);AddRuntimeInterfaceFiles(files,args,true);}
    else if(properties || resource_paths || union_paths || managed_union_paths)AddRuntimeInterfaceFiles(files,args);
    if(resource_paths || union_paths || managed_union_paths)args.insert(args.end(),{"-include","objc3c/runtime/objc3_keypath.h"});
    auto ast=clang::tooling::buildASTFromCodeWithArgs(source,args,"catalog-source","clang-tool",
        std::make_shared<clang::PCHContainerOperations>(),clang::tooling::getClangStripDependencyFileAdjuster(),files);
    assert(ast && !ast->getDiagnostics().hasErrorOccurred());
    assert(ApplyModuleOwnedNames(ast->getASTContext(),ast->getASTContext().getTranslationUnitDecl(),
        "fixture.catalog","test",{}));
    auto artifact=EmitSemanticInterface(ast->getASTContext(),"fixture.catalog","test");
    for(const auto &diagnostic:artifact.diagnostics)
      std::fprintf(stderr,"family %u %s: %s (%s)\n",family,diagnostic.code.c_str(),diagnostic.message.c_str(),diagnostic.subject.c_str());
    assert(artifact.success);
    const auto capabilities=SupportedSemanticInterfaceCapabilities();
    auto decoded=DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,capabilities);
    assert(decoded);
    const std::string expected=owning_closures?kNativeLambdaArgumentABISemantic:mrc_records?kNativeMRCRecordPayloadSemantic:union_bytes?kBorrowedUnionStorageSemantic:deduced_groups?kSdkRuntimeDeclarationSemantic:emission_policies?kNativeEmissionPolicySemantic:static_proofs?kBorrowedResultProofSemantic:union_carriers?kNativeARCRecordPayloadSemantic:unnamed_enums?kNativeUnnamedEnumSemantic:storage_lifetime?kNativeStorageLifetimeSemantic:managed_union_paths?kResourceKeyPathValueSemantic:atomic_managed?kAtomicManagedPropertySemantic:implicit_checked?kCheckedCppRecordResultSemantic:arc_records?kNativeARCRecordPayloadSemantic:carrier_lambdas?kNativeCarrierLambdaSemantic:mixed_storage?kCppCarrierCompositionSemantic:byref_cpp || byref_c?kNativeByrefStorageSemantic:aligned_c || aligned_blocks?kNativeBlockAlignmentSemantic:cpp_values?kCppCarrierCallSemantic:mixed_calls?kMixedCarrierCallSemantic:cpp_runtime?kCppCarrierRuntimeSemantic:registers?kCheckedCppRecordResultSemantic:storage?kCppCarrierStorageSemantic:messages?kCheckedCppMessageSemantic:composition?kCppCarrierCompositionSemantic:carrier_calls?kCppCarrierCallSemantic:resource_paths || union_paths?kResourceKeyPathValueSemantic:properties?kAsyncEntryABISemantic:producers || results?kCxxBlockExpressionSemantic:blocks?kAutomaticOwnerUnwindSemantic:cxx?kCxxIncompleteTemplateSemantic:"objc3.c-inline-definitions.v1";
    assert(std::find(decoded->required_semantics.begin(),decoded->required_semantics.end(),expected)!=decoded->required_semantics.end());
    auto bytes=objc3::fuzz::Pack(artifact.source_bytes,artifact.sidecar_json);assert(!bytes.empty());
    std::string first;
    for(unsigned replay=0;replay!=20;++replay) {
      std::string observed;
      assert(objc3::fuzz::Codec(bytes.data(),bytes.size(),&observed)==objc3::fuzz::Outcome::Accepted);
      if(!replay)first=observed;else assert(observed==first);
    }
    auto changed=objc3::fuzz::Pack(artifact.source_bytes+" ",artifact.sidecar_json);
    assert(objc3::fuzz::Codec(changed.data(),changed.size())==objc3::fuzz::Outcome::SemanticRejected);
    auto restricted=std::vector<llvm::StringRef>(capabilities.begin(),capabilities.end());
    restricted.push_back("objc3.unknown.catalog.v1");
    decoded->required_semantics.push_back("objc3.unknown.catalog.v1");
    auto unknown=EncodeSemanticInterfaceSidecar(*decoded,restricted);assert(unknown);
    auto unsupported=objc3::fuzz::Pack(artifact.source_bytes,*unknown);
    assert(objc3::fuzz::Codec(unsupported.data(),unsupported.size())==objc3::fuzz::Outcome::SemanticRejected);
  }
}
