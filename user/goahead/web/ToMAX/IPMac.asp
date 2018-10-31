
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("IPMac");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

//Butterlate.setTextDomain("firewall");

//function initTranslation()
//{
//	var e;
//
//	e = document.getElementById("uttsave");
//	e.value= _("save");
//	e = document.getElementById("add");
//	e.value= _("add");
//	e = document.getElementById("delall");
//	e.value= _("delall");
//	e= document.getElementById("del");
//	e.value= _("del");
//
//	e = document.getElementById("attention1");
//	e.innerHTML= _("attention1")
//
//}
<% aspOutArpBindGlobalConfig();
	aspOutArpBindList();
%>

var strSecondMenuFocus="<%write(com_bandList);%>";
//交互变量
/*
var UserNames=new Array();
var IPs=new Array();
var Macs=new Array();
var Allows=new Array();
//var AllowOtherEnables="on";
var totalrecs=1;
var max_totalrecs=128;

UserNames[0]="cxhu";
IPs[0]="1.1.1.1";
Macs[0]="001122334455";
Allows[0]="yes";
*/
recsPerPage[0]=10;
lineButton[0]=10;
currpage[0]=1;
recscount[0]=totalrecs;
maxrecs[0]=max_totalrecs;
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]="<%write(ipm_bandInfo);%>";
var titles1=new Array(3);
titles1[0]="<%write(com_user);%>";
titles1[1]="<%write(com_IP);%>";
titles1[2]="<%write(com_MAC);%>";
titles1[3]="<%write(com_allow);%>";

arrayData1=new Array(UserNames,arp_IPs,arp_Macs,Allows);
sortable[0]=new Array(1,1,1,1);
sortType[0]=new Array(TYPE_STR,TYPE_IP,TYPE_STR,TYPE_STR);
arrayCount[0]=4;

<!--
function dsplist(index,list)
{
	if(index==0) dspList(index,list,920,titles1,4,arrayData1,1,1);
}
/******************list***********************/
function SaveUserAllow(index,i)
{
	if (Allows[i].toLowerCase()=="no")　//复选框原来的值是"DENY"，即不选中，那么点击后调用本函数时状态应该是选中
	{
		str="<%write(com_allow);%>";
		ARPBind_List.Allow.value="yes";
	}
	else
	{
		str="<%write(com_ban);%>";
		ARPBind_List.Allow.value="no";
	}
	ARPBind_List.AllowID.value=UserNames[i];
	ARPBind_List.AllowIP.value=arp_IPs[i];
	ARPBind_List.AllowMAC.value=arp_Macs[i].replace(/:/g,"");
	if (confirm("<%write(com_really);%>"+str+"<%write(com_relNet);%>"))
	{
		 ARPBind_List.action ="/goform/formArpBindAllow";
		 ARPBind_List.submit();
	}
	else
	{
		dsplisthead(index,frame11);
    	dsplist(index,frame12);
	}
}
function init(){
	var f=ARPBindGloable_Config;
	f.AllowOtherEnable.checked=(AllowOtherEnables=="on"?true:false);
}
function save(){
	ARPBindGloable_Config.action="/goform/formArpBindGlobalConfig";
	ARPBindGloable_Config.submit();
}
function EditConfig(val,i){
	if(val==0){ 
		ARPBind_List.id.value="";
		if(eval(max_totalrecs-totalrecs)<1)	{
			alert("<%write(com_editAlert);%>");
			return;
		}
	}
	else  ARPBind_List.id.value=UserNames[i];
	ARPBind_List.action="IPMac_edit.asp";
	ARPBind_List.submit();
}
function DeleteConfig(index){
	if(confirm("<%write(com_deleteTip);%>")){
		ARPBind_List.delstr.value=UserNames[index];
		ARPBind_List.action="/goform/formArpBindDel";
	    ARPBind_List.submit();
		
	}
}
function delAll(){
	if(confirm("<%write(com_deleteAllTip);%>")){
		ARPBind_List.action="/goform/formArpBindDelAll";
		ARPBind_List.submit();
	}
}

function addconfig(){
	parent.main.location='IPMacConfig.asp';
}

function delIpMac(){
	if (genData(ARPBind_List,0,frame12,UserNames,'<%write(ipm_IPMacInfo);%>') == true){
	    ARPBind_List.action="/goform/formArpBindDel";
	    ARPBind_List.submit();
	}
}

