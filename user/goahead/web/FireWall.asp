<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("FireWall");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

//Butterlate.setTextDomain("firewall");

//function initTranslation()
//{ 
//	var e;
//	e = document.getElementById("add");
//	e.value= _("add");
//	e = document.getElementById("delall");
//	e.value= _("delall");
//	e= document.getElementById("del");
//	e.value= _("del");
//
//}
var strSecondMenuFocus="<%write(com_control);%>";
<%aspOutFireWall(); %>
var web_idipssg=new Array();



var EffectTimeRngs=new Array();
var IPRanges=new Array();
var destIPRanges=new Array();
var PolicyEnablesw=new Array();
var Statussw=new Array();
var FilterTypesw=new Array();
for(i=0;i<totalrecs;i++){
	web_idipssg[i] = PolicyNames[i];
	//EffectTimeRngs[i]=Effect_Dates[i]+"&nbsp;"+Effect_Times[i];
	if(FilterTypes[i]=="2"||FilterTypes[i]=="3"||FilterTypes[i]=="4"||FilterTypes[i]=="5"){
		Protocols[i]=DesFromPorts[i]=DesEndPorts[i]=DesFromIPs[i]=DesEndIPs[i]=SouFromPorts[i]=SouEndPorts[i]="";
	}
	else
		FilterKeys[i]="";
    if(fwGrpType[i] == "fwgrp")
    {
		FilterKeys[i]= "G(" + fwGrpName[i] + ")";
		Protocols[i]=DesFromPorts[i]=DesEndPorts[i]=SouFromPorts[i]=SouEndPorts[i]="";
    }else{
		FilterKeys[i]= "C(" + FilterKeys[i] + ")";
    }
		
     if(Protocols[i] == "1")Protocols[i] = "ICMP";
	 if(Protocols[i] == "2")Protocols[i] = "TCP";
	 if(Protocols[i] == "3")Protocols[i] = "UDP";
	 if(Protocols[i] == "4")Protocols[i] = "AH";
	 if(Protocols[i] == "5")Protocols[i] = "ALL";
	
	
	
	
	FilterTypesw[i]=(FilterTypes[i]=="1"?"<%write(fir_ipFilter);%>":(FilterTypes[i]=="2"?"<%write(fir_urlFilter);%>":(FilterTypes[i]=="3"?"<%write(fir_keyFilter);%>":(FilterTypes[i]=="4"?"<%write(fir_dnsFilter);%>":"<%write(fir_macFilter);%>"))));
	if(sourceIPs[i] == 0)
	{
	    IPRanges[i]=FromIPs[i]+"--"+EndIPs[i];
	}
	else
	{
	    IPRanges[i] = ipGrpNames[i]; 
	}
	if(destIPs[i] == 0)
	{
	    destIPRanges[i]=destFromIPs[i]+"--" + destEndIPs[i];
	}
	else
	{
	    destIPRanges[i] = destIpGrpNames[i]; 
	}
	PolicyEnablesw[i]=(PolicyEnables[i]=="1"?"Yes":"No");
	//PolicyEnablesw[i]=(PolicyEnables[i]=="1"?"<%write(com_startUse);%>":"<%write(com_forbidden);%>");
	Statussw[i]=(Statuss[i]=="1"?"<%write(com_allow);%>":"<%write(com_ban);%>");
	EffectTimeRngs[i]="";
	if(fw_days[i]==effectDay)
		EffectTimeRngs[i]="<%write(com_every);%>";
	else{
		for(j=0;j<fw_days[i].length;j++){
			if(fw_days[i].charAt(j)=="1"){
				if(EffectTimeRngs[i].length) EffectTimeRngs[i]+="，";
					EffectTimeRngs[i]+=DateArray[j];
			}
		}
	}
	
	if(fw_timestarts[i]!=effectTime){
		EffectTimeRngs[i]+="；"+fw_timestarts[i]+"-"+fw_timestops[i];
	}
}

