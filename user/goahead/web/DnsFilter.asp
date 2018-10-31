 <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("DnsFilter");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

var AddGroupName=new Array();
var userGroupNames=new Array();
var timeRangeName=new Array();
var errorstr="";
//Butterlate.setTextDomain("firewall");

//function initTranslation() 
//{
//	var e;
//	e = document.getElementById("uttsave");
//	e.value= _("save");
 //
//	e = document.getElementById("domain");
//	e.innerHTML= _("domain");
//	e = document.getElementById("add");
//	e.value= _("add");
//	e= document.getElementById("domain list");
//	e.innerHTML= _("domain list");
//	e = document.getElementById("delete");
//	e.value= _("delete");
//	e = document.getElementById("delete all");
//	e.value= _("delete all");
//	e = document.getElementById("start dns");
//	e.innerHTML= _("start dns");
//	e = document.getElementById("attention2");
//	e.innerHTML= _("attention2");
//
//}
<%aspOutIpGrpList();%>
<%aspOutTimeGrpList();%>
<%aspOutDnsFilterList();%>
<% GetMaxDomainNameNum();%>
<% DnsFilter_Sw();%>
//交互变量
/*
var DnsFilterEnables="on";//全局开关
var DnsLists=new Array();
var maxtotalrecs;
var errorstr="";
DnsLists[0]="www.utt.com.cn";
DnsLists[1]="www.163.com";
*/
//end
var max_totalrecs=MaxDomainName;
var linestr="--------------------------------------------------------------------------------------";
<!--
function init(){
    var f = DnsFilter_Config;
    creatdns();
    DnsFilter_Config.DnsFilterEnable.checked=(DnsFilterEnables=="on"?true:false);
    if(sourceIPs == 0)
    {
	f.FromIP.value=FromIPs;
	f.EndIP.value=EndIPs;
	f.sourceIP[0].checked = true;
	selObject(f,"ipRange");
    }
    else
    {
	f.GroupName.value = ipGrpNames;
	f.sourceIP[1].checked = true;
	selObject(f,"groupSel");
    }
    f.timeRangeName.value = timeRangeNames;
    if(enableTypes == 0)
    {
	f.enableType[0].checked = true;
    }
    else
    {
	f.enableType[1].checked = true;
    }
}
function creatdns(){
    var tbl=DnsFilter_Config;
	if(DnsLists.length>0){
      for (i=0;i<DnsLists.length;i++)
	  {
		var option1=document.createElement("OPTION");
		option1.text=DnsLists[i];
		tbl.DnsList.options.add(option1);
		
	  }
	  option1=document.createElement("OPTION");
	  option1.text=linestr;
	  tbl.DnsList.options.add(option1);
    }
}

function del(){
    var tbl=DnsFilter_Config;
	tbl.delstr.value="";
	 if (tbl.DnsList.selectedIndex == -1) return;
       for (var i=0;i<tbl.DnsList.options.length;i++) {
		if (tbl.DnsList.options[i].selected) {
           	if(tbl.DnsList.options[i].text!=linestr)
		   	{
		      tbl.delstr.value=tbl.delstr.value+DnsLists[i]+",";
		   	} 
		   	else
		   	{
			   alert("<%write(dns_delTip);%>");
			   return;
			}
		 }	 
	  }
	  	 // alert(tbl.delstr.value);
	  DnsFilter_Config.action="/goform/formConfigDnsFilterDel";
	  DnsFilter_Config.submit();
}

function saveGlobal()
{
	var tbl=DnsFilter_Config;
	DnsFilter_Config.action="/goform/formConfigDnsFilterGlobal";
	DnsFilter_Config.submit();
}

