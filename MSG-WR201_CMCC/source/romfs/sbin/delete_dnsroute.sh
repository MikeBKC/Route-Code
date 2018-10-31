
wanNo=$1

for dns in `cat /var/run/wan${wanNo}.dns|sed "s/^nameserver//g"`
do
    ip route delete $dns metric $wanNo
done
#echo -n >/var/run/wan${wanNo}.dns
config-dns.sh ${wanNo}
