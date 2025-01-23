Debian Packages for libdjinterop
================================

This repository is debianised, and it is expected that git-buildpackage is used to build Debian packages.

Build package
-------------

Pbuilder is recommended for this.

```
$ DIST=focal gbp buildpackage
```

Build source package
--------------------

No pbuilder/chroot is needed for this.

```
$ gbp buildpackage -S
```

Incorporate new changes
-----------------------

```
$ git merge <new_tag>
$ gbp dch
$ git commit
```