function save(){
  var tbl=DnsFilter_Config;
  if(max_totalrecs-DnsLists.length<1){
      alert(_("<%write(dns_addTip);%>"));
	  return ;
  }
  for(i=0;i<DnsLists.length;i++){
     if(tbl.addHostFilter.value.toLowerCase()==DnsLists[i].toLowerCase()){
	     alert(_("<%write(dns_existTip);%>"));
		 return ;
	 }
  }
  if (!checkName(tbl.addHostFilter,49,"<%write(dns_domain);%>",0)) return ;
  if (!checFilterName(tbl.addHostFilter,49,"<%write(dns_domain);%>",0)) return ;
  if (!checkData()) return;
  DnsFilter_Config.action="/goform/websHostFilter";
  DnsFilter_Config.submit();
}

function checkData()
{
    var f=DnsFilter_Config;
    if (f.sourceIP[0].checked==true)
    {
	if(!checkIP(f.FromIP,"<%write(com_IPRange);%>",0)) return false;
	if(!checkIP(f.EndIP,"<%write(com_IPRange);%>",0)) return false;
	if(compareIP(f.FromIP.value,f.EndIP.value)==1)
	{
	    alert("<%write(com_IPRangeTip);%>");
	    return false;
	}
	if((f.FromIP.value=="0.0.0.0"&&f.EndIP.value!="0.0.0.0")||(f.FromIP.value!="0.0.0.0"&&f.EndIP.value=="0.0.0.0"))
	{
	    alert("<%write(com_IPRangeTip);%>");
	    return false;
	}
    }
	if (f.enableType[0].checked==true)
	{
		f.domainEnable.value="on";
		//alert(f.domainEnable.value);
	}
	if(f.enableType[1].checked==true)
	{
		f.domainEnable.value="off";
		//alert(f.domainEnable.value);
	}
    return true;
}

function checFilterName(cntrl,maxlen,tips,allownull)
{
	var name=cntrl.value;
	if(name=="")
	{
		if(!eval(allownull))
		{
		  alert(tips+_("<%write(com_null);%>"));
		 cntrl.focus();   
		  return false;
		}
		else
			return true;
	}
  else if(name==0){
		alert(tips+_("<%write(com_zero);%>"));
		return false;
	}
	else if (name.length > maxlen)
	{alert(name.length);
		alert(tips+_("<%write(com_chkLenTip);%>")+maxlen+_("<%write(com_lenAppend);%>"));
		cntrl.focus();
		return false;
	}
	else
	{
		allowChar="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890.-*";
		for (i=0;i<name.length;i++)
		{
			if (allowChar.indexOf(name.substring(i,i+1))==-1)
			{
				alert(tips+_("<%write(dns_include);%>"));
				return false;
			}
		}
	}
	return true;
}
function DelAll(){
	if(confirm(_("<%write(dns_delete);%>"))){
		DnsFilter_Config.action="/goform/formConfigDnsFilterDelAll";
        DnsFilter_Config.submit();
	}
}
function selObject(f,val)
{
	if(val=="ipRange")
	{
		f.FromIP.disabled=false;
		f.EndIP.disabled=false;
		f.GroupName.disabled=true;
	}
	if(val=="groupSel")
	{
		f.FromIP.disabled=true;
		f.EndIP.disabled=true;
		f.GroupName.disabled=false;
	}
}
//-->
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
  <td><script language="javascript">showSecondTitle(_("<%write(dns_dnsFilter);%>"));</script>
  <table width="100%" border="0" height="550" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td>
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"90%",
						2,
						new Array("<%write(dns_dnsFilterConfig);%>",'DnsFilter.asp',1,1),
						new Array("<%write(dns_dnsFilterNotice);%>",'DnsFilter_Notice.asp',Notice,0)
		);
		showThreeMenu();
	</script>
	
  <table width="90%" border="0" cellspacing="0" cellpadding="0" align="center">
<tr>
    <td align="center"  class="three_td" valign="top"><table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td>
	<table border="0" width="600" align='center'>
