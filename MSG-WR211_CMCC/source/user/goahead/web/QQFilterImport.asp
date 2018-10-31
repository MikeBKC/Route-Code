<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("QQFilter");%>
<%langTran("QQFilterImport");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

<%aspExceptQQData();%>
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
		tempRemark = arr2[n];
		flag = 2;
		continue;
	    }
	    if(flag > 1)
	    {
		alert("<%write(com_the);%>" + k + "<%write(com_configErr);%>");
		return ;
	    }
	}
	if(!isNum(tempName))
	{
	    alert("<%write(com_the);%>"+k+"<%write(QQF_hang);%>"+"<%write(qqf_numType);%>");
	    return;
	}
	if(tempRemark == null)
	{
	    tempRemark = "";
	}
	circle[k] = tempName;
	for(var j = 1;j<circle.length;j++)
	{
	    if(j == k)
		continue;
	    if(circle[j] == tempName)
	    {
		alert("<%write(com_the);%>"+j+"<%write(QQF_userName);%>"+k+"<%write(QQF_nameApp);%>");
		return ;
	    }
	}
/*
	if (tempName >= 2700000000 && tempName <= 2799999999) {
	    alert("<%write(qqf_qq27error);%>");
	    return;
	}
	*/
	if ((tempName > 4294967295 && tempName < 13300000000)||(tempName > 18999999999))
	{
	    alert("<%write(com_the);%>"+k+"<%write(QQF_userLen);%>");
	    return ;
	}
	for(var m = 0;m<totalrecs;m++)
	{
	    if(tempName == qqNumber[m] )
	    {
		alert("<%write(com_user);%>"+tempName+"<%write(QQF_userExist);%>");
		return ;
	    }
	}
	if(	getBytesCount2(tempRemark) > 31)
	{
	    alert("<%write(com_the);%>"+k+"<%write(QQF_remarkLen);%>");
	    return ;
	}
	if(parseInt(totalrecs+ k) > max_totalrecs)
	{
	    alert("<%write(QQF_noInst);%>"+k+"<%write(QQF_instApp);%>");
	    return ;
	}
	tbl.data.value += tempName;
	tbl.data.value += " ";
	tbl.data.value += tempRemark;
	tbl.data.value += ";";
	tempName = "";
	tempRemark = "";
    }
    if(!tbl.data.value)
    {
	alert("<%write(QQF_null);%>");
	return ;
    }
    tbl.action="/goform/formQQFilterImport";
    tbl.submit();
}
function init()
{
	 
	var tbl = User_Config;
	User_Config.reset();
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
  <td><script language="javascript">showSecondTitle("<%write(com_QQImp);%>");</script>
  <table width="100%" border="0" height="550" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
  <tr valign="top">
    <td>
<p>&nbsp;</p>
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">

<tr>
     <td align="center" class="three_td" valign="top"><table width="600" border="0" cellspacing="0" cellpadding="0">

<form action="" method="post" name="User_Config" id="User_Config"> 
 
<tr>
    <td align="center" >
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
    <td	 height="30" align="center" valign="top"><input type="button" class="input-button" value="<%write(com_save);%>" onClick="saveall()" id="uttsave"> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
      <input type="button" class="input-button" value="<%write(com_back);%>" onClick="GoBack_Page('QQFilter.asp')" id="returnback"></td>
  </tr>
		</table>		</td>
		<td width="13%">&nbsp;</td>
		</tr>

	</table>
	 
		<table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" height="35">
		 <tr> 
		 <td width="100%" height="20" colspan="4" class="helpfont" ><%write(QQF_helpOne);%></td>
		 </tr><tr>
  <td colspan="4" height="20" class="helpfont" >
<%write(QQF_helpTwo);%></td></tr>
 </table>
 
	    <br /><br />
 </td></tr>
</form> </table>
  </td></tr>
  <script language="javascript">init();</script>
</table> </td>
<!--////end-->

  </tr>
</table></td><td width="15"></td></tr></table>
</body>
</html>
