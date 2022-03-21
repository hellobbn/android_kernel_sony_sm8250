#!/bin/bash

#====================
sourcecheck=0
logcat=1
devicecheck=1
ABdevice=1
autopack=0
#====================
buildtype=clang
timezone="Asia/Shanghai"
build_device="nio"
kernel_name="kernel"
defconfig_path="nio_defconfig"
kbuild_build_user="Nio"
kbuild_build_host="Dedrimer"
support="11"
#====================

print (){
case ${2} in
	"red")
	echo -e "\033[31m $1 \033[0m";;

	"sky")
	echo -e "\033[36m $1 \033[0m";;

	"green")
	echo -e "\033[32m $1 \033[0m";;

	*)
	echo $1
	;;
	esac
}

HOME=/root/termux

if [ ${sourcecheck} -eq 1 ];then
    apt-get update
    apt-get install -y build-essential bc python curl git zip ftp gcc-aarch64-linux-gnu gcc-arm-linux-gnueabi libssl-dev lftp wget libfl-dev gcc bc zip make
fi

if [ ${sourcecheck} -eq 1 ];then
    if [ -d "${HOME}/Anykernel3" ];then
        rm -rf ${HOME}/Anykernel3
        git clone --depth=1 https://github.com/Dedrimer/Anykernel3 ${HOME}/Anykernel3
    else
        git clone --depth=1 https://github.com/Dedrimer/Anykernel3 ${HOME}/Anykernel3
    fi
fi

if [ ${sourcecheck} -eq 1 ];then
    if [ -d "${HOME}/cbl" ];then
        rm -rf ${HOME}/cbl
        git clone --depth=1 https://github.com/HyperLYP/Clang-and-Binutils-for-ARM64-platforms ${HOME}/cbl
    else
        git clone --depth=1 https://github.com/HyperLYP/Clang-and-Binutils-for-ARM64-platforms ${HOME}/cbl
    fi
fi

source=`pwd`
START_TIME=$(date +"%s")
date="`date +"%m%d%H%M"`"
timedatectl set-timezone ${timezone}

print "You are building version:${date}" yellow

clang_path="${HOME}/cbl/bin/clang"
gcc_path="/usr/bin/aarch64-linux-gnu-"
gcc_32_path="/usr/bin/arm-linux-gnueabi-"

args="-j$(nproc --all)  \
            O=out  \
            ARCH=arm64 "

if [ ${buildtype} == gcc ];then
    export PATH=/usr/cbl/aarch64-linux-gnu-
    args+="SUBARCH=arm64 \
    CROSS_COMPILE=aarch64-linux-gnu- \
    CROSS_COMPILE_ARM32=arm-linux-gnueabi- "
else [ ${buildtype} == clang ]
    args+="CC=${clang_path} \
	 CLANG_TRIPLE=aarch64-linux-gnu- \
	 CROSS_COMPILE=${gcc_path} \
	 AR=${HOME}/cbl/bin/llvm-ar \
	 NM=${HOME}/cbl/bin/llvm-nm \
	 OBJCOPY=${HOME}/cbl/bin/llvm-objcopy \
	 OBJDUMP=${HOME}/cbl/bin/llvm-objdump \
	 STRIP=${HOME}/cbl/bin/llvm-strip \
     CROSS_COMPILE_ARM32=${gcc_32_path} "
fi


building(){
    export KBUILD_BUILD_USER=${kbuild_build_user}
    export KBUILD_BUILD_HOST=${kbuild_build_host}
    print "Building Kernel for ${build_device}..." sky
    if [ ${logcat} -eq 1 ];then
        make ${args} ${defconfig_path}&&make ${args} 2>&1 | tee out/kernel.log
        if [ $? = 0 ];then
            echo -e "\033[32m [INFO] Build successfully \033[0m"
        else
            echo -e "\033[31m [ERROR] Build filed \033[0m"
            exit 1
        fi
    else
        make ${args} ${defconfig_path}&&make ${args}
        if [ $? = 0 ];then
            echo -e "\033[32m [INFO] Build successfully \033[0m"
        else
            echo -e "\033[31m [ERROR] Build filed \033[0m"
            exit 1
        fi
    fi
}

