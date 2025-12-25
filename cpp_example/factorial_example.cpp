#include "Factorial.hpp"
#include <cstdio>

int main(int argc, char** argv) {
  // Instantiate instances of the Factorial class, call their calculation methods, print out the results

  cpp_example::Factorial factorial_3;
  cpp_example::Factorial factorial_6(6);

  int factorial3_result = factorial_3.compute();
  int factorial6_result = factorial_6.compute();

  printf("Factorial 3 result: %d\n", factorial3_result);
  printf("Factorial 6 result: %d\n", factorial6_result);

  return 0;
}
