

for file in myimageDif/* ;
do  
    ./compression_tool -d "$file"

done 

for file in myimagePnm/* ;
do  
    ./compression_tool -e "$file"

done 