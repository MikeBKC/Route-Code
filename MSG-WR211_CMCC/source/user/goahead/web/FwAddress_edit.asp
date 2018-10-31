<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<title>艾泰科技</title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="js/common.js"></script>
<script language="javascript">

var errorstr="";var linkPage="";
/////////////////////////////交互变量/////////////////////


var groupNames=new Array();
var RouteEnables="";
var DesIPs="";
var DesMasks="";
var GateWays="";
var Metrics="";
var Profiles="";

<% 
    getWanIfCount();
	var edit_Name="";
	edit_Name=Form("id");
	if ( edit_Name!="") aspOutStaticRouteOneInfo(edit_Name);//需要修改
%> 
var edit_Names = "<% write(Form("id"));%>";


<!--

var flag = false;
function dspChange()
{
    if (AddGroup_Config.idSel[0].checked == true) 
    {
	idList.style.display = 'none';
	idTxt.style.display = '';
    }
    else if (AddGroup_Config.idSel[1].checked == true) {
	idList.style.display = '';
	idTxt.style.display = 'none';
    }
     flag = !flag;
}

function checkContain(text)
{
	var tbl=AddGroup_Config;
	for(var i=0;i<tbl.AddrRange.options.length;i++)
	{
		if(text==tbl.AddrRange.options[i].text)
		{
			alert("本组内已经包含相同的内容");
			return false;
		}
	}
	return true;
}

function addAddrOptions()
{
	var tbl=AddGroup_Config;
	if(tbl.AddrRange.options.length>9)
	{
		alert("地址范围列表不能大于10条地址");
		return false;
	}

	if(!(checkIP(tbl.startIP,"起始地址",0))) return false;
	if(!(checkIP(tbl.endIP,"结束地址",0))) return false;
	if(compareIP(tbl.startIP.value,tbl.endIP.value)==1)
	{
		alert("结束地址必须大于起始地址");
		return false;
	}
	var option = document.createElement("OPTION");
	option.text += tbl.startIP.value;
	option.text +="-";
	option.text += tbl.endIP.value;
	option.value = option.text;
	if(checkContain(option.text)==false)return false;
	AddGroup_Config.AddrRange.add(option);
	tbl.startIP.value="";
	tbl.endIP.value="";
	return true;
}
function addAddrGroupNameOptions(ctl1,ctl2)
{
	var array=new Array();
	var array_value = new Array();
	var t=0;
	for(var i=0;i<ctl1.options.length;i++)
	{
		if(ctl1.options[i].selected)
		{
			array[t]=ctl1.options[i].text;
			array_value[t] = ctl1.options[i].value;
			t++;
		}
	}
	if(eval(t+ctl2.options.length)>10)
	{
		alert("地址范围列表不能大于10条地址");
		return	false;
	}
	addDataToSelect(ctl2,array,array_value,1,"");
	return true;
}

function fromXXtoXX()
{
	if(AddGroup_Config.idSel[0].checked)
		addAddrOptions();
	else{
		if(addAddrGroupNameOptions(AddGroup_Config.oldaddrList,AddGroup_Config.AddrRange)==true)
		delAddrGroupNameOption(AddGroup_Config.oldaddrList,AddGroup_Config.AddrRange);
	}
}

function havename(name,type)
{
	var j;
	for(j = 0;j<addrtotalrecs;j++){   //根据描述找组名
		if(addrremark[j]!=name)
			continue;
		else
			break;
	}
	name = groupNames[j];
	for(var i=0;i<addrtotalrecs;i++)
	{
		if(groupNames[i]==name)
		{
			//if(profiles[i]==type)return true;
			//else{alert("此组名和父组所属的区域不同");return false;}
		}
	}
	alert("没有这个组名");
	return false;
}

function delAddrRange()
{
	var tbl=AddGroup_Config;
	if(tbl.AddrRange.options.selectedIndex!=-1)
	tbl.AddrRange.remove(tbl.AddrRange.options.selectedIndex);
}

