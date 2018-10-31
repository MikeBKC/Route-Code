<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("commonWords");%>
<%langTran("DhcpClient");%>
<title><%write(utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

var errorstr="";
var strSecondMenuFocus=_("<%write(DhcpList);%>");
<% aspDhcpCliList(); %>
recsPerPage[0] =10;
lineButton[0]=0;
currpage[0]=1;
recscount[0]=totalrecs;
maxrecs[0]=totalrecs;
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]=_("<%write(DhcpList);%>");
var titles1=new Array();
titles1[0]=_("<%write(vid);%>");
titles1[1]=_("<%write(ipAddr);%>");
titles1[2]=_("<%write(subnet);%>");
titles1[3]=_("<%write(macAddr);%>");
titles1[4]=_("<%write(remainTime);%>");


arrayData1=new Array(UserVids,UserIPs,UserMasks,UserMacs,UserLeftTimes);
sortable[0]=new Array(1,1,1,1,1);
sortType[0]=new Array(TYPE_NUM,TYPE_IP,TYPE_IP,TYPE_STR,TYPE_NUM);
arrayCount[0]=5;

<!--
function dsplist(index,list)
{
	if(index==0) dspList(index,list,600,titles1,titles1.length,arrayData1,0,0);
}
-->
</script>
</head>
<body onbeforeunload="logOut()" background="images/body_main.png">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle(_("<%write(DHCP);%>"));</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center" class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td><form name="DhcpClient_List" method="post" action="">
	<p>&nbsp;</p>
	<script language="javascript">
        if((platform == "WX20S")){
            var tmp = 0;
        }else{
            var tmp = 1;
        }
		var arrayThreeMenu=new Array(
						"80%",
						4,
						new Array(_("<%write(dhcppool);%>"),'DhcpPoolForAc.asp',1,0),
//						new Array(_("<%write(dhcpSvcSet);%>"),'DhcpServerForAc.asp',0,0),
						new Array(_("<%write(staticDhcp);%>"),'DhcpFixForAc.asp',1,0),
						new Array(_("<%write(dhcpClient);%>"),'DhcpClientForAc.asp',1,1),
						new Array(_("<%write(dnsSet);%>"),'DnsProxyForAc.asp',tmp,0)
		);
		showThreeMenu();
	</script>
   <table id=grouplist style="display" width="80%"  border="0" align="center" cellpadding="0" cellspacing="0" height="300">
  <tr>
    <td align="center" class="three_td" valign="top"><table id=grouplist style="display" width="600"  border="0" align="center" cellpadding="0" cellspacing="0">
	<tr><td colspan="4" height="40">&nbsp;</td></tr>
		    <tr>
		      <td colspan="4"><iframe src="_listhead1.htm" name="frame11" scrolling="no" align="MIDDLE" width="600" height="45" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden ></iframe></td>
		    </tr>
		    <tr>
		      <td colspan="4"><iframe src="_list1.htm" name="frame12"	id="framelist" align="MIDDLE" width="600" height="245" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe></td>
		    </tr>
		    <tr>
		      <td  colspan="4" valign="bottom"><table id=dellist  width="100%"  height="19" border="0" cellspacing="0" cellpadding="1">
			  <tr>
			  <td width="5"></td>
			  <td  class="style6">&nbsp;</td>
			  <td  class="style6" ><input type="hidden" name="delstr" size=10 ><input type="hidden" name="id"></td> 
			  <td width="40" valign="top" align="right" height="35"><input type="button" value="<%write(refresh);%>" class="input-button" onClick="location.reload()" id="update"></td>
			  </tr>
		      </table></td>
		    </tr>
		  </table></td></tr></table>
  </form></td>
  </tr>
</table></td>
  <td width="15"></td>
  </tr>
</table>
</body>
</html>