function ExportIPMac(){
	ARPBind_List.action="/goform/formIPMacExport";
	ARPBind_List.submit();
}
function delIPMac(){
	genData(ARPBind_List,0,frame12,UserNames,'<%write(ipm_IPMacInfo);%>');
	ARPBind_List.action="/goform/formArpBindDel";
	ARPBind_List.submit();
}
//-->
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
  <td><script language="javascript">showSecondTitle("<%write(ipm_ipmBand);%>");</script>
  <table width="100%" border="0" height="550" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td>
 
<p>&nbsp;</p>

	 <table width="95%" border="0" cellspacing="0" cellpadding="0" align="center">
<tr>
    <td align="center"  class="style7"><table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td>
	<table border="0" width="80%" align='center'>
	<form action="" onSubmit="return " name="ARPBindGloable_Config" method="post">
	<tr><td colspan="3" height="20">&nbsp;<input type="hidden" name="arpb" value="1"></td></tr>
<tr>
  <td width="45%" height="25" align="right" valign="middle" id="attention1"><%write(ipm_allow);%></td>
  <td width="2%" height="25">&nbsp;</td>
  <td align="left"  height="25" valign="middle"><input type="checkbox" name="AllowOtherEnable">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_save);%>" onClick="save()" id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button" value="<%write(com_help);%>" class="input-button" onClick="window.open('<% write(com_helppage); %>#IPMac')"></td>
  </tr>
  <tr><td colspan="3" align="center" height="40"><table width=920 border="0" cellspacing="0" cellpadding="0">
  <tr>
   <td background="<%write(com_imgdot);%>"><img src="<%write(com_imgspace);%>" width="8" height="1"></td>
  </tr>
</table>
</td></tr>
  </form></table></td>
  </tr>
  <tr><td>
<table border="0"  align='center' width="80%">
<form name="ARPBind_List" method="post" action="/goform/formArpBindDel">
   <table id=grouplist style="display"  border="0" align="center" cellpadding="0" cellspacing="0">
		    <tr bgcolor="c0ddff">
		      <td colspan="4">
			<iframe src="_listhead1.htm" name="frame11" scrolling="no" align="MIDDLE" width="920" height="45" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden ></iframe></td>
		    </tr>
		    <tr bgcolor="c0ddff">
		      <td colspan="4">
			<iframe src="_list1.htm" name="frame12" id="framelist"	align="MIDDLE" width="920" height="265" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe></td>
		    </tr>
		    <tr>
		      <td  colspan="4" valign="bottom"><table id=dellist  width="920"  height="19" border="0" cellspacing="0" cellpadding="1">
			  <tr>
			  <td width="5"></td>
			  <td  class="style6"><div align="left">
				<input type="checkbox" name="delchk" onClick=selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist'));>
		<script language="javascript">document.write(_("<%write(com_select);%>"));</script></div></td>
			  <td  class="style6" ><input type="hidden" name="delstr" size=10 ><input type="hidden" name="id"><input type="hidden" name="AllowID" ><input type="hidden" name="AllowIP"><input type="hidden" name="AllowMAC"><input type="hidden" name="Allow"></td> 
			  <td width="90" valign="top" align="right"><input type="button" value="<%write(ipm_export);%>" class="input-button" onClick="ExportIPMac()" id="export"></td>
			  <td width="90"   valign="top" align="right"><input type="button" value="<%write(com_addNew);%>" class="input-button" onClick="addconfig()" id="add"></td>
			  <td   width="95"  valign="top" align="right"><input type="button" value="<%write(com_delAll);%>" class="input-button" onClick="delAll()" id="delall"></td>
			 <!-- <td   width="45"   valign="top" align="right"><input type="submit" value="<%write(com_delete);%>" class="input-button" onClick="return(genData(ARPBind_List,0,frame12,UserNames,'<%write(ipm_IPMacInfo);%>'))" id="del"></td>-->
			  <td   width="45"   valign="top" align="right"><input type="button" value="<%write(com_delete);%>" class="input-button" onClick="delIPMac()" id="del"></td>
			  </tr>
		      </table></td>
		    </tr>
			<tr><td colspan="4" height="20">&nbsp;</td></tr>
		  </table>
  </form>
</table></td></tr>
</table> 
  </td></tr><script language="javascript">init();</script>
</table></td>
<!--////end-->
  <td width="15"></td>
  </tr>
</table>
</body>
</html>
