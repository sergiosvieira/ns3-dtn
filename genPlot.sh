#!/bin/bash


myhome=$(echo ~)

rm -rf $myhome/Dropbox/Simulacao/Graficos
mkdir $myhome/Dropbox/Simulacao/Graficos



echo "set terminal postscript eps color 'Times' 20" > res
echo "set encoding iso_8859_1" >> res
echo "set grid ytics lt 0 lw 1" >> res
echo "set grid xtics lt 0 lw 1" >> res
echo "set style line 1 lt 1 lw 3 lc 1" >> res
echo "set style line 2 lt 2 lw 3 lc 7" >> res
echo "set style line 3 lt 1 lw 3 lc 3" >> res
echo "set style line 4 lt 2 lw 3 lc 1" >> res
echo "set pointsize 1.5" >> res
echo "set xlabel 'Tamanho Do Bundle (Bytes)'" >> res
#echo  "set xrange [256:2048]" >> res
#echo "set yrange [0:1]" >> res


myhome=$(echo ~)

for i in "50";do #"100" "150" "200" ;do

	echo "set title \"Testes Para Fluxo de Veículos $i/h\"" >> res

	echo "set output '$myhome/Dropbox/Simulacao/Graficos/$i.eps'" >> res
	
	#echo "plot 'Resultados/$i-RTEpidemic.result' title \"Epidemic\" with lines , 'Resultados/$i-RTSprayAndWait.result' title \"SprayAndWait\" with lines,'Resultados/$i-RTProphet.result' title \"Prophet\" with lines" >> res
	
	echo "set ylabel \"Taxa de Entrega \n (BundleRecebidos/BundleEnviados)"\" >> res
	echo "plot '$myhome/Dropbox/Simulacao/Resultados/$i-RTEpidemic.result' using 1:2:3 notitle with yerrorbars ls 1, '$myhome/Dropbox/Simulacao/Resultados/$i-RTEpidemic.result' using 1:2:3 smooth csplines title \"Epidemic\" with lines ls 1,'$myhome/Dropbox/Simulacao/Resultados/$i-RTProphet.result' using 1:2:3 notitle with yerrorbars ls 2, '$myhome/Dropbox/Simulacao/Resultados/$i-RTProphet.result' using 1:2:3 smooth csplines title \"Prophet\" with lines ls 2, '$myhome/Dropbox/Simulacao/Resultados/$i-RTSprayAndWait.result' using 1:2:3 notitle with yerrorbars ls 3, '$myhome/Dropbox/Simulacao/Resultados/$i-RTSprayAndWait.result' using 1:2:3 smooth csplines title \"SprayAndWait\" with lines ls 3, '$myhome/Dropbox/Simulacao/Resultados/$i-RTTrendOfDelivery.result' using 1:2:3 notitle with yerrorbars ls 4, '$myhome/Dropbox/Simulacao/Resultados/$i-RTTrendOfDelivery.result' using 1:2:3 smooth csplines title \"TrendOfDelivery\" with lines ls 4" >> res
	
	
	echo "set output '$myhome/Dropbox/Simulacao/Graficos/$i-r.eps'" >> res
	
	echo "set ylabel \"Número de Bundles Copiados"\" >> res
	echo "plot '$myhome/Dropbox/Simulacao/Resultados/$i-RTEpidemic.result.r' using 1:2:3 notitle with yerrorbars ls 1, '$myhome/Dropbox/Simulacao/Resultados/$i-RTEpidemic.result.r' using 1:2:3 smooth csplines title \"Epidemic\" with lines ls 1,'$myhome/Dropbox/Simulacao/Resultados/$i-RTProphet.result.r' using 1:2:3 notitle with yerrorbars ls 2, '$myhome/Dropbox/Simulacao/Resultados/$i-RTProphet.result.r' using 1:2:3 smooth csplines title \"Prophet\" with lines ls 2, '$myhome/Dropbox/Simulacao/Resultados/$i-RTSprayAndWait.result.r' using 1:2:3 notitle with yerrorbars ls 3, '$myhome/Dropbox/Simulacao/Resultados/$i-RTSprayAndWait.result.r' using 1:2:3 smooth csplines title \"SprayAndWait\" with lines ls 3, '$myhome/Dropbox/Simulacao/Resultados/$i-RTTrendOfDelivery.result.r' using 1:2:3 notitle with yerrorbars ls 4, '$myhome/Dropbox/Simulacao/Resultados/$i-RTTrendOfDelivery.result.r' using 1:2:3 smooth csplines title \"TrendOfDelivery\" with lines ls 4" >> res
	
	
	echo "set output '$myhome/Dropbox/Simulacao/Graficos/$i-t.eps'" >> res
	echo "set ylabel \"Atraso Médio (Segundos)"\" >> res
	echo "plot '$myhome/Dropbox/Simulacao/Resultados/$i-RTEpidemic.result.t' using 1:2:3 notitle with yerrorbars ls 1, '$myhome/Dropbox/Simulacao/Resultados/$i-RTEpidemic.result.t' using 1:2:3 smooth csplines title \"Epidemic\" with lines ls 1,'$myhome/Dropbox/Simulacao/Resultados/$i-RTProphet.result.t' using 1:2:3 notitle with yerrorbars ls 2, '$myhome/Dropbox/Simulacao/Resultados/$i-RTProphet.result.t' using 1:2:3 smooth csplines title \"Prophet\" with lines ls 2, '$myhome/Dropbox/Simulacao/Resultados/$i-RTSprayAndWait.result.t' using 1:2:3 notitle with yerrorbars ls 3, '$myhome/Dropbox/Simulacao/Resultados/$i-RTSprayAndWait.result.t' using 1:2:3 smooth csplines title \"SprayAndWait\" with lines ls 3, '$myhome/Dropbox/Simulacao/Resultados/$i-RTTrendOfDelivery.result.t' using 1:2:3 notitle with yerrorbars ls 4, '$myhome/Dropbox/Simulacao/Resultados/$i-RTTrendOfDelivery.result.t' using 1:2:3 smooth csplines title \"TrendOfDelivery\" with lines ls 4" >> res
	
	
	echo "set output '$myhome/Dropbox/Simulacao/Graficos/$i-e.eps'" >> res
	echo "set ylabel \"Taxa de Bundles Expirados \n (BundleExpirados/(BundleEnviados + BundleCopiados))"\" >> res
	echo "plot '$myhome/Dropbox/Simulacao/Resultados/$i-RTEpidemic.result.e' using 1:2:3 notitle with yerrorbars ls 1, '$myhome/Dropbox/Simulacao/Resultados/$i-RTEpidemic.result.e' using 1:2:3 smooth csplines title \"Epidemic\" with lines ls 1,'$myhome/Dropbox/Simulacao/Resultados/$i-RTProphet.result.e' using 1:2:3 notitle with yerrorbars ls 2, '$myhome/Dropbox/Simulacao/Resultados/$i-RTProphet.result.e' using 1:2:3 smooth csplines title \"Prophet\" with lines ls 2, '$myhome/Dropbox/Simulacao/Resultados/$i-RTSprayAndWait.result.e' using 1:2:3 notitle with yerrorbars ls 3, '$myhome/Dropbox/Simulacao/Resultados/$i-RTSprayAndWait.result.e' using 1:2:3 smooth csplines title \"SprayAndWait\" with lines ls 3, '$myhome/Dropbox/Simulacao/Resultados/$i-RTTrendOfDelivery.result.e' using 1:2:3 notitle with yerrorbars ls 4, '$myhome/Dropbox/Simulacao/Resultados/$i-RTTrendOfDelivery.result.e' using 1:2:3 smooth csplines title \"TrendOfDelivery\" with lines ls 4" >> res
	
	
	echo "set output '$myhome/Dropbox/Simulacao/Graficos/$i-efi.eps'" >> res
	echo "set ylabel \"Taxa de Eficiência \n (BundleRecebidos/(BundleEnviados + BundleCopiados))"\" >> res
	echo "plot '$myhome/Dropbox/Simulacao/Resultados/$i-RTEpidemic.result.efi' using 1:2:3 notitle with yerrorbars ls 1, '$myhome/Dropbox/Simulacao/Resultados/$i-RTEpidemic.result.efi' using 1:2:3 smooth csplines title \"Epidemic\" with lines ls 1,'$myhome/Dropbox/Simulacao/Resultados/$i-RTProphet.result.efi' using 1:2:3 notitle with yerrorbars ls 2, '$myhome/Dropbox/Simulacao/Resultados/$i-RTProphet.result.efi' using 1:2:3 smooth csplines title \"Prophet\" with lines ls 2, '$myhome/Dropbox/Simulacao/Resultados/$i-RTSprayAndWait.result.efi' using 1:2:3 notitle with yerrorbars ls 3, '$myhome/Dropbox/Simulacao/Resultados/$i-RTSprayAndWait.result.efi' using 1:2:3 smooth csplines title \"SprayAndWait\" with lines ls 3, '$myhome/Dropbox/Simulacao/Resultados/$i-RTTrendOfDelivery.result.efi' using 1:2:3 notitle with yerrorbars ls 4, '$myhome/Dropbox/Simulacao/Resultados/$i-RTTrendOfDelivery.result.efi' using 1:2:3 smooth csplines title \"TrendOfDelivery\" with lines ls 4" >> res
	
	
	echo "set output '$myhome/Dropbox/Simulacao/Graficos/$i-efi2.eps'" >> res
	echo "set ylabel \"Taxa de Eficiência "\" >> res
	echo "plot '$myhome/Dropbox/Simulacao/Resultados/$i-RTEpidemic.result.efi2' using 1:2:3 notitle with yerrorbars ls 1, '$myhome/Dropbox/Simulacao/Resultados/$i-RTEpidemic.result.efi2' using 1:2:3 smooth csplines title \"Epidemic\" with lines ls 1,'$myhome/Dropbox/Simulacao/Resultados/$i-RTProphet.result.efi2' using 1:2:3 notitle with yerrorbars ls 2, '$myhome/Dropbox/Simulacao/Resultados/$i-RTProphet.result.efi2' using 1:2:3 smooth csplines title \"Prophet\" with lines ls 2, '$myhome/Dropbox/Simulacao/Resultados/$i-RTSprayAndWait.result.efi2' using 1:2:3 notitle with yerrorbars ls 3, '$myhome/Dropbox/Simulacao/Resultados/$i-RTSprayAndWait.result.efi2' using 1:2:3 smooth csplines title \"SprayAndWait\" with lines ls 3, '$myhome/Dropbox/Simulacao/Resultados/$i-RTTrendOfDelivery.result.efi2' using 1:2:3 notitle with yerrorbars ls 4, '$myhome/Dropbox/Simulacao/Resultados/$i-RTTrendOfDelivery.result.efi2' using 1:2:3 smooth csplines title \"TrendOfDelivery\" with lines ls 4" >> res
	
done

gnuplot res
rm res

