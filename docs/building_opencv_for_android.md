
### Install the NDK
- Use Android Studio to install the NDK : `Settings / System Settings / Android SDK / SDK Tools / NDK`

- Be sure that you have $NDK_ROOT set : `NDK_ROOT=~/Library/Android/sdk/ndk/25.1.8937393`

### OF apothecary
- get apothecary by getting the submodule :  `git submodule update --init scripts/apothecary`

###  Some changes 
- In NDK 23 and later, the name of the file that configure the android cmake toolchain has changed. This leads to changes in `scripts/apothecary/android_configure.sh` : 
  - change `export ANDROID_CMAKE_TOOLCHAIN=${NDK_ROOT}/build/cmake/android.toolchain.cmake`
  - to `export ANDROID_CMAKE_TOOLCHAIN=${NDK_ROOT}/build/cmake/android-legacy.toolchain.cmake`

### Launch commands to build the opencv libs

- first : in `android-legacy.toolchain.cmake` remove th `-g` Cflag to avoid huge files

- then goto `scripts/apothecary` and :
  - for armv8 (64bits) `launch  ./apothecary -v -t android  -a arm64 update opencv`
  - for armv87 (32bits) `launch  ./apothecary -v -t android  -a armv7 update opencv`

- these commands call the `opencv` formula which itself calls `android_configure.sh`

- this builds the libs using cmake and copy them to  `scripts/apothecary/opencv` with the include files

