<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("PPPoEGlobal");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

/*例外地址组的交互变量*/
/*
var addrGroupName=new Array();
addrGroupName[0]="1";
addrGroupName[1]="2";
addrGroupName[2]="3";
*/


//function initTranslation()   
//{ 
//	var e;
//	e = document.getElementById("pppoeset");
//	e.innerHTML= _("pppoeset");
//	e = document.getElementById("pppoe shuoming"); 
//	e.innerHTML= _("pppoe shuoming");
//	e = document.getElementById("pwdMethod");
//	e.innerHTML= _("pwdMethod");
//	e = document.getElementById("maxSession");
//	e.innerHTML= _("maxSession");
//	e = document.getElementById("startip");
//	e.innerHTML= _("startip");
//	e = document.getElementById("zhuDNS");
//	e.innerHTML= _("zhuDNS");
//	e = document.getElementById("beiDNS");
//	e.innerHTML= _("beiDNS");
//	e = document.getElementById("uttsave");
//	e.value= _("save");
//	e = document.getElementById("uttreset");
//	e.value= _("reset");
//}
var strSecondMenuFocus= "<%write(com_pppGlo);%>";
<%aspOutAddreGrpList();%>
var error1 = errorstr;
<%
aspPppoeServGlobale();
%>
<%
outputLANConfig();
%>

errorstr = error1;
 

function save(){
	var f=PPPoEServer_Config;
	if(checkData()){
		PPPoEServer_Config.lanPPPoEType.value=(PPPoEServer_Config.lanPPPoETypew.checked==true?"SERVER":"DISABLE");

		PPPoEServer_Config.action="/goform/forPppoeServGlobale";
		PPPoEServer_Config.submit();
	}
}
function checkData(){
	var f=PPPoEServer_Config;
	if (!checkIP(f.pppoeStart,"<%write(com_startIP);%>",0)) return false;
	if(multiLan == 1)
	{
	    if((checkSameSubnet(lanIps,f.pppoeStart.value,lanNMs)) || (checkSameSubnet(lanIp2s,f.pppoeStart.value,lanNM2s)) || (checkSameSubnet(lanIp3s,f.pppoeStart.value,lanNM3s)) || (checkSameSubnet(lanIp4s,f.pppoeStart.value,lanNM4s)))
	    {
		alert("<%write(ppg_ipAlert);%>");
		return false;
	    }
	}
	else
	{
	if (checkSameSubnet(lanIps,f.pppoeStart.value,lanNMs))
		{
			alert("<%write(ppg_ipAlert);%>");
			return false;
		}
	}
	if (f.pppoeStart.value=="0.0.0.0")
	{
		alert(_("<%write(ppg_ipTip);%>"));
		f.pppoeStart.focus();
		return false;
	}
    if(showIpcount == "yes")
    {
        if (!checkNumber(f.ipcount,1,65536,"总地址数",0)) return false;
    }
	if (!checkNumber(f.smaxconv,1,maxconv,"<%write(ppg_maxNum);%>",0)) return false;

	if (!checkMainDns(f.pppoePriDns,"<%write(com_MDNS);%>",0)) return false;
	if (!checkIP(f.pppoeSecDns,"<%write(com_SDNS);%>",1)) return false;
	return true;
}
function hidEnable() 
{
		var f=PPPoEServer_Config;
		f.exceptIpGroup.disabled=false;
}
function hidBan() 
{
		var f=PPPoEServer_Config;
		f.exceptIpGroup.disabled=true;
}
function init(){

	var f=PPPoEServer_Config;
	var obj=null;
	f.lanPPPoETypew.checked=(lanPPPoETypes==1?true:false);
	f.PPPoEOnly[0].checked=(onlyPppoes=="1"?true:false);
	f.PPPoEOnly[1].checked=(onlyPppoes=="1"?false:true);
	f.exceptIpGroup.disabled=(onlyPppoes=="1"?false:true);
	if(selfHelpEnable == "null")
	{
	    showTable(selfHelp, 0);
	}
	else
	{
	    f.selfHelpEnable.checked=(selfHelpEnable == "on" ? true:false);
	}
    if (showIpcount == "no")
    {
        document.getElementById("ipcount").style.display="none";
    }
    else
    {
        f.ipcount.value = ipcounts;
    }

	f.exceptIpGroup.value=exceptIpGroups;
	f.pppoeStart.value=pppoeStarts;
	f.pppoePriDns.value=pppoePriDnss;
	f.pppoeSecDns.value=pppoeSecDnss;
    f.vailtype.value = vailtypes;
	f.smaxconv.value = smaxconvs;

	 for(i=1;i<=7;i++){	 
	 	obj=document.getElementById("yutd"+i);
		if(obj) obj.innerHTML="";
	 }
}
/*
function checksecdns(str){
	var f=PPPoEServer_Config;
	if(str != ""){
		showfiledtip(re_checkIP(f.pppoeSecDns,'备DNS服务器'),yutd5);
	}
}
*/
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
  <td><script language="javascript">showSecondTitle(_("<%write(com_pppSer);%>"));</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center" background="<%write(com_bodyMain);%>">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td><form action=""  name="PPPoEServer_Config" method="post">
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
					"100%",
						5,
						new Array(_("<%write(com_pppGlo);%>"),'PPPoEGlobal.asp',1,1),
						new Array(_("<%write(com_pppAcc);%>"),'PPPoEServer.asp',1,0),
						new Array(_("<%write(com_pppSta);%>"),'PPPoEConSta.asp',1,0),
						new Array(_("<%write(com_pppExp);%>"),'PPPoEExport.asp',1,0),
						new Array(_("<%write(com_pppImp);%>"),'PPPoEImport.asp',1,0)
		);
		showThreeMenu();
	</script>
