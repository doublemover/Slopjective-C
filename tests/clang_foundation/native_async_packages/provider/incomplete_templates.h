#ifndef NATIVE_INCOMPLETE_TEMPLATES_H
#define NATIVE_INCOMPLETE_TEMPLATES_H
#include <functional>
namespace NativeIncompleteTemplates {
template<class T=int,int Bias=2> requires(sizeof(T)>0) struct Box;
struct Enclosing {template<class T>struct Opaque;};
template<class T>struct Wrapper {template<class U>struct Opaque;};
template<class T=int> extern T value;
template<class T=int> extern thread_local T local;
template<class T>struct Selection {static constexpr int tag=1;};
template<class T>struct Selection<T*>;
template<>struct Selection<char>;
template<class T>extern int selected;
template<class T>extern int selected<T*>;
template<class T>extern thread_local int selected_local;
template<class T>extern thread_local int selected_local<T*>;
Selection<int*> *provider_selected_pointer(Selection<int*> *pointer);
Selection<char> *provider_character_pointer(Selection<char> *pointer);
int &provider_selected();
int &provider_selected_local();
struct HashKey {int value;};
}
namespace std {
template<>struct hash<NativeIncompleteTemplates::HashKey>;
template<class T,int Bias>struct hash<NativeIncompleteTemplates::Box<T,Bias>>;
}
namespace NativeIncompleteTemplates {
using Hash=std::hash<HashKey>;
using BoxHash=std::hash<Box<>>;
Hash *provider_hash_pointer(Hash *pointer);
BoxHash *provider_box_hash_pointer(BoxHash *pointer);
using Default=Box<>;
Default *provider_pointer(Default *pointer);
int &provider_value();
int &provider_local();
}
#endif
