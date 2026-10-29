#ifndef NATIVE_FOREIGN_IMPORT_ATTRIBUTES_H
#define NATIVE_FOREIGN_IMPORT_ATTRIBUTES_H
namespace NativeForeignAttributes {
struct __attribute__((swift_bridge("Interop.Value"))) Value {int value;};
typedef int Handle __attribute__((swift_newtype(struct)));
int read(Value value) __attribute__((swift_name("Interop.read(_:)")));
int read(Value value){return value.value;}
inline int increment(int value) __attribute__((swift_name("Interop.increment(_:)"),swift_attr("@Sendable"))){return value+1;}
inline int hidden(int value) __attribute__((swift_private)){return value;}
}
#endif
