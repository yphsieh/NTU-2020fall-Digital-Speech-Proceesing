#make SRIPATH=/root/srilm-1.5.10 MACHINE_TYPE=i686-m64
#perl separator_big5.pl corpus.txt > corpus_seg.txt
#ngram-count -text corpus_seg.txt -write lm.cnt -order 2
#ngram-count -read lm.cnt -lm bigram.lm -order 2 -unk 
#make map From=Big5-ZhuYin.map To=ZhuYin-Big5.map
#./mydisambig test_data/example.txt ZhuYin-Big5.map bigram.lm result/example_test.txt
#disambig -text test_data/example.txt -map ZhuYin-Big5.map -lm bigram.lm -order 2 > result2/example_test.txt

for ((idx=1;idx<=10;idx++))
do
    #echo "ZhuYin to Big5: $idx.txt..."
    #disambig -text test_data/$idx.txt -map ZhuYin-Big5.map -lm bigram.lm -order 2 > result2/$idx.txt
    #./mydisambig test_data/$idx.txt ZhuYin-Big5.map bigram.lm result/$idx.txt
    diff -y -W 70 result2/$idx.txt result/$idx.txt > diff_$idx.txt
done
