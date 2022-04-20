#!/bin/sh
set -eou pipefail
set -x
shfmt -w *.sh
uncrustify -c /etc/uncrustify.cfg --replace tests/*.c include/*.c include/*.h src/*.c
find . -name "*.unc-backup*" -type f | xargs -I % unlink %
test -f clib.json && js-beautify -r clib.json