recsPerPage[0] =10;
lineButton[0]=10
currpage[0]=1;
recscount[0]=totalrecs;
maxrecs[0]=max_totalrecs;
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]=_("<%write(fir_ctlList);%>");
var titles1=new Array(14);
titleindex=0;
titles1[titleindex++]=_("<%write(fir_name);%>");
titles1[titleindex++]=_("<%write(com_startUse);%>");
//titles1[titleindex++]=_("<%write(com_addrGrp);%>");
titles1[titleindex++]=_("源地址组");
titles1[titleindex++]=_("目的地址组");
//titles1[titleindex++]=_("ttl4");
titles1[titleindex++]=_("<%write(fir_action);%>");

titles1[titleindex++]=_("<%write(fir_time);%>");
titles1[titleindex++]=_("<%write(fir_filType);%>");
titles1[titleindex++]=_("<%write(fir_content);%>");
titles1[titleindex++]=_("<%write(com_protocol);%>");
titles1[titleindex++]=_("<%write(fir_fromPort);%>");

titles1[titleindex++]=_("<%write(fir_endPort);%>");
//titles1[titleindex++]=_("<%write(fir_fromIP);%>");
//titles1[titleindex++]=_("<%write(fir_endIP);%>");
titles1[titleindex++]=_("<%write(fir_srcfromP);%>");
titles1[titleindex++]=_("<%write(fir_srcendP);%>");



//arrayData1=new Array(PolicyNames,PolicyEnablesw,IPRanges,PRIs,Statussw,EffectTimeRngs,FilterTypesw,FilterKeys,Protocols,DesFromPorts,DesEndPorts,DesFromIPs,DesEndIPs,SouFromPorts,SouEndPorts);
//arrayData1=new Array(PolicyNames,PolicyEnablesw,IPRanges,Statussw,EffectTimeRngs,FilterTypesw,FilterKeys,Protocols,DesFromPorts,DesEndPorts,DesFromIPs,DesEndIPs,SouFromPorts,SouEndPorts);
arrayData1=new Array(PolicyNames,PolicyEnablesw,IPRanges,destIPRanges,Statussw,EffectTimeRngs,FilterTypesw,FilterKeys,Protocols,DesFromPorts,DesEndPorts,SouFromPorts,SouEndPorts);

//arrayCount[0]=14;
arrayCount[0]=14;

<!--
function dsplist(index,list)
{
	//if(index==0) dspList(index,list,1400,titles1,14,arrayData1,1,1);
	if(index==0) dspList(index,list,1400,titles1,13,arrayData1,1,1);
	}
/******************list***********************/
function EditConfig(val,i){
	if(val==0){ 
		AccessCtrlPly_List.id.value="";
		if(eval(max_totalrecs-totalrecs)<1)	{
			alert(_("<%write(com_editAlert);%>"));
			return;
		}
	}
	else AccessCtrlPly_List.id.value=PolicyNames[i];
	AccessCtrlPly_List.action="FireWall_edit.asp";
	AccessCtrlPly_List.submit();
}
function DeleteConfig(index){
	if(confirm(_("<%write(com_deleteTip);%>"))){
		AccessCtrlPly_List.delstr.value=PolicyNames[index];
		AccessCtrlPly_List.action="/goform/formFireWallDel";
	    AccessCtrlPly_List.submit();
	}
}
function delAll(){
	if(confirm(_("<%write(com_deleteAllTip);%>"))){
		AccessCtrlPly_List.action="/goform/formFireWallDelAll";
		AccessCtrlPly_List.submit();
	}
}

function move()
{
	if( web_idipssg.length < 1)
	{
		alert("<%write(fir_moveTip);%>");
		return ;
	}
	Ipssg_Move.action = "/goform/Move_Config"
	Ipssg_Move.submit();
}

