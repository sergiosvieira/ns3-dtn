set terminal png
set xlabel 'Tamanho Do Bundle'
set ylabel 'Taxa de Entrega'
set title "Testes Para Fluxo de Veículos 25/h"
set output 'Graficos/25.png'
plot 'Resultados/25-RTEpidemic.result' title "Epidemic" with lines , 'Resultados/25-RTSprayAndWait.result' title "SprayAndWait" with lines
set title "Testes Para Fluxo de Veículos 50/h"
set output 'Graficos/50.png'
plot 'Resultados/50-RTEpidemic.result' title "Epidemic" with lines , 'Resultados/50-RTSprayAndWait.result' title "SprayAndWait" with lines
set title "Testes Para Fluxo de Veículos 100/h"
set output 'Graficos/100.png'
plot 'Resultados/100-RTEpidemic.result' title "Epidemic" with lines , 'Resultados/100-RTSprayAndWait.result' title "SprayAndWait" with lines
set title "Testes Para Fluxo de Veículos 200/h"
set output 'Graficos/200.png'
plot 'Resultados/200-RTEpidemic.result' title "Epidemic" with lines , 'Resultados/200-RTSprayAndWait.result' title "SprayAndWait" with lines
