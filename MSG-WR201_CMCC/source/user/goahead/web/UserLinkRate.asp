<html >
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<!-- TemplateBeginEditable name="doctitle" -->
<%langTran("common");%>
<%langTran("UserLinkRate");%>
<%langTran("commonWords");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script src="js/common_switch.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" src="js/pie.js"></script>
<script type="text/javascript" ContentType="text/xml">
//alert("1");
var errorstr="";
var perIpActCon = new Array();
var toEdit = new Array();
var toRemark = new Array();
var UserNames = new Array();
var Macs = new Array();
var AuthTypes = new Array();
var IPs = new Array();
var inbits = new Array();
var outbits = new Array();
var rxBytes= new Array();
var txBytes= new Array();
var startTimes = new Array();
var groupContents = new Array();
var groupContents1 = new Array();
var groupContents2 = new Array();
var ids=new Array();
var totalrecs = 0;
var remarks=new Array();
globalCheckTypeFlag = 0;/*备注输入错误时不显示红叉*/


/*
var pieFlowValues = {"聊天软件":100,"P2P软件":7,"股票软件":6,"网络视频":5,"网络游戏":4,"购物网站":3,"社交网站":2,"网页游戏":1,"邮件":6,"论坛":1,"其他":1};
var pieActValues = {"聊天软件":100,"P2P软件":7,"股票软件":6,"网络视频":5,"网络游戏":4,"购物网站":3,"社交网站":2,"网页游戏":1,"邮件":6,"论坛":1};
*/
var pieFlowValues = {"<%write(ulr_im);%>":0,"<%write(ulr_p2p);%>":0,"<%write(ulr_stock);%>":0,"<%write(ulr_video);%>":0,"<%write(ulr_game);%>":0,"<%write(ulr_shop);%>":0,"<%write(ulr_sns);%>":0,"<%write(ulr_webGame);%>":0,"<%write(ulr_mail);%>":0,"<%write(ulr_bbs);%>":0,"<%write(ulr_other);%>":0};
var pieActValues = {"<%write(ulr_im);%>":0,"<%write(ulr_p2p);%>":0,"<%write(ulr_stock);%>":0,"<%write(ulr_video);%>":0,"<%write(ulr_game);%>":0,"<%write    (ulr_shop);%>":0,"<%write(ulr_sns);%>":0,"<%write(ulr_webGame);%>":0,"<%write(ulr_mail);%>":0,"<%write(ulr_bbs);%>":0};
var perIpActs = new Array();
var perIpLevels = new Array();

<%aspOutUserStatusGlobal();%>
var pieFlowColors = new Array("#4dbbf6","#e55636", "#19aa5f", "#a8c656","#514e95","#e25693","#e8a73f","#bdbdbd","#9556e3","#0131ff", "#553910");
var pieFlow = new pieInit(pieFlowValues,pieFlowColors, "#677bde");
var pieActColors = new Array("#4dbbf6","#e55636", "#19aa5f", "#a8c656","#514e95","#e25693","#e8a73f","#bdbdbd","#9556e3","#0131ff");
var pieAct = new pieInit(pieActValues,pieActColors, "#677bde");

/*重写表格*/
var oldDspList=dspList;
dspList=function()
{
    oldDspList.apply(this,arguments);//等价于oldDspList(arugumens[0],arguments[1]...arguments[argumens.length-1])
    var td = frame12.document.getElementById('tableList').rows[1].cells;
    td[0].style.width = "10px";
    td[1].style.width = "80px";
    td[2].style.width = "80px";
    td[3].style.width = "55px";
    td[4].style.width = "80px";
    td[5].style.width = "30px";
    td[6].style.width = "70px";
    td[7].style.width = "30px";
    td[8].style.width = "70px";
    td[9].style.width = "130px";
    td[10].style.width = "90px";
    td[11].style.width = "100px";
    td[12].style.width = "30px";
    td[13].style.width = "30px";
}

