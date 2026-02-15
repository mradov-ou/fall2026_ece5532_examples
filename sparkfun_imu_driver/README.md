# Install udev Rules

Copy the udev rules file to the system directory:
   ```bash
sudo cp udev/99-sparkfun-imu.rules /etc/udev/rules.d/
   ```

Reload udev rules:
   ```bash
   sudo udevadm control --reload-rules && sudo udevadm trigger
   ```
