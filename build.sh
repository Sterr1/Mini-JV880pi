#!/bin/bash

set -e 
set -x

if [ -z "${RPI}" ] ; then
  echo "\$RPI missing, exiting"
  exit 1
fi

SDHOST_OPTION=""
if [ "${RPI}" == "3" ]; then
    SDHOST_OPTION="-o USE_SDHOST"
fi

if [ "${RPI}" -lt "3" ]; then
  echo "Raspberry Pi ${RPI} is not supported (need >=3)"
  exit 1
fi

export TOOLCHAIN_PREFIX="aarch64-none-elf-"

OPTIONS="-o USE_PWM_AUDIO_ON_ZERO -o SAVE_VFP_REGS_ON_IRQ -o REALTIME -o SCREEN_DMA_BURST_LENGTH=1"
if [ "${RPI}" -gt "1" ]; then
    OPTIONS="${OPTIONS} -o ARM_ALLOW_MULTI_CORE"
fi

if [ -n "${SDHOST_OPTION}" ]; then
    OPTIONS="${OPTIONS} ${SDHOST_OPTION}"
fi

source USBID.sh
if [ "${USB_VID}" ] ; then
    OPTIONS="${OPTIONS} -o USB_GADGET_VENDOR_ID=${USB_VID}"
fi
if [ "${USB_DID}" ] ; then
    OPTIONS="${OPTIONS} -o USB_GADGET_DEVICE_ID_BASE=${USB_DID}"
fi

cd circle-stdlib/
make mrproper || true
./configure -r ${RPI} --prefix "${TOOLCHAIN_PREFIX}" ${OPTIONS} -o KERNEL_MAX_SIZE=0x400000
make -j

cd libs/circle/addon/display/
make clean || true
make -j

cd ../wlan/
make clean || true
make -j

cd ../sensor/
make clean || true
make -j

cd ../Properties/
make clean || true
make -j
cd ../../../..

cd ..

if git describe --tags --dirty --always --long > /dev/null 2>&1; then
    VERSION=$(git describe --tags --dirty --always --long)
else
    VERSION="unknown-$(date +%Y%m%d)"
fi

IS_DIRTY=$(echo "${VERSION}" | grep -q "dirty" && echo "1" || echo "0")
SHORT_VERSION=$(echo "${VERSION}" | sed 's/^\([^-]*\).*$/\1/')

cat > src/version.h << EOF
#ifndef VERSION_H
#define VERSION_H

#define VERSION_STRING "${VERSION}"
#define VERSION_SHORT "${SHORT_VERSION}"
#define VERSION_IS_DIRTY ${IS_DIRTY}

#endif // VERSION_H
EOF

cd src
make clean
echo "***** DEBUG *****"
env
rm -rf ./gcc-* || true
grep -r 'aarch64-none-elf' . || true
find . -type d -name 'aarch64-none-elf' || true
make -j
ls *.img
cd ..

# mkdir -p releases
# cp src/kernel8.img "releases/MiniJV880-${VERSION}.img"