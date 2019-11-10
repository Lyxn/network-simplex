#!/bin/bash
set -e
set -x

for tf in test_*
do
    ./"${tf}"
done
