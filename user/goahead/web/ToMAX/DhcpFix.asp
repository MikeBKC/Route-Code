<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("DHCP"); %>
<% langTran("common"); %>
<title><% write(com_utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

<%getSysLang();%>
var strSecondMenuFocus="<% write(StaticDHCP); %>";
<% outputDhcpStaticList();  %>

//交互变量
/*
var FixIPs=new Array();
var FixUserNames=new Array();
var FixMacs=new Array();
*/

recsPerPage[0] =10;
lineButton[0]=10;
currpage[0]=1;
recscount[0]=totalrecs;
maxrecs[0]=max_totalrecs;
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]="<% write(StaticDHCPList); %>";
var titles1=new Array(3);
titles1[0]="<% write(com_user); %>";
titles1[1]="<% write(IPAddr); %>";
titles1[2]="<% write(MACAddr); %>";


arrayData1=new Array(FixUserNames,FixIPs,FixMacs);
sortable[0]=new Array(1,1,1,1);
sortType[0]=new Array(TYPE_STR,TYPE_IP,TYPE_STR);
arrayCount[0]=3;

<!--
function dsplist(index,list)
{
	if(index==0) dspList(index,list,920,titles1,3,arrayData1,1,1);
}
/******************list***********************/
function EditConfig(val,i){
	if(val==0) 
	{	
		FixDhcp_List.id.value="";
		if(eval(max_totalrecs-totalrecs)<1)	{
			alert("<% write(MAXTitleConf); %>");
			return;
		}
	}
	else FixDhcp_List.id.value=FixUserNames[i];
	FixDhcp_List.action="DhcpFix_edit.asp";
	FixDhcp_List.submit();
}
function DeleteConfig(index){
	if(confirm("<% write(com_warning3); %>")){
		FixDhcp_List.delstr.value=FixUserNames[index];
		FixDhcp_List.action="/goform/formDhcpListStaticDel";
	    FixDhcp_List.submit();
	}
}
function delAll(){
	if(confirm("<% write(com_warning4); %>")){
		FixDhcp_List.action="/goform/formDhcpListStaticDelAll";
		FixDhcp_List.submit();
	}
}
-->
</script>
</head>
<body onbeforeunload="logOut()">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle("<% write(DHCP); %>");</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center" class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td><form name="FixDhcp_List" method="post" action="/goform/formDhcpListStaticDel">
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"95%",
						4,
						new Array("<% write(DHCPset); %>",'DhcpServer.asp',1,0),
						new Array("<% write(StaticDHCP); %>",'DhcpFix.asp',1,1),
						new Array("<% write(DHCPAutoBind); %>",'DhcpAutoBind.asp',1,0),
						new Array("<% write(DHCPClient); %>",'DhcpClient.asp',1,0)
		);
		showThreeMenu();
	</script>
   <table id=grouplist style="display" width="95%"  border="0" align="center" cellpadding="0" cellspacing="0" height="300">
  <tr>
    <td align="center" class="three_td" valign="top"><table id=grouplist style="display" width="600"  border="0" align="center" cellpadding="0" cellspacing="0">
	<tr><td colspan="4" height="40">&nbsp;</td></tr>
		    <tr>
		      <td colspan="4"><iframe src="_listhead1.htm" name="frame11" scrolling="no" align="MIDDLE" width="920" height="45" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden ></iframe></td>
		    </tr>
		    <tr>
		      <td colspan="4"><iframe src="_list1.htm" name="frame12" id="framelist" align="MIDDLE" width="920" height="270" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe></td>
		    </tr>
		    <tr>
		      <td  colspan="4" valign="bottom"><table id=dellist  width="100%"  height="19" border="0" cellspacing="0" cellpadding="1">
			  <tr>
			  <td width="5"></td>
			  <td  class="style6"><div align="left">
				<input type="checkbox" name="delchk" onClick=selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist'));>
				<script language="javascript">document.write("<% write(com_select); %>");</script>
		</div></td>
			  <td  class="style6" ><input type="hidden" name="delstr" size=10 ><input type="hidden" name="id"></td> 
			   <td width="90" valign="top" align="right"><input type="button" value="<% write(com_addNew); %>" class="input-button" onClick="EditConfig(0)" id="add"></td>
			  <td width="95" valign="top" align="right"><input type="button" value="<% write(com_delAll); %>" class="input-button" onClick="delAll()" id="delall"></td>
			 <td width="45" valign="top" align="right"><input type="submit" value="<% write(com_delete); %>" class="input-button" onClick="return(genData(FixDhcp_List,0,frame12,FixUserNames,'<% write(StaticDHCPMsg); %>'))" id="del">
			 <br></td>
			  </tr>
				 <tr>
		      <td  colspan="4" height="30" >&nbsp;</td></tr>
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