var ajaxObj=ajax();
if(ajaxObj)//初始化成功
{
	ajaxObj.url="/goform/formRateForUserLink";
	ajaxObj.processHandle=function (str)
	{
		pieActValues = null;
		pieFlowValues = null;
		var Poptotal = UserNames.length;
		UserNames.popN(Poptotal);
		Macs.popN(Poptotal);
		AuthTypes.popN(Poptotal);
		IPs.popN(Poptotal);
		inbits.popN(Poptotal);
		outbits.popN(Poptotal);
		rxBytes.popN(Poptotal);
		txBytes.popN(Poptotal);
		startTimes.popN(Poptotal);
		groupContents.popN(Poptotal);
		perIpActCon.popN(Poptotal);
		toEdit.popN(Poptotal);
		toRemark.popN(Poptotal);
		perIpActs.popN(Poptotal);
		perIpLevels.popN(Poptotal);
		eval(str);
		pieAct.VALUES = pieActValues;
		pieFlow.VALUES = pieFlowValues;
		try{                                                                            
		    pieAct.drawPieFun();                                            
		    pieFlow.drawPieFun();                                           
		}catch(e) {};    
		E('level1Count').innerHTML=perIpLevels.countData("level1");
		for(i=0;i<totalrecs;i++){
		    var runday=parseInt((eval(startTimes[i])/3600)/24);
		    var runhour=parseInt(eval(startTimes[i])/3600%24);
		    var runmiu=parseInt((eval(startTimes[i])%3600)/60);
		    var runsec=parseInt((eval(startTimes[i])%3600)%60);
		    startTimes[i] =runday+"<%write(com_day);%>"+runhour+"<%write(com_hour);%>"+runmiu+"<%write(com_minute);%>"+runsec+"<%write(com_second);%>";
		
		    if((groupContents[i] != null) && (groupContents[i].length>10))
		    {
				groupContents1[i]=groupContents[i].slice(0,10);
				groupContents1[i]+="  <B>...</B>";
				groupContents2[i] = groupContents[i].replace(/ /g,"&#13;");
				groupContents[i] = '<span title="'+ groupContents2[i] + '">' + groupContents1[i] +'</span>';
		    }
		    	
			ids[i]=i;
			perIpActs[i]=getPerIpActCon(i);
			toEdit[i]="<img src='<%write(com_imgedit1);%>' title='<%write(ulr_modify);%>' style='cursor:pointer'  border='0' onclick='parent.showEdit(event,"+i+")'>";
			toRemark[i]=getPerIpRemark(i);
		}
		rowMajorArrSort(arrayData1, 0, 0, 0);
		for(i=0;i<totalrecs;i++){
			perIpLevels[i]=getPerIpActLevel(i);
		}
		recscount[0]=totalrecs;
		maxrecs[0]=totalrecs;
		Count.innerHTML = totalrecs;
		E('levelCount').innerHTML=totalrecs;
		RefreshList();
	}
}
function getPerIpRemark(i)
{
	var ret="";
	if(AuthTypes[i])
	{//认证方式存在
		ret = "<img src='../images/"+(remarks[i]?"<%write(com_imgremark1);%>":"<%write(com_imgremark2);%>")+"'title='<%write(ulr_remark);%>' style='cursor:pointer' border='0'  onclick='parent.showRemark(event,"+i+")'>";
	}
	return ret;
}
//获取每个ip的上网行警告级别
function getPerIpActLevel(i)
{
	var color;
	if(perIpLevels[i] == "level1")
	{
		color='red';
	}
	else if(perIpLevels[i] == "level2")
	{
		color='yellow';
	}
	else if(perIpLevels[i] == "level3")
	{
		color='green';
	}
	else
	{
	    color='';
	}
	var ret = "<div style='background-color:"+color+";width:12px;height:20px'></div>";
	return ret;
}
function getPerIpPerAct(percent,color)
{
	var ret = "";
	if(percent>0)
	{
		ret = "<td width='"+percent+"%' style='background-color:"+color+"'></td>"
	}
	return ret;
}
//获取每个ip的上网行为内容条图
function getPerIpActCon(index)
{
	var i = 0;
	var sum=0;
	var ret = "";
	for(i = 0;i< pieActColors.length;i++)
	{//获取该ip上网行为的和值
	   perIpActs[index][i] = eval(perIpActs[index][i]);
	   sum += perIpActs[index][i];
	}
	if(sum > 0)
	{
		ret = "<table cellpadding='0' cellspacing='0' width='100'><tr height='20'>"
		for(i = 0;i< pieActColors.length;i++)
		{//获得该ip上网行为的百分比
	  	 	ret += getPerIpPerAct(perIpActs[index][i]/sum*100, pieActColors[i]);
		}
		ret += "</tr></table>";
	}
	return ret;
}
function RefreshList(index, listhead, list, listTail) {
    dsplisthead(0, frame11);
    dsplistTail(0, listTail);
    dsplist(0, frame12);
}
//totalrecs = IPs.length;

