#!/bin/bash

find engine/public -iname *.[ch]pp | xargs clang-format -i
find engine/private -iname *.[ch]xx | xargs clang-format -i
