#!/bin/bash

#rm -rf Resultados
#mkdir Resultados


protocols=" RTSprayAndWait RTProphet RTEpidemic"

#"50-11" "100-18"

for mob in  "150-25" "200-32";do

	vh=$(echo $mob | cut -d "-" -f1)
	nn=$(echo $mob | cut -d "-" -f2)
	
	echo "Numero de Nós: $nn"
	echo "Numero de Veiculos: $vh"
	
	for traf in "256" "512" "1024" "2048"  ; do
		numBundlesSent=$(cat /home/joagf/workspace/dtn-ns3/ns-3.13/examples/mobility/$traf | wc -l)
		
		
		for i in `seq 1 30`; do
			seed=$RANDOM
			Vseed[$i]=$seed
		done
		
		for protocol in $protocols; do
			
			cnt=0
			
			
			for i in `seq 1 30`; do
				echo "0" > $protocol.out
				echo "Testando $protocol-$i com trafego $traf"
				echo "Numero de Bundles Enviados: $numBundlesSent"
				./waf --run "aodv-test --st=500 --ss=${Vseed[$i]} --pt=$protocol --ce=$mob --tr=$traf --nn=$nn"

				#Leitura do Arquivo
				
				num=$(cat $protocol.out)
				
				((cnt=cnt+num))
				echo "$num Bundles Receiveds"
				
				echo $num >> Resultados/$mob.$traf-$protocol		
			
			done
			cnt=$(echo "scale=5; $cnt / 30" | bc) #Tirando Média
			cnt=$(echo "scale=5; $cnt / $numBundlesSent" | bc) #Sobre O Número de Bundles Enviados esse valor tem que ser extraido do Traffic
			conf=$(python ItervalC.py Resultados/$mob.$traf-$protocol)
			echo "$traf $cnt $conf" >> "Resultados/$vh-$protocol.result"
			rm $protocol.out
		
		done
		
	
	done

done

./genPlot.sh







