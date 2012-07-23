#Leitura do Arquivo
cnt=0
while read num 
do 
a=$num	#essa linha é apenas para não bugar o script
done < file.txt
((cnt=cnt+a))
echo "$a Bundles Receiveds"	



