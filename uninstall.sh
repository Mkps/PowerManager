#!/bin/bash
set -e

echo "Stopping and disabling service..."
sudo systemctl disable --now powermanager.service || true

echo "Removing installed files..."
sudo rm -f /usr/bin/PowerManager
sudo rm -f /usr/bin/PowerManagerDbus
sudo rm -f /usr/share/dbus-1/system-services/com.mkps.powermanager.service
sudo rm -f /usr/share/dbus-1/system.d/com.mkps.powermanager.conf
sudo rm -f /etc/systemd/system/powermanager.service

echo "Reloading systemd..."
sudo systemctl daemon-reload

echo "Uninstallation complete."
