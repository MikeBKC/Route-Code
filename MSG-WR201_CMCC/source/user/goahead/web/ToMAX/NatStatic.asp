<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("NatStatic");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

//Butterlate.setTextDomain("firewall");

//function initTranslation()
//{
//	var e;  
//
//	e = document.getElementById("add");
//	e.value= _("add");
//	e = document.getElementById("delall");
//	e.value= _("delall");
//	e= document.getElementById("del");
//	e.value= _("del");
//
//}

var strSecondMenuFocus="<%write(com_staticMap);%>";
<% aspOutNatStaticMap(); %>
<%getWanIfCount();%>
 
//var IDs=new Array();var ConfigEnables = new Array();var Protocols = new Array();var OutPorts=new Array();var IPs=new Array();var InnerPorts=new Array();var PortNums=new Array();var NatBinds=new Array();var totalrecs=0;var errorstr="";

//var max_totalrecs=eval("50");
 
if ((typeof(snmp) == 'undefined')){
    var  snmp = 0;
}
var ConfigEnablesw=new Array();
for(i=0;i<totalrecs;i++){
	/* 无线接口 */
	var slotIndex = NatBinds[i].slice(NatBinds[i].length - 1);
	if (is3GPort(slotIndex,wanIfCount)) {
		NatBinds[i] = "3G";
	} else if (isAPCliPort(slotIndex, wanIfCount)) {
		NatBinds[i] = "APClient";
	}
	
	ConfigEnablesw[i]=(ConfigEnables[i]=="1"?"<%write(com_startUse);%>":"<%write(com_forbidden);%>");
	Protocols[i]=(Protocols[i]=="1"?"TCP":(Protocols[i]=="2"?"UDP":"TCP/UDP"));
}
recsPerPage[0] =10;
lineButton[0]=10;
currpage[0]=1;
recscount[0]=totalrecs;
maxrecs[0]=max_totalrecs;
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]="<%write(nat_staticList);%>";
var titles1=new Array(7);
titles1[0]="<%write(nat_staMapName);%>";
titles1[1]="<%write(com_status);%>";
titles1[2]="<%write(com_protocol);%>";
titles1[3]="<%write(nat_exterFPort);%>";
titles1[4]="<%write(com_IP);%>";
titles1[5]="<%write(nat_innerFPort);%>";
titles1[6]="<%write(nat_portNum);%>";
titles1[7]="<%write(nat_NATBind);%>";


arrayData1=new Array(IDs,ConfigEnablesw,Protocols,OutPorts,IPs,InnerPorts,PortNums,NatBinds,ConfigEnables);
sortable[0]=new Array(1,1,1,1,1,1,1,1,1);
sortType[0]=new Array(TYPE_STR,TYPE_CHINESE,TYPE_STR,TYPE_NUM,TYPE_IP,TYPE_NUM,TYPE_NUM,TYPE_STR,TYPE_STR);
arrayCount[0]=9;

function dsplist(index,list)
{
	if(index==0) dspList(index,list,920,titles1,8,arrayData1,1,1);
}
/******************list***********************/
function EditConfig(val,i){
	if(val==0){ 
		NatStatic_List.id.value="";
		if(eval(max_totalrecs-totalrecs)<1)	{
			alert("<%write(com_editAlert);%>");
			return;
		}
	}
	else{
		if(IDs[i].toLowerCase()=="admin"){
			alert("<%write(nat_editAlert);%>");
			return ;
		}
		/* 支持l2tp */
		if (L2TPList == 1) {
			if (IDs[i].toLowerCase()=="l2tp") {
				alert("<%write(nat_editAlert1);%>");
				return;
			}
		}
		/* 支持pptp */
		if (PPTPClientList == 1) {
			if (IDs[i].toLowerCase()=="pptp") {
				alert("<%write(nat_editAlert1);%>");
				return;
			}
		}
		/* 支持snmp */
		if (snmp == 1) {
			if (IDs[i].toLowerCase()=="snmp") {
				alert("<%write(nat_editAlert1);%>");
				return;
			}
		}

		NatStatic_List.id.value=IDs[i];
	}
	NatStatic_List.action="NatStatic_edit.asp";
	NatStatic_List.submit();
}
function DeleteConfig(index){
	if(IDs[index].toLowerCase()=="admin"){
		alert("<%write(nat_editAlert2);%>");
		return;
	}
	if(confirm("<%write(com_deleteTip);%>")){
		NatStatic_List.delstr.value=IDs[index];
		NatStatic_List.action="/goform/formNatStaticMapDel";
	    NatStatic_List.submit();
	}
}
function delAll(){
	if(confirm("<%write(com_deleteAllTip);%>")){
		NatStatic_List.action="/goform/formNatStaticMapDelAll";
		NatStatic_List.submit();
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
  <td><script language="javascript">showSecondTitle(_("<%write(com_staticMap);%>"));</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center" class="table_bg" >
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td><form name="NatStatic_List" method="post" action="/goform/formNatStaticMapDel">
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"95%",
						3,
						new Array(_("<%write(com_staticMap);%>"),'NatStatic.asp',1,1),
						new Array(_("<%write(com_NATRule);%>"),'NatRule.asp',1,0),
						new Array("DMZ",'DMZ.asp',1,0)
		);
		showThreeMenu();
	</script>
   <table id=grouplist style="display" width="95%"  border="0" align="center" cellpadding="0" cellspacing="0" height="470">
  <tr>
    <td align="center" class="three_td" valign="top"><table id=grouplist style="display" width="600"  border="0" align="center" cellpadding="0" cellspacing="0">
	<tr><td colspan="4" height="40">&nbsp;</td></tr>
		    <tr bgcolor="c0ddff">
		      <td colspan="4"><iframe src="_listhead1.htm" name="frame11" scrolling="no" align="MIDDLE" width="920" height="45" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden ></iframe></td>
		    </tr>
		    <tr bgcolor="c0ddff">
		      <td colspan="4"><iframe src="_list1.htm" name="frame12" id="framelist" align="MIDDLE" width="920" height="265" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe></td>
		    </tr>
		    <tr>
		      <td  colspan="4" valign="bottom"><table id=dellist  width="100%"  height="19" border="0" cellspacing="0" cellpadding="1">
			  <tr>
			  <td width="5"></td>
			  <td  class="style6"><div align="left">
				<input type="checkbox" name="delchk" onClick=selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist'));>
		<script language="javascript">document.write(_("<%write(com_select);%>"));</script>		</td>
			  <td  class="style6" ><input type="hidden" name="delstr" size=10 ><input type="hidden" name="id"></td> 
			   <td width="90" valign="top" align="right"><input type="button" value="<%write(com_addNew);%>" class="input-button" onClick="EditConfig(0)" id="add"></td>
			   <td width="100" valign="top" align="right"><input type="button" value="<%write(com_delAll);%>" class="input-button" onClick="delAll()" id="delall"></td>
			 <td width="40" valign="top" align="right"><input id="del" type="submit" value="<%write(com_delete);%>" class="input-button" onClick="return(genData(NatStatic_List,0,frame12,IDs,'<%write(nat_staticInfo);%>'))"></td>
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
