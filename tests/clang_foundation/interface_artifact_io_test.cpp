#include "clang_foundation/interface_artifact_io.h"
#include "clang_foundation/interface_artifact_io_testing.h"
#include "clang_foundation/interface_codec.h"

#include <cassert>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>
#include <stdexcept>
#include <vector>

using namespace objc3::clang_foundation;
namespace fs = std::filesystem;

namespace {

InterfaceEmissionArtifact MakeArtifact(
    const std::vector<std::string> &required_semantics = {}) {
  InterfaceEmissionArtifact artifact;
  artifact.success = true;
  artifact.source_bytes = "/* canonical interface */\nextern int sample;\n";
  SemanticInterfaceSidecar sidecar;
  sidecar.module.canonical_name = "fixture.sample";
  sidecar.module.target_triple = "x86_64-unknown-linux-gnu";
  sidecar.module.runtime_abi = "no-objc-runtime;manual-reference-counting";
  sidecar.module.extension_abi = "objc3-extension-abi-2:test-digest";
  sidecar.module.language_revision = "objc3-disabled";
  sidecar.module.language_profile = "objc3=disabled;profile=compatibility;strict-concurrency=false;arc=false";
  sidecar.module.package_identity = "test-package";
  sidecar.required_semantics = required_semantics;
  InterfaceDeclarationIdentity declaration;
  declaration.stable_id = "c:@V@sample";
  declaration.kind = "Var";
  declaration.qualified_name = "sample";
  declaration.export_root = "sample";
  declaration.canonical_type_identity = "int";
  sidecar.declarations.push_back(std::move(declaration));
  sidecar.module.semantic_fingerprint = ComputeInterfaceSourceSha256(
      BuildSemanticFingerprintInput(sidecar));
  artifact.identity = sidecar.module;
  sidecar.canonical_source_sha256 =
      ComputeInterfaceSourceSha256(artifact.source_bytes);
  std::vector<llvm::StringRef> encoder_semantics;
  for (const auto &semantic : required_semantics)
    encoder_semantics.push_back(semantic);
  auto encoded = EncodeSemanticInterfaceSidecar(sidecar, encoder_semantics);
  assert(static_cast<bool>(encoded));
  artifact.sidecar_json = std::move(*encoded);
  return artifact;
}

fs::path MakeTempDirectory() {
  auto directory = fs::temp_directory_path() /
      ("objc3i-io-test-" + std::to_string(
          std::chrono::steady_clock::now().time_since_epoch().count()));
  fs::create_directory(directory);
  return directory;
}

void WriteBytes(const fs::path &path, const std::string &bytes) {
  std::ofstream output(path, std::ios::binary);
  output.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
  assert(static_cast<bool>(output));
}

std::string ReadBytes(const fs::path &path) {
  std::ifstream input(path, std::ios::binary);
  return std::string(std::istreambuf_iterator<char>(input),
                     std::istreambuf_iterator<char>());
}

void TestPairReplacementAndMalformedArtifactRefusal() {
  const auto directory = MakeTempDirectory();
  const auto source_path = directory / "sample.objc3i";
  const auto sidecar_path = directory / "sample.objc3i.json";
  WriteBytes(source_path, "old source bytes\n");
  WriteBytes(sidecar_path, "old sidecar bytes\n");

  auto malformed = MakeArtifact();
  malformed.source_bytes += "tampered\n";
  auto rejected = WriteSemanticInterfaceArtifact(malformed, source_path.string());
  assert(!rejected.success);
  assert(ReadBytes(source_path) == "old source bytes\n");
  assert(ReadBytes(sidecar_path) == "old sidecar bytes\n");

  auto artifact = MakeArtifact();
  auto written = WriteSemanticInterfaceArtifact(artifact, source_path.string());
  assert(written.success);
  assert(ReadBytes(source_path) == artifact.source_bytes);
  assert(ReadBytes(sidecar_path) == artifact.sidecar_json);
  auto decoded = DecodeSemanticInterfaceSidecar(
      ReadBytes(sidecar_path), ReadBytes(source_path), {});
  assert(static_cast<bool>(decoded));
  fs::remove_all(directory);
}

void TestFilesystemFailurePreservesExistingSource() {
  const auto directory = MakeTempDirectory();
  const auto source_path = directory / "failure.objc3i";
  const auto sidecar_path = fs::path(source_path.string() + ".json");
  WriteBytes(source_path, "prior source\n");
  fs::create_directory(sidecar_path);

  auto artifact = MakeArtifact();
  auto failed = WriteSemanticInterfaceArtifact(artifact, source_path.string());
  assert(!failed.success);
  assert(ReadBytes(source_path) == "prior source\n");
  assert(fs::is_directory(sidecar_path));
  fs::remove_all(directory);
}

void TestKnownNativeSemanticCapabilitiesAndUnknownRefusal() {
  const auto directory = MakeTempDirectory();
  const auto source = directory / "native.objc3i";
  const auto sidecar = fs::path(source.string() + ".json");
  auto known = MakeArtifact({"objc3.canonical-function-effects.v1",
                             "objc3.native-carrier-types.v1"});
  auto accepted = WriteSemanticInterfaceArtifact(known, source.string());
  assert(accepted.success);
  assert(ReadBytes(source) == known.source_bytes);
  assert(ReadBytes(sidecar) == known.sidecar_json);

  auto unknown = MakeArtifact({"objc3.future-semantic.v1"});
  auto rejected = WriteSemanticInterfaceArtifact(unknown, source.string());
  assert(!rejected.success);
  assert(ReadBytes(source) == known.source_bytes);
  assert(ReadBytes(sidecar) == known.sidecar_json);
  fs::remove_all(directory);
}

void TestObjectAndInterfaceTripleRollbackAcrossDirectories() {
  const auto root = MakeTempDirectory();
  const auto interface_directory = root / "interfaces";
  const auto object_directory = root / "objects";
  fs::create_directory(interface_directory);
  fs::create_directory(object_directory);
  const auto source_path = interface_directory / "sample.objc3i";
  const auto sidecar_path = fs::path(source_path.string() + ".json");
  const auto object_path = object_directory / "sample.o";
  const auto staged_object = object_directory / "sample.o.compile-stage";
  WriteBytes(source_path, "prior interface source\n");
  WriteBytes(sidecar_path, "prior interface sidecar\n");
  WriteBytes(object_path, "prior object\n");
  WriteBytes(staged_object, "new object bytes\n");

  auto artifact = MakeArtifact();
  const auto bad_object_destination = object_directory / "is-a-directory.o";
  fs::create_directory(bad_object_destination);
  const auto staged_for_bad_target = object_directory / "bad-target.stage";
  WriteBytes(staged_for_bad_target, "new object bytes\n");
  auto bad_object_target = WriteSemanticInterfaceArtifactWithObject(
      artifact, source_path.string(), staged_for_bad_target.string(),
      bad_object_destination.string());
  assert(!bad_object_target.success);
  assert(ReadBytes(source_path) == "prior interface source\n");
  assert(ReadBytes(sidecar_path) == "prior interface sidecar\n");
  assert(ReadBytes(object_path) == "prior object\n");
  assert(ReadBytes(staged_for_bad_target) == "new object bytes\n");
  fs::remove(staged_for_bad_target);
  fs::remove(bad_object_destination);

  const auto pair_failure_dir = root / "pair-failure";
  fs::create_directory(pair_failure_dir);
  const auto bad_pair_source = pair_failure_dir / "sample.objc3i";
  const auto bad_pair_sidecar = fs::path(bad_pair_source.string() + ".json");
  const auto bad_pair_object = pair_failure_dir / "sample.o";
  const auto bad_pair_stage = pair_failure_dir / "sample.stage.o";
  WriteBytes(bad_pair_source, "pair-failure source\n");
  fs::create_directory(bad_pair_sidecar);
  WriteBytes(bad_pair_object, "pair-failure object\n");
  WriteBytes(bad_pair_stage, "staged pair-failure object\n");
  auto bad_pair_target = WriteSemanticInterfaceArtifactWithObject(
      artifact, bad_pair_source.string(), bad_pair_stage.string(),
      bad_pair_object.string());
  assert(!bad_pair_target.success);
  assert(ReadBytes(bad_pair_source) == "pair-failure source\n");
  assert(fs::is_directory(bad_pair_sidecar));
  assert(ReadBytes(bad_pair_object) == "pair-failure object\n");
  assert(ReadBytes(bad_pair_stage) == "staged pair-failure object\n");

  auto failed = testing::WriteSemanticInterfaceArtifactWithObjectFailingBeforePairPublish(
      artifact, source_path.string(), staged_object.string(), object_path.string(),
      1);
  assert(!failed.success);
  assert(ReadBytes(source_path) == "prior interface source\n");
  assert(ReadBytes(sidecar_path) == "prior interface sidecar\n");
  assert(ReadBytes(object_path) == "prior object\n");
  assert(ReadBytes(staged_object) == "new object bytes\n");

  auto written = WriteSemanticInterfaceArtifactWithObject(
      artifact, source_path.string(), staged_object.string(), object_path.string());
  assert(written.success);
  assert(ReadBytes(source_path) == artifact.source_bytes);
  assert(ReadBytes(sidecar_path) == artifact.sidecar_json);
  assert(ReadBytes(object_path) == "new object bytes\n");
  assert(!fs::exists(staged_object));
  fs::remove_all(root);
}

}  // namespace