<form  name="DnsFilter_Config" method="post">
	<tr><td colspan="4" height="20">&nbsp;</td></tr>	

	<tr>
	<td width="30%" height="30" align="right" valign="middle" id="start dns"><%write(dns_start);%></td>
	<td width="3%">&nbsp;</td>
	<td align="left" valign="middle" colspan="2">&nbsp;<input type="checkbox" name="DnsFilterEnable">&nbsp;</td>
	</tr>
 	<tr>
	<td width="30%" height="30" align="right" valign="middle"></td>
	<td width="3%">&nbsp;</td>
	<td align="left" valign="middle" colspan="2" id="attention2"><%write(dns_startTip);%></td>
	<tr>
	<td width="30%" height="30" align="right" valign="middle"><%write(dns_effectWay);%></td>
	<td width="3%">&nbsp;</td>
	<td align="left" valign="middle" colspan="2"><input type="radio" name="enableType" value="ACCEPT" checked="checked">&nbsp;<%write(dns_accept);%></td>
	 </tr>
	 <tr>
	<td width="30%" height="30" align="right" valign="middle">&nbsp;</td>
	<td width="3%">&nbsp;</td>
	<td align="left" valign="middle" colspan="2"><input type="radio" name="enableType" value="FORBID">&nbsp;<%write(dns_forbiden);%>&nbsp;<input type="hidden" name="domainEnable"></td>
	 </tr> 
  <tr>
  <td width="30%" height="30" align="right" valign="middle" id="NetAction"><%write(dns_netAction);%></td>
  <td width="3%"><!--<img src="images/qt.gif" width="7" height="9">-->&nbsp;</td>
  <td align="left" valign="middle" colspan="2"><input type="radio" name="sourceIP" value="ipRange" onClick="selObject(this.form,this.value)" checked><%write(com_segment);%>&nbsp;&nbsp;&nbsp;<input name="FromIP" type="text" size="15" maxlength="15" onKeyPress="ipOnly()" value="0.0.0.0">&nbsp;&nbsp;<script language="javascript">document.write(_("<%write(com_toIP);%>"));</script>&nbsp;<!--<img src="images/qt.gif" width="7" height="9">-->&nbsp;<input name="EndIP" type="text" size="15" maxlength="15" onKeyPress="ipOnly()" value="0.0.0.0"></td>
  </tr>
	<tr>
	<td height="30" align="right" valign="middle">&nbsp;</td>
	<td>&nbsp;</td>
	<td align="left" valign="middle" colspan="2"><input type="radio" name="sourceIP" value="groupSel" onClick="selObject(this.form,this.value)"><%write(com_userGrp);%>&nbsp;<select name="GroupName" id="GroupName">
	   <option value=""><%write(com_userAll);%></option>
	   <script language="javascript">
	   for(var i=0;i<addGroupNames.length;i++)
	   {
	   		document.write('<option value='+addGroupNames[i]+'>'+addGroupNames[i]+'</option>');
	   }
	   </script>
	   </select>
	 </td>
	 </tr>

	<tr>
	<td height="30" align="right" valign="middle"><%write(dns_effectTime);%></td>
	<td>&nbsp;</td>
	<td align="left" valign="middle" colspan="2">&nbsp;<select name="timeRangeName" id="timeRangeName">
	   <option value=""><%write(dns_allTime);%></option>
	   <script language="javascript">
	   for(var i=0;i<addTimeGroupNames.length;i++)
	   {
	   		document.write('<option value='+addTimeGroupNames[i]+'>'+addTimeGroupNames[i]+'</option>');
	   }
	   </script>
	   </select>
	 </td>
	 </tr>
	<tr>
	<td height="40" align="right" valign="middle">&nbsp;&nbsp;</td>
	<td>&nbsp;</td>
	<td align="left" valign="middle" colspan="2">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
	  <input type="button" class="input-button" value="<%write(com_save);%>"  onClick="saveGlobal()" id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_help);%>" onClick="window.open('<% write(com_helppage); %>#DnsFilter')"></td>
	 </tr>
	 
  <tr><td colspan="4" align="center" height="30"><table width=600 border="0" cellspacing="0" cellpadding="0">
  <tr>
   <td background="<%write(com_imgdot);%>"><img src="<%write(com_imgspace);%>" width="8" height="1"></td>
  </tr>
