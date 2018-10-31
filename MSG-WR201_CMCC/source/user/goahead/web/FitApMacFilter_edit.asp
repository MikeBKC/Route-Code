
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("FitApMacFilter"); %>
<% langTran("common"); %>
<title><% write(com_utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

<% 
//write(GetInstMaxNum(0)); 
%>
<%
	var edit_Name="";
	edit_Name=Form("oldName");
	if (edit_Name != "")
	{
		getOneServiceZoneMacFilter(edit_Name);
	}
%>	
<% getServiceZone();%>
var edit_Names = "<% write(Form("oldName"));%>";
var edit_Macs="<% write (Form("oldMac")); %>";

//start交互
//var MacFilterEnables="0";var filterRules="";var filterMacs=new Array();var totalrecs=0;var errorstr="";
//var SZones=new Array();
//end交互
//filterRules=(filterRules==""?"1":filterRules);
 
function init(){
	var f=MacFilterGloable_Config;
	var text="";	
	if((edit_Names!="")&& (edit_Macs == ""))
	{
		f.confName.value = confName;
		f.oldConfName.value=edit_Names;
		if(mode =="1")
		{
		    f.filterRule[0].checked=true;
		    f.filterRule[1].checked=false;
		}else if(mode=="2")
		{
		    f.filterRule[0].checked=false;
		    f.filterRule[1].checked=true;
		}
		for(var j=0;j<SZnames.length;j++)
		{
			eval("f.SZ_"+SZnames[j]).checked = true;	
		}
		f.Action.value = "modify";
		for(var i=0;i<macData.length;i++)
		{
			text += macData[i]+"\r";
		}
		f.textarea.value = text;
	}else if((edit_Names == "")&&(edit_Macs != ""))
	{
		var filterMacs = new Array();
		filterMacs=edit_Macs.split(",");
	    for(var i=0;i<filterMacs.length;i++)
	    {
	    text += filterMacs[i]+"\r";
	    }
	    f.textarea.value = text;
	}

}
var userAgent = navigator.userAgent.toLowerCase();
var is_opera = userAgent.indexOf('opera') != -1 && opera.version();
var is_moz = (navigator.product == 'Gecko') && userAgent.substr(userAgent.indexOf('firefox') + 8, 3);
var is_ie = (userAgent.indexOf('msie') != -1 && !is_opera) && userAgent.substr(userAgent.indexOf('msie') + 5, 3);

function save(){
	var tbl=MacFilterGloable_Config;
	tbl.data.value = "";
	var arr1 = new Array();
	var arr2 = new Array();
	var k=0;
	var tempMAC;
	var circleMAC = new Array();

	if(re_checkSpecialName(tbl.confName, '<%write(mf_confName);%>'))
	{
	    alert("<%write(mf_nameInvalid);%>");
		 return;
	}
	if(tbl.confName.value == "")
	{
		alert("<%write(mf_confNameNull);%>");
		return;
	}

	if(tbl.filterRule[0].checked)
	{
	    tbl.mode.value="1";
	}else{
	    tbl.mode.value="2";
	}
	arr1 = tbl.textarea.value.split("\n");
	for(var i = 0;i<arr1.length-1;i++)
	{
	    if((is_ie == false) || (i == arr1.length - 1))
	    {
		arr1[i] = arr1[i].slice(0,arr1[i].length);		/*非IE内核浏览器不去掉每行后面的回车*/
	    }
	    else
	    {
		arr1[i] = arr1[i].slice(0,arr1[i].length-1);		/*IE内核浏览器去掉每行后面的回车*/
	    }
	}
	
	for(var i = 0;i<arr1.length;i++)
	{

 		arr1[i] = arr1[i].replace(/	\s*/g," ");/*去除多余的空白（空格，制表符等）*/
		if(arr1[i] == "" || arr1[i].replace(/(^\s*)|(\s*$)/g, "") == "")  //去掉空行或者只含空格的行    added by cli.
			continue;
		//alert("arr1["+i+"] = "+arr1[i]);
		//tbl.data.value += arr1[i];
		//tbl.data.value += ";";
		var temp;
		k++;
	
		tempMAC = arr1[i];
		tempMAC = tempMAC.replace(/-/g,'');
		tempMAC = tempMAC.replace(/:/g,'');
		tempMAC = tempMAC.toLowerCase();

		if(!checktempMac(tempMAC,null,0))
		{
			alert("<%write(com_the);%>"+k+"<%write(mf_macErr);%>");
			return ;
		}
		//alert("tempMAC = "+tempMAC);
		circleMAC[k] = tempMAC;
		for(var j = 0;j<circleMAC.length;j++)
		{
			if(j == k)
				continue;
			if(circleMAC[j] == tempMAC)
			{
				alert("<%write(com_the);%>"+j+"<%write(mf_macSame);%>"+k+"<%write(mf_macAppend);%>");
				return ;
			}

		}
	
	tbl.data.value += tempMAC;
	tbl.data.value += "\r\n";
	}
	//alert("data = " + tbl.data.value );
	if(!tbl.data.value)
	{
		alert("<%write(mf_filterNull);%>");
		return ;
	}
	tbl.servicezone.value = "";	
	for(var sznum = 0; sznum < SZones.length ; sznum++)
	{
		if(eval("tbl.SZ_"+SZones[sznum]).checked)	
		{
			tbl.servicezone.value += SZones[sznum];	
			tbl.servicezone.value += ",";	
		}
	}
	if(tbl.servicezone.value=="")
	{
		alert("<%write(mf_haveSZ);%>");
		return ;
	}
    var num = max_totalrecs;
	if(max_totalrecs-k<0){
		alert("<%write(mf_maxTip);%>"+num+"<%write(mf_maxAppend);%>");
		return ;
	}

	tbl.action="/goform/formFitApMacFilter";
	tbl.submit();
}

function fillSZ()
{
	var i;
	if(SZones.length > 5)
	{
		for(i=0; i < 5; i++)
		{
			document.write('<td width="4%" align="right"><input type="checkbox" name="SZ_' +SZones[i]+'"><\/td>'+'<td width="10%" align="left" height="22">' + SZones[i] + '<\/td>' );
		}
		document.write('<tr><td width="28%" height="25" align="right">&nbsp;<\/td>');
		document.write('<td width="2%" height="25">&nbsp;</td>');
		for( i = 5; i<SZones.length; i++)
		{
			document.write('<td width="4%" align="right"><input type="checkbox" name="SZ_' +SZones[i]+'"><\/td>'+'<td width="10%" align="left" height="22">' + SZones[i] + '<\/td>' );
		}
		for(i = 0;i<10 - SZones.length;i++)
		{
			document.write('<td width="4%">&nbsp;<\/td>');
			document.write('<td width="10%">&nbsp;</td>');
		}
		document.write('<\/tr>');
	}
	else
	{
		for( i=0; i < SZones.length; i++)
		{
			document.write('<td width="4%" align="right"><input type="checkbox" name="SZ_' +SZones[i]+'"><\/td>'+'<td width="10%" align="left" height="22">' + SZones[i] + '<\/td>' );
		}
		for(i = 0;i<5 - SZones.length;i++)
		{
			document.write('<td width="4%" >&nbsp;<\/td>');
			document.write('<td width="10%">&nbsp;</td>');
		}
	
	}

}
function Reset(){
    MacFilterGloable_Config.reset();
    init();

}
//-->
</script>
</head>
<body onbeforeunload="logOut()" background="images/body_main.png">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle("<% write(WirelessMacFilter); %>");</script>
  <table width="100%" border="0" height="550" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
  <tr valign="top">
    <td>
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"80%",
						2,
						new Array("<%write(mf_list);%>",'FitApMacFilter.asp',1,0),
						new Array("<%write(mf_conf);%>",'FitApMacFilter_edit.asp',1,1)
		);
		showThreeMenu();
	</script>
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<tr>
    <td align="center"  class="three_td">
	<table width="100%" align="center"  border="0" cellspacing="0" cellpadding="0">
	<form action="" onSubmit="return " name="MacFilterGloable_Config" method="post">
	<table border="0" width="80%" align='center' border="0" cellspacing="0" cellpadding="0">
	<tr><td colspan="12" height="20">&nbsp;</td></tr>
  <tr>
  <td width="28%" height="30" align="right"><%write(mf_confName);%></td>
  <td width="2%">&nbsp;</td>
  <td width="70%" colspan="10"><input name="confName" type="text" class="input" maxlength="11"> </td>
  <input type="hidden" value="" name="oldConfName">
  <input type="hidden" value="" name="mode">
  <input type="hidden" value="" name="servicezone">
  <input type="hidden" value="add" name="Action">
  </tr>
  <tr>
  <td width="28%" height="25" align="right" valign="middle" ><% write(FilterRule); %></td>
  <td width="2%" height="25">&nbsp;</td>
  <td width="70%" align="left"  height="25" valign="middle" colspan="10">
  <input type="radio" value="1" name="filterRule"  checked="checked">  <font style="font-size:10px; color:#666666"><script language="javascript">document.write("<% write(Allow); %>");</script></font></td>
  </tr>
   <tr>
  <td width="28%" height="25" align="right" valign="middle"></td>
  <td width="2%" height="25">&nbsp;</td>
  <td width="70%" align="left"  height="25" valign="middle" colspan="10">