function newfromXXtoXX()
{
	var tbl=AddGroup_Config;
	for(var i=0;i<tbl.AddrRange.options.length;i++)
	{
		if(tbl.AddrRange.options[i].selected)
		{
			if(tbl.AddrRange.options[i].text.length>11)
			{
				tbl.idSel[0].checked=true;
				addr=strtok("-",tbl.AddrRange.options[i].text);
				tbl.AddrRange.remove(i);
				if(!verifyDottedIP(addr[0])){alert("IP地址段输入错误");return;}
				if(addr[1]==undefined){alert("IP地址段输入错误");return;}
				if(!verifyDottedIP(addr[1])){alert("IP地址段输入错误");return;}
				tbl.startIP.value=addr[0];
				tbl.endIP.value=addr[1];
				dspChange();
			}
			else
			{
				if(tbl.AddrRange.options[i].text==tbl.groupName.value)
				{
					alert("此地址组包含了自己");
					tbl.AddrRange.remove(i);
					return;
				}

				if(!havename(tbl.AddrRange.options[i].text,tbl.profile.options[tbl.profile.selectedIndex].value))
				{
					tbl.AddrRange.remove(i);
					return;
				}

				tbl.idSel[1].checked=true;
				var option = document.createElement("OPTION");
				option.text = tbl.AddrRange.options[i].text;
				option.value = tbl.AddrRange.options[i].value;
				tbl.oldaddrList.add(option);
				tbl.AddrRange.remove(i);
				dspChange();
			}
			break;
		}
	}
}

function init(){
	var f=AddGroup_Config;
	if(edit_Names!=""&&edit_Names!=null){//alert();
		f.RouteName.value=RouteNames;
		f.RouteNameold.value=RouteNames;
		f.RouteEnablew.checked=(RouteEnables=="1"?true:false);
		f.DesIP.value=DesIPs;
		f.DesMask.value=DesMasks;
		f.GateWay.value=GateWays;
		f.Metric.value=Metrics;
		selVal(f.Profile,Profiles);
		//f.Action.value="modify";
		f.RouteName.disabled = true;
	}
}

function checkDataBeforeSave()
{
var definedName=new Array("Inter_all","Extern_all");
	var tbl=AddGroup_Config;

	if(!checkName(tbl.groupName,11,"地址组名",0)) return false;
	//if(tbl.Action[0].checked)
	for(var i=0;i<groupNames.length;i++)
	{
		if((tbl.groupName.value).toLowerCase()==addrremark[i].toLowerCase())
		{
			alert("已包含组名"+tbl.groupName.value);
			return false;
		}
	}
	//if(tbl.Action[0].checked)
	for(var i=0;i<definedName.length;i++)
	{
		if((tbl.groupName.value).toLowerCase()==definedName[i].toLowerCase())
		{
			alert("不能使用系统默认名字"+tbl.groupName.value);
			return false;
		}
	}
	if(tbl.AddrRange.options.length==0)
	{
		alert("不能保存内容为空的组");
		return false;
	}
	return true;
}

function save()
{
	var tbl=AddGroup_Config;

	if (tbl.groupName.value.toLowerCase() == "noticeany")
	{
		alert("不能新建祖名为"+tbl.groupName.value+"的组");
		return;
	}

	if(tbl.idSel[0].checked==true)
	{
		if(tbl.startIP.value!=""||tbl.endIP.value!="")
		if(addAddrOptions()==false)return;
	}

	if(checkDataBeforeSave()==false)return;

	tbl.temp1.value="";
	for(var i=0;i<tbl.AddrRange.options.length;i++)
	{
	tbl.temp1.value+=tbl.AddrRange.options[i].value;
	tbl.temp1.value+=";";
	}
	for(var j=tbl.AddrRange.options.length;j<10;j++)
	{
	tbl.temp1.value+=";";
	}
	tbl.groupName.disabled=false;
	//itbl.profile.disabled=false;
	tbl.linkPage.value = linkPage;
	/*
	if (tbl.Action[0].checked) {
		if(maxrecs[0] - addrtotalrecs < 1)  
		{	    
	   		alert("没有空闲实例数可增加");	    
	    	return ;	
		}
	}
	*/
	//AddGroup_Config.action="/goform/";
	AddGroup_Config.submit();
}

