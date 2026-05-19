#include "artifacts/objc3_frontend_runtime_import_artifacts.h"

#include <algorithm>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "support/objc3_runtime_metadata_record_set.h"

namespace objc3::artifacts::frontend {
namespace {

std::string BuildRuntimeMetadataClassRecordMergeKey(
    const Objc3RuntimeMetadataClassSourceRecord &record) {
  return record.record_kind + "|" + record.name;
}

std::string BuildRuntimeMetadataProtocolRecordMergeKey(
    const Objc3RuntimeMetadataProtocolSourceRecord &record) {
  return record.name;
}

std::string BuildRuntimeMetadataCategoryRecordMergeKey(
    const Objc3RuntimeMetadataCategorySourceRecord &record) {
  return record.record_kind + "|" + record.class_name + "|" +
         record.category_name;
}

std::string BuildRuntimeMetadataPropertyRecordMergeKey(
    const Objc3RuntimeMetadataPropertySourceRecord &record) {
  return record.owner_kind + "|" + record.owner_name + "|" +
         record.property_name;
}

std::string BuildRuntimeMetadataMethodRecordMergeKey(
    const Objc3RuntimeMetadataMethodSourceRecord &record) {
  return record.owner_kind + "|" + record.owner_name + "|" + record.selector +
         "|" + (record.is_class_method ? "class" : "instance");
}

std::string BuildRuntimeMetadataIvarRecordMergeKey(
    const Objc3RuntimeMetadataIvarSourceRecord &record) {
  return record.owner_kind + "|" + record.owner_name + "|" +
         record.property_name + "|" + record.ivar_binding_symbol;
}

template <typename RecordT, typename KeyFn>
std::vector<RecordT> BuildMergedRuntimeMetadataRecordVector(
    const std::vector<const std::vector<RecordT> *> &imported_record_vectors,
    const std::vector<RecordT> &local_records,
    KeyFn key_fn) {
  std::unordered_map<std::string, RecordT> merged_by_key;
  for (const auto *records : imported_record_vectors) {
    for (const auto &record : *records) {
      merged_by_key.emplace(key_fn(record), record);
    }
  }
  for (const auto &record : local_records) {
    merged_by_key[key_fn(record)] = record;
  }
  std::vector<std::pair<std::string, RecordT>> ordered_records;
  ordered_records.reserve(merged_by_key.size());
  for (auto &entry : merged_by_key) {
    ordered_records.push_back(std::move(entry));
  }
  std::sort(ordered_records.begin(),
            ordered_records.end(),
            [](const auto &lhs, const auto &rhs) {
              return lhs.first < rhs.first;
            });
  std::vector<RecordT> merged;
  merged.reserve(ordered_records.size());
  for (auto &entry : ordered_records) {
    merged.push_back(std::move(entry.second));
  }
  return merged;
}

std::vector<const Objc3ImportedRuntimeModuleSurface *>
BuildSortedImportedRuntimeModuleSurfacePointers(
    const std::vector<Objc3ImportedRuntimeModuleSurface> &imported_surfaces) {
  std::vector<const Objc3ImportedRuntimeModuleSurface *> sorted;
  sorted.reserve(imported_surfaces.size());
  for (const auto &surface : imported_surfaces) {
    sorted.push_back(&surface);
  }
  std::sort(sorted.begin(),
            sorted.end(),
            [](const auto *lhs, const auto *rhs) {
              return lhs->frontend_closure_summary.module_name <
                     rhs->frontend_closure_summary.module_name;
            });
  return sorted;
}

}  // namespace

std::vector<std::string> BuildSerializedRuntimeMetadataReusedModuleNames(
    const std::string &local_module_name,
    const std::vector<Objc3ImportedRuntimeModuleSurface> &imported_surfaces) {
  std::set<std::string> names;
  if (!local_module_name.empty()) {
    names.insert(local_module_name);
  }
  for (const auto *surface :
       BuildSortedImportedRuntimeModuleSurfacePointers(imported_surfaces)) {
    if (surface->uses_serialized_runtime_metadata_payload &&
        !surface->reused_module_names_lexicographic.empty()) {
      names.insert(surface->reused_module_names_lexicographic.begin(),
                   surface->reused_module_names_lexicographic.end());
    } else if (!surface->frontend_closure_summary.module_name.empty()) {
      names.insert(surface->frontend_closure_summary.module_name);
    }
  }
  return std::vector<std::string>(names.begin(), names.end());
}

Objc3RuntimeMetadataSourceRecordSet BuildSerializedRuntimeMetadataReuseRecordSet(
    const Objc3RuntimeMetadataSourceRecordSet
        &local_runtime_metadata_source_records,
    const std::vector<Objc3ImportedRuntimeModuleSurface> &imported_surfaces) {
  std::vector<const std::vector<Objc3RuntimeMetadataClassSourceRecord> *>
      imported_classes;
  std::vector<const std::vector<Objc3RuntimeMetadataProtocolSourceRecord> *>
      imported_protocols;
  std::vector<const std::vector<Objc3RuntimeMetadataCategorySourceRecord> *>
      imported_categories;
  std::vector<const std::vector<Objc3RuntimeMetadataPropertySourceRecord> *>
      imported_properties;
  std::vector<const std::vector<Objc3RuntimeMetadataMethodSourceRecord> *>
      imported_methods;
  std::vector<const std::vector<Objc3RuntimeMetadataIvarSourceRecord> *>
      imported_ivars;
  for (const auto *surface :
       BuildSortedImportedRuntimeModuleSurfacePointers(imported_surfaces)) {
    imported_classes.push_back(
        &surface->runtime_metadata_source_records.classes_lexicographic);
    imported_protocols.push_back(
        &surface->runtime_metadata_source_records.protocols_lexicographic);
    imported_categories.push_back(
        &surface->runtime_metadata_source_records.categories_lexicographic);
    imported_properties.push_back(
        &surface->runtime_metadata_source_records.properties_lexicographic);
    imported_methods.push_back(
        &surface->runtime_metadata_source_records.methods_lexicographic);
    imported_ivars.push_back(
        &surface->runtime_metadata_source_records.ivars_lexicographic);
  }

  Objc3RuntimeMetadataSourceRecordSet merged;
  merged.classes_lexicographic = BuildMergedRuntimeMetadataRecordVector(
      imported_classes,
      local_runtime_metadata_source_records.classes_lexicographic,
      BuildRuntimeMetadataClassRecordMergeKey);
  merged.protocols_lexicographic = BuildMergedRuntimeMetadataRecordVector(
      imported_protocols,
      local_runtime_metadata_source_records.protocols_lexicographic,
      BuildRuntimeMetadataProtocolRecordMergeKey);
  merged.categories_lexicographic = BuildMergedRuntimeMetadataRecordVector(
      imported_categories,
      local_runtime_metadata_source_records.categories_lexicographic,
      BuildRuntimeMetadataCategoryRecordMergeKey);
  merged.properties_lexicographic = BuildMergedRuntimeMetadataRecordVector(
      imported_properties,
      local_runtime_metadata_source_records.properties_lexicographic,
      BuildRuntimeMetadataPropertyRecordMergeKey);
  merged.methods_lexicographic = BuildMergedRuntimeMetadataRecordVector(
      imported_methods,
      local_runtime_metadata_source_records.methods_lexicographic,
      BuildRuntimeMetadataMethodRecordMergeKey);
  merged.ivars_lexicographic = BuildMergedRuntimeMetadataRecordVector(
      imported_ivars,
      local_runtime_metadata_source_records.ivars_lexicographic,
      BuildRuntimeMetadataIvarRecordMergeKey);
  merged.deterministic = true;
  return merged;
}

}  // namespace objc3::artifacts::frontend
