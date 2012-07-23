#!/bin/bash

#rm -rf ~/Dropbox/Simulacao/Resultados
#mkdir ~/Dropbox/Simulacao/Resultados


protocols="RTProphet" #RTEpidemic  RTSprayAndWait RTSprayAndWait"

#"50-11"


for mob in "200-32" ;do 


	vh=$(echo $mob | cut -d "-" -f1)
	nn=$(echo $mob | cut -d "-" -f2)
	
	echo "Numero de Nós: $nn"
	echo "Numero de Veiculos: $vh"
	
	for traf in "512"; do
		numBundlesSent=$(cat ~/workspace/dtn-ns3-2/ns-3.13/examples/mobility/$traf | wc -l)
		
		
		for i in `seq 1 14`; do
			seed=$RANDOM
			Vseed[$i]=$seed
		done
		
		for protocol in $protocols; do
			
			cnt=0
			cntCopy=0
			cntDelay=0
			cntExpired=0
			cntEfi=0
			cntEfi2=0
			
			for i in `seq 1 14`; do
				echo "0" > $protocol.out
				echo "0" > $protocol.expired
				echo "0" > $protocol.out.efi2
				rm $protocol.out.t
				echo "Testando $protocol-$i com trafego $traf"
				echo "Numero de Bundles Enviados: $numBundlesSent"
				./waf --run "aodv-test --st=500 --ss=${Vseed[$i]} --pt=$protocol --ce=$mob --tr=$traf --nn=$nn"

				#Leitura do Arquivo
				
				num=$(cat $protocol.out) #Número de Bundles Recebidos
				numCopy=$(cat $protocol.out.r) #Numero de Cópias dos Bundles
				numExpired=$(cat $protocol.expired)
				numEfi2=$(cat $protocol.out.efi2)
				
				
				delayMax=$(sort $protocol.out.t | tail -n1)
				delayMin=$(sort $protocol.out.t | head -n1) 
				
				if [ -z $delayMax ]
					then
					   echo 'Delat Fail'
					   delayMax=250
					   delayMin=250
					else
					  
					   echo 'Delay Ok'
					fi
				
				((numTotalBundles=numCopy+numBundlesSent))
				
				numExpiredT=$(echo "scale=5; $numExpired / $numTotalBundles" | bc)
				cntExpired=$(echo "scale=5; $cntExpired + $numExpired / $numTotalBundles" | bc)
				
				
				if  [ $num == 0 ]
					then
						numEfi2=0
						echo "numEfi2 0"
					else
						numEfi2=$(echo "scale=5; $numEfi2 / $num" | bc)
						echo "numEfi $numEfi2"

				fi

				numEfi=$(echo "scale=5; $num / $numTotalBundles" | bc)
                                cntEfi=$(echo "scale=5; $cntEfi + $numEfi" | bc)
                                

                                
                                cntEfi2=$(echo "scale=5; $cntEfi2 + $numEfi2" | bc)
				
				
				((cnt=cnt+num))
				((cntCopy=cntCopy+numCopy))
				
				
				
				cntDelay=$(echo "scale=5; $cntDelay + ($delayMax + $delayMin) / 2" | bc)
				echo "$cntDelay Accumulate"
				echo "$num Bundles Receiveds"
				echo "$numCopy Bundles Copies"
				echo "$numTotalBundles Bundles Totals"
				echo "$numExpired  $numExpiredT Bundles Expireds"
				echo "$numEfi efficiency"
				echo "$numEfi2 efficiency2"
				echo "scale=5; ($delayMax + $delayMin) / 2" | bc
				
				
				num=$(echo "scale=5; $num / $numBundlesSent" | bc)
				echo $num >> ~/Dropbox/Simulacao/Resultados/$mob.$traf-$protocol
				echo $numCopy >> ~/Dropbox/Simulacao/Resultados/$mob.$traf-$protocol.r
				echo $numExpiredT >> ~/Dropbox/Simulacao/Resultados/$mob.$traf-$protocol.e
				echo "scale=5; ($delayMax + $delayMin) / 2" | bc >> ~/Dropbox/Simulacao/Resultados/$mob.$traf-$protocol.t
				echo $numEfi >> ~/Dropbox/Simulacao/Resultados/$mob.$traf-$protocol.efi
				echo $numEfi2 >> ~/Dropbox/Simulacao/Resultados/$mob.$traf-$protocol.efi2
				
			
			done
			cnt=$(echo "scale=5; $cnt / 30" | bc) #Tirando Média
			cnt=$(echo "scale=5; $cnt / $numBundlesSent" | bc) #Sobre O Número de Bundles Enviados esse valor tem que ser extraido do Traffic
			conf=$(python -W ignore::DeprecationWarning ItervalC.py  ~/Dropbox/Simulacao/Resultados/$mob.$traf-$protocol)
			echo "$traf $cnt $conf" >> ~/Dropbox/Simulacao/Resultados/$vh-$protocol.result
			
			
			cntCopy=$(echo "scale=5; $cntCopy / 30" | bc)
			confCopy=$(python -W ignore::DeprecationWarning ItervalC.py  ~/Dropbox/Simulacao/Resultados/$mob.$traf-$protocol.r)
			echo "$traf $cntCopy $confCopy" >> ~/Dropbox/Simulacao/Resultados/$vh-$protocol.result.r
			
			cntDelay=$(echo "scale=5; $cntDelay / 30" | bc)
			confDelay=$(python -W ignore::DeprecationWarning ItervalC.py  ~/Dropbox/Simulacao/Resultados/$mob.$traf-$protocol.t)
			echo "$traf $cntDelay $confDelay" >> ~/Dropbox/Simulacao/Resultados/$vh-$protocol.result.t
			
			cntExpired=$(echo "scale=5; $cntExpired / 30" | bc)
			confExpired=$(python -W ignore::DeprecationWarning ItervalC.py  ~/Dropbox/Simulacao/Resultados/$mob.$traf-$protocol.e)
			echo "$traf $cntExpired $confExpired" >> ~/Dropbox/Simulacao/Resultados/$vh-$protocol.result.e
			
			cntEfi=$(echo "scale=5; $cntEfi / 30" | bc)
                        confEfi=$(python -W ignore::DeprecationWarning ItervalC.py  ~/Dropbox/Simulacao/Resultados/$mob.$traf-$protocol.efi)
                        echo "$traf $cntEfi $confEfi" >> ~/Dropbox/Simulacao/Resultados/$vh-$protocol.result.efi
                        
                        cntEfi2=$(echo "scale=5; $cntEfi2 / 30" | bc)
                        confEfi2=$(python -W ignore::DeprecationWarning ItervalC.py  ~/Dropbox/Simulacao/Resultados/$mob.$traf-$protocol.efi2)
                        echo "$traf $cntEfi2 $confEfi2" >> ~/Dropbox/Simulacao/Resultados/$vh-$protocol.result.efi2
			
			rm $protocol.out
			rm $protocol.out.r
			rm $protocol.out.t
		
		done
		
	
	done

done

./genPlot.sh







