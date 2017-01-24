# Presentation
You can find project presentation
[here](https://docs.google.com/presentation/d/1hUlK1RsZiFjX9cnxC7wpaolrl5mcfRCkvJgFPqZJQYg/pub?start=false&loop=false&delayms=3000).

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