zipfile(){
    zipfiles="${kernel_name}-${date}.zip"
    cp -f out/arch/arm64/boot/Image.gz-dtb ~/Anykernel3
    cd ~/Anykernel3
    sed -i "s|kernel.string=|kernel.string=${kernel_name}|" "anykernel.sh"
    sed -i "s|do.devicecheck=|do.devicecheck=${devicecheck}|" "anykernel.sh"
    sed -i "s|device.name1=|device.name1=${build_device}|" "anykernel.sh"
    sed -i "s|supported.versions=|supported.versions=${support}|" "anykernel.sh"
    if [ ${ABdevice} -eq 1 ];then
        sed -i "s|is_slot_device=|is_slot_device=1|" "anykernel.sh"
    else
        sed -i "s|is_slot_device=|is_slot_device=0|" "anykernel.sh"
    fi
    zip -r "${zipfiles}" *
    mv -f "${zipfiles}" ${HOME}
    sed -i "s|kernel.string=${kernel_name}|kernel.string=|" "anykernel.sh"
    sed -i "s|do.devicecheck=${devicecheck}|do.devicecheck=|" "anykernel.sh"
    sed -i "s|device.name1=${build_device}|device.name1=|" "anykernel.sh"
    sed -i "s|supported.versions=${support}|supported.versions=|" "anykernel.sh"
    if [ ${ABdevice} -eq 1 ];then
        sed -i "s|is_slot_device=1|is_slot_device=|" "anykernel.sh"
    else
        sed -i "s|is_slot_device=0|is_slot_device=|" "anykernel.sh"
    fi
    cd ${HOME}
    cd $source
    print "All done.Find it at ${HOME}/${zipfiles}" green
}

#if [ -d "${HOME}/cbl" ];then
#    print "You have lost some important sources,please echo sourcecheck=1 to fix it" red
#    exit 1
#fi

if [ -d "out" ];then
    read -p "There are something you have done.Do you want to rebuild it?" Dev
        if  [ ${Dev} = y ];then
            if [ ${autopack} -eq 1 ];then
                rm -rf out
                building
                zipfile
                END_TIME=`date +%s`
                EXEC_TIME=$((${END_TIME} - ${START_TIME}))
                EXEC_TIME=$((${EXEC_TIME}/60))
                echo "Runtime is: ${EXEC_TIME} min"
            else
                rm -rf out
                building
                END_TIME=`date +%s`
                EXEC_TIME=$((${END_TIME} - ${START_TIME}))
                EXEC_TIME=$((${EXEC_TIME}/60))
                echo "Runtime is: ${EXEC_TIME} min"
            fi
        else [ ${Dev} = n ]
            if [ ${autopack} -eq 1 ];then
                rm -rf out
                building
                zipfile
                END_TIME=`date +%s`
                EXEC_TIME=$((${END_TIME} - ${START_TIME}))
                EXEC_TIME=$((${EXEC_TIME}/60))
                echo "Runtime is: ${EXEC_TIME} min"
            else
                rm -rf out
                building
                END_TIME=`date +%s`
                EXEC_TIME=$((${END_TIME} - ${START_TIME}))
                EXEC_TIME=$((${EXEC_TIME}/60))
                echo "Runtime is: ${EXEC_TIME} min"
            fi
        fi
else
    if [ ${autopack} -eq 1 ];then
        building
        zipfile
        END_TIME=`date +%s`
        EXEC_TIME=$((${END_TIME} - ${START_TIME}))
        EXEC_TIME=$((${EXEC_TIME}/60))
        echo "Runtime is: ${EXEC_TIME} min"
    else
        building
        END_TIME=`date +%s`
        EXEC_TIME=$((${END_TIME} - ${START_TIME}))
        EXEC_TIME=$((${EXEC_TIME}/60))
        echo "Runtime is: ${EXEC_TIME} min"
    fi
fi
