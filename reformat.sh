#!/usr/bin/env bash

filepaths=$(find . -type f \( -name "*.cpp" -or -name "*.h" \))
for filepath in $filepaths; do
    echo "Formatting ${filepath}..."
    clang-format-5.0 -style=file -i "${filepath}"
done
