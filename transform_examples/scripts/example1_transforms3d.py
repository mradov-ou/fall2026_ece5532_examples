#! /usr/bin/env python3

import numpy as np
from transforms3d.euler import euler2quat
from transforms3d.quaternions import quat2mat

if __name__ == "__main__":

    translation = np.array([10, 3, 0])

    psi = 120 * np.pi / 180
    q = euler2quat(0, 0, psi)
    rotation_matrix = quat2mat(q)

    vehicle_coordinates = np.array([25, 0, 0])

    global_coordinates = rotation_matrix @ vehicle_coordinates + translation

    print(f'Target position in global coordinates: {global_coordinates}')
