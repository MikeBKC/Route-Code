<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("NatRule");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">
//Butterlate.setTextDomain("firewall");

var strSecondMenuFocus="<%write(com_NATRule);%>";
<%aspOutNatRule(); %>
<%getWanIfCount();%>

//var RuleIDs=new Array();var NatTypes = new Array();var OutIPs = new Array();var InFromIPs=new Array();var InEndIPs=new Array();var Binds=new Array();var totalrecs=0;var errorstr="";
 

 
var NatTypesw=new Array();
for(i=0;i<totalrecs;i++){
	/* 绑定无线接口 */
	var slotIndex = Binds[i].slice(Binds[i].length - 1);
	if (is3GPort(slotIndex,wanIfCount)) {
		Binds[i] = "3G";
	} else if (isAPCliPort(slotIndex, wanIfCount)) {
		Binds[i] = "APClient";
	}
	NatTypesw[i]=(NatTypes[i]=="2"?"One2One":(NatTypes[i]=="3"?"Passthrough":"EasyIP"));
}
recsPerPage[0] =10;
lineButton[0]=10;
currpage[0]=1;
recscount[0]=totalrecs;
maxrecs[0]=max_totalrecs;
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]="<%write(nar_natRuleList);%>";
var titles1=new Array(5);
titles1[0]="<%write(nar_ruleName);%>";
titles1[1]="<%write(nar_natType);%>";
titles1[2]="<%write(nar_exterIP);%>";
titles1[3]="<%write(com_innerStartIP);%>";
titles1[4]="<%write(com_innerEndIP);%>";
titles1[5]="<%write(nar_bind);%>";


arrayData1=new Array(RuleIDs,NatTypesw,OutIPs,InFromIPs,InEndIPs,Binds,NatTypes);
sortable[0]=new Array(1,1,1,1,1,1,1);
sortType[0]=new Array(TYPE_STR,TYPE_STR,TYPE_IP,TYPE_IP,TYPE_IP,TYPE_STR,TYPE_STR);
arrayCount[0]=7;

<!--
function dsplist(index,list)
{
	if(index==0) dspList(index,list,920,titles1,6,arrayData1,1,1);
}
/******************list***********************/
function EditConfig(val,i){
	if(val==0){ 
		NatRule_List.id.value="";
		if(eval(max_totalrecs-totalrecs)<1)	{
			alert("<%write(com_editAlert);%>");
			return;
		}
		
	}
	else {
		if(RuleIDs[i].toLowerCase()=="default"){
			alert("<%write(nar_modiTip);%>");
			return ;
		}
	    NatRule_List.id.value=RuleIDs[i];
	}
	NatRule_List.action="NatRule_edit.asp";
	NatRule_List.submit();
}
function DeleteConfig(index){
	if(RuleIDs[index].toLowerCase()=="default"){
		alert("<%write(nar_delTip);%>");
		return;
	}
	if(confirm("<%write(com_deleteTip);%>")){
		NatRule_List.delstr.value=RuleIDs[index];
		NatRule_List.action="/goform/formNatRuleDel";
	    NatRule_List.submit();
	}
}
function delAll(){
	if(confirm("<%write(com_deleteAllTip);%>")){
		NatRule_List.action="/goform/formNatRuleDelAll";
		NatRule_List.submit();
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
  <td><script language="javascript">showSecondTitle(_("<%write(com_NATRule);%>"));</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center" class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td><form name="NatRule_List" method="post" action="/goform/formNatRuleDel">
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"95%",
						3,
						new Array(_("<%write(com_staticMap);%>"),'NatStatic.asp',1,0),
						new Array(_("<%write(com_NATRule);%>"),'NatRule.asp',1,1),
						new Array("DMZ",'DMZ.asp',1,0)
		);
		showThreeMenu();
	</script>
   <table id=grouplist style="display" width="95%"  border="0" align="center" cellpadding="0" cellspacing="0" height="300">
  <tr>
    <td align="center" class="three_td" valign="top"><table id=grouplist style="display" width="920"  border="0" align="center" cellpadding="0" cellspacing="0">
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
		<script language="javascript">document.write(_("<%write(com_select);%>"));</script></div></td>

			  <td  class="style6" ><input type="hidden" name="delstr" size=10 ><input type="hidden" name="id"></td> 
			   <td width="90" valign="top" align="right"><input type="button" value="<%write(com_addNew);%>" class="input-button" onClick="EditConfig(0)" id="add"></td>
			  <td width="100" valign="top" align="right"><input type="button" value="<%write(com_delAll);%>" class="input-button" onClick="delAll()" id="delall"></td>
			 <td width="40" valign="top" align="right"><input id="del" type="submit" value="<%write(com_delete);%>" class="input-button" onClick="return(genData(NatRule_List,0,frame12,RuleIDs,'<%write(nar_natRuleInfo);%>'))"></td>
			  </tr>
		      </table></td>
		    </tr>
			<tr><td>&nbsp;</td></tr>
		  </table></td></tr></table>
  </form></td>
  </tr>
</table></td>
  <td width="15"></td>
  </tr>
</table>
</body>
</html>
