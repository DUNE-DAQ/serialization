#!/usr/bin/env bash

# PAR 2021-01-21: Copied and modified from appfwk

# Really ugly and temporary glue to run moo code generator.
# This will simplify and move into CMake.

mydir=$(dirname $(realpath $BASH_SOURCE))
srcdir=$(dirname $mydir)

# Wrap up the render command.  This bakes in a mapping to file name
# which would be better somehow captured by the schema itself.
render () {
    local name="$1" ;    shift
    local What="$1" ;    shift
    local is_test="$1" ; shift
    
    local name_lc=$( echo "$name" | tr '[:upper:]' '[:lower:]' )
    local outdir="${1:-$srcdir/include/serialization/${name_lc}}"
    if [ "${is_test}" = "TEST" ]; then
        outdir="${1:-$srcdir/test/src/serialization/${name_lc}}"
    fi
    local what="$(echo $What | tr '[:upper:]' '[:lower:]')"
    local tmpl="o${what}.hpp.j2"
    local outhpp="$outdir/${What}.hpp"
    mkdir -p $outdir
    set -x
    moo -g '/lang:ocpp.jsonnet' \
        -M $mydir -T $mydir \
        -A path="dunedaq.serialization.${name_lc}" \
        -A ctxpath="dunedaq" \
        -A os="serialization-${name}-schema.jsonnet" \
        render omodel.jsonnet $tmpl \
        > $outhpp || exit -1
    set +x
    echo $outhpp
}


render NetworkObjectSender Structs
render NetworkObjectSender Nljs

render NetworkObjectReceiver Structs
render NetworkObjectReceiver Nljs

render fsd Structs TEST
render fsd Nljs    TEST
render fsd Msgp    TEST