maxrecs[0]=totalrecs;   //列表最大显示量
var strSecondMenuFocus="<%write(com_userSta);%>";

currpage[0]=1;
recscount[0]=totalrecs;
recsPerPage[0] =20;
lineButton[0] =20;
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]="<%write(ulr_userList);%>";
var titles1 = new Array();
var titles1Index = 0;
titles1[titles1Index++] = "&nbsp;";
titles1[titles1Index++] = "<%write(com_user);%>";
titles1[titles1Index++] = "<%write(com_MAC);%>";
titles1[titles1Index++] = "<%write(ulr_auth);%>";
/*titles1[titles1Index++] = "<%write(ulr_xuhao);%>";*/
titles1[titles1Index++] = "<%write(com_IP);%>";
titles1[titles1Index++] = "<%write(ulr_up);%>(Kbit/s)";
titles1[titles1Index++] = "<%write(ulr_upAll);%>(Mbit)";
titles1[titles1Index++] = "<%write(ulr_down);%>(Kbit/s)";
titles1[titles1Index++] = "<%write(ulr_downAll);%>(Mbit)";
titles1[titles1Index++] = "<%write(ulr_netTime);%>";
titles1[titles1Index++] = "<%write(ulr_group);%>";
titles1[titles1Index++] = "<%write(ulr_netAction);%>";
titles1[titles1Index++] = "<%write(ulr_set);%>";
titles1[titles1Index++] = "<%write(ulr_remark);%>";

sortable[0] = new Array(1,1,1,1,1,1,1,1,1,1,1,1,0,1);
arrayCount[0] = 15;

sortType[0] = new Array(TYPE_STR,TYPE_STR,TYPE_STR,TYPE_STR,TYPE_IP,TYPE_NUM,TYPE_NUM,TYPE_NUM,TYPE_NUM,TYPE_DATE,TYPE_STR,TYPE_STR,TYPE_STR,TYPE_STR);

var arrayData1=new Array(perIpLevels,UserNames, Macs, AuthTypes, IPs, inbits, txBytes, outbits, rxBytes, startTimes, groupContents,perIpActs,toEdit,toRemark,ids);
maxDspLen[0]=new Array(0,10,0,0,0,0,0,0,0,0,0,0,0,0,0);

function dsplist(index,list)
{
//alert("5");
	if(index==0) dspList(index, list, 910, titles1, titles1.length, arrayData1, 0, 0);
}

ajaxObj.start=function() {
	document.getElementById("off").disabled = false;
	var fun=(ajaxObj.method=='get'?ajaxObj.get:ajaxObj.send);
	fun();
	var interval = document.getElementById("interval").value * 1000;
	if(ajaxObj.isAutoRefresh) ajaxObj.ptr=setInterval(fun,interval);
	document.getElementById("on").disabled = true;
}	
ajaxObj.Stop=function()
{
	document.getElementById("on").disabled = false;
	clearInterval(ajaxObj.ptr);
	document.getElementById("off").disabled = true;
}

