#!/bin/sh

#作者：曹永祥
#功能：开启VPN后，使用DNS代理，指定域名解析服务器，并添加DNS服务器路由走VPN接口
#日期：2015.9.18

if [ $# != 5 ];then
    echo "Usage: dnsmasqConf.sh <Operation> <DNS Server 1> <DNS Server 2> <pppname> <setname>"
    echo "<Operation> should be 'add' 'del' 'addGlobal' or 'delGlobal'"
    exit;
fi

opt=$1
DNSServer1=$2
DNSServer2=$3
pppname=$4
dnsmasqfile=/etc/dnsmasq.conf
confdir=/etc/dnsmasq.d/
filename=gfwlist.conf
ipsetfile=/etc/chinaip.list
setname=$5

if [ ${opt} == 'add' ];then
#国内IP添加ipset规则
ipset -X $setname
cat $ipsetfile | ipset -R &
#指定dnsmasq配置文件目录
mkdir -p $confdir
echo "conf-dir=${confdir}" > $dnsmasqfile
#添加域名解析格式：server=/域名字段/DNS服务器
echo "#GFW_LIST
server=/6park.com/${DNSServer1}
server=/ac.playstation.net/${DNSServer1}
server=/appspot.com/${DNSServer1}
server=/asahichinese.com/${DNSServer1}
server=/asiafriendfinder.com/${DNSServer1}
server=/bbc.com/${DNSServer1}
server=/bbc.co.uk/${DNSServer1}
server=/bbci.co.uk/${DNSServer1}
server=/bbcimg.co.uk/${DNSServer1}
server=/bit.ly/${DNSServer1}
server=/blogger.com/${DNSServer1}
server=/blogspot.com/${DNSServer1}
server=/brightcove.com/${DNSServer1}
server=/chartbeat.net/${DNSServer1}
server=/cloudfront.net/${DNSServer1}
server=/conviva.com/${DNSServer1}
server=/corpus4u.org/${DNSServer1}
server=/cyanogenmod.org/${DNSServer1}
server=/dl.playstation.net/${DNSServer1}
server=/dropbox.com/${DNSServer1}
server=/dw.de/${DNSServer1}
server=/dwnews.com/${DNSServer1}
server=/dw-world.de/${DNSServer1}
server=/facebook.com/${DNSServer1}
server=/facebook.net/${DNSServer1}
server=/fbcdn.net/${DNSServer1}
server=/googleapis.com/${DNSServer1}
server=/google.com/${DNSServer1}
server=/googleusercontent.com/${DNSServer1}
server=/googlevideo.com/${DNSServer1}
server=/huaren.us/${DNSServer1}
server=/imrworldwide.com/${DNSServer1}
server=/lovesphinx.tk/${DNSServer1}
server=/mobile01.com/${DNSServer1}
server=/nintendowifi.net/${DNSServer1}
server=/nyt.com/${DNSServer1}
server=/nytimes.com/${DNSServer1}
server=/openvpn.net/${DNSServer1}
server=/revsci.net/${DNSServer1}
server=/scanscout.com/${DNSServer1}
server=/scorecardresearch.com/${DNSServer1}
server=/sexinsex.net/${DNSServer1}
server=/slideshare.net/${DNSServer1}
server=/sourceforge.net/${DNSServer1}
server=/statcounter.com/${DNSServer1}
server=/t66y.com/${DNSServer1}
server=/target.com/${DNSServer1}
server=/twitpic.com/${DNSServer1}
server=/twitter.com/${DNSServer1}
server=/tw.rd.yahoo.com/${DNSServer1}
server=/update.playstation.net/${DNSServer1}
server=/wikipedia.org/${DNSServer1}
server=/wretch.cc/${DNSServer1}
server=/youtube.com/${DNSServer1}
server=/ytimg.com/${DNSServer1}
server=/wenxuecity.com/${DNSServer1}
server=/247realmedia.com/${DNSServer1}
server=/businessweek.com/${DNSServer1}
server=/bloomberg.com/${DNSServer1}
server=/btrd.net/${DNSServer1}
server=/2mdn.net/${DNSServer1}
server=/bwbx.io/${DNSServer1}
server=/ooyala.com/${DNSServer1}
server=/tw.news.yahoo.com/${DNSServer1}
server=/appledaily.com.tw/${DNSServer1}
server=/libertytimes.com.tw/${DNSServer1}
server=/twimg.edgesuite.net/${DNSServer1}
server=/tw.img.nextmedia.com/${DNSServer1}
server=/1-apple.com.tw/${DNSServer1}
server=/pixfs.net/${DNSServer1}
server=/pimg.tw/${DNSServer1}
server=/pixnet.net/${DNSServer1}
server=/pixnet.cc/${DNSServer1}
server=/pixanalytics.com/${DNSServer1}
server=/pixplug.in/${DNSServer1}
server=/fetchvideo.com/${DNSServer1}
server=/kissyoutube.com/${DNSServer1}
server=/wikinet.org/${DNSServer1}
server=/wikkii.com/${DNSServer1}
server=/privatetunnel.com/${DNSServer1}
server=/openvpn.org/${DNSServer1}
server=/tw.yahoo.com/${DNSServer1}
server=/sina.com.tw/${DNSServer1}
server=/pchome.com.tw/${DNSServer1}
server=/mitbbs.com/${DNSServer1}
server=/erepublik.net/${DNSServer1}
server=/erepublik.com/${DNSServer1}
server=/mobile01.com/${DNSServer1}
server=/hkgolden.com/${DNSServer1}
server=/bmediaasia.com/${DNSServer1}
server=/blogspot.com/${DNSServer1}
server=/previewshots.com/${DNSServer1}
server=/blogblog.com/${DNSServer1}
server=/blogger.com/${DNSServer1}
server=/vimeocdn.com/${DNSServer1}
server=/mingpaonews.com/${DNSServer1}
server=/mingpao.com/${DNSServer1}
server=/rthk.org.hk/${DNSServer1}
server=/rthk.hk/${DNSServer1}
server=/akamaihd.net/${DNSServer1}
server=/bitshare.com/${DNSServer1}
server=/cocoapods.org/${DNSServer1}
server=/wordpress.com/${DNSServer1}
server=/pchome.com.tw/${DNSServer1}
" > ${confdir}${filename}
#添加DNS路由
ip route add ${DNSServer1}/32 dev $pppname
killall dnsmasq ;dnsmasq &
elif [ ${opt} == 'addGlobal' ];then
#全局模式
    resolvfile=/etc/resolvVPN.conf
    echo "#Interface VPN" > $resolvfile
    echo "nameserver ${DNSServer1}" >> $resolvfile
    echo "nameserver ${DNSServer2}" >> $resolvfile
    killall dnsmasq ; dnsmasq -r $resolvfile &
    ip route add ${DNSServer1}/32 dev $pppname
    ip route add ${DNSServer2}/32 dev $pppname
elif [ ${opt} == 'del' ];then
    ipset -F $setname
    ipset -X $setname
    rm -rf $resolvfile
    rm -rf ${confdir}${filename}
    killall dnsmasq ; dnsmasq &
    ip route del ${DNSServer1}/32 dev $pppname
    ip route del ${DNSServer2}/32 dev $pppname
else
    echo "Usage: dnsmasqConf.sh <Operation> <DNS Server 1> <DNS Server 2> <pppname> <setname>"
    echo "<Operation> should be 'add' 'del' 'addGlobal' or 'delGlobal'"
    exit;
fi
ip route flush ca
exit;
