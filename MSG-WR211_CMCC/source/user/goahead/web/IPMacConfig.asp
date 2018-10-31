
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("IPMac");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">


//function initTranslation()
//{
//
//}


var i_totalrecs = 0;
//var totalrecs = 40; //用于判断已经绑定了多少条数
//var maxtotalrecs = 50; //总计能够绑定多少条数
var i_ids = new Array(); 
var i_ips = new Array(); 
var i_macs = new Array();
var i_types = new Array(); 
var i_intfs = new Array();
var i_uses = new Array(); 
var i_flag = new Array(); 
var i_old_ip = new Array(); 
var i_ages = new Array(); 
<%aspOutReadArp();%>


var tmpArr = new Array(i_flag, i_ips, i_macs);/*数组对象被赋值时默认按地址传递*/
rowMajorArrSort(tmpArr, ipCmp, 1, 0);/*按ip地址升序排序*/
var len = 0;
var color = "Green";
var newids=new Array();
var newIPs=new Array();
var newMacs=new Array();
var newAllows=new Array();
var ipmacBindType=new Array();
var dhcpBindType=new Array();
var newBindType=new Array();
var newtotalrecs=totalrecs;
var num;
 

var strSecondMenuFocus="<%write(com_bandConf);%>";
 
function fillData(index,i)
{
	var tbl = User_Config;
	tbl.Action[1].checked = true;
	tbl.UserID.value = ids[i];
	tbl.textarea.value = IPs[i]+" "+Macs[i]+" "+Remark[i];
}
function radioClick()
{
	var tbl = User_Config;
	if (tbl.Action[0].checked)
	{
		tbl.hidId.value = "";
	}
	if (tbl.Action[1].checked)
	{
		tbl.hidId.value = tbl.fillId.value;
	}
}
 

function readArp() {
	var tbl = User_Config;
	if(!tbl.ipAddr.value){
			alert("<%write(ipm_readArp);%>");
			return;
		}
	if(!checkIP(tbl.ipAddr, "<%write(com_IP);%>", 0))
		return ;
	tbl.action = "/goform/formReadArp";
	User_Config.submit();
}


var userAgent = navigator.userAgent.toLowerCase();
var is_opera = userAgent.indexOf('opera') != -1 && opera.version();
var is_moz = (navigator.product == 'Gecko') && userAgent.substr(userAgent.indexOf('firefox') + 8, 3);
var is_ie = (userAgent.indexOf('msie') != -1 && !is_opera) && userAgent.substr(userAgent.indexOf('msie') + 5, 3);
function save()
{
	var tbl = User_Config;
	tbl.data.value = "";
	var arr1 = new Array();
	var arr2 = new Array();
	var k=0;
	var tempIP;
	var tempMAC;
	var tempRemark;
	var circleIP = new Array();
	var circleMAC = new Array();
	var circleRemark = new Array();
	var flag = 0;
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
		arr2 = arr1[i].split(" ");	
		flag =0;
	
		for(var n = 0;n<arr2.length;n++)
		{
			if(arr2[n].indexOf(" ") != -1 || arr2[n] =="" || arr2[n] == " ") //去掉空格
				continue;
			tempRemark = "";
			if(flag == 0)
			{
				tempIP = arr2[n];
				flag = 1;
				continue;
			}
			if(flag == 1)
			{
				tempMAC = arr2[n];
				flag = 2;
				continue;
			}
			if(flag ==2)
			{
				tempRemark = arr2[n];
				flag = 3;
				continue;
			}
			if(flag > 2)
			{
				alert("<%write(com_the);%>" + k + "<%write(com_configErr);%>");
				return ;
			}
			
		}
		//alert("tempIP = "+tempIP);
		if(!verifyDottedIP(tempIP))
		{
			alert("<%write(com_the);%>"+k+"<%write(ipm_ipErr);%>");
			return ;
		}
		if(tempMAC == null)
		{
			alert("<%write(com_the);%>"+k+"<%write(ipm_macNull);%>");
			return ;
		}
		else
		{
			tempMAC = tempMAC.replace(/-/g,'');
			tempMAC = tempMAC.replace(/:/g,'');
		}
 
		if(!checktempMac(tempMAC,null,0))
		{
			alert("<%write(com_the);%>"+k+"<%write(ipm_macErr);%>");
			return ;
		}
		if(tempRemark == null)
		{
			tempRemark = "";
		}
		if(!checkName(tempRemark,13,"<%write(com_the);%>"+k+"<%write(ipm_hang);%>"+"<%write(com_user);%>",1)) return false;
		//alert("tempIP = "+tempIP);
		//alert("tempMAC = "+tempMAC);
		//alert("tempRemark = "+tempRemark);
		circleIP[k] = tempIP;
		circleMAC[k] = tempMAC;
		circleRemark[k] = tempRemark;
		for(var j = 1;j<circleIP.length;j++)
		{
			if(j == k)
				continue;
			if(circleIP[j] == tempIP)
			{
				alert("<%write(com_the);%>"+j+"<%write(ipm_ipSame);%>"+k+"<%write(ipm_ipAppend);%>");
				return ;
			}
			if(circleMAC[j] == tempMAC)
			{
				alert("<%write(com_the);%>"+j+"<%write(ipm_macSame);%>"+k+"<%write(ipm_macAppend);%>");
				return ;
			}
			if(circleRemark[j] && circleRemark[j] == tempRemark)
			{
				alert("<%write(com_the);%>"+j+"<%write(ipm_userSame);%>"+k+"<%write(ipm_userAppend);%>");
				return ;
			}
		}
	
	tbl.data.value += tempIP;
	tbl.data.value += " ";
	tbl.data.value += tempMAC;
	tbl.data.value += " ";
	tbl.data.value += tempRemark;
	tbl.data.value += "\r\n";
	}
	//alert("data = " + tbl.data.value );
	if(!tbl.data.value)
	{
		alert("<%write(ipm_bandNull);%>");
		return ;
	}
	
        num = maxtotalrecs-totalrecs;
		if(maxtotalrecs-totalrecs-k<0){
			alert("<%write(ipm_maxTip);%>"+num+"<%write(ipm_maxAppend);%>");
			return ;
		
		}
		
		
    tbl.action="/goform/formAddReadArp";
	tbl.submit();
}

