if [ -z "$1" ];then #if there has been no file given which should be prepared for the dictionary
	echo "Usage: $0 <text file>"
	exit
fi

cat $1 | tr '\n' ' ' | tr '\t' ' ' | sed 's/\./ \. /g' | sed 's/_/ _ /g' | sed 's/-/ - /g' | sed 's/,/ , /g' | sed 's/;/ ; /g' | sed 's/!/ ! /g' | sed 's/?/ ? /g' | sed 's/\"/ \" /g' | sed 's/(/ ( /g' | sed 's/)/ ) /g' | sed 's/\[/ \[ /g' | sed 's/]/ ] /g' | sed 's/\*/ \* /g' | sed "s/'/ ' /g" | sed 's/"/ " /g' | sed 's/`/ ` /g' | sed 's/ \+ / /g'
