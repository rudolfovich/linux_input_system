#!/bin/sh

export CPPFLAGS="-fPIC -DPIC -pipe"
export SDIR="$(readlink -f "$(dirname $0)")"

function build()
{
	local cppflags_base="${CPPFLAGS}"
	local cppflags="${CPPFLAGS}"
	local cfgflags=""
	local platform=$1
	if [ -z "${platform}" ]; then
		echo "required [platform] argument"
		exit 1
	fi
	local CC=
	local CXX=
	case "${platform}" in
		linux)
			;;
		raspberrypi)
			cfgflags+=" --host arm-linux-gnueabihf"
			CC=/opt/toolchains/raspberrypi/bin/arm-linux-gnueabihf-gcc
			CXX=/opt/toolchains/raspberrypi/bin/arm-linux-gnueabihf-g++
			;;
		cubieboard2)
			;;
		dune)
			;;
		stlinux)
			;;
		*)
			echo "Error: unsupported platform [${platform}]"
			exit 1;
			;;
	esac
	echo "Building platform [${platform}]"
	local build_dir="${SDIR}/build/${platform}"
	#echo " - build_dir  = [${build_dir}]"
	mkdir -p "${build_dir}"
	if [ 0 -ne "$?" ]; then
		echo "Failed to create dir [${build_dir}]"
		exit 1
	fi
	cd "${build_dir}"
	if [ 0 -ne "$?" ]; then
		echo "Failed to change dir [${build_dir}]"
		exit 1
	fi
	if [ "$(pwd)" != "${build_dir}" ]; then
		echo "Failed to change dir [${build_dir}] confirm"
		exit 1
	fi
	rm -rf *
	if [ 0 -ne "$?" ]; then
		echo "!!! rm * from [$(pwd)] !!!"
		echo "Failed to clear dir [${build_dir}]"
		exit 1
	fi

	
	CPPFLAGS="${cppflags} -g" \
	cfgflags="${cfgflags}" \
	CC=$CC \
	CXX=$CXX \
	build_conf "${platform}" "Debug"

	CPPFLAGS="${cppflags} -O3" \
	cfgflags="${cfgflags}" \
	CC=$CC \
	CXX=$CXX \
	build_conf "${platform}" "Release"
	export CPPFLAGS="${cppflags_base}"

	cd "${SDIR}"
	if [ 0 -ne "$?" ]; then
		echo "Failed to change dir [${SDIR}]"
		exit 1
	fi
	rm -rf "${build_dir}"
	if [ 0 -ne "$?" ]; then
		echo "Failed to remove dir [${build_dir}]"
		exit 1
	fi
}

function build_conf()
{
	local platform=$1
	if [ -z "${platform}" ]; then
		echo "required [platform] argument"
		exit 1
	fi
	local conf=$2
	if [ -z "${conf}" ]; then
		echo "required [conf] argument"
		exit 1
	fi
	local dest="$(readlink -f "${SDIR}/../lib/linux/${platform}/${conf}")"
	mkdir -p "${dest}"
	if [ 0 -ne "$?" ]; then
		echo "Failed to create dir [${dest}]"
		exit 1
	fi
	echo " Build configuration [${conf}]..."
	echo " -  CPPFLAGS is $CPPFLAGS"
	echo " -  Configuring..."
	echo "running [${SDIR}/configure ${cfgflags} CPPFLAGS='$CPPFLAGS' CC=$CC CXX=$CXX]" > configure_cloud.log
	${SDIR}/configure \
		${cfgflags} \
		CPPFLAGS="$CPPFLAGS" \
		CC="$CC" \
		CXX="$CXX" \
		>> configure_cloud.log
	if [ 0 -ne "$?" ]; then
		echo " ===== configure_cloud.log ===== "
		tail "configure_cloud.log"
		echo " ===== configure_cloud.log ===== "
		echo "Failed to configure [${conf}]"
		exit 1
	fi
	echo " -  Building..."
	make -j$(nproc) all > build_cloud.log
	if [ 0 -ne "$?" ]; then
		echo " ===== build_cloud.log ===== "
		tail "build_cloud.log"
		echo " ===== build_cloud.log ===== "
		echo "Failed to build [${conf}]"
		exit 1
	fi
	cp "./src/libinputsystem.a" "${dest}"
	if [ 0 -ne "$?" ]; then
		echo "Failed to deploy [${conf}]"
		exit 1
	fi
	rm -rf *
	if [ 0 -ne "$?" ]; then
		echo "!!! rm * from [$(pwd)] !!!"
		echo "Failed to clear dir [$(pwd)]"
		exit 1
	fi
	echo " -  done"
}

build linux
build raspberrypi
#build cubieboard2
#build dune
#build stlinux
