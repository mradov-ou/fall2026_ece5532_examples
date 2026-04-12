#pragma once
#include <eigen3/Eigen/Dense>

static constexpr double A = 6378137.0;
static constexpr double E2 = 6.6943799014e-3;
static constexpr double DEG2RAD = (M_PI / 180.0);

Eigen::Vector3d llh_to_ecef(const double& lat_deg, const double& lon_deg, const double& alt) {
  double lat = DEG2RAD * lat_deg;
  double lon = DEG2RAD * lon_deg;
  Eigen::Vector3d output;
  double n_val = A / sqrt(1.0 - E2 * sin(lat) * sin(lat));
  output.x() = (n_val + alt) * cos(lat) * cos(lon);
  output.y() = (n_val + alt) * cos(lat) * sin(lon);
  output.z() = (n_val * (1 - E2) + alt) * sin(lat);
  return output;
}

Eigen::Matrix3d llh_to_rot_mat(const double& lat_deg, const double& lon_deg) {
  double lat = DEG2RAD * lat_deg;
  double lon = DEG2RAD * lon_deg;
  Eigen::Matrix3d output;
  output << -sin(lon),             cos(lon),                0.0,
            -sin(lat) * cos(lon), -sin(lat) * sin(lon), cos(lat),
             cos(lat) * cos(lon),  cos(lat) * sin(lon), sin(lat);
  return output;
}