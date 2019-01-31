
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
# - Download source code from github to ROOT_DIR/${repo} directory.

# Stop the script on first error.
set -e

# Script downloads given release from the github user/repository.
# Read input arguments.
user=$1
repo=$2
version=$3  
echo "Downloading the ${version} release from: ${user}/${repo}. Please wait..."

# Find the archive. (NOT WORKING IN LGTM! :])
#version=$( curl --silent "https://api.github.com/repos/${user}/${repo}/releases/latest" | grep '"tag_name":' | sed -E 's/.*"([^"]+)".*/\1/' )
#echo "Found version: ${version}"

# Download the archive.
curl -sOL "https://github.com/${user}/${repo}/archive/${version}.tar.gz"

# Unpack the archive.
echo "Unpacking the archive"
tar xzvf ${version}.tar.gz
# Rename directory.
mv ${repo}* ${repo}

# Cleanup.
rm ${version}.tar.gz
echo "Done"

