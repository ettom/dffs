# About
dffs is a minimal FUSE-based pseudo-filesystem written in C, designed for educational and testing purposes. It exposes
two special files, `/used` and `/avail`, that can be read from and written to in order to adjust the "used" and
"available" space in the filesystem.

I wrote this mostly so I can have [homepage](https://github.com/benphelps/homepage) display the space usage for
filesystems that it doesn't have direct access to. I have it set up so a systemd timer periodically writes the usage of
some other filesystem to the special files. I then make the dffs mountpoints available to the Docker container via a
volume. It's set up roughly like this:

```sh
fs="z0"
mountpoint="/var/lib/homepage/dffs/$fs"
dffs -o fsname="dffs-$fs" "$mountpoint"
while :; do
    zfs get -o value -Hp used "$fs" > "$mountpoint/used"
    zfs get -o value -Hp avail "$fs" > "$mountpoint/avail"
    sleep 60
done
```

# Build
```sh
make
```

or

```nix
nix-build -E 'with import <nixpkgs> {}; callPackage ./. {}'
```
