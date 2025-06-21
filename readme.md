# liblogging

liblogging is a single module to manage logs for SERiF and related projects.

This has been broken out of the main serif project to allow for more modularity

## Building
In order to build liblogging you need `meson>=1.5.0`. This can be installed with `pip`

```bash
pip install "meson>=1.5.0"
```

Then from the root liblogging directory it is as simple as

```bash
meson setup build --buildtype=release
meson compile -C build
meson test -C build
```

this will auto generate a pkg-config file for you so that linking other libraries to liblogging is easy.

