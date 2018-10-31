<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<%langTran("common");%>
<%langTran("HomePage");%>
<%langTran("serviceZone");%>
<%langTran("SysInfo");%>
<%langTran("EasyTask");%>
<%langTran("WlanBase");%>
<%langTran("UserSui");%>
<%langTran("RunStatus_Sui");%>
<title><%write(com_runStatus);%></title>
<link href="css/style_new.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="script/jquery-1.8.1.min.js"></script> 
<script type="text/javascript" src="js/common.js"></script> 
</head>

<script type="text/javascript">
<% outputSystemNowTime();%>
<% outputSystemUpTime();%>
/*输出无线网设置信息*/
<% getWifiInfo();%>
<%outputChangemodel();%>
//进行时间处理
sysDate=new Date(UTCtoLocal(eval(sysDateTimes),0) * 1000);
Year= sysDate.getFullYear();
Month= sysDate.getMonth();
Day= sysDate.getDate();
if (10 > sysDate.getHours()) {
    Hour="0"+sysDate.getHours();
} else {
    Hour=""+sysDate.getHours();
}
if (10 > sysDate.getMinutes()) {
    Minute="0"+sysDate.getMinutes();
} else {
    Minute=""+sysDate.getMinutes();
}
if (10 > sysDate.getSeconds()) {
    Second="0"+sysDate.getSeconds();
} else {
    Second=""+sysDate.getSeconds();
}
Month1=Month+1;//路由器内部0表示1月，1表示2月……
var systimes=Year.toString()+"-"+Month1.toString()+"-"+Day.toString()+"   "+Hour.toString()+":"+Minute.toString()+":"+Second.toString();

