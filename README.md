# HevDnsProxy

A standalone DNS proxy for Android 4.3+.

**Features**
* Replace dns proxy of netd.
* Use user specific dns servers.
* Apply any iptables rules that were set up to allow/deny access to specific UIDs (apps).
* JNI, user specific policy.

## How to Build

```bash
mkdir hev-dns-proxy
cd hev-dns-proxy
git clone git://github.com/heiher/hev-dns-proxy jni
ndk-build
```

## How to Run

```bash
# Switch to root user
adb root
# Push executable to device
adb push hev-dns-proxy /data/hev-dns-proxy
adb shell
# Run new dns proxy in background
/data/hev-dns-proxy -p /dev/socket/hev-dns-proxy -a 8.8.8.8 -b 8.8.4.4 &
cd /dev/socket
# Change ownership and SELinux context for socket file of new dns proxy
chown root:inet hev-dns-proxy
chcon u:object_r:dnsproxyd_socket:s0 hev-dns-proxy
# Backup socket file of netd
mv dnsproxyd dnsproxyd.netd
# Enable new dns proxy
mv hev-dns-proxy dnsproxyd
```

## Authors
* **The Android Open Source Project** - https://www.android.com
* **Kevin Cernekee** - https://github.com/cernekee
* **Heiher** - https://hev.cc

## License
Apache
