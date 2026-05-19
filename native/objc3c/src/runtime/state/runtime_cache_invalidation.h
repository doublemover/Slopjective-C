#pragma once

#include "runtime/dispatch/runtime_resolution_records.h"
#include "runtime/state/runtime_state_records.h"

namespace objc3c::runtime {

inline void BumpRuntimeClassGraphGenerationUnlocked(RuntimeState &state) {
  ++state.class_graph_generation;
}

inline void BumpRuntimeCategoryAttachmentGenerationUnlocked(
    RuntimeState &state) {
  ++state.category_attachment_generation;
}

inline void BumpRuntimeProtocolDeclarationGenerationUnlocked(
    RuntimeState &state) {
  ++state.protocol_declaration_generation;
}

inline void BumpRuntimeStorageSurfaceGenerationUnlocked(RuntimeState &state) {
  ++state.storage_surface_generation;
}

inline void BumpRuntimeMethodSurfaceGenerationUnlocked(RuntimeState &state) {
  ++state.method_surface_generation;
}

inline void StampMethodCacheMutationGenerationsUnlocked(
    MethodCacheEntry &entry,
    const RuntimeState &state) {
  entry.cache_class_graph_generation = state.class_graph_generation;
  entry.cache_category_attachment_generation =
      state.category_attachment_generation;
  entry.cache_protocol_declaration_generation =
      state.protocol_declaration_generation;
  entry.cache_storage_surface_generation = state.storage_surface_generation;
  entry.cache_method_surface_generation = state.method_surface_generation;
}

inline bool MethodCacheMutationGenerationsMatchUnlocked(
    const RuntimeState &state,
    const MethodCacheEntry &entry) {
  return entry.cache_class_graph_generation == state.class_graph_generation &&
         entry.cache_category_attachment_generation ==
             state.category_attachment_generation &&
         entry.cache_protocol_declaration_generation ==
             state.protocol_declaration_generation &&
         entry.cache_storage_surface_generation ==
             state.storage_surface_generation &&
         entry.cache_method_surface_generation ==
             state.method_surface_generation;
}

inline void StampPropertyLookupCacheMutationGenerationsUnlocked(
    PropertyLookupCacheEntry &entry,
    const RuntimeState &state) {
  entry.cache_class_graph_generation = state.class_graph_generation;
  entry.cache_category_attachment_generation =
      state.category_attachment_generation;
  entry.cache_storage_surface_generation = state.storage_surface_generation;
}

inline bool PropertyLookupCacheMutationGenerationsMatchUnlocked(
    const RuntimeState &state,
    const PropertyLookupCacheEntry &entry) {
  return entry.cache_class_graph_generation == state.class_graph_generation &&
         entry.cache_category_attachment_generation ==
             state.category_attachment_generation &&
         entry.cache_storage_surface_generation ==
             state.storage_surface_generation;
}

}  // namespace objc3c::runtime
