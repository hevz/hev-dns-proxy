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
nkd-build
```

## How to Run

```bash
adb root
adb push hev-dns-proxy /data/hev-dns-proxy
adb shell
/data/hev-dns-proxy -p /dev/socket/hev-dns-proxy -a 8.8.8.8 -b 8.8.4.4 &
cd /dev/socket
chown root:inet hev-dns-proxy
chcon u:object_r:dnsproxyd_socket:s0 hev-dns-proxy
mv dnsproxyd hev-dns-proxy; mv dnsproxyd.netd dnsproxyd
```

## Authors
* **The Android Open Source Project** - https://www.android.com
* **Kevin Cernekee** - https://github.com/cernekee
* **Heiher** - https://hev.cc

## License
Apache
