
#include "matrix_stack.hpp"

int main() {
  matrix_stack a;

  // build a matrix stack
  for (int i = 0; i != 100000; ++i) {
    matrix m;
    m.translate((float)i, 0, 0);

    // Add a parent object with 5 children.
    // modify this to test deeper stacks.
    int parent = a.add_matrix(m, 0);
    for (int j = 0; j != 5; ++j) {
      m.rotateZ(30);
      m.translate(1, 0, 0);
      a.add_matrix(m, parent);
    }
  }

  // copy to b
  matrix_stack b = a;

  printf("compute a\n");
  {
    auto start = std::chrono::high_resolution_clock::now();
    a.compute();
    auto end = std::chrono::high_resolution_clock::now();
    printf("%10dns\n", (int)std::chrono::nanoseconds(end - start).count());
  }

  printf("compute b\n");
  {
    auto start = std::chrono::high_resolution_clock::now();
    b.compute();
    auto end = std::chrono::high_resolution_clock::now();
    printf("%10dns\n", (int)std::chrono::nanoseconds(end - start).count());
  }

  printf("difference=%f\n", a.distance(b));
}
