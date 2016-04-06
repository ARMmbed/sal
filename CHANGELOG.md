# Change Log
All notable changes to this project will be documented in this file.
This project adheres to [Semantic Versioning](http://semver.org/).

##[Unreleased]

## [v1.3.1]
### Added
* Target dependency on sal-stack-atmel

## [v1.1.6]
### Added
* Configuration file to support test coverage generation

## [v1.1.5]
### Improved
* Update tests to use mbed-greentea test infrastructure

## [v1.1.4]
### Updated
* Incremented mesh-api dependency major version

## [v1.1.2]
### Added
* A new function pointer: ```socket_accept_v2_ptr```
* A new ```socket_accept``` API, which selects between ```socket_accept_ptr``` and ```socket_accept_v2_ptr```.

## [v1.1.1]
### Improved
* Changed ```inet_pton``` to manipulate ```struct socket_addr``` directly

[Unreleased]: https://github.com/ARMmbed/sal/compare/v1.3.1...HEAD
[v1.1.7]: https://github.com/ARMmbed/sal/compare/v1.1.6...v1.3.1
[v1.1.6]: https://github.com/ARMmbed/sal/compare/v1.1.5...v1.1.6
[v1.1.5]: https://github.com/ARMmbed/sal/compare/v1.1.4...v1.1.5
[v1.1.4]: https://github.com/ARMmbed/sal/compare/v1.1.3...v1.1.4
[v1.1.2]: https://github.com/ARMmbed/sal/compare/v1.1.1...v1.1.2
[v1.1.1]: https://github.com/ARMmbed/sal/compare/v1.1.0...v1.1.1
