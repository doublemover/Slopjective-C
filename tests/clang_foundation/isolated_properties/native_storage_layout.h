#ifndef OBJC3_NATIVE_STORAGE_LAYOUT_H
#define OBJC3_NATIVE_STORAGE_LAYOUT_H
#ifdef __cplusplus
#include <string>
#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
extern "C" const char *objc_skip_typespec(const char *);
static void native_storage_layout_suite() {
 const char *native="{__objc3_cxx_storage_v2_s128_a128=[128c]}";
 assert(objc_sizeof_type(native)==128 && objc_alignof_type(native)==128);
 assert(*objc_skip_typespec(native)==0);
 const char *nested="{Native=B{__objc3_cxx_storage_v2_s128_a128=[128c]}c}";
 struct NativeStorageOuter {bool tag;alignas(128) char value[128];char tail;};
 assert(objc_sizeof_type(nested)==sizeof(NativeStorageOuter));
 assert(objc_alignof_type(nested)==alignof(NativeStorageOuter));
 const char *array="[3{__objc3_cxx_storage_v2_s64_a32=[64c]}]";
 assert(objc_sizeof_type(array)==192 && objc_alignof_type(array)==32);
 const char *active="(Native=c{__objc3_cxx_storage_v2_s128_a128=[128c]})";
 assert(objc_sizeof_type(active)==128 && objc_alignof_type(active)==128);
 const char *qualified="r{__objc3_cxx_storage_v2_s32_a32=[32c]}";
 assert(objc_sizeof_type(qualified)==32 && objc_alignof_type(qualified)==32);
 for(const char *suffix:{"","0_a32=[32c]}","032_a32=[32c]}","32_a0=[32c]}",
   "32_a3=[32c]}","16_a32=[16c]}","33_a32=[33c]}","32_a32=[31c]}",
   "32_a32=[32i]}","32_a32=[32c]","32_a32=[32c}","32_a32=32c]}",
   "32_a","32_a32","32_32=[32c]}","184467440737095516160_a32=[32c]}",
   "32_a184467440737095516160=[32c]}"}) {
  std::string malformed="{__objc3_cxx_storage_v2_s";malformed+=suffix;
  for(int query=0;query!=3;++query) {
   pid_t child=fork();assert(child>=0);
   if(child==0) {
    struct rlimit no_core={0,0};setrlimit(RLIMIT_CORE,&no_core);close(2);
    if(query==0)(void)objc_sizeof_type(malformed.c_str());
    else if(query==1)(void)objc_alignof_type(malformed.c_str());
    else (void)objc_skip_typespec(malformed.c_str());
    _exit(0);
   }
   int status;assert(waitpid(child,&status,0)==child);
   assert(WIFSIGNALED(status) && WTERMSIG(status)==SIGABRT);
  }
 }
}
#else
static void native_storage_layout_suite(void){}
#endif
#endif
