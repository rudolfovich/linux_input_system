#!/bin/sh

SDIR="$(readlink -f "$(dirname $0)")"
( \
	mkdir -p "${SDIR}/build" && \
	cd "${SDIR}/build" && \
	rm -rf * && \
	CPPFLAGS="-g -fPIC -DPIC=1 -pipe" "${SDIR}/configure" && \
	make -j2 all && \
	cp "${SDIR}/build/src/libinputsystem.a" "${SDIR}/../lib/linux/linux/" &&
	echo "success!" \
) || \
(
	echo "failed!" && \
	exit 1 \
)
