# Change Log
All notable changes to this project will be documented in this file.
This project adheres to [Semantic Versioning](http://semver.org/).

##[Unreleased]

## [v1.1.2]
### Added

* A new function pointer: ```socket_accept_v2_ptr```
* A new ```socket_accept``` API, which selects between ```socket_accept_ptr``` and ```socket_accept_v2_ptr```.

## [v1.1.1]
### Improved

* Changed ```inet_pton``` to manipulate ```struct socket_addr``` directly
