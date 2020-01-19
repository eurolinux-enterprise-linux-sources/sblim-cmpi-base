#!/bin/bash

# test sblim-cmpi-base package

SCRIPT_PATH=`dirname ${BASH_SOURCE}`

#******************************************************************************#

export SBLIM_TESTSUITE_RUN=1;

#******************************************************************************#

declare -a CLASSNAMES;
CLASSNAMES=([0]=Linux_ComputerSystem [1]=Linux_OperatingSystem \
[2]=Linux_UnixProcess [3]=Linux_Processor \
[4]=Linux_RunningOS [5]=Linux_OSProcess [6]=Linux_CSProcessor \
[7]=Linux_OperatingSystemStatisticalData [8]=Linux_OperatingSystemStatistics \
[9]=Linux_BaseBoard [10]=Linux_CSBaseBoard)

declare -i max=10;
declare -i i=0;

cd $SCRIPT_PATH
while(($i<=$max))
do
  . ${SCRIPT_PATH}/run.sh ${CLASSNAMES[$i]} $1 $2 $3 $4 || exit 1;
  i=$i+1;
done
