#! /usr/bin/python3

import sys

if __name__ == '__main__':
    # Print each argument to the terminal
    print('Argument list:')
    for arg in sys.argv:
        print(arg)

    # Make sure the user passes two values to be multiplied
    if len(sys.argv) != 3:
        print('Incorrect number of arguments')
        sys.exit(1)

    # Parse input values, perform multiplication, and print the result

    a = int(sys.argv[1])
    b = int(sys.argv[2])

    c = a * b
    print(f'Result: {c}')
