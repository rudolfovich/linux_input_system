#!/bin/sh

export CPPFLAGS="-fPIC -DPIC -pipe"
export SDIR="$(readlink -f "$(dirname $0)")"

function build()
{
	local platform=$1
	local arch="default"
	local cppflags_base="${CPPFLAGS}"
	local cppflags="${CPPFLAGS}"
	local ldflags_base="${LDFLAGS}"
	local ldflags="${LDFLAGS}"
	local cfgflags=""
	local tmp_build_dir=""
	if [ -z "${platform}" ]; then
		echo "required [platform] argument"
		exit 1
	fi
	local CC=
	local CXX=
	case "${platform}" in
		"linux")
			arch="x86_64-linux-gnueabihf"
            cfgflags+=" --host ${arch}"
            cppflags+=" -m64"
            ldflags+=" -m64"
			;;
		"linux-i686")
			arch="i686-linux-gnueabihf"
            cfgflags+=" --host ${arch}"
            cppflags+=" -m32"
            ldflags+=" -m32"
			;;
		"raspberrypi")
            arch="arm-linux-gnueabihf"
			cfgflags+=" --host arm-linux-gnueabihf"
			CC=/opt/toolchains/raspberrypi/bin/arm-linux-gnueabihf-gcc
			CXX=/opt/toolchains/raspberrypi/bin/arm-linux-gnueabihf-g++
			;;
		"cubieboard")
            arch="arm-linux-gnueabihf"
			cfgflags+=" --host arm-linux-gnueabihf"
			CC=/opt/toolchains/raspberrypi/bin/arm-linux-gnueabihf-gcc
			CXX=/opt/toolchains/raspberrypi/bin/arm-linux-gnueabihf-g++
			;;
		"dune")
			;;
		"stlinux")
			;;
		*)
			echo "Error: unsupported platform [${platform}]"
			exit 1;
			;;
	esac
	echo "Building platform [${platform}] arch is [${arch}]"
	tmp_build_dir="${SDIR}/build/${platform}"
	echo " - _Build directory is [${tmp_build_dir}]"
	mkdir -p "${tmp_build_dir}"
	if [ 0 -ne "$?" ]; then
		echo "Failed to create dir [${tmp_build_dir}]"
		exit 1
	fi
	cd "${tmp_build_dir}"
	if [ 0 -ne "$?" ]; then
		echo "Failed to change dir [${tmp_build_dir}]"
		exit 1
	fi
	if [ "$(pwd)" != "${tmp_build_dir}" ]; then
		echo "Failed to change dir [${tmp_build_dir}] confirm"
		exit 1
	fi
	rm -rf *
	if [ 0 -ne "$?" ]; then
		echo "!!! rm * from [$(pwd)] !!!"
		echo "Failed to clear dir [${tmp_build_dir}]"
		exit 1
	fi

	
	CPPFLAGS="${cppflags} -g" \
	cfgflags="${cfgflags}" \
	CC=$CC \
	CXX=$CXX \
    LDFLAGS="${ldflags}"
	build_conf "${platform}" "${arch}" "Debug"

	CPPFLAGS="${cppflags} -O3" \
	cfgflags="${cfgflags}" \
	CC=$CC \
	CXX=$CXX \
    LDFLAGS="${ldflags}"
	build_conf "${platform}" "${arch}" "Release"
	export CPPFLAGS="${cppflags_base}"
	export LDFLAGS="${ldflags_base}"

	cd "${SDIR}"
	if [ 0 -ne "$?" ]; then
		echo "Failed to change dir [${SDIR}]"
		exit 1
	fi
	rm -rf "${tmp_build_dir}"
	if [ 0 -ne "$?" ]; then
		echo "Failed to remove dir [${tmp_build_dir}]"
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
	local arch=$2
	if [ -z "${arch}" ]; then
		echo "required [arch] argument"
		exit 1
	fi
	local conf=$3
	if [ -z "${conf}" ]; then
		echo "required [conf] argument"
		exit 1
	fi
	local dest="$(readlink -f "${SDIR}/../lib/linux/${platform}-${arch}/${conf}")"
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
        LDFLAGS="$LDFLAGS" \
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

build "linux"
#build "raspberrypi"
#build "cubieboard"
#build "dune"
#build "stlinux"
#build "cubieboard"
