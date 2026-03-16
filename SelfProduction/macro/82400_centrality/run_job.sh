#! /bin/bash
# export USER="$(id -u -n)"
# export LOGNAME=${USER}
# export HOME=/sphenix/u/${LOGNAME}

# source /opt/sphenix/core/bin/sphenix_setup.sh -n ana.520
# export MYINSTALL=/sphenix/user/ChengWei/INTT/ForINTTBarrelNIM/Production/dNdEta_Run2023/dNdEtaINTT/build/install
# export MYINSTALL2=/sphenix/user/ChengWei/SelfINTTProduction/Saturated_chip_rejection/2025_Nov_12/offline/packages/intt/build/install
# export LD_LIBRARY_PATH=$MYINSTALL/lib:$MYINSTALL2/lib:$LD_LIBRARY_PATH
# export ROOT_INCLUDE_PATH=$MYINSTALL/include:$MYINSTALL2/include:$ROOT_INCLUDE_PATH
# source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL $MYINSTALL2


source /opt/sphenix/core/bin/sphenix_setup.sh -n ana.537
export MYINSTALL=/sphenix/user/ChengWei/INTT/INTTdNdEtaOO/SelfProduction/CheckInttRawHit/build/install
export MYINSTALL2=/sphenix/user/ChengWei/INTT/INTTdNdEtaOO/SelfProduction/dNdEta_Run2023/dNdEtaINTT/build/install
export LD_LIBRARY_PATH=$MYINSTALL/lib:$MYINSTALL2/lib:$LD_LIBRARY_PATH
export ROOT_INCLUDE_PATH=$MYINSTALL/include:$MYINSTALL2/include:$ROOT_INCLUDE_PATH
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL $MYINSTALL2

# source /opt/sphenix/core/bin/sphenix_setup.sh -n ana.520
# export MYINSTALL=/sphenix/user/ChengWei/INTT/ForINTTBarrelNIM/Production/dNdEta_Run2023/dNdEtaINTT/build/install
# export MYINSTALL2=/sphenix/user/ChengWei/SelfINTTProduction/Saturated_chip_rejection/2025_Nov_12/offline/packages/intt/build/install
# export MYINSTALL3=/sphenix/user/ChengWei/SelfINTTProduction/Saturated_chip_rejection/2025_Nov_12/offline/packages/trackreco/build/install
# export LD_LIBRARY_PATH=$MYINSTALL/lib:$MYINSTALL2/lib:$MYINSTALL3/lib:$LD_LIBRARY_PATH
# export ROOT_INCLUDE_PATH=$MYINSTALL/include:$MYINSTALL2/include:$MYINSTALL3/include:$ROOT_INCLUDE_PATH
# source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL $MYINSTALL2 $MYINSTALL3


# export LD_LIBRARY_PATH=$MYINSTALL3/lib:$LD_LIBRARY_PATH
# export ROOT_INCLUDE_PATH=$MYINSTALL3/include:$ROOT_INCLUDE_PATH
# source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL3


# export MYINSTALL1=/sphenix/user/ChengWei/silicon_seeding/SiSeeding_check/build/install
# export MYINSTALL2=/sphenix/user/ChengWei/silicon_seeding/sPH_coresoftware/sPH_coresoftware_Oct2_2025/offline/packages/trackreco/build/install
# # note: Add both library paths
# export LD_LIBRARY_PATH=$MYINSTALL1/lib:$MYINSTALL2/lib:$LD_LIBRARY_PATH
# # note: Add both include paths
# export ROOT_INCLUDE_PATH=$MYINSTALL1/include:$MYINSTALL2/include:$ROOT_INCLUDE_PATH
# # note: If you want to run setup for both, usually you only need one,
# # note: but you can call source twice if necessary:
# source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL1 $MYINSTALL2

echo $ROOT_INCLUDE_PATH | sed -e "s/:/\n/g"

process=$1
runnumber=$2
nEvents=$3
output_directory=$4
input_directory=$5
input_filename=$6
formatted_process_id=$(printf "%05d" ${process})

echo process: ${process}
echo runnumber: ${runnumber}
echo nEvents: ${nEvents}
echo output_directory: ${output_directory}
echo input_directory: ${input_directory}
echo input_filename: ${input_filename}

root.exe -q -b INTTRawHit_check_BarrelNIM.C\(${process},${runnumber},${nEvents},\"${output_directory}\",\"${input_directory}\",\"${input_filename}\"\)

# mv ${out_file_directory}/${out_file_name} ${out_file_directory}/completed
#todo : check the output filename
# mv ${output_directory}/BcoDiffNtuple_${runnumber}_clonehitremoveBCO_hotchannelremove_hitQA_checkclonehit_${formatted_process_id}.root ${output_directory}/completed

echo all done process the INTTRawHit_check_BarrelNIM.C, process_id ${process_id}
