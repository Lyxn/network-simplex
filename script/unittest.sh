#!/bin/bash
set -e

for tf in test_*
do
    echo "${tf}"
    ./"${tf}"
    echo
done
