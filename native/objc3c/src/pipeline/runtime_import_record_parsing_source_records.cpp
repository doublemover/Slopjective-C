#include "pipeline/runtime_import_preservation_owners.h"

#include <utility>
#include <vector>

#include "pipeline/runtime_import_record_parsing_source_record_owners.h"

namespace objc3c::pipeline::runtime_import_preservation {
namespace {

template <typename RecordT>
bool ParseRecordArray(const RuntimeImportJsonValue::Object &root,
                      const std::string &declarations_name,
                      const std::string &record_name,
                      std::vector<RecordT> &records,
                      bool (*parser)(const RuntimeImportJsonValue::Object &,
                                     RecordT &, std::string &),
                      std::string &error) {
  const RuntimeImportJsonValue *declarations_value =
      FindMember(root, declarations_name);
  if (declarations_value == nullptr) {
    error = "missing JSON object member '" + declarations_name + "'";
    return false;
  }
  const RuntimeImportJsonValue::Object *declarations_object =
      AsObject(*declarations_value);
  if (declarations_object == nullptr) {
    error = "JSON member '" + declarations_name + "' must be an object";
    return false;
  }
  const RuntimeImportJsonValue *records_value =
      FindMember(*declarations_object, record_name);
  if (records_value == nullptr) {
    error = "missing JSON array member '" + record_name + "'";
    return false;
  }
  const RuntimeImportJsonValue::Array *records_array =
      AsArray(*records_value);
  if (records_array == nullptr) {
    error = "JSON member '" + record_name + "' must be an array";
    return false;
  }
  records.clear();
  records.reserve(records_array->size());
  for (const RuntimeImportJsonValue &element : *records_array) {
    const RuntimeImportJsonValue::Object *record_object = AsObject(element);
    if (record_object == nullptr) {
      error = "JSON array member '" + record_name + "' must contain objects";
      return false;
    }
    RecordT record;
    if (!parser(*record_object, record, error)) {
      return false;
    }
    records.push_back(std::move(record));
  }
  return true;
}

}  // namespace

bool ParseRuntimeMetadataSourceRecordSetContents(
    const RuntimeImportJsonValue::Object &root,
    const std::string &declarations_name,
    Objc3RuntimeMetadataSourceRecordSet &record_set,
    std::string &error) {
  if (!ParseRecordArray(root, declarations_name, "classes",
                        record_set.classes_lexicographic,
                        ParseImportedRuntimeClassSourceRecord, error) ||
      !ParseRecordArray(root, declarations_name, "protocols",
                        record_set.protocols_lexicographic,
                        ParseImportedRuntimeProtocolSourceRecord, error) ||
      !ParseRecordArray(root, declarations_name, "categories",
                        record_set.categories_lexicographic,
                        ParseImportedRuntimeCategorySourceRecord, error) ||
      !ParseRecordArray(root, declarations_name, "properties",
                        record_set.properties_lexicographic,
                        ParseImportedRuntimePropertySourceRecord, error) ||
      !ParseRecordArray(root, declarations_name, "methods",
                        record_set.methods_lexicographic,
                        ParseImportedRuntimeMethodSourceRecord, error) ||
      !ParseRecordArray(root, declarations_name, "ivars",
                        record_set.ivars_lexicographic,
                        ParseImportedRuntimeIvarSourceRecord, error)) {
    return false;
  }
  record_set.deterministic = true;
  return true;
}

}  // namespace objc3c::pipeline::runtime_import_preservation
