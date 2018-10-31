<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("FireWall");%>
<%langTran("PolicyRoute");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

var strSecondMenuFocus="<%write(com_policyroute);%>";
var web_idipssg=new Array();

/* test varliable */
var routeNames=new Array();
var Allows=new Array();
var bind=new Array();
var srcip=new Array();
var dstip=new Array();
var protocol=new Array();
var fromport=new Array();
var toport=new Array();
var totalrecs=0;
var max_totalrecs=20;
var errorstr="";
<%
getWanIfCount();
getWanConfigCount();

aspOutPolicyRouteConf();
%>


var profile=new Array();
var Protocols=new Array();
var EffectTimeRngs=new Array();
for(i=0;i<totalrecs;i++){
	web_idipssg[i] = routeNames[i];
		var slotIndex = Profile[i].slice(Profile[i].length - 1);
		profile[i] = Profile[i];
		if (is3GPort(slotIndex,wanIfCount)) {
			profile[i] = "3G";
		} else if (isAPCliPort(slotIndex,wanIfCount)) {
			profile[i] = "APClient";
		}
		if(wanIfConf[slotIndex] == 0)
		{
		    profile[i] = profile[i]+"(<%write(pr_invalid);%>)";
		} else {
		}
	if(srcIPs[i] == "ipRange"){
	    srcip[i] = srcipFrom[i]+"-"+srcipEnd[i];
	}else if(srcIPs[i] == "groupSel"){
	    srcip[i] = srcipGrps[i];
	}
	if(dstIPs[i] == "ipRange"){
	    dstip[i] = dstipFrom[i]+"-"+dstipEnd[i];
	}else if(dstIPs[i] == "groupSel"){
	    dstip[i] = dstipGrps[i];
	}
		
     if(protocols[i] == "1")Protocols[i] = "ICMP";
	 if(protocols[i] == "6")Protocols[i] = "TCP";
	 if(protocols[i] == "17")Protocols[i] = "UDP";
	 if(protocols[i] == "51")Protocols[i] = "AH";
	 if(protocols[i] == "0")Protocols[i] = "ALL";
	
	//PolicyEnablesw[i]=(PolicyEnables[i]=="1"?"<%write(com_startUse);%>":"<%write(com_forbidden);%>");
	//Statussw[i]=(Statuss[i]=="1"?"<%write(com_allow);%>":"<%write(com_ban);%>");
	EffectTimeRngs[i]="";
	if(pr_days[i]==effectDay)
		EffectTimeRngs[i]="<%write(com_every);%>";
	else{
		for(j=0;j<pr_days[i].length;j++){
			if(pr_days[i].charAt(j)=="1"){
				if(EffectTimeRngs[i].length) EffectTimeRngs[i]+="，";
					EffectTimeRngs[i]+=DateArray[j];
			}
		}
	}
	
	if(pr_timestarts[i]!=effectTime){
		EffectTimeRngs[i]+="；"+pr_timestarts[i]+"-"+pr_timestops[i];
	}
}

recsPerPage[0] =10;
lineButton[0] =10;
currpage[0]=1;
recscount[0]=totalrecs;
maxrecs[0]=max_totalrecs;
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]=_("<%write(pr_configlist);%>");
var titles1=new Array(9);
titleindex=0;
titles1[titleindex++]=_("<%write(pr_routename);%>");
titles1[titleindex++]=_("<%write(com_allow);%>");
titles1[titleindex++]=_("<%write(com_Interface);%>");
titles1[titleindex++]=_("<%write(fir_srcIP);%>");
titles1[titleindex++]=_("<%write(pr_dstip);%>");
titles1[titleindex++]=_("<%write(com_protocol);%>");
titles1[titleindex++]=_("<%write(fir_fromPort);%>");
titles1[titleindex++]=_("<%write(fir_endPort);%>");
titles1[titleindex++]=_("<%write(fir_time);%>");



//arrayData1=new Array(PolicyNames,PolicyEnablesw,IPRanges,PRIs,Statussw,EffectTimeRngs,FilterTypesw,FilterKeys,Protocols,DesFromPorts,DesEndPorts,DesFromIPs,DesEndIPs,SouFromPorts,SouEndPorts);
arrayData1=new Array(routeNames,Allows,profile,srcip,dstip,Protocols,fromport,toport,EffectTimeRngs);
arrayCount[0]=9;

<!--
function dsplist(index,list)
{
	if(index==0) dspList(index,list,1000,titles1,9,arrayData1,1,1);
	}
/******************list***********************/
function EditConfig(val,i){
	if(val==0){ 
		PolicyRoute_List.id.value="";
		if(eval(max_totalrecs-totalrecs)<1)	{
			alert(_("<%write(com_editAlert);%>"));
			return;
		}
	}
	else PolicyRoute_List.id.value=routeNames[i];
	PolicyRoute_List.action="PolicyRoute_edit.asp";
	PolicyRoute_List.submit();
}
function DeleteConfig(index){
	if(confirm(_("<%write(com_deleteTip);%>"))){
		PolicyRoute_List.delstr.value=routeNames[index];
		PolicyRoute_List.action="/goform/formPolicyRouteDel";
	    PolicyRoute_List.submit();
	}
}
function delAll(){
	if(confirm(_("<%write(com_deleteAllTip);%>"))){
		PolicyRoute_List.action="/goform/formPolicyRouteDelAll";
		PolicyRoute_List.submit();
	}
}

