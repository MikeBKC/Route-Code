for ser in `nvram_get 2860 PptpClients |sed "s/;/\n/g" |sed "s/^.*,//g"`
do
ip route del $ser dev $1
done
