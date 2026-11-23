@import example.LogSummary;

int main(int argc, char **argv) {
  return @example.LogSummary.summarize_log(argc > 1 ? argv[1] : "sample.log");
}
