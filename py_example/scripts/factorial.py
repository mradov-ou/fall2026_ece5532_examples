#! /usr/bin/python3

# Add the input value as an argument to the constructor.
# Implement a method to do the factorial calculations.
class Factorial:
    def __init__(self, n = 3):
        self.n_ = n
    
    def compute(self):
        result = 1
        for i in range(self.n_):
            result = result * (self.n_ - i)

        return result


if __name__ == '__main__':
    # Instantiate instances of the Factorial class, call their calculation methods, print out the results
    
    factorial_3 = Factorial()
    factorial_6 = Factorial(6)

    fact_val_3 = factorial_3.compute()
    fact_val_6 = factorial_6.compute()

    print(f'Factorial 3 value: {fact_val_3}')
    print(f'Factorial 6 value: {fact_val_6}')
