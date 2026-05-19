#include "runtime/storage/property_ivar_layout_index.h"

#include "runtime/images/image_descriptor.h"
#include "runtime/storage/property_layout_rules.h"

#include <algorithm>
#include <cstdint>

namespace objc3c::runtime {

namespace {

std::string RuntimePropertyIvarLayoutLookupKey(const char *layout_identity,
                                               const char *property_name) {
  const std::string layout =
      layout_identity != nullptr ? layout_identity : "";
  const std::string property =
      property_name != nullptr ? property_name : "";
  return layout + "\n" + property;
}

void AddRuntimePropertyIvarLayoutLookup(
    std::unordered_map<std::string, const EmittedIvarDescriptor *> &lookup,
    std::unordered_set<std::string> &ambiguous_keys,
    const char *layout_identity,
    const EmittedIvarDescriptor &descriptor) {
  if (layout_identity == nullptr || layout_identity[0] == '\0' ||
      descriptor.property_name == nullptr ||
      descriptor.property_name[0] == '\0') {
    return;
  }
  const std::string key =
      RuntimePropertyIvarLayoutLookupKey(layout_identity,
                                         descriptor.property_name);
  const auto inserted = lookup.emplace(key, &descriptor);
  if (!inserted.second && inserted.first->second != &descriptor) {
    ambiguous_keys.insert(key);
    lookup.erase(inserted.first);
  }
}

const EmittedIvarDescriptor *FindRuntimePropertyIvarDescriptorByLayoutLookup(
    const std::unordered_map<std::string, const EmittedIvarDescriptor *> &lookup,
    const std::unordered_set<std::string> &ambiguous_keys,
    const char *layout_identity,
    const char *property_name) {
  if (layout_identity == nullptr || layout_identity[0] == '\0' ||
      property_name == nullptr || property_name[0] == '\0') {
    return nullptr;
  }
  const std::string key =
      RuntimePropertyIvarLayoutLookupKey(layout_identity, property_name);
  if (ambiguous_keys.find(key) != ambiguous_keys.end()) {
    return nullptr;
  }
  const auto found = lookup.find(key);
  return found != lookup.end() ? found->second : nullptr;
}

}  // namespace

bool BuildRuntimePropertyIvarLayoutIndex(
    const RegisteredImageMetadata &image,
    const std::string &ivar_owner_identity,
    RuntimePropertyIvarLayoutIndex &index) {
  index = RuntimePropertyIvarLayoutIndex{};
  if (image.ivar_descriptor_root == nullptr) {
    return false;
  }

  for (std::uint64_t descriptor_index = 0;
       descriptor_index < image.ivar_descriptor_count;
       ++descriptor_index) {
    const auto *descriptor = static_cast<const EmittedIvarDescriptor *>(
        RuntimeAggregateEntry(image.ivar_descriptor_root, descriptor_index));
    if (descriptor == nullptr ||
        descriptor->declaration_owner_identity == nullptr ||
        descriptor->property_name == nullptr ||
        descriptor->ivar_binding_symbol == nullptr ||
        descriptor->ivar_binding_symbol[0] == '\0') {
      return false;
    }
    if (ivar_owner_identity != descriptor->declaration_owner_identity) {
      continue;
    }

    const std::size_t offset =
        descriptor->offset_global != nullptr
            ? static_cast<std::size_t>(*descriptor->offset_global)
            : static_cast<std::size_t>(descriptor->offset_bytes);
    const std::size_t size = static_cast<std::size_t>(descriptor->size_bytes);
    const std::size_t alignment = std::max<std::size_t>(
        static_cast<std::size_t>(descriptor->alignment_bytes), 1u);
    if (!RuntimePropertyIvarDescriptorHasStrictPublishedLayout(
            *descriptor, offset, alignment)) {
      return false;
    }
    if (size != 0u &&
        !RuntimePropertyIvarStorageExtentIsAddressable(offset, size)) {
      return false;
    }
    if (size != 0u) {
      index.max_storage_end = std::max(index.max_storage_end, offset + size);
      index.max_alignment = std::max(index.max_alignment, alignment);
    }
    index.published_owner_size =
        std::max<std::size_t>(index.published_owner_size,
                              static_cast<std::size_t>(
                                  descriptor->owner_size_bytes));
    index.ivars_by_binding_symbol.emplace(descriptor->ivar_binding_symbol,
                                          descriptor);
    AddRuntimePropertyIvarLayoutLookup(index.ivars_by_layout_symbol,
                                       index.ambiguous_layout_symbols,
                                       descriptor->layout_record->layout_symbol,
                                       *descriptor);
    AddRuntimePropertyIvarLayoutLookup(index.ivars_by_layout_replay_key,
                                       index.ambiguous_layout_replay_keys,
                                       descriptor->layout_replay_key,
                                       *descriptor);
  }
  return true;
}

std::size_t RuntimePropertyIvarLayoutInstanceSize(
    const RuntimePropertyIvarLayoutIndex &index) {
  return std::max<std::size_t>(
      index.published_owner_size,
      AlignRuntimePropertyStorageSize(index.max_storage_end,
                                      index.max_alignment));
}

const EmittedIvarDescriptor *FindRuntimePropertyIvarDescriptorForProperty(
    const RuntimePropertyIvarLayoutIndex &index,
    const EmittedPropertyDescriptor &descriptor) {
  if (descriptor.ivar_binding_symbol != nullptr &&
      descriptor.ivar_binding_symbol[0] != '\0') {
    const auto found =
        index.ivars_by_binding_symbol.find(descriptor.ivar_binding_symbol);
    if (found != index.ivars_by_binding_symbol.end()) {
      return found->second;
    }
  }
  const EmittedIvarDescriptor *by_layout_symbol =
      FindRuntimePropertyIvarDescriptorByLayoutLookup(
          index.ivars_by_layout_symbol, index.ambiguous_layout_symbols,
          descriptor.ivar_layout_symbol, descriptor.property_name);
  if (by_layout_symbol != nullptr) {
    return by_layout_symbol;
  }
  return FindRuntimePropertyIvarDescriptorByLayoutLookup(
      index.ivars_by_layout_replay_key, index.ambiguous_layout_replay_keys,
      descriptor.ivar_layout_replay_key, descriptor.property_name);
}

}  // namespace objc3c::runtime
