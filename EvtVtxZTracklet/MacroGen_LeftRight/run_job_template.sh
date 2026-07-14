#! /bin/bash

source /opt/sphenix/core/bin/sphenix_setup.sh -n ana.538

process=$1
runnumber=$2
nEvents=$3
output_directory=$4
input_directory=$5
input_filename=$6
macro_name=$7
formatted_process_id=$(printf "%05d" ${process})

echo "process          : ${process}"
echo "runnumber        : ${runnumber}"
echo "nEvents          : ${nEvents}"
echo "output_directory : ${output_directory}"
echo "input_directory  : ${input_directory}"
echo "input_filename   : ${input_filename}"
echo "macro_name       : ${macro_name}"

root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\"\)

echo "all done process ${macro_name}.C, process_id ${process}"
