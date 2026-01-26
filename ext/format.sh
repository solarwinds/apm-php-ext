#!/bin/sh
find . -type f \( -name '*.c' -o -name '*.h' -o -name '*.cpp' \) -exec clang-format-18 -i {} +