function Reset(){
	AddGroup_Config.reset();
	init();
}
//-->
</script>
</head>
<body onLoad=""onbeforeunload="logOut()" background="images/body_main.png">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle('地址组配置');</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td><form action="/goform/"  name="AddGroup_Config" method="post">
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr><td>&nbsp;&nbsp;</td></tr>
  <tr>
    <td align="center" class="style7">
	
	
	
	
	
	<div>
		<!--<fieldset>-->
		<br><br>
		  <table width="100%"	 border="0" cellspacing="0" cellpadding="0">
		  <tr>
		  <td><table width="100%"  border="0" cellspacing="0" cellpadding="0">
		  <tr>
		  <td width="36%" height="30" align="right">地址组名</td>
		  <td width="4%">&nbsp;</td>
		  <td width="60%"><input name="groupName" type="text"	onClick=""   class="input" maxlength="11"></td>
		  </tr>
		  </table>
		  </td>
		  </tr>
			<tr>
		  <td>
		  <table width="100%"  border="0" cellspacing="0" cellpadding="0">
		  <!--<tr>
		  <td width="36%" height="30" align="right">所属区域</td>
		  <td width="4%">&nbsp;</td>
		  <td width="60%"><select name="profile" id="profile" style=" width:133px" class="input"  onChange="changeprofile()">
			<option selected value="LAN">LAN</option>
			<script language="javascript">
			for(var i=1;i<=wanIfCount;i++){
			    document.write('<option value="WAN'+i+'" >WAN'+i+'</option>');
			}
			</script>
		    </select></td>
		  </tr>-->
		  
		  <tr>
			<td height="0" colspan="3">
			<input type="hidden" name="hidDomain" id="hidDomain" size="10">
			<input type="hidden" name="hidID" id="hidId" size="10">
			<input type="hidden" name="fillId" id="fillId" size="10">
			<input type="hidden" name="temp1" id="fillId" size="10">
			<input type="hidden" name="linkPage" size="10"></td>
		  </tr>
		  </table>
		  </td>
			</tr>
	    </table>
		
		
		<div align="center">
		<table style="display" width="100%"  border="0" align="center" cellpadding="0" cellspacing="0">
		  <tr>
		    <td height="156" colspan="3" align="left" valign="top"> <table style="display" width="90%"	border="0" align="center" cellpadding="0" cellspacing="0">
			<tr>
			  <td colspan="3"> <table style="display:" width="99%"	border="0" align="center" cellpadding="0" cellspacing="0">
				<td width="31%" height="40"> </td>
				<td width="31%" > <input type="radio" id="idSel1" name="idSel" checked onClick="dspChange()" ><label for="idSel1">
&nbsp;新地址 </label></td>
				<td width="38%"> <input type="radio" id="idSel2" name="idSel" onClick="dspChange()"><label for="idSel2">
