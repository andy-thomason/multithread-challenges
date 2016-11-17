////////////////////////////////////////////////////////////////////////////////
//
// Challenge: build a method compute a matrix_stack in parallel
//
////////////////////////////////////////////////////////////////////////////////


#include <algorithm>
#include <vector>
#include <array>
#include <atomic>
#include <future>
#include <cmath>

class alignas(16) matrix {
  float v_[16];
public:
  matrix() {
    static const float identity[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
    std::copy(identity, identity+16, v_);
  }

  matrix(float v[16]) {
    std::copy(v, v+16, v_);
  }

  matrix(const matrix &src) {
    std::copy(src.v_, src.v_+16, v_);
  }

  // note: I haven't checked the maths! Excercise for the reader :o)
  matrix operator*(const matrix &rhs) const {
    matrix res;
    const float *a = v_;
    const float *b = rhs.v_;
    float *c = res.v_;

    for (int i = 0; i != 4; ++i) {
      // note: a good optimiser will convert this to SIMD.
      // swap this for row-major matrices.
      *c++ = a[0] * b[0] + a[1] * b[4] + a[2] * b[8 ] + a[3] * b[12];
      *c++ = a[0] * b[1] + a[1] * b[5] + a[2] * b[9 ] + a[3] * b[13];
      *c++ = a[0] * b[2] + a[1] * b[6] + a[2] * b[10] + a[3] * b[14];
      *c++ = a[0] * b[3] + a[1] * b[7] + a[2] * b[11] + a[3] * b[15];
      a += 4;
    }
    return res;
  }

  void rotateZ(float angle) {
    float ca = std::cos(angle * (3.14159f/180));
    float sa = std::cos(angle * (3.14159f/180));
    for (int i = 0; i != 4; ++i) {
      float c = ca * v_[i*4+0] + sa * v_[i*4+1];
      float s = -sa * v_[i*4+0] + ca * v_[i*4+1];
      v_[i*4+0] = c;
      v_[i*4+1] = s;
    }
  }

  void translate(float x, float y, float z) {
    for (int i = 0; i != 4; ++i) {
      v_[i*4+3] += x * v_[i*4+0] + y * v_[i*4+1] + z * v_[i*4+2];
    }
  }

  // compute the abs difference
  float distance(const matrix &rhs) const {
    float d = 0;
    for (size_t i = 0; i != 16; ++i) {
      d += std::abs(v_[i]-rhs.v_[i]);
    }
    return d;
  }
};

class matrix_stack {
  std::vector<matrix> local_matrices;
  std::vector<matrix> world_matrices;
  std::vector<int> parents;
public:
  matrix_stack() {
  }

  int add_matrix(matrix &local_matrix, int parent=-1) {
    int result = (int)local_matrices.size();
    local_matrices.emplace_back(local_matrix);
    world_matrices.emplace_back();
    parents.emplace_back(parent);
    return result;
  }

  void compute() {
    // note: this assumes that parents[i] < i
    for (size_t i = 0; i != parents.size(); ++i) {
      if (parents[i] >= 0) {
        world_matrices[i] = local_matrices[i] * world_matrices[parents[i]];
      } else {
        world_matrices[i] = local_matrices[i];
      }
    }
  }

  // Modify this function to compute the stack.
  // Build a for loop in the lambda.
  // Use work_item to decide which work item to operator on.
  // Question: how do you resolve the dependencies?
  // You may need to change the structure a little!
  void compute_parallel() {
    std::atomic<int> work_item{0};
    std::array<std::future<void>, 4> futures;
    for (auto &f : futures) {
      std::async(
        std::launch::async,
        []() {
          // your lambda here
        }
      );
    }
    // destructors should wait for futures
  }

  // test the result
  float distance(const matrix_stack &rhs) const {
    float d = 0;
    for (size_t i = 0; i != world_matrices.size(); ++i) {
      d += world_matrices[i].distance(rhs.world_matrices[i]);
    }
    return d;
  }
};
