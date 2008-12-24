\ant\bin\link_extract part-0\*.xml part-120000\*.xml part-150000\*.xml part-180000\*.xml part-210000\*.xml part-240000\*.xml part-270000\*.xml part-30000\*.xml part-300000\*.xml part-330000\*.xml part-360000\*.xml part-390000\*.xml part-420000\*.xml part-450000\*.xml part-480000\*.xml part-510000\*.xml part-540000\*.xml part-570000\*.xml part-60000\*.xml part-600000\*.xml part-630000\*.xml part-90000\*.xml > anchor_text.txt

\ant\bin\link_index anchor_text.txt > anchor_text.idx

\ant\bin\link_extract_pass2 anchor_text.idx part-0\*.xml part-120000\*.xml part-150000\*.xml part-180000\*.xml part-210000\*.xml part-240000\*.xml part-270000\*.xml part-30000\*.xml part-300000\*.xml part-330000\*.xml part-360000\*.xml part-390000\*.xml part-420000\*.xml part-450000\*.xml part-480000\*.xml part-510000\*.xml part-540000\*.xml part-570000\*.xml part-60000\*.xml part-600000\*.xml part-630000\*.xml part-90000\*.xml > anchor_index.idx

\ant\bin\link_index_merge anchor_index.idx anchor_text.idx > links.idx
