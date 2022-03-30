# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/VSARM/sdk/pico/pico-sdk/tools/pioasm"
  "C:/Users/vivek/Desktop/Code/Multilslope IIA/build/pioasm"
  "C:/Users/vivek/Desktop/Code/Multilslope IIA/build/pioasm"
  "C:/Users/vivek/Desktop/Code/Multilslope IIA/build/pioasm/tmp"
  "C:/Users/vivek/Desktop/Code/Multilslope IIA/build/pioasm/src/PioasmBuild-stamp"
  "C:/Users/vivek/Desktop/Code/Multilslope IIA/build/pioasm/src"
  "C:/Users/vivek/Desktop/Code/Multilslope IIA/build/pioasm/src/PioasmBuild-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/vivek/Desktop/Code/Multilslope IIA/build/pioasm/src/PioasmBuild-stamp/${subDir}")
endforeach()
