#! /bin/sh

ins_num=01
dck_num=01
file="PokerExec"
##--  get last dir name like s13t200_00
result_pwd=`pwd`
dir_name=`basename ${result_pwd}`_${ins_num}
# if [ ! -e $file ]; then
#   ../../sys/Make2019.sh
# fi
../../sys/BinCompile.sh ../../src 7 5 PokerExec.c
echo ../../sys/DeckInit 1000 > Deck_${ins_num}.txt
../../sys/DeckInit 1000 > Deck_${ins_num}.txt
./PokerExec ${dir_name} 10 Deck_${ins_num}.txt 0 > result.txt
# ./PokerExec ${dir_name} 500 Deck_${ins_num}.txt 0 > result.txt