void TestDependencyRecordProductTransaction() {
  const auto root = MakeTempDirectory();
  const auto record = root / "dependency.json";
  std::string failure;
  unsigned calls = 0;
  auto reject_products = [&] { ++calls; return false; };
  assert(!PublishCompilationProductsWithDependencyRecord(
      record.string(), "new record", reject_products, failure));
  assert(calls == 1 && !fs::exists(record));
  WriteBytes(record, "prior record\n");
  assert(!PublishCompilationProductsWithDependencyRecord(
      record.string(), "new record", reject_products, failure));
  assert(ReadBytes(record) == "prior record\n");
  try {
    const bool published = PublishCompilationProductsWithDependencyRecord(
        record.string(), "new record", []() -> bool {
          throw std::runtime_error("product publisher failed");
        }, failure);
    (void)published;
    assert(false && "publisher exception must propagate");
  } catch (const std::runtime_error &) {
    assert(ReadBytes(record) == "prior record\n");
  }
  assert(PublishCompilationProductsWithDependencyRecord(
      record.string(), "new record", [&] {
        assert(ReadBytes(record) == "new record\n");
        return true;
      }, failure));
  assert(ReadBytes(record) == "new record\n");
  const auto blocked = root / "directory.json";
  fs::create_directory(blocked);
  assert(!PublishCompilationProductsWithDependencyRecord(
      blocked.string(), "new record", [&] {
        assert(false && "invalid record destination must not publish products");
        return true;
      }, failure));
  assert(fs::is_directory(blocked));
  for (const auto &entry : fs::directory_iterator(root))
    assert(!entry.path().filename().string().starts_with(".objc3i-txn-"));
  fs::remove_all(root);
}

int main() {
  TestPairReplacementAndMalformedArtifactRefusal();
  TestFilesystemFailurePreservesExistingSource();
  TestKnownNativeSemanticCapabilitiesAndUnknownRefusal();
  TestObjectAndInterfaceTripleRollbackAcrossDirectories();
  TestDependencyRecordProductTransaction();
  return 0;
}
