#! /bin/bash

cd /sphenix/user/ChengWei/INTT/INTTdNdEtaOO/AI_framework
# for bin in $(seq 0 1); do
for bin in $(seq 0 10); do
    python3 run_pass.py pass_configs/MC_HIJING_baseline.py --step all \
        --set _centrality_bin=${bin} \
        --set _subfoldername=baseline \
        --set _cluster_adc=15 \
        --set _cluster_phi_size=40 \
        --set _delta_phi_cut=0.15;

    sleep 20;
done

for bin in $(seq 0 10); do
    python3 run_pass.py pass_configs/MC_HIJING_baseline.py --step all \
        --set _centrality_bin=${bin} \
        --set _subfoldername=NoClusPhiCut \
        --set _cluster_phi_size=200;

    sleep 20;
done

for bin in $(seq 0 10); do
    python3 run_pass.py pass_configs/MC_HIJING_baseline.py --step all \
        --set _centrality_bin=${bin} \
        --set _subfoldername=ClusADCTight \
        --set _cluster_adc=35;

    sleep 20;
done

for bin in $(seq 0 10); do
    python3 run_pass.py pass_configs/MC_HIJING_baseline.py --step all \
        --set _centrality_bin=${bin} \
        --set _subfoldername=ClusADCLoose \
        --set _cluster_adc=0;

    sleep 20;
done

for bin in $(seq 0 10); do
    python3 run_pass.py pass_configs/MC_HIJING_baseline.py --step all \
        --set _centrality_bin=${bin} \
        --set _subfoldername=DeltaPhiTight \
        --set _delta_phi_cut=0.1;

    sleep 20;
done

for bin in $(seq 0 10); do
    python3 run_pass.py pass_configs/MC_HIJING_baseline.py --step all \
        --set _centrality_bin=${bin} \
        --set _subfoldername=DeltaPhiLoose \
        --set _delta_phi_cut=0.2;

    sleep 20;
done