function sltOnchg()
{
	if (document.getElementById("on").disabled)
	{
		var index = document.getElementById("interval").value;
		clearInterval(ajaxObj.ptr);
		var fun=(ajaxObj.method=='get'?ajaxObj.get:ajaxObj.send);
		fun();
		if(ajaxObj.isAutoRefresh)
		{
			ajaxObj.ptr=setInterval(fun, (index * 1000));
		}
	}
}
var moveDivObj = null;
function showEdit(e,index)
{
    var obj=document.getElementsByName("frame12")[0];
	var xDiff=obj.offsetLeft;//x方向实际坐标与iframe的差值
	var yDiff=obj.offsetTop;//y方向实际坐标与iframe的差值
	var width = width || 230;
	var height = height || 230;
	var x = x || e.clientX;
	var y = y || e.clientY;
	while(obj=obj.offsetParent)//获取实际left与top值
	{
		xDiff+=obj.offsetLeft;
		yDiff+=obj.offsetTop;
	}
	x = x + xDiff - width;
	y = y + yDiff ;
	
    moduleDisplay("trEdit", 1);
	moduleDisplay("trRemark", 0);

	moveDivObj.show(width, height, x , y, "<%write(ulr_set);%>");
	StatEdit.ipaddr.value=IPs[ids.searchData(index)];/*要编辑的ip地址*/
	document.getElementById("editBt").focus();
}
function showRemark(e,index)
{
    var obj=document.getElementsByName("frame12")[0];
	var xDiff=obj.offsetLeft;//x方向实际坐标与iframe的差值
	var yDiff=obj.offsetTop;//y方向实际坐标与iframe的差值
	var width = width || 230;
	var height = height || 230;
	var x = x || e.clientX;
	var y = y || e.clientY;
	while(obj=obj.offsetParent)//获取实际left与top值
	{
		xDiff+=obj.offsetLeft;
		yDiff+=obj.offsetTop;
	}
	x = x + xDiff - width;
	y = y + yDiff ;
	
    moduleDisplay("trEdit", 0);
	moduleDisplay("trRemark", 1);
	moveDivObj.show(width, height,x,y,"<%write(ulr_remark);%>");
	StatEdit.userName.value=UserNames[index];
	StatEdit.authType.value=AuthTypes[ids.searchData(index)];
	StatEdit.remark.value=remarks[index];
	StatEdit.remark.focus();
}
function init()
{
	var obj=document.getElementById("tbEdit");
	moveDivObj=moveDiv(obj, StatEdit);/*生成可移动层*/
	moveDivObj.init("<%write(ulr_edit);%>");/*初始化*/
	moduleDisplay(obj, 1);
	/*
	UserStatusGlobal.UserStatusEnable.checked=(UserStatusEnable=="on"?true:false);
	*/
	if (UserStatusEnable == "on") {
		UserStatusGlobal.UserStatusEnable1.value  ="off";
		IDbutton.value = "<%write(ulr_close);%>";
	} else {
		UserStatusGlobal.UserStatusEnable1.value  ="on";
		IDbutton.value = "<%write(ulr_open);%>";
	}
	if(ajaxObj)
	{
		ajaxObj.start();
	}
}

/*
function save()
{
    var tbl = UserStatusGlobal;
    tbl.action = "/goform/formUserStatusGlobalConfig";
    tbl.submit();
}
*/
/* 开启识别统计按钮 */
function save()
{
    var tbl = UserStatusGlobal;
    tbl.action = "/goform/formUserStatusGlobalConfig";
    tbl.submit();
}
function clearData()
{
    var tbl = UserStatusGlobal;
    tbl.action = "/goform/UserStatusClearConfig";
    tbl.submit();
}
function saveEdit()
{
	StatEdit.action="/goform/ClearIpBvStats";
	StatEdit.submit();
}
function saveRemark()
{
    if(checkRemark(document.getElementsByName("remark")[0], 31, "<%write(ulr_remark);%>", 1))
    {
	StatEdit.action="/goform/formUserStatusRemark";
	StatEdit.submit();
    }
}
</script>

