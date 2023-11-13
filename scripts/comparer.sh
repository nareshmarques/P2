for alfa_1 in $(seq 10 1 20); do
echo -ne "$alfa_1\t"
scripts/run_vad.sh $alfa_1 | fgrep TOTAL; 
done | sort -t: -k2n


