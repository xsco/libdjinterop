Contributing to libdjinterop
============================

If you are interested in contributing to `libdjinterop`, thank you!

The easiest way to contribute is to look at the
[issues list](https://github.com/xsco/libdjinterop/issues) and see if there is
anything that you might like to try.

Although `libdjinterop` is a C++ library, there are usually both coding and
non-coding tasks available.  All contributions are hugely appreciated!

C++ Style Guide
---------------

C++ coding conventions in `libdjinterop` broadly follow the
[Core C++ Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)
from ISO C++.

Furthermore, there is a [.clang-format](.clang-format) file in the repository,
and all code contributions should conform to this.

Perhaps one day there will be more opinionated formatting/linting, or indeed
a pre-commit hook to enforce it.

Human-readable identifiers in code should be written in American English.
Documentation (including docstrings) is written in (British) English, because
the original author of the project who wrote most of it is English.  It is best
to avoid using identifiers whose spelling differs between different dialects of
English, if there are no other significant influencing factors affecting the
choice of identifier.

Code Comments
-------------

Code comments should explain why, not what.  If the code is easily understood,
a comment is likely not needed.

Docstrings in the public API must not mention implementation details which do
not matter to the caller.

Docstrings and comments should not mention the way things "used to be", only
current or planned future behaviour.

Planned future behaviour may be indicated by adding a "TODO" comment with the
following format:

```c++
// TODO: [#1234] Add foo support to the widget factory.
```

The number in square brackets, preceded by a `#` symbol, must be an identifier
for the planned future change on the project's
[issue tracker](https://github.com/xsco/libdjinterop/issues).  The rationale
for requiring a formal link to an issue is to ensure there is a place to
provide additional information for a planned change, for others to pick up and
consider, so that it does not solely exist in the prior author's head.

Note that there may be legacy "TODO" comments already in the codebase that do
not conform to the above format: do not copy these, and do not add any more.
They may be removed or modified to conform to the above one day.

Unit Testing
------------

High unit test coverage is _strongly encouraged_ for all C++ code contributions.
It may become a mandatory requirement one day.

It is also _strongly encouraged_ that unit tests follow the "arrange/act/assert"
pattern, specifically:

* Only one arrange/act/assert section in a unit test.
* The "act" section should be a _single_ invocation of the behaviour under test.
* Delineate the arrange/act/assert sections in a unit test with the following
  formatting conventions:

```c++
// Arrange
//  (arranging statements go here)

// Act
//  (single statement representing behaviour-under-test goes here)

// Assert
//  (assertions go here)
```

If more than one section is effected with a single statement, they can be
combined, for example:

```c++
// Arrange
//  (arranging statements go here)

// Act/Assert
//  (single statement with assertion goes here)
```

Combining act/assert is typical when asserting that the behaviour under test
throws an exception.  Combining arrange/act is used when there is no arrangement
needed for a test.

Any proposed deviation from the arrange/act/assert pattern is expected to
require a strong supporting argument to do so.

External Dependencies
---------------------

Introducing any additional external dependencies into `libdjinterop` is a big
decision that requires proper consideration.  The aim is for the library to be
built and compiled with minimal additional requirements, in order to make
wider adoption as easy as possible.

For an external dependency to be considered, ideally it should be readily
available on all stable versions of each target operating system, as well as
be a stable and mature library.

For dependencies that do not meet these criteria, another option is to 'vendor'
the dependencies into the codebase under the [ext/](ext) directory hierarchy.
However, packaging for package managers often requires projects to be built in
a 'de-vendored' manner, so caution is advised if relying on this approach.

OS Compatibility
----------------

The library is expected to compile on:

* Windows - all supported versions
* Linux:
  * Ubuntu - all LTS versions
  * Red Hat and friends - all supported versions
  * Arch Linux
* macOS - all recent versions

The biggest user of this library is undoubtedly the [Mixxx](https://mixxx.org)
open source DJ application.  As such, it is a very sensible idea to ensure that
the library always compiles on the operating systems that Mixxx targets too.

AI-Assisted Contributions
-------------------------

`libdjinterop` takes a mostly-permissive view on AI-assisted code contributions
to the project:

| Type | Stance |
|------|--------|
| Contributor | The primary author (contributor) of any contribution _must_ be a human user. |
| Code | Permitted - contributors _may_ use AI to write code they submit. |
| Documentation | Permitted - contributors _may_ use AI to write documentation, such as docstrings and markdown files. |
| Disclosure | Encouraged - contributors _should_ disclose when an AI tool has been used for a contribution. |
| Disclosure (co-author commit trailers) | Prohibited - "assisted by" or "co-author" trailers (etc.) referencing an AI _must not_ be used on any commit. |
| Attestation | The human author(s) of any contribution _must_ be able to collectively understand every line of what they submit. |

Regarding PRs and code review:

* Reviewers _must_ be human users.
* AI _may_ be used to assist human reviewers with code reviews.  The (human)
  reviewer remains ultimately accountable for their review.
* A reviewer _must_ understand every line of what they review in order to
  provide a positive approval, even if review assistance has been provided by
  an AI.
* Whether AI assistance has been used or not to generate the contribution under
  review _should not_ factor into review: the contribution must be assessed to
  the same quality standards either way.
