#!/bin/bash
# Disable all GPU capabilities
export DISABLE_WSL2_GPU=1
export WSL2_D3D=0

# CPU affinity and priority
taskset -c 0 nice -n 5 ./CFGParser "$@"

# Fallback if still crashing
if [ $? -eq 139 ]; then
    echo "Falling back to null renderer"
    export QT_QPA_PLATFORM=minimal
    ./CFGParser "$@" --no-graphics
fi