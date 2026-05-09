int objc3_runtime_replay_registered_images_for_testing(void) {
  RuntimeState &state = ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  return ReplayRegisteredImagesForTestingUnlocked(state, RegisterImageUnlocked);
}

// optional/key-path runtime-helper freeze anchor: the live Part 3
// runtime helper boundary intentionally remains minimal here. Optional sends
// execute through objc3_runtime_lookup_selector plus checked runtime dispatch
// status handling, while validated typed key-path sites currently contribute
// retained descriptor handles and descriptor sections without claiming full
// runtime evaluation yet.

// error-runtime/bridge-helper anchor: the current runnable Part 6
// slice now routes thrown-error storage, bridge normalization, and catch-kind
// matching through this same private bootstrap-internal helper cluster instead
// of treating raw function-local slots as the runtime boundary.
// live catch/bridge/runtime integration anchor: linked Part 6
// object-code probes now execute through these helpers and consume the
// snapshot state below to prove live store/load/bridge/catch traffic.
