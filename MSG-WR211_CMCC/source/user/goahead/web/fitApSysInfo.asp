<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("fitAPSysInfo");%>
<%langTran("commonWords");%>
<title><%write(utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

//交互begin
<%
	aspOutPutApBasicInfoUrcp();
//	write('var deviceName = "testAP";\n');
//	write('var softVer = "WA1800Nv1.0.0-120528";\n');
//	write('var firmVer = "V2.0";\n');
//	write('var serialNo = "11460053";\n');
//	write('var macForInterface = "0022aa5e8f1b";\n');
//	write('var ipAddr = "192.168.1.254";\n');
//	write('var subMask = "255.255.255.0";\n');
//	write('var gateway = "192.168.1.1";\n');
//	write('var macForWlan = "0022aa5e8f1a";\n');
//	write('var channel = 6;\n');
//	write('var wlanMode = "11b/g/n混合";\n');
//	write('var rate = "300M";\n');
//	write('var SZ = "SZ_1";\n');
//	write('var ssid = "SSID_A";\n');
//	write('var workingMode = "瘦AP";\n');
//	write('var cltsCount = 2;\n');
%>
//交互end


function init()
{
	var d;
	d = document.getElementById("devName_value");
	d.innerHTML = deviceName;
	d = document.getElementById("softVer_value");
	d.innerHTML = softVer;
	d = document.getElementById("hardVer_value");
	d.innerHTML = firmVer;
	d = document.getElementById("serialNo_value");
	d.innerHTML = serialNo;
	d = document.getElementById("macForWlan_value");
	d.innerHTML = macForWlan;
	d = document.getElementById("channel_value");
	d.innerHTML = channel;
	d = document.getElementById("wireMode_value");
	d.innerHTML = wlanMode;
	d = document.getElementById("wireRate_value");
	d.innerHTML = rate;
	d = document.getElementById("macForInterface_value");
	d.innerHTML = macForInterface;
	d = document.getElementById("ip_value");
	d.innerHTML = ipAddr;
	d = document.getElementById("mask_value");
	d.innerHTML = subMask;
	d = document.getElementById("gateway_value");
	d.innerHTML = gateway;
	d = document.getElementById("svcZone_value");
	d.innerHTML = SZ;
	d = document.getElementById("ssid_value");
	d.innerHTML = ssid;
	d = document.getElementById("workmode_value");
	if (workingMode == 0)	//fit
	{
		d.innerHTML = _("FIT AP");
	}
	else	//fat
	{
		d.innerHTML = _("FAT AP");
	}
	d = document.getElementById("cltS_value");
	d.innerHTML = cltsCount;
	
}

</script>
<style type="text/css">
.style24 {border:1px solid #408080}
</style>
</head>
<body onLoad="init();" onbeforeunload="logOut()"  style="background:url(images/body_main.png);  background-repeat:repeat-x">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle(_("<%write(sysInfo);%>"));</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">//showerror(errorstr);</script>
  <tr valign="top">
   <td>
<form name="fitApSysInfo" method="post">
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
  <tr>
    <td align="center" class="style7">
    <br><br>
    <table width="600" border="0" cellspacing="0" cellpadding="0">
      <tr>
        <td height="" valign="top"><table width="280" border="0" align="center"  cellpadding="4" cellspacing="1" class="style22">
          <tr class="style18">
            <td colspan="2"><strong id="baseInfor_td"><%write(baseInfo);%></strong></td>
            </tr>
          <tr>
            <td width="<% write(sys_width);%>" id="devName_td"><%write(devName);%></td>
            <td id="devName_value"></td>
          </tr>
          <tr class="style18">
            <td id="softVer_td"><%write(softVer);%></td>
            <td id="softVer_value"></td>
          </tr>
          <tr>
            <td id="hardVer_td"><%write(hardVer);%></td>
            <td id="hardVer_value"></td>
          </tr>
          <tr class="style18">
            <td id="serialNo_td"><%write(serialNum);%></td>
            <td id="serialNo_value"></td>
          </tr>
        </table></td>
        <td>&nbsp;</td>
        <td valign="top"><table width="280" border="0" align="center"  cellpadding="4" cellspacing="1" class="style22">
          <tr class="style18">
            <td colspan="2"><strong id="wrieST_td"><%write(wireST);%></strong></td>
            </tr>
          <tr>
            <td width="<% write(sys_width);%>" id="mac_td1"><%write(macAddr);%></td>
            <td id="macForWlan_value"></td>
          </tr>
          <tr class="style18">
            <td id="channel_td"><%write(channel);%></td>
            <td id="channel_value"></td>
          </tr>
          <tr>
            <td id="wireMode_td"><%write(wireMode);%></td>
            <td id="wireMode_value"></td>
          </tr>
          <tr class="style18">
            <td id="wireRate_td"><%write(wireRate);%></td>
            <td id="wireRate_value"></td>
          </tr>
        </table></td>
      </tr>
      <tr>
        <td height="18">&nbsp;</td>
        <td>&nbsp;</td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td valign="top"><table width="280" border="0" align="center"  cellpadding="4" cellspacing="1" class="style22">
          <tr class="style18">
            <td colspan="2"><strong id="portInfor_td"><%write(portInfo);%></strong></td>
            </tr>
          <tr>
            <td width="<% write(sys_width);%>" id="mac_td2"><%write(macAddr);%></td>
            <td id="macForInterface_value"></td>
          </tr>
          <tr class="style18">
            <td id="ip_td"><%write(ipAddr);%></td>
            <td id="ip_value"></td>
          </tr>
          <tr>
            <td id="mask_td"><%write(subMask);%></td>
            <td id="mask_value"></td>
          </tr>
          <tr class="style18">
            <td id="gateway_td"><%write(gateway);%></td>
            <td id="gateway_value"></td>
          </tr>
        </table></td>
        <td>&nbsp;</td>
        <td valign="top"><table width="280" border="0" align="center"  cellpadding="4" cellspacing="1" class="style22">
          <tr class="style18">
            <td colspan="2"><strong id="APSt_td"><%write(APST);%></strong></td>
            </tr>
          <tr>
            <td width="<% write(sys_width);%>" id="svcZone_td"><%write(SZ);%></td>
            <td style="word-break:break-all; word-wrap:break-all;" id="svcZone_value"></td>
          </tr>
          <tr class="style18">
            <td>SSID</td>
            <td style="word-break:break-all; word-wrap:break-all;" id="ssid_value"></td>
          </tr>
          <tr>
            <td id="workmode_td"><%write(workMode);%></td>
            <td id="workmode_value"></td>
          </tr>
          <tr class="style18">
            <td id="cltS_td"><%write(cltS);%></td>
            <td id="cltS_value"></td>
          </tr>
        </table></td>
      </tr>
    </table>
    <br>
    <table width="600" border="0" align="center"  cellpadding="0" cellspacing="0">
    <tr>
    <td align="right">
    <input type="button" value="<%write(refresh);%>" class="input-button" onClick="location.reload()" id="refresh">&nbsp;&nbsp;&nbsp;
    </td>
    </tr>
    </table>
    <br><br>
    </td>
  </tr>
</table>
</form>
</td>
  </tr>
</table></td>
  <td width="15"></td>
  </tr>
</table>
</body>
</html>
