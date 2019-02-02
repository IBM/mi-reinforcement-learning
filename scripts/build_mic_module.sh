#!/bin/bash
# Copyright (C) tkornuta, IBM Corporation 2015-2019
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Assumes that:
# - script is executed in ROOT_DIR (directory containing CMakeList).
# - starts and ends in ROOT_DIR
# TARGET_DIR (directory where files will be installed) is passed as first argument.

# Stop the script on first error.
set -e

# Script configures cmake and installs the module to dir passed as first argument.
# Out-of-source build.

# Read input arguments.
if [ $# -eq 0 ]; then
    TARGET_DIR="mic"
else
    TARGET_DIR=$1
fi
echo "Installing module to ${TARGET_DIR}"

# Prepare installation dir.
rm -Rf build # Always fresh-and-clean!
mkdir build
cd build

# Overwrite compiler!
if [[ "${COMPILER}" != "" ]]; then export CXX=${COMPILER}; fi

# Configure cmake.
cmake .. -DCMAKE_INSTALL_PREFIX=../${TARGET_DIR}
# Build and install.
make -j4 install VERBOSE=1
cd ..
