Overview
========

This directory contains small example applications that illustrate the use
of `libdjinterop`.

This application can be minimally compiled in isolation with an invocation
similar to the below (adjust for your favourite compiler as appropriate):

```shell
g++ -std=c++17 `pkg-config --cflags djinterop` engine_prime.cpp `pkg-config --libs djinterop`
```