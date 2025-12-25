#pragma once

namespace cpp_example {

  // Add the input value as an argument to the constructor.
  // Implement a method to do the factorial calculations.
  class Factorial {
    public:
      Factorial(int n = 3);
      int compute();
    
    private:
      int n_;

  };

}
