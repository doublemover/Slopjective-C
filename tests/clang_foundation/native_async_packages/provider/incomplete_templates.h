#ifndef NATIVE_INCOMPLETE_TEMPLATES_H
#define NATIVE_INCOMPLETE_TEMPLATES_H
namespace NativeIncompleteTemplates {
template<class T=int,int Bias=2> requires(sizeof(T)>0) struct Box;
struct Enclosing {template<class T>struct Opaque;};
template<class T>struct Wrapper {template<class U>struct Opaque;};
template<class T=int> extern T value;
template<class T=int> extern thread_local T local;
using Default=Box<>;
Default *provider_pointer(Default *pointer);
int &provider_value();
int &provider_local();
}
#endif
