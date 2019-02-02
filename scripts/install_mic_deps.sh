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
# - ROOT_DIR is the root of the project. 
# - script is executed in ROOT_DIR (starts and ends in that dir).
# - ROOT_DIR/build/ - build directory of a given project.
# - ROOT_DIR/deps/ - directory where all MIC modules/dependencies will be downloaded and build. 
# - ROOT_DIR/mic/ - target directory (where all MIC modules will be installed).

# Stop the script on first error.
set -e

# List of MI modules with versions.
modules=( "mi-toolchain" "mi-algorithms" "mi-visualization" "mi-neural-nets" )
versions=( "v1.1.1" "v1.2.2" "v1.2.2" "v1.2.2" )

# Prepare dir for dependencies.
rm -Rf deps # Always fresh-and-clean!
mkdir deps

# Prepare target dir.
if [ $# -eq 0 ]; then
    TARGET_DIR="mic"
else
    TARGET_DIR=$1
fi
echo "Installing dependencies to ${TARGET_DIR}/"
if [ ! -d ${TARGET_DIR} ]; then
    mkdir ${TARGET_DIR}
fi

# Iterate over list of modules.
cd deps
for ((i=0;i<${#modules[@]};++i)); do
    module=${modules[i]}
    version=${versions[i]}
    # Clone, configure and install module.
    ./../scripts/download_release.sh IBM ${module} ${version}

    # Build module.
    cd ${module}
    ./../../scripts/build_mic_module.sh ../../${TARGET_DIR}
    echo "${module} installed"
    cd ..
done
cd ..
