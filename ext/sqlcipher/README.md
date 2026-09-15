This directory contains the generated SQLCipher amalgamation vendored into libdjinterop.

## Building the Amalgamation

The amalgamation is generated from upstream SQLCipher and requires a C compiler, `make`, and OpenSSL headers.

1. Clone the repository at v4.16.0:

    ```shell
    $ git clone --branch v4.16.0 https://github.com/sqlcipher/sqlcipher
    $ cd sqlcipher
    ```

2. Configure with the options SQLCipher requires:

    ```shell
    $ ./configure --with-tempstore=yes \
        CFLAGS="-DSQLITE_HAS_CODEC \
        -DSQLITE_EXTRA_INIT=sqlcipher_extra_init \
        -DSQLITE_EXTRA_SHUTDOWN=sqlcipher_extra_shutdown" \
        LDFLAGS="-lcrypto"
    ```

3. Build the amalgamation:

    ```shell
    $ make sqlite3.c
    ```

4. Copy the generated `sqlite3.c`, `sqlite3.h`, and `sqlite3ext.h` into this directory.