<table width="100%" border="0" cellspacing="0" cellpadding="0" align="center" height="250">
  <tr>
    <td align="center" class="three_td" valign="top">
  <table border="0" width="600" align='center'>
  <tr><td colspan="4" height="30">&nbsp;</td></tr> 
    <tr>
		<td width="33%" align="right" height="25" id="pppoeset"><%write(ppg_start);%></td>
		<td width="3%">&nbsp;</td>
		<td colspan="2" height="20"><input name="lanPPPoETypew" type="checkbox" ><input type="hidden" name="lanPPPoEType"></td>
	      </tr>
		  <tr><td width="33%"></td>
		  <td width="3%"></td>
		  <td colspan="2" class="helpfont" id="pppoe shuoming"><%write(ppg_startTip);%></td></tr>
		  
          <tr  style="display:">
		<td width="33%" align="right"  id="pppoeset"><%write(ppg_pppoeSet);%></td>
		<td width="3%">&nbsp;</td>
		<td colspan="2" height="20">
		<div align="left">
		<input name="PPPoEOnly" type="radio" value="1" onClick="hidEnable()" /><%write(com_startUse);%>&nbsp;&nbsp;&nbsp;
		 <input name="PPPoEOnly" type="radio" value="0" onClick="hidBan()" checked="checked" /> <%write(com_forbidden);%> 
		 </div>
		 </td>
	      </tr>
		  <tr id="disIpGroup" style="display">
		  	<td width="33%" height="20" align="right" id = "exceptAddGroup"><%write(ppg_except);%></td>
			 <td height="20" align="left" td width="3%">&nbsp;</td>
			 <td colspan="2" height="20" td width="217">
			 	<select name="exceptIpGroup" id="exceptIpGroup" class="input" style="width:70px" disabled="disabled">
		  	  		<option value=""><%write(ppg_none);%></option>
					<script language="javascript">
		    		for(var i=0;i<addrGroupName.length;i++)
		    		{
						document.write('<option value='+addrGroupName[i]+'>'+addrGroupName[i]+'</option>');
		    		}
		    		</script>		  
			  	</select>
			</td>
		  </tr>
      <tr>
		<td width="33%" align="right" id="startip"><%write(com_startIP);%></td>
		<td width="3%" ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
		<td height="20"><input name="pppoeStart" type="text" onChange="showfiledtip(re_checkIP(this,'<%write(com_startIP);%>'),yutd1)" onKeyPress="ipOnly()" size="18" maxlength="15" ></td><td id="yutd1"></td>
		</tr>
   
        <tr id = ipcount>
		  <td width="33%" align="right" id="zongcount">总地址数</td>
		<td width="3%" ><img src="images/qt.gif" width="7" height="9"></td>
		<td height="20"><input name="ipcount" type="text" onKeyPress="figureOnly()" size="18" maxlength ="5" onChange="showfiledtip(re_checkNumber(this,1,65535,'总地址数'),yutd3)"></td><td id="yutd3"></td>
	      </tr>
		  
