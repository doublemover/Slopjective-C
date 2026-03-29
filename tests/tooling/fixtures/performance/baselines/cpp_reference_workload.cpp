#include <cstdlib>
#include <string>

struct BenchWidget {
  std::string name;
  int count;

  int score() const {
    int total = count;
    for (unsigned char ch : name) {
      total += static_cast<int>(ch);
    }
    return total;
  }
};

int main() {
  BenchWidget widget{"aurora", 7};
  return widget.score() % 31;
}
