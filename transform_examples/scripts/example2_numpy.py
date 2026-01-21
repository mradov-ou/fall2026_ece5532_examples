#! /usr/bin/env python3
import numpy as np
import math


if __name__ == "__main__":

    translation = np.array([14, 14, 0])
    # psi = 120 * np.pi / 180
    psi = math.radians(120)
    rotation_matrix = np.array([[np.cos(psi), -np.sin(psi), 0],
                                [np.sin(psi), np.cos(psi), 0],
                                [0, 0, 1]])
    global_coordinates = np.array([5, 27, 0])

    vehicle_coordinates = rotation_matrix.transpose() @ global_coordinates - rotation_matrix.transpose() @ translation

    print(f'Global position in vehicle coordinates: {vehicle_coordinates}')
