#!/bin/bash
# this must be run from its containing dir to have paths work out

set -x


# the template to use for the 5 C binary makefiles
# note I threw a hell of a canary in there...
template="../script/cmake-template.txt"

# ...i told you. a hell of a canary
canary="{CANARY}"

FLAGS="0 1 2 3 fast"

# I hate how stack dir changes don't have quiet flags.
# truly.
pushd ../makefiles/ > /dev/null

for s in $FLAGS ; do
  fname="Makefile.opt${s}"

  # my template has some comments for context
  # output all lines of template aside from comments
  # and redirect into new makefile
  grep -v '^#' "${template}" > "${fname}"
  {
    printf "#\n# Makefile for 1/5 C binaries: optimization: ${s}\n#\n\n"
    cat "${fname}"
  }   > "${fname}.tmp" && mv "${fname}.tmp" "${fname}"

  # finally, replace the canary
  sed -i "s/${canary}/${s}/g" "${fname}"
done

popd > /dev/null

