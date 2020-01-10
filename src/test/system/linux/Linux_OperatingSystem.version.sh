#!/bin/sh

files=(`find /etc/ -type f -maxdepth 1 -name *release* 2>/dev/null`)

if [ ${files[0]} ]; then
   if [ "${files[0]}" == "/etc/lsb-release"  -a "${files[1]}" ]; then
      echo `cat ${files[1]}`
   else
      echo `cat ${files[0]}`
   fi
else
   echo "Linux" 
fi

