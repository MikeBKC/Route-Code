<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("PPPoEImport");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">
//Butterlate.setTextDomain("firewall");

//function initTranslation()
//{
//
//}
<%aspGetPppoeUser();%> 
 
function getBytesCount2(str) 
{ 
	if (str == null) 
	{ 
		return 0; 
	} 
	else 
	{ 
		return (str.length + str.replace(/[\u0000-\u00ff]/g, "").length); 
	} 
}
//功能：检查是否合法英文用户名并返回提示信息
function re_checkPppoeEnglishName(name, tips) {
    var retips = "";
    var allowChar = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890.<>/?[]{}-_+=|`~!@$^&*()";
    for (i = 0; i < name.length; i++) {
        if (allowChar.indexOf(name.substring(i, i + 1)) == -1) retips = tips +"<%write(ppi_nameTip);%>";
    }
    return retips;;
}
function re_checkPppoePassword1(pas1, maxlen, allownull) {
    var retips = "";
    maxlen = toNumber(maxlen, 0, maxlen.length);
    if (pas1.length > maxlen)
	{
		retips = "<%write(ppi_pwdTip);%>" + maxlen + "<%write(ppi_pwdApp);%>";
    }
	else if (pas1 == "")
	{
		if (!eval(allownull))
		{
			retips = "<%write(ppi_pwdNull);%>";
		}
		else
		{
			retips = "";
		}
    }
    if (pas1.indexOf(",") != -1 || pas1.indexOf(";") != -1 || pas1.indexOf("；") != -1 || pas1.indexOf("%") != -1 || pas1.indexOf("\"") != -1 || pas1.indexOf("\'") != -1 || pas1.indexOf("\\") != -1 || 　pas1.indexOf(" ") != -1)
	{
        retips = "<%write(ppi_pwdExcep);%>";
    }
    return retips;
}
var userAgent = navigator.userAgent.toLowerCase();
var is_opera = userAgent.indexOf('opera') != -1 && opera.version();
var is_moz = (navigator.product == 'Gecko') && userAgent.substr(userAgent.indexOf('firefox') + 8, 3);
var is_ie = (userAgent.indexOf('msie') != -1 && !is_opera) && userAgent.substr(userAgent.indexOf('msie') + 5, 3);
function saveall()
{
	var retips;
	var tbl = User_Config;
	tbl.data.value = "";
	var arr1 = new Array();
	var arr2 = new Array();
	var k=0;
	var tempName;
	var tempPasswd;
	var tempRemark;
	var circle = new Array();
	var flag = 0;
	arr1 = tbl.textarea.value.split("\n");
	for(var i = 0;i<arr1.length;i++)
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
		arr1[i] = arr1[i].replace(/\t/g," ");				  //去掉制表符			added by cli.
		if(arr1[i] == "" || arr1[i].replace(/(^\s*)|(\s*$)/g, "") == "")  //去掉空行或者只含空格的行    added by cli.
			continue;
		var temp;
		k++;
		arr2 = arr1[i].split(" ");	
		flag =0;
		var n;
		for(n = 0;n<arr2.length;n++)
		{			
			if(arr2[n].indexOf(" ") != -1 || arr2[n] =="" || arr2[n] == " ") 
				continue;
			
			if(flag == 0)
			{
				tempName = arr2[n];
				flag = 1;
				continue;
			}
			if(flag == 1)
			{
				tempPasswd = arr2[n];
				flag = 2;
				continue;
			}
			if(flag > 1)
			{
				alert("<%write(com_the);%>" + k + "<%write(com_configErr);%>");
				return ;
			}
			if(flag ==2)
			{
				tempRemark = arr2[n];
				flag = 3;
				continue;
			}		
		}
		retips = re_checkPppoeEnglishName(tempName,"<%write(com_user);%>");
		if(retips != "")
		{
			alert("<%write(com_the);%>"+k+"<%write(ppi_hang);%>"+retips);
			return;
		}
		if(tempPasswd == undefined)
		{
		    alert("<%write(ppi_passwd);%>");
		    return;
		}
		retips = re_checkPppoePassword1(tempPasswd,31,0);
		if(retips != "")
		{
			alert("<%write(com_the);%>"+k+"<%write(ppi_hang);%>"+retips);
			return;
		}
		if(tempPasswd == null)
		{
			alert("<%write(com_the);%>"+k+"<%write(ppi_pwdInput);%>");
			return ;
		}
		if(tempRemark == null)
		{
			tempRemark = "";
		}
		if(tempPasswd == "")
		{
			alert("<%write(com_the);%>"+k+"<%write(ppi_input);%>");
			return ;
		}
		circle[k] = tempName;
		for(var j = 1;j<circle.length;j++)
		{
			if(j == k)
				continue;
			if(circle[j] == tempName)
			{
				alert("<%write(com_the);%>"+j+"<%write(ppi_userName);%>"+k+"<%write(ppi_nameApp);%>");
				return ;
			}
		}
		if(tempName.length > 31)
		{
			alert("<%write(com_the);%>"+k+"<%write(ppi_userLen);%>");
			return ;
		}
		if(tempPasswd.length > 31)
		{
			alert("<%write(com_the);%>"+k+"<%write(ppi_pwdLen);%>");
			return ;
		}
		for(var m = 0;m<totalrecs;m++)
		{
			if(tempName == PPPoENames[m] )
			{
				alert("<%write(com_user);%>"+tempName+"<%write(ppi_userExist);%>");
				return ;
			}
		}
		if(	getBytesCount2(tempRemark) > 31)
		{
			alert("<%write(com_the);%>"+k+"<%write(ppi_remarkLen);%>");
			return ;
		}
		if(parseInt(totalrecs+ k) > max_totalrecs)
		{
			alert("<%write(ppi_noInst);%>"+k+"<%write(ppi_instApp);%>");
			return ;
		}
	tbl.data.value += tempName;
	tbl.data.value += " ";
	tbl.data.value += tempPasswd;
	tbl.data.value += " ";
	tbl.data.value += tempRemark;
	tbl.data.value += ";";
	tempName = "";
	tempPasswd = "";
	tempRemark = "";
	}
	if(!tbl.data.value)
	{
		alert("<%write(ppi_null);%>");
		return ;
	}
	tbl.action="/goform/formPppoeImport";
	tbl.submit();
}
function init()
{
	 
	var tbl = User_Config;
	User_Config.reset();
}

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
  <td><script language="javascript">showSecondTitle(_("<%write(com_pppSer);%>"));</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"class="table_bg">
  <tr valign="top">
    <td><form name="User_Config" method="post" action="">
	<p>&nbsp;</p>

   <table id=grouplist style="display" width="95%"  border="0" align="center" cellpadding="0" cellspacing="0" height="400">
  <tr>
    <td align="center" class="style7" valign="top"><table id=grouplist style="display" width="700"  border="0" align="center" cellpadding="0" cellspacing="0">
			  <tr>
     <table border="0" width="600" align='center' >
  <tr><td colspan="3" height="30">&nbsp;</td></tr>
		<tr>
		<td>&nbsp;</td>
		<td align="center"><textarea id="textarea" name="textarea" style="font-size:15px; font-family:'新宋体'; width:492px; height:250px"></textarea></td>
		<td>&nbsp;</td>
		</tr>		
		<tr>
		<td width="10%" height="40">&nbsp;</td>
		<td width="77%">
		<table width="100%"  style="width:500px;" align="center">
		<tr>
		

		<td  ><input type="hidden" name="PageId"/>
			<input type="hidden" name="data" />
			<input type="hidden" name="UserID" /></td>
		</tr>
		<tr>
    <td	 height="30" align="center" valign="top"><input type="button" class="input-button" value="<%write(com_save);%>" onClick="saveall()" id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_back);%>" onClick="GoBack_Page('PPPoEServer.asp')"></td>
  </tr>
		</table>		</td>
		<td width="13%">&nbsp;</td>
		</tr>

	</table>
	 
		<table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" height="35">
		 <tr><td width="13%"></td>
		 <td width="87%" height="20" colspan="4" class="helpfont" ><%write(ppi_helpOne);%></td>
		 </tr><tr><td width="13%"></td>
  <td width="87%" colspan="4" height="20" class="helpfont" >
<%write(ppi_helpTwo);%></td></tr>
 </table>
 
			  </tr>
		  </table></td></tr>
<script language="javascript">init();</script></table>
  </form></td>
  </tr>
</table></td> 
  <td width="15"></td>
  </tr>
</table>
</body>
</html>