function init()
{
	 
	var tbl = User_Config;
	User_Config.reset();

	if(i_flag.length > 0)
	{
		for(var i = 0;i< i_totalrecs;i++)
		{
	
			tbl.textarea.value += i_ips[i];
			var temp = i_macs[i].split(":");
			for(var j = 0;j<temp.length;j++)
				tbl.textarea.value += temp[j];
			tbl.textarea.value += " " + " " + " " + " ";
			tbl.textarea.value += "\n";
		}
	}
	if(i_old_ip.length == 0)
		tbl.ipAddr.value = ip1s;
	else
		tbl.ipAddr.value = i_old_ip[0];

}
function selectIP()
{
	var count = 0;
	User_Config.Action[0].checked=1;

	if (User_Config.Arp.selectedIndex == -1) return;

	for (var i=0;i<User_Config.Arp.options.length;i++) {
		if (User_Config.Arp.options[i].selected) {
			if (++count > 1) {
				alert("<%write(ipm_onlyOne);%>");
				return;
			}
		}
	}

	if (count > 1) {
		alert("<%write(ipm_onlyOne);%>");
		return;
	}

	if (User_Config.Arp.options[User_Config.Arp.selectedIndex].text!="-------------------------------------------------")
	{
		var tmp = "";
		var array = null;

		index=User_Config.Arp.selectedIndex;
		User_Config.ID.value=trimstr(i_ips[index]);
		User_Config.oldID.value=trimstr(i_ips[index]);
		User_Config.IP.value=trimstr(i_ips[index]);

		array = strtok(":", i_macs[index]);

		for (var i=0;i<array.length;i++) {
			tmp += array[i];
		}

		User_Config.MAC.value = tmp;
	}
		User_Config.ID.disabled=false;
		User_Config.IP.disabled=false;
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
  <td><script language="javascript">showSecondTitle("<%write(ipm_ipmBand);%>");</script>
  <table width="100%" border="0" height="550" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td>
<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"80%",
						2,
						new Array("<%write(com_bandList);%>",'IPMac.asp',1,0),
						new Array("<%write(com_bandConf);%>",'IPMacConfig.asp',1,1)
		);
		showThreeMenu();
	</script>
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">

<tr>
     <td align="center" class="three_td" valign="top"><table width="600" border="0" cellspacing="0" cellpadding="0">

<form action="/goform/User_Config" method="post" name="User_Config" id="User_Config"> 
 
<tr>
    <td align="center" >
  <table border="0" width="600" align='center' >
		<tr height="50" valign="bottom">
		<td width="10%" >&nbsp;</td>
		<td width="77%">
		<table width="100%" align="center" >
		<tr>
		

		<td width="450" align="right" ><input type="radio" name="Action" value="modify" onClick=radioClick() style="display:none"><input name="Action" type="radio" value="add" checked onClick=radioClick() style="display:none"><%write(com_segment);%>:
          <input type="text" name="ipAddr" id="ipAddr" class="input" size="14" onKeyPress="ipOnly()"/>
          &nbsp;<font color="#666666">/24</font></td>
		<td width="38"  align="right"> <input  type="button"  class="input-button" name="readArp2" id="readArp2" onClick="readArp()" value="<%write(ipm_scan);%>" /></td>
		</tr>
		</table>		</td>
		<td width="13%">&nbsp;</td>
		</tr>
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
		<td  align="right"><input name="save22" type="button" class="input-button" value="<%write(ipm_band);%>" onClick="save()" id="uttsave" ></td>
		</tr>
		</table>		</td>
		<td width="13%">&nbsp;</td>
		</tr>

	</table>
	 
		<table width="100%" border="0" align="center" cellpadding="0" cellspacing="0" height="35">
		 <tr> 
		 <td width="100%" height="20" colspan="4" class="helpfont" ><%write(ipm_helpOne);%></td>
		 </tr><tr>
  <td colspan="4" height="20" class="helpfont" ><%write(ipm_helpTwo);%></td></tr><tr>
  <td colspan="4" height="20" class="helpfont" ><%write(ipm_helpThree);%></td></tr>
 </table>
 
	    <br /><br />
 <!--<table class="htable" width="100%">
 <tr >
 <td>使用提示：</td>
 </tr>	
 <tr>
   <td>1.在本页面文本框中可实现一个或多个IP/MAC的绑定，并能使用粘贴、复制、删除等操作对文本框进行编辑；</td>
 </tr> 	
 <tr>
   <td>2.配置IP/MAC绑定时，其输入格式为“IP+MAC+描述”，例如：192.168.16.100 0022aaAFCdb3 张三；</td>
 </tr>
 <tr><td>3.注意在上述输入格式中IP与MAC、MAC与描述之间可有一个或多个空格。	</td></tr>	
 </table>-->	
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
