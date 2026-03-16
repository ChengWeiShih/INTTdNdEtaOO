#! /bin/bash
# export USER="$(id -u -n)"
# export LOGNAME=${USER}
# export HOME=/sphenix/u/${LOGNAME}

source /opt/sphenix/core/bin/sphenix_setup.sh -n ana.538
export MYINSTALL=/sphenix/user/ChengWei/INTT/INTTdNdEtaOO/SelfProduction/GetCentralityDist/build/install
export LD_LIBRARY_PATH=$MYINSTALL/lib:$LD_LIBRARY_PATH
export ROOT_INCLUDE_PATH=$MYINSTALL/include:$ROOT_INCLUDE_PATH
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

echo $ROOT_INCLUDE_PATH | sed -e "s/:/\n/g"


root.exe -q -b F4A_macro.C

echo all done process the F4A_macro.C
