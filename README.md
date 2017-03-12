DynCount -- dynamic instruction counter
===

Created by Anthony Carno (acarno at vt dot edu).

This Pin tool counts the number instructions dynamically executed, broken
down by instruction type (specifically, the instruction mnemonic, NOT the
opcode).

It was built against Pin for Linux, rev 81205 (see [here](https://software.intel.com/en-us/articles/pintool-downloads)).

To use:

* Download and extract Pin.
* Clone this repository to a location *outside of* the Pin directory.
* Inside of this repo's folder, run `make PIN_ROOT=<directory of Pin>`. This
  will create a folder (probably obj-intel64) containing `dyncount.so`.
* Run Pin using the command `<directory of Pin>/pin -t <directory of repo>/obj-intel64/dyncount.so -o <name of output> -- <program> <program args>`