<tr>
		<td width="33%" align="right" id="zhuDNS"><%write(com_MDNS);%></td>
		<td width="3%" ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
		<td height="20"><input name="pppoePriDns" type="text" onChange="showfiledtip(re_checkMainDns(this,'<%write(com_MDNS);%>'),yutd4)" onKeyPress="ipOnly()" size="18" maxlength="15"></td><td id="yutd4"></td>
	      </tr>
		  
		    <tr>
		<td width="33%"  align="right" id="beiDNS"><%write(com_SDNS);%></td>
		<td width="3%">&nbsp;</td>
		<td height="20"><input name="pppoeSecDns" type="text" onChange="showfiledtip(re_checkIP_empty(this,'<%write(com_SDNS);%>'),yutd5);" onKeyPress="ipOnly()" size="18" maxlength="15"></td><td id="yutd5"></td>
	      </tr>	
		  
<tr><td width="33%"></td>
		  <td width="3%"></td>
		  <td colspan="2" class="helpfont" id="pppoe shuoming"><%write(ppg_dnsTip);%></td></tr>
		  <tr>
		<tr id="selfHelp" style="display:">
		<td width="33%"  align="right" id="pwdChange"><%write(ppg_pwdMod);%></td>
		<td width="3%">&nbsp;</td>
		<td height="20"><div align="left"><input name="selfHelpEnable" type="checkbox" id="selfHelpEnable"></div></td>
		<td id=""></td>
	      </tr>	
	
<tr>
			 <td width="33%" height="20" align="right" id = "pwdMethod"><%write(com_checkPasswd);%></td>
			 <td height="20" align="left" td width="3%">&nbsp;</td>
			 <td colspan="2" height="20" td width="217"><select name="vailtype" >

			       <option value="PAP" >PAP</option>
			       <option value="CHAP">CHAP</option>
				   <option value="EITHER" selected>AUTO</option>
			     </select></td>
		</tr>
		
		 <tr>
			 <td width="33%" height="20" align="right" id = "maxSession"><%write(ppg_session);%></td>
			 	<td width="3%" ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
			 <td  height="23"><input name="smaxconv" type="text"   id="smaxconv" onKeyPress="figureOnly()" onChange="showfiledtip(re_checkNumber(this,1,maxconv,'<%write(ppg_session);%>'),yutd6)" size="18"></td><td id="yutd6"></td>
		 </tr>
		  
		  
	  <tr><td colspan="4" height="20">&nbsp;</td></tr>
	   <tr><td colspan="4">
  <table width="30%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td width="62%"  height="30" align="center" valign="top"><input type="button" class="input-button" value="<%write(com_save);%>" onClick="save()" id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_reset);%>" onClick="init()" id="uttreset">&nbsp;&nbsp;&nbsp;<input type="button" value="<%write(com_help);%>" class="input-button" onClick="window.open('<% write(com_helppage); %>#PPPoEGlobalEdit')">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>
  </tr>
</table>

  </td></tr>
  </table></td>
  </tr>
</table>
</form></td>
  </tr>
  <script language="javascript">init();</script>
</table></td>
  <td width="15"></td>
  </tr>
</table>
</body>
</html>