<%aspOutPortStatics("WAN1");%>
if(ChangeModel == 0)
{
    $(document).ready(function(){ 
            $("#sys_time").html(systimes);
            $("#run_time").html(runtimes);
            /*2.4G无线网络设置*/
            var wirelessstatus="<%write(com_close);%>";
            if (WrlessEnables == "on") {
            wirelessstatus = "<%write(enable);%>";
            }
            $("#wireless_status").html(wirelessstatus);
            $("#wireless_name").html(ssids);
            $("#wireless_mac").html(wlanCurMac);
            var wirelesschannel="<%write(com_auto);%>";
            if (channels != 0) {
            wirelesschannel= channels;
            }
            $("#wireless_channel").html(wirelesschannel);
            /*5G无线信息设置*/
            if (switch5G == "1") {
            $(".wifi5G").show();
            var wirelessstatus_5g="<%write(com_close);%>";
            if (WrlessEnables_5g == "on") {
                wirelessstatus_5g = "<%write(enable);%>";
            }
            $("#wireless_status_5g").html(wirelessstatus_5g);
            $("#wireless_name_5g").html(ssids_5g);
            $("#wireless_mac_5g").html(wlanCurMac_5g);
            var wirelesschannel_5g="<%write(com_auto);%>";
            if (channels_5g != 0) {
                wirelesschannel_5g= channels_5g;
            }
            $("#wireless_channel_5g").html(wirelesschannel_5g);
            } else {
                $(".wifi5G").hide();
            }

            var internet_ip="<%getWanIp(1);%>";
            $("#internet_ip").html(internet_ip);

            var internetmask="<%getWanNetmask(1);%>";
            $("#internet_mask").html(internetmask);

            var internetgw="<%getWanGetWay(1);%>";
            $("#internet_gw").html(internetgw);

            var internetdns="<%getWanDns(1, 1);%>";
            $("#internet_dns").html(internetdns);

            var internetmac="<%getWanMac(1);%>";
            $("#internet_mac").html(internetmac);

            var lanip="<%getLanIp();%>";
            $("#lan_ip").html(lanip);

            var lanmask="<%getLanNetmask();%>";
            $("#lan_mask").html(lanmask);

            var lanmac="<%getLanMac();%>";
            $("#lan_mac").html(lanmac);

            var bytesreceive=byteFormat(counters[2]);
            $("#bytes_receive").html(bytesreceive);

            var bytessend=byteFormat(counters[3]);
            $("#bytes_send").html(bytessend);

            var packetreceive=counters[0];
            $("#packet_receive").html(packetreceive);

            var packetsend=counters[1];
            $("#packet_send").html(packetsend);
    });
}else{

 $(document).ready(function(){ 
            $("#sys_time").html(systimes);
            $("#run_time").html(runtimes);
            /*2.4G无线网络设置*/
            var wirelessstatus="<%write(com_close);%>";
            if (WrlessEnables == "on") {
            wirelessstatus = "<%write(enable);%>";
            }
            $("#wireless_status").html(wirelessstatus);
            $("#wireless_name").html(ssids);
            $("#wireless_mac").html(wlanCurMac);
            var wirelesschannel="<%write(com_auto);%>";
            if (channels != 0) {
            wirelesschannel= channels;
            }
            $("#wireless_channel").html(wirelesschannel);
            /*5G无线信息设置*/
            if (switch5G == "1") {
            $(".wifi5G").show();
            var wirelessstatus_5g="<%write(com_close);%>";
            if (WrlessEnables_5g == "on") {
                wirelessstatus_5g = "<%write(enable);%>";
            }
            $("#wireless_status_5g").html(wirelessstatus_5g);
            $("#wireless_name_5g").html(ssids_5g);
            $("#wireless_mac_5g").html(wlanCurMac_5g);
            var wirelesschannel_5g="<%write(com_auto);%>";
            if (channels_5g != 0) {
                wirelesschannel_5g= channels_5g;
            }
            $("#wireless_channel_5g").html(wirelesschannel_5g);
            } else {
                $(".wifi5G").hide();
            }

            var internet_ip="<%getWanIp(1);%>";
            $("#internet_ip").html(internet_ip);

            var internetmask="<%getWanNetmask(1);%>";
            $("#internet_mask").html(internetmask);

            var internetgw="<%getWanGetWay(1);%>";
            $("#internet_gw").html(internetgw);

            var internetdns="<%getWanDns(1, 1);%>";
            $("#internet_dns").html(internetdns);

            var internetmac="<%getWanMac(1);%>";
            $("#internet_mac").html(internetmac);
            
            if(internetgw != "")
            {
                var lanip="<%getLanIp();%>";
                $("#lan_ip").html(lanip);

                var lanmask="<%getLanNetmask();%>";
                $("#lan_mask").html(lanmask);

                var lanmac="<%getLanMac();%>";
                $("#lan_mac").html(lanmac);
            }else{
                var lanip="<%getWanIp(1);%>";
                $("#lan_ip").html(lanip);

                var lanmask="<%getWanNetmask(1);%>";
                $("#lan_mask").html(lanmask);

                var lanmac="<%getWanMac(1);%>";
                $("#lan_mac").html(lanmac);
            }

            var bytesreceive=byteFormat(counters[2]);
            $("#bytes_receive").html(bytesreceive);

            var bytessend=byteFormat(counters[3]);
            $("#bytes_send").html(bytessend);

            var packetreceive=counters[0];
            $("#packet_receive").html(packetreceive);

            var packetsend=counters[1];
            $("#packet_send").html(packetsend);
    });
}
</script>
<body>
<div class="warp">

