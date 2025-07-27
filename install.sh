#!/bin/bash
set -e

# Install binaries
install -Dm755 PowerManagerDbus /usr/bin/PowerManagerDbus
install -Dm755 PowerManager /usr/bin/PowerManager

# D-Bus service
install -Dm644 data/com.mkps.powermanager.service /usr/share/dbus-1/system-services/com.mkps.powermanager.service

# polkit rules
install -Dm644 data/com.mkps.powermanager.conf /usr/share/dbus-1/system.d/com.mkps.powermanager.conf

# systemd unit
install -Dm644 data/powermanager.service /etc/systemd/system/powermanager.service

# Reload and start
systemctl daemon-reload
systemctl enable --now powermanager.service

echo "Installation complete."