</table>
</td></tr>

   
  <tr>
  <td width="30%" height="40" align="right" valign="middle" id="domain"><%write(dns_domain);%></td>
  <td width="3%">&nbsp;</td>
  <td align="left" width="40%" valign="middle"><input name="addHostFilter"  class="input"type="text" size="45" maxlength="49"></td>
  <td height="40" align="left" valign="middle"><input type="button" value="<%write(com_add);%>" onClick="save()" class="input-button" id="add"></td>
  </tr>

   <tr>
   <td width="30%" height="30" align="right" valign="middle" > </td>
     <td width="3%">&nbsp;</td>
	 <td colspan="4" height="20" class="helpfont" ><%write(dns_dnsTip);%></td>
   
   </tr>
  <tr>
	   <tr><td colspan="4" height="20"><input type="hidden" name="delstr"></td></tr>
  <tr>
    <td width="20%" height="30" align="right" valign="top" id="domain list"><%write(dns_dnsList);%></td>
	<td width="3%" height="40">&nbsp;</td>
	<td align="left" colspan="2"><select class="input" name="DnsList" size=10 multiple  id="dnslist"  color:blue  style="font-size:10pt;SCROLLBAR-FACE-COLOR: #ffffff;  SCROLLBAR-HIGHLIGHT-COLOR: #ffffff;  #c0ddff 1px solid; SCROLLBAR-SHADOW-COLOR: #ffffff; LINE-HEIGHT: 20px; SCROLLBAR-ARROW-COLOR: #c0ddff; SCROLLBAR-TRACK-COLOR: #ffffff; SCROLLBAR-DARKSHADOW-COLOR: #c0ddff; SCROLLBAR-BASE-COLOR: #c0ddff;border:1px solid #c0ddff; width:460px " border=0  >
      
    </select></td>
  </tr>
  <tr><td colspan="4" align="center" height="30" valign="bottom" >
  <table width="180" border="0" cellspacing="0" cellpadding="0">
		      <tr>
			<td  width="52%"><input type="button" value="<%write(com_delete);%>" class="input-button" onClick="del()" id="delete"></td>
			<td ><input type="button" value="<%write(com_remove);%>" class="input-button" onClick="DelAll()" id="delete all"></td>
		      </tr>
		    </table>
			</td></tr>
  <!--<tr><td colspan="4" align="center" height="40"><table width=600 border="0" cellspacing="0" cellpadding="0">
  <tr>
   <td background="images/dot.gif"><img src="images/space.gif" width="8" height="1"></td>
  </tr>
</table>
</td></tr>-->
  </form></table></td>
  </tr>
  <!--<tr><td>
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
  <form action="/goform/formConfigDnsFilterGlobal" method="post" name="DnsFilter_Global">
  <tr><input type="hidden" name="gg" value=1>
    <td width="43%" height="30" align="right" id="start dns">启用域名过滤</td>
	<td width="2%" >&nbsp;</td>
	<td ><input type="checkbox" name="DnsFilterEnable">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<input type="submit" class="input-button" value="保存"  id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="帮助" onClick="window.open('<% write(com_helppage); %>#DnsFilter')"></td>
  </tr>
   <tr><td class="helpfont" height="30" align="center" colspan="3" id="attention2">打勾表示启用域名过滤功能，只有启用域名过滤，配置的域名过滤才生效。</td></tr>
 
  <tr><td colspan="3" height="20">&nbsp;</td></tr>
  </form>
  
</table>

  </td></tr>-->
</table> 
  </td></tr><script language="javascript">init();</script>
</table></td></tr></table></td>
<!--////end-->
  <td width="15"></td>
  </tr>
</table>
</body>
</html>
