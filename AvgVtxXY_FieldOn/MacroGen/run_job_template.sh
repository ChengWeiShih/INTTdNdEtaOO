#! /bin/bash
# export USER="$(id -u -n)"
# export LOGNAME=${USER}
# export HOME=/sphenix/u/${LOGNAME}

source /opt/sphenix/core/bin/sphenix_setup.sh -n ana.538

process=$1
nFiles=$2
runnumber=$3
nEvents=$4
output_directory=$5
input_directory=$6
input_filename=$7
macro_name=$8        # e.g. Run_AvgVtxXY_FieldOn_data_82391_baseline
formatted_process_id=$(printf "%05d" ${process})

echo "process          : ${process}"
echo "nFiles           : ${nFiles}"
echo "runnumber        : ${runnumber}"
echo "nEvents          : ${nEvents}"
echo "output_directory : ${output_directory}"
echo "input_directory  : ${input_directory}"
echo "input_filename   : ${input_filename}"
echo "macro_name       : ${macro_name}"

root.exe -q -b ${macro_name}.C\(${process},${nFiles},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\"\)

echo "all done process ${macro_name}.C, process_id ${process}"
