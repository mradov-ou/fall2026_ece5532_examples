#include <cstdio>
#include <cstdlib>

int main(int argc, char** argv) {

  // Print each argument to the terminal
  for (int i = 0; i < argc; i++) {
    printf("%d: %s\n", i, argv[i]);
  }

  // Make sure the user passes two values to be multiplied
  if (argc != 3) {
    printf("Invalid number of arguments\n");
    return 1;
  }

  // Parse input values, perform multiplication, and print the result
  int a = std::atoi(argv[1]);
  int b = std::atoi(argv[2]);

  int c = a * b;
  printf("Result: %d\n", c);

  return 0;
}