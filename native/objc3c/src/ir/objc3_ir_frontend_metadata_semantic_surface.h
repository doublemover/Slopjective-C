#pragma once

#include <cstddef>
#include <string>

struct Objc3IRFrontendSemanticSurfaceMetadata {
  std::size_t object_pointer_type_spellings = 0;
  std::size_t pointer_declarator_entries = 0;
  std::size_t pointer_declarator_depth_total = 0;
  std::size_t pointer_declarator_token_entries = 0;
  std::size_t nullability_suffix_entries = 0;
  std::size_t generic_suffix_entries = 0;
  std::size_t terminated_generic_suffix_entries = 0;
  std::size_t unterminated_generic_suffix_entries = 0;
  bool deterministic_object_pointer_nullability_generics_handoff = false;
  std::size_t global_symbol_nodes = 0;
  std::size_t function_symbol_nodes = 0;
  std::size_t interface_symbol_nodes = 0;
  std::size_t implementation_symbol_nodes = 0;
  std::size_t interface_property_symbol_nodes = 0;
  std::size_t implementation_property_symbol_nodes = 0;
  std::size_t interface_method_symbol_nodes = 0;
  std::size_t implementation_method_symbol_nodes = 0;
  std::size_t top_level_scope_symbols = 0;
  std::size_t nested_scope_symbols = 0;
  std::size_t scope_frames_total = 0;
  std::size_t implementation_interface_resolution_sites = 0;
  std::size_t implementation_interface_resolution_hits = 0;
  std::size_t implementation_interface_resolution_misses = 0;
  std::size_t method_resolution_sites = 0;
  std::size_t method_resolution_hits = 0;
  std::size_t method_resolution_misses = 0;
  bool deterministic_symbol_graph_handoff = false;
  bool deterministic_scope_resolution_handoff = false;
  std::string deterministic_symbol_graph_scope_resolution_handoff_key;
};