<input type="radio" value="2" name="filterRule">  <font style="font-size:10px; color:#666666"><script language="javascript">document.write("<% write(Forbid); %>");</script></font></td>
  </tr>
  <tr>
	 <td align="right"  width="28%" height="25"><%write(mf_SZ);%></td>
	 <td  width="2%">&nbsp;</td>
	 <script language="javascript">fillSZ();</script>
	 </tr>
  <tr>
    <td height="20" colspan="12" align="center"><%write(mf_MacAddrList);%></td>
  </tr>
  <tr>
    <td colspan="12" align="center"> 
    <textarea id="textarea" name="textarea" style="font-size:15px; font-family:'新宋体'; width:500px; height:100px"></textarea><input type="hidden" name="data" id="data" value=""></td>
  </tr>
  <tr><td colspan="12" height="20">&nbsp;</td></tr>
  <tr><td  colspan="12">
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td  height="30" align="center" valign="top"><input type="button" class="input-button" value="<% write(com_save); %>" onClick="save()" id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<% write(com_reset); %>" onClick="Reset()" id="uttreset">&nbsp;&nbsp;&nbsp;<input type="button"  class="input-button" value="<% write(com_help); %>" onClick="window.open('<% write(com_helppage); %>#WlanMacFilter')">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<% write(com_back); %>"  onClick="GoBack_Page('FitApMacFilter.asp')" id="goback"></td>
  </tr>
</table>
  </td></tr>
	</table>
  </form></table></td>
  </tr>
</table> 
</td></tr>
</table></td>
  <td width="15"></td><script language="javascript">init();</script>
  </tr>
</table>
</body>
</html>
