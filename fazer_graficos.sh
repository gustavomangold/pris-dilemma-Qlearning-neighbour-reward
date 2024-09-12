#!/bin/bash
rho="1.00000"
LSIZE="100"
INIT_TIME_FOR_AVERAGES=3000
ARQ_MED_TEMPO="med.gp"

if [ -f $ARQ_MED_TEMPO ]; then
	rm $ARQ_MED_TEMPO
fi
echo "unset logscale x" > $ARQ_MED_TEMPO
echo -n "plot " >> $ARQ_MED_TEMPO

let m=1
for rho in 1.00000 0.80000 0.60000 0.40000 0.20000
do
	name="TeMp_rho_${rho}.gp"
	if [ -f "media_temporal_rho_${rho}.txt" ]; then
		rm media_temporal_rho_${rho}.txt
	fi

	if [ -f $name ]; then
		rm $name
	fi

	
	echo "unset  logscale x" > $name
	echo "set xlabel \"t\" " >> $name
	echo -n "plot	" >> $name

	let n=1
	for T in $(seq 1.00 0.10 2.00)
	do
		for arq in $(ls T${T}*rho${rho}*.dat)
		do
			SS=$(echo $arq| cut -d'_' -f 3)

			echo -n "$T" >> media_temporal_rho_${rho}.txt
			echo "awk -v INITIAL_TIME=\"${INIT_TIME_FOR_AVERAGES}\" -f media_temporal_8col.awk $arq >> media_temporal_rho_${rho}.txt"
			awk -v INITIAL_TIME="${INIT_TIME_FOR_AVERAGES}" -f media_temporal_8col.awk $arq >> media_temporal_rho_${rho}.txt

		done

		if [ $n -ne 1 ]; then
			echo -n ", "  >> $name
		fi
		echo -n "\"$arq\" u 1:(\$8/(\$2+\$3)) w l t \"$T\"" >> $name
		let n=1+$n
	done
	echo " ">> $name
	gnuplot --persist $name


	if [ $m -ne 1 ]; then
		echo -n ", "  >> $ARQ_MED_TEMPO
	fi
	echo -n "\"media_temporal_rho_${rho}.txt\" u 1:(\$2/(\$2+\$3)) w lp  t \"r=${rho}\"" >> $ARQ_MED_TEMPO
	let m=1+$m
done
echo " ">> $ARQ_MED_TEMPO
gnuplot --persist $ARQ_MED_TEMPO