function move()
{
	if( web_idipssg.length < 1)
	{
		alert("<%write(pr_moveTip);%>");
		return ;
	}
	Ipssg_Move.action = "/goform/formPolicyRouteMove_Config"
	Ipssg_Move.submit();
}
function save()
{
	PolicyRoute_List.action="/goform/formPolicyRouteGlobalConfig";
	PolicyRoute_List.submit();
}
function init()
{
	PolicyRoute_List.GlobalEnable.checked= (prEnable == "on");
}
function SaveUserAllow(index,i)
{
	if (Allows[i].toLowerCase()=="no")　//复选框原来的值是"DENY"，即不选中，那么点击后调用本函数时状态应该是选中
	{
		str="<%write(com_allow);%>";
		PolicyRoute_List.Allow.value="yes";
	}
	else
	{
		str="<%write(com_ban);%>";
		PolicyRoute_List.Allow.value="no";
	}
	PolicyRoute_List.AllowID.value=routeNames[i];
	//alert(IPSec_List.AllowID.value);
	if (confirm("<%write(com_really);%>"+str+""))
	{
		 PolicyRoute_List.action ="/goform/formPolicyRouteAllow";
		 PolicyRoute_List.submit();
	}
	else
	{
		dsplisthead(index,frame11);
    	dsplist(index,frame12);
	}
}
-->
</script>
</head>
<body onbeforeunload="logOut()" background="<%write(com_bodyMain);%>">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle(_("<%write(com_policyroute);%>"));</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center" class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td ><form name="PolicyRoute_List" method="post" action="/goform/formPolicyRouteDel">
	<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
    <td align="center" class="style7">
   <table id=grouplist  width="80%"  border="0" align="center" cellpadding="0" cellspacing="0"><tr><td colspan="4" height="20">&nbsp;</td></tr>


   			<tr><td colspan="4">
				<table width="100%">
				<tr>
				<td width="350" align="right">&nbsp;<%write(pr_start);%>&nbsp;</td>
				<td width="20"><input type="checkbox" name="GlobalEnable" id="GlobalEnable"></td>
				<td width="10">&nbsp;</td>
				<td width="281"><input type="button" class="input-button" value="<%write(com_save);%>" onClick="save()"></td>
				</tr>
				</table>
			</td>
			</tr>
			
			<tr><td colspan="4" height="20">
			<table width="100%" border="0" cellspacing="0" cellpadding="0">
			<tr>
			<td background="<%write(com_imgdot);%>"><img src="<%write(com_imgspace);%>" width="8" height="1"></td>
			</tr>
			</table>
			</td></tr>

		     <tr bgcolor="c0ddff">
		      <td colspan="4"><iframe src="_listhead1.htm" name="frame11"  scrolling="no" align="MIDDLE" width="600" height="45" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden ></iframe></td>
		    </tr>
		    <tr bgcolor="c0ddff">
		      <td colspan="4"><iframe src="_list1.htm" name="frame12" id="framelist" align="MIDDLE" width="600" height="280" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe></td>
		    </tr>
		    <tr>
		      <td  colspan="4"    ><table id=dellist  width="600"  height="30" border="0" cellspacing="0" cellpadding="1">
			  <tr colspan="4" valign="bottom">
			  <td width="5"></td>
			  <td  class="style6"><div align="left">
				<input type="checkbox" name="delchk" onClick=selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist'));>
				<script language="javascript">document.write(_("<%write(com_select);%>"));</script>
		</div></td>
			  <td  class="style6" ><input type="hidden" name="delstr" size=10 ><input type="hidden" name="id"></td> <input type="hidden" name="Allow"><input type="hidden" name="AllowID">
			   <td align="right" valign="bottom"><input type="button" value="<%write(com_addNew);%>" class="input-button" onClick="EditConfig(0)" id="add">&nbsp;<input type="button" value="<%write(com_delAll);%>" class="input-button" onClick="delAll()" id="delall">	&nbsp;&nbsp;&nbsp;<input type="submit" value="<%write(com_delete);%>" class="input-button" onClick="return(genData(PolicyRoute_List,0,frame12,routeNames,'<%write(com_policyroute);%>'))" id="del"></td>
</tr>
		      </table></td>
		    </tr> </form>
            
                  <form name="Ipssg_Move" method="post" action="/goform/">
                  <tr align="left">
                  <td>
		<table width="600"  cellpadding="0" cellspacing="0" style="display:">
			<tr>
			
			<td height="50" colspan="2"><%write(com_fromPol);%>&nbsp;
			  <select name="sID" id="sID" class="input" style="width:120px">
<script language="javascript">
		for (i = 0; i < web_idipssg.length; i++) {

			document.write('<option value="'+web_idipssg[i]+'">'+web_idipssg[i]+'</option>');
		}
</script>
			</select>&nbsp;<input  type="button" class="input-button"  name="mov" id="mov" value=" <%write(com_moveTo);%> " onClick="move()" />&nbsp;<%write(com_policy);%>&nbsp;
			  <select name="tID" id="tID " class="input" style="width:120px" />
<script language="javascript">
		for (i = 0; i < web_idipssg.length; i++) {
			document.write('<option value="'+web_idipssg[i]+'">'+web_idipssg[i]+'</option>');
		}
</script>		      
			  </select>&nbsp;<%write(com_before);%></td>
			 
			</tr>
			</table>            </td></tr>
            <tr><td colspan="4" height="20">&nbsp;</td></tr>
			</form>
     
            
		  </table>
          
       
              
          
          
		  </td></tr></table>
 </td>
  </tr>
</table></td>
  <td width="15"><script language="javascript">init();</script></td>
  </tr>
</table>
</body>
</html>