</head>

<body onbeforeunload="logOut()" onLoad="init();" background="<%write(com_bodyMain);%>">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <tr>
  <td width="15"></td>
  <td><script language="javascript">showSecondTitle('<%write(com_userSta);%>');</script>
  <table width="100%" border="0" height="500" cellspacing="0" cellpadding="0" align="center" class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td>
  <table width="90%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
    <td align="center"  class="style7"><table width="100%" border="0" cellspacing="0" cellpadding="0">
   
  <tr><td>
<table border="0"  align='center' width="680" style="display:none">
  <form name="UserStatusGlobal" method="post" action="">
    <tr><td colspan="3" height="20">&nbsp;</td></tr>
	<tr>
	    <td height="20" width="40%" align="right" id="UserStatus_Switch"><%write(ulr_start);%></td>
		<td align="left" width="5%"><input type="checkbox" name="UserStatusEnable" id="UserStatusEnable"></td>
		<td align="left">
		<input type="hidden" name="UserStatusEnable1" value=""><input type="hidden" name="UserStatusEnbleOld"><input type="button" class="input-button" id="uttsave" value="<%write(com_save);%>"  onClick="save()">
		<input type="button" value="<%write(ulr_clear);%>" class="input-button" onClick="clearData()">
		</td>
	</tr>
	<tr>
  <tr><td colspan="3" align="center" height="40"><table width=680 border="0" cellspacing="0" cellpadding="0">
  <tr>
   <td background="<%write(com_imgdot);%>"><img src="<%write(com_imgspace);%>" width="8" height="1"></td>
  </tr>
</table>
</td></tr><input type="hidden" value="1" name="re">
  </form> 
  </table></td></tr> 
  <tr>
  <td>
  
  <table border="0"  align='center' width="740">
    <tr><td height="5">&nbsp;</td></tr>
    <tr><td colspan="3" height="20">&nbsp;</td></tr>
	  <tr>
	    <td height="20" width="50%" align="center"><%write(ulr_flowAnaly);%></td>
		<td width="1"></td>
		<td align="center"><%write(ulr_actAnaly);%></td>
	</tr>
    <tr>
	  <td align="center"><embed id="svgPieFlow" name="svgPieFlow" src="other/pieFlow.svg" type="image/svg+xml" width="315px" height="230px" /></td>
	  <td></td>
	  <td align="center"><embed id="svgPieAct" name="svgPieAct" src="other/pieAct.svg" type="image/svg+xml" width="315px" height="230px"  /></td>
	</tr>
    <tr><td  colspan="3" height="5">&nbsp;</td></tr>
	<tr><td colspan="3" align="left" valign="middle"> <a href="http://download.adobe.com/pub/adobe/magic/svgviewer/win/3.x/3.03/en/SVGView.exe" target="_blank">(<%write(ulr_showErr);%>)</a></td>
    	</tr>
	<tr>
	<td height="20" width="40%" align="center"><input type="button" value="<%write(ulr_clear);%>" class="input-button" onClick="clearData()">&nbsp;&nbsp;<input type="button" value="<%write(ulr_open);%>" id="IDbutton" class="input-button" onClick="save()"></td>
	<td width="1%"></td>
	  <td align="center"><%write(ulr_total);%><span id = "Count">0</span><%write(ulr_linkNet);%></td>
	  </tr>
    <tr><td height="5">&nbsp;</td></tr>
	<tr>
	<td colspan="3" width="100%" align="center"><%write(ulr_tip);%></td>
	</tr>
    <tr>
	  <td height="5"></td>
	  <td></td>
	</tr>
    
  <tr><td colspan="3" align="center" height="40"><table width=650 border="0" cellspacing="0" cellpadding="0">
  <tr>
   <td background="<%write(com_imgdot);%>"><img src="<%write(com_imgspace);%>" width="8" height="1"></td>
  </tr>
