<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html> 
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<title>联动管理-网络拓扑</title>
<link rel=stylesheet type="text/css" href="../css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="../js/common.js"></script>
<script type="text/javascript" src="js/common_switch.js"></script>
<script type="text/javascript" src="js/topo.js"></script>
<style type="text/css">
.lifont{ font-size:12px; font-family:"黑体", "华文楷体", "宋体"}
</style>
<script language="javascript">
var errorstr="";
<%
var show=Form("show");
if (show == 'yes') aspOutPutGetTopoy();
else write("var maxDepth=0;"); 
%>
//此格式输出topoNode(parent,mac,ip,name,deviceType,version,isVirtual,warningLevel,parentPort,port,vlanId,desc)
var topo=new topoInit(maxDepth,'level');
topo.start();
function changeShowModel(tagObj,fullScreenTag,normalTag)
{
	this.curMostOuter="70,955,70";//parent.document.getElementById('mostOuterFrameSet').getAttribute("cols");
	this.curMain="88,*,29";//parent.document.getElementById('mainFrameSet').getAttribute("rows");
	this.curMiddle="205,*";//parent.document.getElementById('middleFrameSet').getAttribute("cols");
	this.tagObj=(typeof(tagObj) == 'string') ? document.getElementById(tagObj) : tagObj;
	//这里的tagObj必须含有value属性。否者需重写change成员函数
	this.fullScreenTag=fullScreenTag || "全屏显示";
	this.normalTag=normalTag || "退出全屏";
	if(window.parent!=window.self) 
	{
		if(parent.document.getElementById('mostOuterFrameSet').getAttribute("cols") == this.curMostOuter)
		{
			this.tagObj.value=this.fullScreenTag;
		}
		else
		{
			this.tagObj.value=this.normalTag;
		}
	}
	/*******************
	全屏显示
	*******************/
	this.showInFullScreen=function()
	{
		if(window.parent==window.self) return;
		parent.document.getElementById('mostOuterFrameSet').setAttribute("cols","0,*,0");
		parent.document.getElementById('mainFrameSet').setAttribute("rows","0,*,0");
		parent.document.getElementById('middleFrameSet').setAttribute("cols","0,*");
	}
	/**************************
	退出全屏
	***************************/
	this.showInNormal=function()
	{
		if(window.parent==window.self) return;
		parent.document.getElementById('mostOuterFrameSet').setAttribute("cols",this.curMostOuter);
		parent.document.getElementById('mainFrameSet').setAttribute("rows",this.curMain);
		parent.document.getElementById('middleFrameSet').setAttribute("cols",this.curMiddle);
	}
	/*********
	改变显示模式
	**********/
	this.change=function()
	{
		if(window.parent==window.self) return;
		if(this.tagObj.value == this.fullScreenTag)
		{
			this.showInFullScreen();
			this.tagObj.value=this.normalTag;
		}
		else
		{
			this.showInNormal();
			this.tagObj.value=this.fullScreenTag;
			
		}
	}
	
}
var csm=null;
var moveDivObj = null;
function init()
{
	var obj=document.getElementById("tbPs");
	moveDivObj=moveDiv(obj, TOPOY);/*生成可移动层*/
	moveDivObj.init("联动管理");/*初始化*/
	try
	{
		obj.style.display="table";
	}
	catch(e)
	{
		obj.style.display="block";
	}
	
	if(maxDepth < 1)
	{
		document.getElementById('noTopoy').style.display="block";
	}
	else
	{
		document.getElementById('topoyCaption').innerHTML="<font color=red>提示：</font>点击拓扑图中的设备图片，可对该设备进行远程管理";
	}
	csm=new changeShowModel("changeShow","全屏显示","退出全屏");
}
function unLoad()
{
	logOut();
	if(unloadFlag)
	{
		csm.showInNormal();
	}
}
function save()
{
	unloadFlag=0;
	TOPOY.action="/goform/formTopoyDiscovery"
	TOPOY.submit();
}
function showPs(x,y,width,heigth)
{
	var obj=null;
	var divTopo=document.getElementById("divTopo");
	var width=width || 230;
	var heigth=heigth || 80;
	var xDiff=divTopo.offsetLeft;//x方向实际坐标与svg的差值
	var yDiff=divTopo.offsetTop;//y方向实际坐标与svg的差值
	while(divTopo=divTopo.offsetParent)//获取实际left与top值
	{
		xDiff+=divTopo.offsetLeft;
		yDiff+=divTopo.offsetTop;
	}
	moveDivObj.show(width, heigth, x+xDiff, y+yDiff);
	obj=document.getElementById("passStr");
	inputRight(obj);
	obj.value="";
	obj.focus();
}
function hidePs()
{
	moveDivObj.hide();
}
function manageOk()
{
	if(!checkPassword1(TOPOY.passStr,11,1)) return;
	openForm(TOPOY,"remote_manage.asp",750,470);
	hidePs();
	//TOPOY.target="_blank";
	//TOPOY.action="remote_manage.asp";
	//TOPOY.submit();
}
var unloadFlag=1;
</script>
</head>
<body onLoad="init()" onbeforeunload="logOut()" background="../images/body_main.png" style="background-repeat:repeat-x">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
   <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
   <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle("网络拓扑");</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr,80);//显示保存返回的错误信息</script>
  <tr valign="top">
    <td>
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
    <td align="center"  class="style7"><table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td>
	<table border="0" width="80%" align='center'><tr><td class="gapTd"></td><td align="center">
    <!--//直接将页面的form填在此处，例如：<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
 <form name="TOPOY" id="TOPOY"   method="post">
       <input type="hidden" value="topoy" name="parentPage"> 
       <input type="hidden"  name="ipStr" id="ipStr">
       <input type="hidden"  name="macStr" id="macStr">
       <input type="text" style="display:none"><!--此控件不可删除，用于解决ie单个input则按[ENTER]自动提交的特性-->
	<table border="0"  align='center' id="topoTd" width="100%">
    <tr><td height="15" colspan="2"></td></tr>    
    <tr><td width="42%"><input type="button" class="input-button" value="全屏显示" id="changeShow" onClick="csm.change();">
    <input type="button" class="input-button" value="拓扑发现"  onClick="save();">
    <input type="button" class="input-button" value="帮助" name="input2" onClick="window.open('../rhelpinfo.htm#topoy')"></td>
      <td width="58%" align="right" valign="bottom" id="topoyCaption"></td>
    </tr>
    <tr id="noTopoy" style="display:none"><td colspan="2" align="center">请点击拓扑发现按钮，进行网络拓扑发现</td></tr>
    <tr align="center"><td colspan="2">
    <script language="javascript">
	document.write('<div  id="divTopo"><embed   src="other/tu.svg" style="width:'+topo.topoWidth+'px;height:'+topo.topoHeight+'px;margin:0;padding:0;" wmode="transparent" type="image/svg+xml" pluginspage="http://www.adobe.com/svg/viewer/install/"></embed></div>' );
    </script></td></tr>
    <tr><td height="22" colspan="2" align="left" valign="middle"> <a href="http://www.adobe.com/svg/viewer/install/" target="_blank">(如果不能正常显示请安装svgviewer)</a></td></tr>
    <tr height="20"><td colspan="2"></td></tr>  
    </table>

    <table id="tbPs"  style="display:none">
    <tr><td height="31" colspan="2"  align="center">请输入密码:
      <input type="password" size="13" maxlength="11" value="" onChange="checkPassword1(this,11,1);" class="input" name="passStr" id="passStr"></td></tr>
    <tr><td colspan="2" align="center"><input name="input" type="button" class="input-button" onClick="manageOk()" value="确定">
      <input name="" type="button" class="input-button"  value="取消" onClick="hidePs();"></td></tr>
    </table>
  
    </form>
 </td> <td class="gapTd"></td></tr></table>
 </td>
  </tr>
</table> 
  </td></tr>
</table></td></tr></table></td>
<!--////end-->
<td width="15"></td>
  </tr>
</table>
</body>
</html>

