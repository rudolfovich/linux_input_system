#!/bin/sh
SDIR="$(readlink -f "$(dirname $0)")"
( \
mkdir -p "${SDIR}/build" && \
cd "${SDIR}/build" && \
rm -rf * && \
"${SDIR}/configure" && \
make -j2 all && \
cp "${SDIR}/build/src/libinputsystem.a" "${SDIR}/../cloudgames/third-party/lib/linux/linux/" &&
echo "success!" && \
exit 0 \
) || \
echo "failed!" && \
exit 1
