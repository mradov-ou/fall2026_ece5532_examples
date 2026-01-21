#! /usr/bin/env python3
import numpy as np
import math


if __name__ == "__main__":

    translation = np.array([10, 3, 0])
    # psi = 120 * np.pi / 180
    psi = math.radians(120)
    rotation_matrix = np.array([[np.cos(psi), -np.sin(psi), 0],
                                [np.sin(psi), np.cos(psi), 0],
                                [0, 0, 1]])
    vehicle_coordinates = np.array([25, 0, 0])

    global_coordinates = rotation_matrix @ vehicle_coordinates + translation

    print(f'Target position in global coordinates: {global_coordinates}')

