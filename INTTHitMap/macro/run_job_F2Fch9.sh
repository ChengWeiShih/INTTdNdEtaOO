#! /bin/bash
# export USER="$(id -u -n)"
# export LOGNAME=${USER}
# export HOME=/sphenix/u/${LOGNAME}

source /opt/sphenix/core/bin/sphenix_setup.sh -n ana.538
export MYINSTALL=/sphenix/user/ChengWei/INTT/INTTdNdEtaOO/INTTHitMap/build/install
export LD_LIBRARY_PATH=$MYINSTALL/lib:$LD_LIBRARY_PATH
export ROOT_INCLUDE_PATH=$MYINSTALL/include:$ROOT_INCLUDE_PATH
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

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

root.exe -q -b F4A_F2_Fch9.C\(${process},${runnumber},${nEvents},\"${output_directory}\",\"${input_directory}\",\"${input_filename}\"\)

# mv ${out_file_directory}/${out_file_name} ${out_file_directory}/completed
#todo : check the output filename
# mv ${output_directory}/BcoDiffNtuple_${runnumber}_clonehitremoveBCO_hotchannelremove_hitQA_checkclonehit_${formatted_process_id}.root ${output_directory}/completed

echo all done process the F4A_F2_Fch9.C, process_id ${process_id}
