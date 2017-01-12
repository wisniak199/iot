# Build
You need gcc/g++ for arm.

On Debian-based Linux distro:
```bash
sudo add-apt-repository ppa:team-gcc-arm-embedded/ppa
sudo apt update
sudo apt install gcc-arm-embedded
```

Then:
```bash
cd sensor # or wifi
make # I assume you have make installed.
```