</table>
</td></tr>
  </table>
  
<table border="0"  align='center' width="680">
<form name="UserStats" method="post" action="">
	<tr><td colspan="3" height="10">&nbsp;</td></tr>
	<tr><td colspan="3">
   <table id=grouplist style="display"  border="0" align="center" cellpadding="0" cellspacing="0">
		    <tr>
		      <td  colspan="4" valign="bottom"><table id=dellist  width="100%"  height="19" border="0" cellspacing="0" cellpadding="1">
			  <tr>
			  <td width="5"></td>
			  <td  class="style6">&nbsp;</td>
			  <td  class="style6" align="left"><%write(ulr_autoFresh);%>
                  <select name="interval" id="interval" class="input" onChange="sltOnchg()">
                      <option value="1">1</option>
                      <option value="2">2</option>
                      <option value="3">3</option>
                      <option value="4">4</option>
                      <option value="5" selected>5</option>
                  </select><%write(com_second);%>
    		</td> 
			  <td width="40" align="right"><input type="button" value="<%write(ulr_stopFresh);%>" class="input-button" onClick="ajaxObj.Stop()" id="off" name="off"></td>
				<td width="40" align="right"><input type="button" value="<%write(ulr_openFresh);%>" class="input-button" onClick="ajaxObj.start()" id="on" name="on"></td>
			  </tr>
		      </table></td>
		    </tr>
	<tr><td colspan="3" height="10">&nbsp;</td></tr>
   <tr>
   <td width="30%"  align="left"><span style="width:8px;height:8px;background-color:red">&nbsp;</span><%write(ulr_serious);%> <span style="width:8px; height:8px; background-color:yellow">&nbsp;</span><%write(ulr_slight);%> <span style="width:8px; height:8px; background-color:green">&nbsp;</span><%write(ulr_normal);%></td>
   <td  colspan="3" align="right"><%write(ulr_total);%><span id="levelCount"></span><%write(ulr_linkNet);%>，<font color="red"><span id="level1Count"></span><%write(ulr_body);%></font><%write(ulr_affect);%></td>
   </tr>
		    <tr>
		      <td colspan="4">
			<iframe src="_listhead1.htm" name="frame11"  align="MIDDLE" width="680" height="45" scrolling="no"  marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden ></iframe></td>
		    </tr>
		    <tr>
		      <td colspan="4">
			<iframe src="_list1.htm" name="frame12"	id="framelist"  align="MIDDLE" width="680" height="485" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe></td>
<script language="javascript" type="text/javascript">
//alert("8");
</script>
		    </tr>
		  </table></td></tr>
  </form>
</table>
<form name="StatEdit" method="post" action="">
<input type="hidden" name="ipaddr">
<input type="hidden" name="userName">
<input type="hidden" name="authType">
   <table id="tbEdit"  style="display:">
   <tr id="trEdit"><td><table>
    <tr><td height="31" align="center"><%write(ulr_click);%></td></tr>
    <tr><td align="center"><input name="clearBt" id="clearBt" type="button" class="input-button" onClick="saveEdit()" value="<%write(ulr_clear);%>"></td></tr>
    </table></td></tr>
    <tr id="trRemark"><td><table>
    <tr><td height="31" align="center"><textarea name="remark"  style="font-size:15px; font-family:'新宋体'; width:180px; height:60px"></textarea></td></tr>
    <tr><td align="center"><input name="remarkBt" type="button" class="input-button" onClick="saveRemark()" value="<%write(com_save);%>"></td></tr>
    </table></td></tr>
    </table>
</form>
</td></tr>
</table> 
  </td></tr>
</table></td>
<!--////end-->
  <td width="15"></td>
  </tr>
</table>
</body>
</html>

