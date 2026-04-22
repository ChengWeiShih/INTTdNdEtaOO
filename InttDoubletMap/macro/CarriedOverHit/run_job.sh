#! /bin/bash
# export USER="$(id -u -n)"
# export LOGNAME=${USER}
# export HOME=/sphenix/u/${LOGNAME}

source /opt/sphenix/core/bin/sphenix_setup.sh -n ana.538

# export MYINSTALL1=/sphenix/user/ChengWei/build_sPH_coresoftware/EvtIDReporter/install
# export MYINSTALL=/sphenix/user/ChengWei/INTT/INTT/general_codes/CWShih/INTTChipOccupancy/build/install

# export LD_LIBRARY_PATH=$MYINSTALL1/lib:$LD_LIBRARY_PATH
# export ROOT_INCLUDE_PATH=$MYINSTALL1/include:$ROOT_INCLUDE_PATH

# export LD_LIBRARY_PATH=$MYINSTALL/lib:$LD_LIBRARY_PATH
# export ROOT_INCLUDE_PATH=$MYINSTALL/include:$ROOT_INCLUDE_PATH

# source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL1
# source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

# echo $ROOT_INCLUDE_PATH | sed -e "s/:/\n/g"

process=$1
runnumber=$2
nEvents=$3
output_directory=$4
input_directory=$5
input_filename=$6
macro_name=$7
formatted_process_id=$(printf "%05d" ${process})

echo process: ${process}
echo runnumber: ${runnumber}
echo nEvents: ${nEvents}
echo output_directory: ${output_directory}
echo input_directory: ${input_directory}
echo input_filename: ${input_filename}
echo macro_name: ${macro_name}

root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",0,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",1,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",2,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",3,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",4,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",5,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",6,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",7,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",8,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",9,0\)
sleep 5
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",10,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",11,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",12,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",13,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",14,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",15,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",16,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",17,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",18,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",19,0\)
sleep 5
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",20,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",21,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",22,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",23,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",24,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",25,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",26,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",27,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",28,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",29,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",30,0\)
sleep 5
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",31,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",32,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",33,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",34,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",35,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",36,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",37,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",38,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",39,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",40,0\)
sleep 5
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",41,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",42,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",43,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",44,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",45,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",46,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",47,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",48,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",49,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",50,0\)
sleep 5
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",51,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",52,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",53,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",54,0\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",55,0\)
sleep 5



root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",0,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",1,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",2,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",3,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",4,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",5,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",6,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",7,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",8,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",9,1\)
sleep 5
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",10,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",11,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",12,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",13,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",14,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",15,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",16,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",17,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",18,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",19,1\)
sleep 5
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",20,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",21,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",22,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",23,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",24,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",25,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",26,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",27,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",28,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",29,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",30,1\)
sleep 5
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",31,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",32,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",33,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",34,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",35,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",36,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",37,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",38,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",39,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",40,1\)
sleep 5
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",41,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",42,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",43,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",44,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",45,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",46,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",47,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",48,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",49,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",50,1\)
sleep 5
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",51,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",52,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",53,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",54,1\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",55,1\)
sleep 5



root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",0,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",1,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",2,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",3,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",4,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",5,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",6,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",7,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",8,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",9,2\)
sleep 5
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",10,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",11,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",12,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",13,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",14,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",15,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",16,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",17,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",18,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",19,2\)
sleep 5
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",20,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",21,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",22,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",23,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",24,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",25,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",26,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",27,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",28,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",29,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",30,2\)
sleep 5
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",31,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",32,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",33,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",34,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",35,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",36,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",37,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",38,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",39,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",40,2\)
sleep 5
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",41,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",42,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",43,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",44,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",45,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",46,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",47,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",48,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",49,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",50,2\)
sleep 5
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",51,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",52,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",53,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",54,2\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",55,2\)
sleep 5



root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",0,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",1,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",2,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",3,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",4,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",5,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",6,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",7,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",8,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",9,3\)
sleep 5
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",10,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",11,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",12,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",13,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",14,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",15,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",16,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",17,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",18,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",19,3\)
sleep 5
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",20,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",21,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",22,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",23,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",24,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",25,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",26,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",27,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",28,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",29,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",30,3\)
sleep 5
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",31,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",32,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",33,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",34,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",35,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",36,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",37,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",38,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",39,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",40,3\)
sleep 5
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",41,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",42,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",43,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",44,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",45,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",46,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",47,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",48,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",49,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",50,3\)
sleep 5
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",51,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",52,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",53,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",54,3\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",55,3\)
sleep 5



root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",0,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",1,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",2,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",3,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",4,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",5,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",6,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",7,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",8,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",9,4\)
sleep 5
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",10,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",11,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",12,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",13,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",14,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",15,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",16,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",17,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",18,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",19,4\)
sleep 5
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",20,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",21,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",22,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",23,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",24,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",25,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",26,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",27,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",28,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",29,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",30,4\)
sleep 5
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",31,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",32,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",33,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",34,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",35,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",36,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",37,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",38,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",39,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",40,4\)
sleep 5
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",41,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",42,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",43,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",44,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",45,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",46,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",47,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",48,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",49,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",50,4\)
sleep 5
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",51,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",52,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",53,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",54,4\)
root.exe -q -b ${macro_name}.C\(${process},${runnumber},${nEvents},\"${input_directory}\",\"${input_filename}\",\"${output_directory}\",55,4\)
sleep 5


echo all done process the ${macro_name}.C, process_id ${process}
