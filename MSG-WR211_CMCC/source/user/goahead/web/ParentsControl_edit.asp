<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<%langTran("common");%>
<%langTran("DnsFilter");%>
<%langTran("ParentsControl");%>
<title><%write(com_parentscontrol);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk" />
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
</head>
<style type="text/css">

</style>
<script type="text/javascript">
var urlCount=10;
var defaultMacAddress="ffaaffeeddcc";
var macAddress = new Array();
macAddress[0]="00aaffeeddcc";
macAddress[1]="ffaaffeeddcc";
macAddress[2]="eeaaffeeddcc";
macAddress[3]="99aaffeeddcc";
<%
getArpFromArptable();
getCurrentIPMacInfo();
aspOutTimeGrpList();
%>
<%
	var edit_Name="";
	edit_Name=Form("id");
	if ( edit_Name!="") getOneParentsControlInfo(edit_Name);//需要修改
%>
//i_macs[1]="99aaffeeddcc";
var edit_Names = "<% write(Form("id"));%>";

function init()
{
	var f=ParentControl_Config;
	var exist_flag = 0;
	if(edit_Names!=""&&edit_Names!=null){
		f.Action.value="modify";
		f.username.value=userName;
		f.usernameOld.value=userName;
		f.AllowControlEnable.checked=(Enable=="1"?true:false);
		f.timeRangeName.value=timeGrp;
		document.getElementById("writeMacAddress").value=userMac;
		currentMac = userMac;

		for(i=0;i<confurlCount;i++){
		    var option1=document.createElement("OPTION");
		    option1.text=userUrl[i];
		    option1.value=userUrl[i];
		    f.SiteList.options.add(option1);
		}

	} else {
	
	}
/*		
	for(var i=0;i<i_totalrecs;i++) {
	    var option=document.createElement("OPTION");
	    option.text=i_macs[i];
	    option.value=i_macs[i];
	    f.selMacAddress.options.add(option);
	}
		*/
		if(currentMac != "") {
		document.getElementById("writeMacAddress").value=currentMac.toLowerCase();
		}
		for(var i=0;i<i_totalrecs;i++){
		    var option = document.createElement("option");
                    option.value = i_macs[i].toLowerCase();
                    option.innerHTML = i_macs[i].toLowerCase();
		    if(i_macs[i].toLowerCase() == currentMac.toLowerCase()) {
			option.selected = true;
			exist_flag = 1;
		    }
                    document.getElementById("selMacAddress").appendChild(option); 
		}
		if(exist_flag == 0) {
		    if(currentMac != "") {
		    var option = document.createElement("option");
                    option.value = currentMac.toLowerCase();
                    option.innerHTML = currentMac.toLowerCase();
		    option.selected = true;
                    document.getElementById("selMacAddress").appendChild(option); 
		    }
		}
}

