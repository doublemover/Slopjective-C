#include "standard_library_test_support.h"
#include "llvm/ADT/ScopeExit.h"

using namespace objc3::clang_foundation;
using namespace objc3::clang_foundation::standard_library_test;

namespace {
void Run(const fs::path &root, const char *standard, int ownership,
         const char *optimization,
         const std::vector<std::string> &library_arguments) {
  fs::create_directories(root / "provider");
  auto request = [&](const fs::path &input, const fs::path &output) {
    return Request(input, output, standard, ownership, optimization,
                   library_arguments);
  };
  const auto source = root / "provider/module.mm";
  Write(source, R"cpp(
#include <format>
#include <iterator>
#include <numeric>
#include <string>
#include <type_traits>
namespace Customization {
struct Key {int value;};
struct Cursor {
  int *current;
  int &operator*()const{return *current;}
  Cursor &operator++(){++current;return *this;}
  Cursor operator++(int){auto old=*this;++*this;return old;}
  bool operator==(const Cursor &)const=default;
};
}
namespace std {
template<>struct formatter<Customization::Key,char>:formatter<int,char> {
  template<class Context>
  auto format(const Customization::Key &key,Context &context)const {
    return formatter<int,char>::format(key.value,context);
  }
};
template<>struct iterator_traits<Customization::Cursor> {
  using iterator_category=forward_iterator_tag;
  using iterator_concept=forward_iterator_tag;
  using value_type=int;
  using difference_type=ptrdiff_t;
  using pointer=int *;
  using reference=int &;
};
}
namespace Customization {
inline std::string render(Key key){return std::format("key={:04}",key);}
inline int total(Cursor first,Cursor last){return std::accumulate(first,last,0);}
}
)cpp");
  auto provider = request(source, root / "provider.o");
  provider.module_owned_names = true;
  provider.module_source_root = (root / "provider").string();
  provider.module_name = "fixture.StandardCustomizations";
  provider.package_identity = "standard-customizations-v1";
  provider.interface_exports = {"Customization"};
  provider.emit_semantic_interface = true;
  auto built = Compile(provider);
  Success(built);
  assert(built.interface_artifact && built.interface_artifact->success);
  const auto &artifact = *built.interface_artifact;
  const auto interface = root / "Customizations.objc3i";
  Write(interface, artifact.source_bytes);
  Write(interface.string() + ".json", artifact.sidecar_json);
  assert(fs::remove(source));
  for (const char *name : {"a", "b"}) {
    const auto path = root / (std::string("consumer-") + name + ".mm");
    Write(path, "extern \"C\" int check_" + std::string(name) + R"cpp(() {
  static_assert(std::forward_iterator<Customization::Cursor>);
  static_assert(std::is_same_v<std::iter_value_t<Customization::Cursor>,int>);
  static_assert(std::is_same_v<std::iter_reference_t<Customization::Cursor>,int &>);
  if(Customization::render({42})!="key=0042")return 1;
  if(std::format("{:06}",Customization::Key{17})!="000017")return 2;
  int values[]={11,13,18};
  Customization::Cursor first{values},last{values+3};
  if(Customization::total(first,last)!=42 || std::distance(first,last)!=3)return 3;
  auto advanced=first;std::advance(advanced,2);
  return *advanced==18?0:4;
}
)cpp" + "extern \"C\" void *address_" + name +
                    "(){return reinterpret_cast<void *>(&Customization::render);}\n");
    auto consumer = request(path, root / (std::string(name) + ".o"));
    InterfaceImportInput imported;
    imported.source_path = interface.string();
    imported.locked_identity = artifact.identity;
    consumer.imported_interfaces = {imported};
    Success(Compile(consumer));
  }
  const auto main = root / "main.cpp";
  Write(main, R"cpp(extern "C" int check_a();extern "C" int check_b();
extern "C" void *address_a();extern "C" void *address_b();
int main(){return check_a() || check_b() || address_a()!=address_b();}
)cpp");
  auto entry = request(main, root / "main.o");
  Success(Compile(entry));
  LinkAndExecute(root, entry.output_path, ownership != 0, library_arguments);
}
} // namespace

int main(int argc, char **argv) {
  unsigned profiles=3;
  std::vector<std::string> library_arguments;
  for(int i=1;i<argc;++i) {
    const std::string argument(argv[i]);
    if(argument=="--arc-only")profiles=2;
    else if(argument=="--cxx-only")profiles=1;
    else library_arguments.push_back(argument);
  }
  llvm::SmallString<128> temporary;
  assert(!llvm::sys::fs::createUniqueDirectory("objc3-standard-customizations", temporary));
  const fs::path root(temporary.c_str());
  auto cleanup = llvm::scope_exit([&] { fs::remove_all(root); });
  if(profiles&1) {
    std::cerr << "STANDARD CUSTOMIZATIONS C++20 O0\n";
    Run(root / "cxx20", "c++20", 0, "-O0", library_arguments);
  }
  if(profiles&2) {
    std::cerr << "STANDARD CUSTOMIZATIONS Objective-C++23 ARC O2\n";
    Run(root / "arc23", "c++23", 2, "-O2", library_arguments);
  }
}
