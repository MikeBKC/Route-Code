<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("commonWords");%>
<%langTran("DhcpFix");%>

<title><%write(utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

<%
	outputDhcpStaticList();
/*
	write('var pool = new Array("default", "pool_2");\n');
	write('var FixUserNames=new Array("user1", "user2");\n');
	write('var FixIPs = new Array("192.168.20.100", "172.16.16.100");\n');
	write('var FixMacs = new Array("0022aa5e8f11", "0022aa6581c2");\n');
	write('var totalrecs=2;\n');
	write('var max_totalrecs=256;\n');
	write('var errorstr = "";\n');
*/
%>
var strSecondMenuFocus=_("<%write(staticDhcp);%>");

//交互变量
/*
var FixIPs=new Array();
var FixUserNames=new Array();
var FixMacs=new Array();
*/

recsPerPage[0] =5;
lineButton[0]=0;
currpage[0]=1;
recscount[0]=totalrecs;
maxrecs[0]=max_totalrecs;
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]=_("<%write(staticDhcpList);%>");
var titles1=new Array();
titles1[0]=_("<%write(name);%>");
titles1[1]=_("<%write(poolName);%>");
titles1[2]=_("<%write(poolVid);%>");
titles1[3]=_("<%write(ipAddr);%>");
titles1[4]=_("<%write(macAddr);%>");


arrayData1=new Array(FixUserNames,pool,FixVids,FixIPs,FixMacs);
sortable[0]=new Array(1,1,1,1,1,1);
sortType[0]=new Array(TYPE_STR,TYPE_STR,TYPE_NUM,TYPE_IP,TYPE_STR);
arrayCount[0]=5;

<!--
function dsplist(index,list)
{
	if(index==0) dspList(index,list,600,titles1,titles1.length,arrayData1,1,1);
}
/******************list***********************/
function EditConfig(val,i){
	if(val==0) 
	{	
		FixDhcp_List.id.value="";
		if(eval(max_totalrecs-totalrecs)<1)	{
			alert(_("<%write(maxConfigTips);%>"));
			return;
		}
	}
	else FixDhcp_List.id.value=FixUserNames[i];
	FixDhcp_List.action="DhcpFix_editForAc.asp";
	FixDhcp_List.submit();
}
function DeleteConfig(index){
	if(confirm(_("<%write(warning3);%>"))){
		FixDhcp_List.delstr.value=FixUserNames[index];
		FixDhcp_List.action="/goform/formDhcpListStaticDel";
	    FixDhcp_List.submit();
	}
}
function delAll(){
	if(confirm(_("<%write(warning4);%>"))){
		FixDhcp_List.action="/goform/formDhcpListStaticDelAll";
		FixDhcp_List.submit();
	}
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
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center" class="table_bg" >
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td><form name="FixDhcp_List" method="post" action="/goform/formDhcpListStaticDel">
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
						new Array(_("<%write(staticDhcp);%>"),'DhcpFixForAc.asp',1,1),
						new Array(_("<%write(dhcpClient);%>"),'DhcpClientForAc.asp',1,0),
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
		      <td colspan="4"><iframe src="_list1.htm" name="frame12" id="framelist" align="MIDDLE" width="600" height="140" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe></td>
		    </tr>
		    <tr>
		      <td  colspan="4" valign="bottom">
              <table id=dellist  width="100%"  height="19" border="0" cellspacing="0" cellpadding="1">
			  <tr>
			  <td  class="style6"><div align="left">
				<input type="checkbox" name="delchk" onClick=selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist'));>
				<script language="javascript">document.write(_("<%write(allSelect);%>/<%write(allUnselect);%>"));</script>
		</div></td>
			  <td valign="top" align="right" class="style6" ><input type="hidden" name="delstr" size=10 ><input type="hidden" name="id">
			   <input type="button" value="<%write(addNew);%>" class="input-button" onClick="EditConfig(0)" id="add">
			  <input type="button" value="<%write(delAll);%>" class="input-button" onClick="delAll()" id="delall">
			 <input type="submit" value="<%write(del);%>" class="input-button" onClick='return(genData(FixDhcp_List,0,frame12,FixUserNames,_("<%write(staticDHCPMsg);%>")))' id="del"></td>
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