function save()
{
	var tbl=ParentControl_Config;
	if(checkData()){
		//tbl.AllowControlEnable.value=;
		if(tbl.Action.value == "add")
			tbl.usernameOld.value= tbl.username.value;

		ParentControl_Config.action="/goform/formParentsControlConf";
		ParentControl_Config.submit();
	}
}
function checkData()
{
	var f = ParentControl_Config;
	if(f.Action.value == "modify")
	{
	    if(f.usernameOld.value != f.username.value)
	    {
		alert("<%write(par_usererror);%>");
		f.username.value=userName;
		return false;
	    }
	}
	if(!checkName(f.username,11,"<%write(com_user);%>",0)) return false;
	if(!checkMac(f.writeMacAddress,"<%write(com_MAC);%>",0)) return false;
	getDomain();
	return true;
}
function checkNameAllowSpace(cntrl,maxlen,tips,allownull)
{
	var name=cntrl.value;
	if(name=="")
	{
		if(!eval(allownull))
		{
		  alert(tips+"<%write(com_null);%>");
		  cntrl.focus();
		  return false;
		}
		else
			return true;
	}
	else if (name.length > maxlen)
	{
		alert(tips+"<%write(com_chkLenTip);%>"+maxlen+"<%write(com_lenAppend);%>");
		cntrl.focus();
		return false;
	}
	else if (name.indexOf(";")!=-1||name.indexOf(",")!=-1||name.indexOf("%")!=-1 || name.indexOf("\"")!=-1 || name.indexOf(":")!=-1 || name.indexOf("\'")!=-1 || name.indexOf("\\")!=-1)
	{
		alert(tips+"<%write(com_chkName);%>");
		return false;
	}
	else
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
function getDomain()
{
    var tbl=ParentControl_Config;
    for(var i=0;i<tbl.SiteList.length;i++){
	if(tbl.SiteList[i].value!=""){
	    tbl.SiteVals.value += tbl.SiteList[i].value+',';
	}
    }
    tbl.SiteVals.value=tbl.SiteVals.value.substring(0,tbl.SiteVals.value.length-1);
}
function addDomain()
{
    var tbl=ParentControl_Config;
    if(urlCount-tbl.SiteList.length<1){
	alert(_("<%write(dns_addTip);%>"));
	return ;
    }
    for(i=0;i<tbl.SiteList.length;i++){
	if(tbl.allowSite.value.toLowerCase()==tbl.SiteList[i].value.toLowerCase()){
	    alert(_("<%write(dns_existTip);%>"));
	    return ;
	}
    }
    if (!checkName(tbl.allowSite,45,"<%write(dns_domain);%>",0)) return ;
    if (!checFilterName(tbl.allowSite,45,"<%write(dns_domain);%>",0)) return ;

    var option1=document.createElement("OPTION");
    option1.text=tbl.allowSite.value;
    option1.value=tbl.allowSite.value;
    tbl.SiteList.options.add(option1);
    tbl.allowSite.value="";
}
function Reset(){
	ParentControl_Config.reset();
	if(ParentControl_Config.selMacAddress.length>0) {
	    for(var i=(ParentControl_Config.selMacAddress.length)-1;i>=0;i--){
		ParentControl_Config.selMacAddress.remove(i);
	    }
	}
	document.getElementById("writeMacAddress").value="";
	if(ParentControl_Config.SiteList.length>0) {
	    for(var i=(ParentControl_Config.SiteList.length)-1;i>=0;i--){
		ParentControl_Config.SiteList.remove(i);
	    }
	}
	init();
}
function del(){
    var tbl=ParentControl_Config;
    if (tbl.SiteList.selectedIndex == -1) return;
    for (var i=(tbl.SiteList.options.length)-1;i>=0;i--) {
	if (tbl.SiteList.options[i].selected) {
	    //if(tbl.DnsList.options[i].text!=linestr)
		tbl.SiteList.remove(i);
	}
    }
}
function DelAll(){
    var tbl=ParentControl_Config;
    if(confirm(_("<%write(dns_delete);%>"))){
    for (var i=(tbl.SiteList.options.length)-1;i>=0;i--) {
		tbl.SiteList.remove(i);
    }
    }
}

</script>
<body background="<%write(com_bodyMain);%>" onLoad="init();" style="background-color:#ffffff;">
<table width="100%" border="0" cellspacing="0" cellpadding="0" height="100%">
<!--*****头部间隔****-->

<tr>
	<td colspan="3" height="15"></td>

</tr>
<!--****中间****-->
<tr>
	<td width="15"></td>
	<td>
    	<script language="javascript">//showSecondTitle("家长控制");</script>
		<table width="100%" border="0" height="100%" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
        <script language="javascript">//showerror(errorstr);</script>
        	<tr valign="top">
				<td>
                	<form action="" method="post" name="ParentControl_Config">
                    	<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
                        <p>&nbsp;</p>
                        	
                        	<tr>
                 				<td align="center" class="style7">
                                	<table border="0" width="600" align='center' cellpadding="0" cellspacing="0">
                                    	<tr><td colspan="4" height="10"><input type="hidden" name="Action" value="add">&nbsp;</td></tr>
                                    	<tr>
                                            <td width="27%" height="30" align="right" valign="middle"><%write(com_startUse);%></td>
                                            <td width="3%" height="30">&nbsp;</td>
                                            <td align="left"  height="30" valign="middle"><input type="checkbox" name="AllowControlEnable" style=" margin:0; padding:0"/></td>
                                            <td align="left" height="25">&nbsp;</td>                        	
                                        </tr>
                                        <tr>
                          					<td width="27%" height="30" align="right" valign="middle"><%write(com_user);%></td>
                          					<td width="3%" height="30"><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
                          					<td align="left"  height="30" valign="middle"><input type="text" name="username" class="input" style=" margin:0; padding:0;height:20;" /></td><input type="hidden" name="usernameOld" id="usernameOld">
                                			<td align="left" height="30">&nbsp;</td>
                          				</tr>
                                        
                                         <tr>
                          					<td width="27%" height="30" align="right" valign="middle"><%write(par_childMac);%></td>
                          					<td width="3%" height="30"><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
                          					<td align="left" height="30" valign="middle">
                                                <span>     
  <div   style="position:absolute;z-index:2; padding:0; margin:0;">   
      <select   class="input" name="selMacAddress"  id="selMacAddress" style="position:absolute;top:-11px; width:   140px; height: 20px; clip:   rect(2   138   18   121); "   onChange='writeMacAddress.value=this.options[this.selectedIndex].value'>   
          <!--<option   value="00aaffeeddtt">00aaffeeddtt</option>   
          <option   value="ffaaffeeddtt">ffaaffeeddtt</option>   
          <option   value="eeaaffeeddtt">eeaaffeeddtt</option>   
          <option   value="99aaffeeddtt">99aaffeeddtt</option>  --> 
          
      </select> 
  </div>  
  <div   style="position:absolute;z-index:1; padding:0; margin:0;">     
      <input  class="input" maxlength="17" name="writeMacAddress" id="writeMacAddress"  type="text"   style=" border-color:#7f9db9;position:absolute; top:-11px; width:   140px; height:   20px;">   
  </div> 
  </span> 
                                            </td>
                                			<td align="left" height="30">&nbsp;</td>
                          				</tr>

                                        <tr>
                                            <td width="27%" height="30" align="right" valign="middle"><%write(par_allows);%></td>
                                            <td width="3%" height="30">&nbsp;</td>
                                            <td align="left" height="30" valign="middle"><input type="text" name="allowSite" class="input" size="45" maxlength="49" style=" margin:0; padding:0;height:20;" /><input type="button" value="<%write(com_add);%>" name="addSite" class="input-button" onClick="addDomain()"/></td>
                                            <td align="left" height="30">&nbsp;</td>
                                        </tr>
                                        <tr><td colspan="4" height="5"></td></tr>

                                        <tr>
                                           <td width="27%" height="30" align="right" valign="middle" > </td>
                                             <td width="3%">&nbsp;</td>
                                             <td colspan="2" height="20" class="helpfont" ><%write(par_addDomainTip);%></td>
                                           
                                         </tr>
										<tr><td colspan="4" height="20">&nbsp;</td></tr>
                                        
                                        <tr>
                                            <td width="27%" height="30" align="right" valign="top" id="domain list"><%write(par_siteList);%></td>
                                            <td width="3%" height="40">&nbsp;</td>
                                            <td align="left" colspan="2"><select class="input" name="SiteList" size=10 multiple  id="sitelist"  color:blue  style="font-size:10pt;SCROLLBAR-FACE-COLOR: #ffffff;  SCROLLBAR-HIGHLIGHT-COLOR: #ffffff;  #c0ddff 1px solid; SCROLLBAR-SHADOW-COLOR: #ffffff; LINE-HEIGHT: 20px; SCROLLBAR-ARROW-COLOR: #c0ddff; SCROLLBAR-TRACK-COLOR: #ffffff; SCROLLBAR-DARKSHADOW-COLOR: #c0ddff; SCROLLBAR-BASE-COLOR: #c0ddff;border:1px solid #c0ddff; width:370px " border=0  >
                                              
                                            </select></td><input type="hidden" name="SiteVals" id="SiteVals">
                                          </tr>
					<tr>
					    <td width="27%" height="30" align="right" valign="middle">&nbsp;</td>
					    <td width="3%" height="30">&nbsp;</td>
					    <td height="30" colspan="2" align="center" valign="middle"><input type="button" value="<%write(com_delete);%>" class="input-button" onClick="del()" id="delete"> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<input type="button" value="<%write(com_remove);%>" class="input-button" onClick="DelAll()" id="delete all"></td>
					</tr>

                                         <tr><td colspan="4" height="20">&nbsp;</td></tr>
                                        <tr>
                                            <td width="27%" height="30" align="right" valign="middle"><%write(dns_effectTime);%></td>
                                            <td width="3%" height="30">&nbsp;</td>
                                            <td align="left"  height="30" valign="middle"><select name="timeRangeName" class="input">
                                                <option value=""><%write(dns_allTime);%></option>
						<script language="javascript">
						for(var i=0;i<addTimeGroupNames.length;i++)
						{
						document.write('<option value='+addTimeGroupNames[i]+'>'+addTimeGroupNames[i]+'</option>');
						}
						</script>
                                            </select></td>
                                            <td align="left" height="30">&nbsp;</td>
                                        </tr>
                                         <tr>
                                           
                                             <td width="27%" height="30" align="right" valign="middle" > </td>
                                             <td width="3%">&nbsp;</td>
                                             <td colspan="2" height="20" class="helpfont" ><%write(par_enter);%><a href="javascript:parent.main.location='TimeRange.asp'"><span style="font-weight:bold"><%write(com_netAction);%></span>-><span style="font-weight:bold;"><%write(com_timeConf);%></span></a><%write(par_toSetTime);%></td>
                                           
                                        </tr>
                                         <tr><td colspan="4" align="center" height="40"> 
					 <input type="button" name="uttsave" class="input-button" value="<%write(com_save);%>"  onClick="save()" id="uttsave"/>
					 <input type="button" name="resetControl" class="input-button" value="<%write(com_reset);%>" onClick="Reset()"/>
					 <input type="button" name="help" class="input-button" value="<%write(com_help);%>" onClick="window.open('<% write(com_helppage); %>#ParentsControl')"/> 
					 <input type="button" name="return" onClick="GoBack_Page('ParentsControl.asp')" class="input-button" value="<%write(com_back);%>" /></td></tr>
                                        <tr><td colspan="4" height="10">&nbsp;</td></tr>
                                    </table>
								</td>  	
                            </tr>
						</table>
                    </form>
                </td>            
            </tr>
		</table>
    </td>
    	<td width="15"></td>
</tr>
</table>

</body>
</html>