function SaveUserAllow(index,i)
{
 
	if (PolicyEnablesw[i].toLowerCase()=="no")
	{
		str="<%write(com_startUse);%>";
		AccessCtrlPly_List.Allow.value="1";
	}
	else
	{
		str="<%write(com_forbidden);%>";
		AccessCtrlPly_List.Allow.value="0";
	}
	if (confirm("<%write(com_really);%>"+str+"<%write(fir_configTip);%>"))
	{
		 AccessCtrlPly_List.AllowID.value = PolicyNames[i];
		 AccessCtrlPly_List.action ="/goform/formFireWallAllow";
		 AccessCtrlPly_List.submit();
	}
	else
	{
		dsplisthead(index,frame11);
    	dsplist(index,frame12);
	}
}


function save()
{
	AccessCtrlPly_List.action="/goform/formFireWallGlobalConfig";
	AccessCtrlPly_List.submit();
}

function init()
{
	var f=AccessCtrlPly_List;
	f.GlobalEnable.checked=((GlobalEnable=="on")?true:false);
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
  <td><script language="javascript">showSecondTitle(_("<%write(com_control);%>"));</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center" class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td >
	<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">

<p>&nbsp;</p>
<tr>
    <td align="center" class="style7">
   <table id=grouplist  width="600"  border="0" align="center" cellpadding="0" cellspacing="0">
	<form name="AccessCtrlPly_List" method="post" action="/goform/formFireWallDel">
		<tr><td colspan="4" height="20">&nbsp;</td></tr>



	<tr>
	  <td colspan="4">
	    <table width="100%">
	      <tr>
	        <td width="42%" height="25" align="right" valign="middle" id="attention1"><%write(fir_start_global);%></td>
		<td width="2%" height="25"><input type="checkbox" name="GlobalEnable" id="GlobalEnable"></td>
		<td align="left"  height="25" valign="middle">&nbsp;&nbsp;<input type="button" class="input-button" 

value="<%write(com_save);%>" onClick="save()"></td>
	      </tr>
	    </table>
          </td>
	</tr>		
	<tr>
	  <td colspan="4" height="20">
	    <table width="100%" border="0" cellspacing="0" cellpadding="0">
	      <tr>
		<td background="<%write(com_imgdot);%>"><img src="<%write(com_imgspace);%>" width="8" height="1"></td>
	      </tr>
	    </table>
	  </td>
	</tr>
	<tr height="25"><td></td></tr>


		     <tr bgcolor="c0ddff">
		      <td colspan="4"><iframe src="_listhead1.htm" name="frame11" scrolling="no"  align="MIDDLE" width="600" height="45" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden ></iframe></td>
		    </tr>
		    <tr bgcolor="c0ddff">
		      <td colspan="4"><iframe src="_list1.htm" name="frame12" id="framelist" align="MIDDLE" width="600" height="265" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe></td>
		    </tr>
		    <tr>
		      <td  colspan="4"    ><table id=dellist  width="600"  height="30" border="0" cellspacing="0" cellpadding="1">
			  <tr colspan="4" valign="bottom">
			  <td width="5"></td>
			  <td  class="style6"><div align="left">
				<input type="checkbox" name="delchk" onClick=selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist'));>
				<script language="javascript">document.write(_("<%write(com_select);%>"));</script>
		</div></td>
			  <td  class="style6" ><input type="hidden" name="delstr" size=10 ><input type="hidden" name="id"><input type="hidden" name="AllowID"><input type="hidden" name="Allow"></td> 
			   <td align="right" valign="bottom"><input type="button" value="<%write(com_addNew);%>" class="input-button" onClick="EditConfig(0)" id="add">&nbsp;<input type="button" value="<%write(com_delAll);%>" class="input-button" onClick="delAll()" id="delall">	&nbsp;&nbsp;&nbsp;<input type="submit" value="<%write(com_delete);%>" class="input-button" onClick="return(genData(AccessCtrlPly_List,0,frame12,PolicyNames,'<%write(fir_ctlInfo);%>'))" id="del"></td>
</tr>
		      </table></td>
		    </tr> </form>
            
                  <form name="Ipssg_Move" method="post" action="/goform/Ipssg_Move">
                  <tr align="left">
                  <td>
		<table width="600"  cellpadding="0" cellspacing="0">
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
  <td width="15"><script>init();</script> </td>
  </tr>
</table>
</body>
</html>