<br>
	<table border="0" cellpadding="0" cellspacing="1" width="500px" id="runstatusInfo" class="group" style="background:#5e5e5e;" >
    	<tr class="tab_info">
        	<td width="150px"><%write(systemnow);%></td>
            <td colspan="2" id="sys_time"></td>
        </tr>
        <tr class="tab_info">
        	<td><%write(sys_runTime);%></td>
            <td colspan="2" id="run_time"></td>
        </tr>
         <tr class="tab_title">
        	<td colspan="3"><%write(run_wire2);%></td>
           
        </tr>
        <tr class="tab_info">
        	<td><%write(run_wireST);%></td>
            <td colspan="2" id="wireless_status"></td>
        </tr>
        <tr class="tab_info">
        	<td><%write(eas_wireName);%></td>
            <td colspan="2" id="wireless_name"></td>
        </tr>
        <tr class="tab_info">
        	<td><%write(com_MAC);%></td>
            <td colspan="2" id="wireless_mac"></td>
        </tr>
        <tr class="tab_info">
        	<td><%write(Channel);%></td>
            <td colspan="2" id="wireless_channel"></td>
        </tr>
         <tr class="tab_title wifi5G">
        	<td colspan="3"><%write(run_wire5);%></td>
        </tr>
        <tr class="tab_info wifi5G">
        	<td><%write(run_wireST);%></td>
            <td colspan="2" id="wireless_status_5g"></td>
        </tr>
        <tr class="tab_info wifi5G">
        	<td><%write(eas_wireName);%></td>
            <td colspan="2" id="wireless_name_5g"></td>
        </tr>
        <tr class="tab_info wifi5G">
        	<td><%write(com_MAC);%></td>
            <td colspan="2" id="wireless_mac_5g"></td>
        </tr>
        <tr class="tab_info wifi5G">
        	<td><%write(Channel);%></td>
            <td colspan="2" id="wireless_channel_5g"></td>
        </tr>
        <script>
        var internetgw_test="<%getWanGetWay(1);%>";
        if((ChangeModel == 1 && internetgw_test != "") || ChangeModel == 0)
        {
           document.write('<tr class="tab_title">');
           document.write('<td colspan="3"><%write(run_internet);%></td>');           
           document.write('</tr>');
           document.write('<tr class="tab_info">');
           document.write('<td><%write(com_IP);%></td>');
           document.write('<td colspan="2" id="internet_ip"></td>');
           document.write('</tr>');
           document.write('<tr class="tab_info">');
           document.write('<td><%write(com_NetMask);%></td>');
           document.write('<td colspan="2" id="internet_mask"></td>');
           document.write('</tr>');
           document.write('<tr class="tab_info">');
           document.write('<td><%write(run_gateWay);%></td>');
           document.write('<td colspan="2" id="internet_gw"></td>');
           document.write('</tr>');
           document.write('<tr class="tab_info">');
           document.write('<td><%write(run_DNS);%></td>');
           document.write('<td colspan="2" id="internet_dns"></td>');
           document.write('</tr>');
           document.write('<tr class="tab_info">');
           document.write('<td><%write(com_MAC);%></td>');
           document.write('<td colspan="2" id="internet_mac"></td>');
           document.write('</tr>');
        }
    
        if((ChangeModel == 1 && internetgw_test == "") || ChangeModel == 0)
        {
           document.write('<tr class="tab_title">');
           document.write('<td colspan="3"><%write(run_localInt);%></td>');           
           document.write('</tr>');
           document.write('<tr class="tab_info">');
           document.write('<td><%write(com_IP);%></td>');
           document.write('<td colspan="2" id="lan_ip"></td>');
           document.write('</tr>');
           document.write('<tr class="tab_info">');
           document.write('<td><%write(com_NetMask);%></td>');
           document.write('<td colspan="2" id="lan_mask"></td>');
           document.write('</tr>');
           document.write('<tr class="tab_info">');
           document.write('<td><%write(com_MAC);%></td>');
           document.write('<td colspan="2" id="lan_mac"></td>');
           document.write('</tr>');
        }

        </script>
        <tr class="tab_title">
        	<td colspan="3"><%write(usr_totalBit);%></td>           
        </tr>
        <tr class="tab_info">
        	<td><%write(run_BitType);%></td>
            <td><%write(run_rece);%></td>
            <td><%write(run_send);%></td>
        </tr>
        <tr class="tab_info">
        	<td><%write(run_Bits);%></td>
            <td id="bytes_receive"></td>
            <td id="bytes_send"></td>
        </tr>
        <tr class="tab_info">
        	<td><%write(run_date);%></td>
            <td id="packet_receive"></td>
            <td id="packet_send"></td>
        </tr>
    </table>
    <table border="0" cellpadding="0" cellspacing="1" style="background:#5e5e5e;" class="group" >
	<tr><td  class="tab_savetd" align="right"><span><input type="button" class="btn" onClick="location.reload();"  value="<%write(com_fresh);%>"/></span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td></tr>
    </table>
<br>
<br>
</div>
</body>
</html>
