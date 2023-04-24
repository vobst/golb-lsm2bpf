# LSMs Jmp'ing on BPF Trampolines

This repository contains code snippets complementing my
[blog post]()
on Kernel Runtime Security Instrumentation (KRSI).

## How to use this Repository

To build the toy LSM used in the blog post, get
[libbpf-bootstrap](https://github.com/libbpf/libbpf-bootstrap)
and place the files `mini_lsm.*` under `examples/c`. Then
add `mini_lsm` to the APPS in the Makefile. Load the programs using
```
$ make mini_lsm
$ sudo ./mini_lsm
```

To test the execution of programs or scripts stored in memfds
build the `memfd_exec` program.
```
$ make
$ ./memfd_exec
Usage: ./memfd_exec <script|elf>
```
You shouldn't be surprised that it deletes itself after being
executed :)

The Volatility plugin can be found in the repository of the
[BPFVol3](https://github.com/vobst/BPFVol3)
project. To test the plugin we provide a
[memory image](https://owncloud.fraunhofer.de/index.php/s/sAXBW6HycFAqbio)
and the symbols file (18c2747e19df38432fbfbdf4ed36921c.isf.json).
