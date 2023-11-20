#!/bin/bash

alfa_1=${1:-3.51}
alfa_2=${2:-6}
count_ms=${3:-7}
count_mv=${4:-36}


# Be sure that this file has execution permissions:
# Use the nautilus explorer or chmod +x run_vad.sh

# Establecemos que el código de retorno de un pipeline sea el del último programa con código de retorno
# distinto de cero, o cero si todos devuelven cero.
set -o pipefail

# Write here the name and path of your program and database
DIR_P2=$HOME/PAV/P2
DB=$DIR_P2/db.v4
CMD="$DIR_P2/bin/vad -1 $alfa_1 -2 $alfa_2 -s $count_ms -m $count_mv" 

for filewav in $DB/*/*wav; do
#    echo
    echo "**************** $filewav ****************"
    if [[ ! -f $filewav ]]; then 
	    echo "Wav file not found: $filewav" >&2
	    exit 1
    fi

    filevad=${filewav/.wav/.vad}

    $CMD -i $filewav -o $filevad || exit 1

# Alternatively, uncomment to create output wave files
#    filewavOut=${filewav/.wav/.vad.wav}
#    $CMD $filewav $filevad $filewavOut || exit 1

done

scripts/vad_evaluation.pl $DB/*/*lab

exit 0
