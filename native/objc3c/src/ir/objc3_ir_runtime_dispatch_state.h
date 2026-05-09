#pragma once

#include <cstddef>
#include <set>
#include <string>

struct Objc3IRRuntimeDispatchCallState {
  std::set<std::string> runtime_dispatch_symbols_used;
  bool runtime_dispatch_call_emitted = false;
  std::size_t direct_dispatch_call_sites_emitted = 0;
  std::size_t runtime_dispatch_call_sites_emitted = 0;
  std::size_t selector_pool_gep_sites_emitted = 0;

  void Reset();
  void NoteDirectDispatchCall();
  void NoteSelectorPoolGep();
  void NoteRuntimeDispatchCall(const std::string &dispatch_symbol);
};