&nbsp;已有地址 </label></td>
			      </tr> </table></td>
			</tr>
			<tr>
			  <td width="25%"></td>
			  <td width="75%" align="center" colspan="3"> <div style="height:114px;width:200px;border: 1px solid #adafae" id="idTxt" class="input">
			    <table style=" background-color:#FFFFFF;display" width="200"  border="0" align="center" cellpadding="0" cellspacing="0">
                <tr><td height="25"></td></tr>
                  <tr>
                    <td width="30%" valign="middle">&nbsp;起始地址: </td>
                    <td width="5%"></td>
                    <td width="65%" height="36"><input name="startIP" type="text" class="input" id="startIP" onKeyPress="ipOnly()" size="16" />                    </td>
                  </tr>
                  <tr>
                    <td valign="middle">&nbsp;结束地址: </td>
                    <td width="5%"></td>
                    <td width="65%" height="22"><input name="endIP" type="text" class="input" id="endIP" onKeyPress="ipOnly()" size="16" />                    </td>
                  </tr>
                  <tr>
                    <td height="30"></td>
                  </tr>
                </table>
			  </div></td>
			</tr>
			<tr>
			 <td width="25%"></td>
			  <td width="75%" align="center" colspan="3"> <table style="background-color:#FFFFFF;display:none" bgcolor="#FFFFFF" width="100%"  border="0" align="center" cellpadding="0" cellspacing="0" id="idList">
				<td width="10%"> </td>
				<td width="224" colspan="3" valign="top"> <div align="left" style=" background-color:#FFFFFF width=200px;overflow-x:auto">
				  <select class="input" name="oldaddrList" size=7 multiple  id="oldaddrList" ondblclick="fromXXtoXX()"style=" background-color: #ffffff; SCROLLBAR-FACE-COLOR: #ffffff;  SCROLLBAR-HIGHLIGHT-COLOR: #ffffff;	#c0ddff 1px solid; SCROLLBAR-SHADOW-COLOR: #ffffff; LINE-HEIGHT: 20px; SCROLLBAR-ARROW-COLOR: #c0ddff; SCROLLBAR-TRACK-COLOR: #ffffff; SCROLLBAR-DARKSHADOW-COLOR: #c0ddff; SCROLLBAR-BASE-COLOR: #c0ddff;border:1px solid #c0ddff; width:200px" border=0>
                  </select>
				</div></td>
			      </tr> </table></td>
			</tr>
		      </table></td>
		    <td width="53%" valign="top"> <table style="display" width="100%"  border="0" align="center" cellpadding="0" cellspacing="0">
			<tr>
			  <td width="13%" height="40" valign="middle">&nbsp;</td>
			  <td align="center" width="230px"><font style="FONT-FAMILY: "Arial", ",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",","Helvetica", ",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",","sans-serif";FONT-SIZE: 12px">地址范围列表:</font></td>
			  <td></td>
			</tr>
			<tr>
			  <td rowspan="4"><table id="idImg">
			      <tr>
				<td><input name="m1" type="button" value="==>" onClick="fromXXtoXX()" style="width:65px"/></td>
			      </tr>
			      <tr>
				<td height="30"><input name="m2" type="button" value="<==" onClick="newfromXXtoXX()" style="width:65px" /> </td>
			      </tr>
 <tr>
				<td height="30"><input name="delete" type="button" value="删 除" onClick="delAddrRange()" style="width:65px"/></td>
			      </tr>

			     </table></td>
			  <td width="87%">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
			    <!--<select class="input" size=7 name="AddrRange"  multiple	 id="addrRange" ondblclick="delarrow()" style="border:0px;width=200px" border=0 >
							</select> -->
			    <select class="input" name="AddrRange" size=7  id="AddrRange" ondblclick="newfromXXtoXX()"style="width:200px" border=0>
                </select></td>
			</tr>
		      </table></td>

		  </tr>
		</table>
	      </div>
	     <!-- </fieldset>-->
		   <!--<table width="100%"	border="0" cellspacing="0" cellpadding="0">
		    <tr>
		    <td height="40" align="center">
		    <table width="100%">
			<tr>
			<td width="41%" height="40" align="right">&nbsp;</td>
			<td width="5%"></td>
			<td width="24%" align="left">&nbsp;</td>
			<td width="29%"><input name="save1" type="button" value=" 保 存 " onClick="save()"/></td>
			<td width="1%">&nbsp;</td>
			</tr>
		    </table>			
		    </td>
		    </tr>
		   </table>
	    <br />
		<table class="htable" width="100%">
          <tr class="header">
            <td>使用提示：</td>
          </tr>
		  <tr>
		    <td>
		  1.配置地址组时，其输入格式为A.B.C.D-A.B.C.E，例如：192.168.16.1-192.168.16.250；
		</td>
		  </tr>
        </table>-->
	</div>
		
	
	
	
	
	
	
  <table border="0" width="90%" align='center'>
  <tr><td colspan="3" height="20"><input type="hidden" name="Action" value="add">&nbsp;</td></tr>

	   <tr><td colspan="4">
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td   height="30" align="center" valign="top"><input type="button" class="input-button" value="保存"  onClick="save()">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="重填" onClick="Reset()">&nbsp;&nbsp;&nbsp;<input type="button" value="帮助" class="input-button" onClick="window.open('<% write(com_helppage); %>#Route')">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="返回" onClick="GoBack_Page('FwAddress.asp')">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>
  </tr>
</table>
  </td></tr>
  </table></td>
  </tr>
</table>
</form></td>
  </tr><script language="javascript">init();</script>
</table></td>
  <td width="15"></td>
  </tr>
</table>
</body>
</html>
