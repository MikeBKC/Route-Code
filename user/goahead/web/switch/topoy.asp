<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html> 
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<title>��������-��������</title>
<link rel=stylesheet type="text/css" href="../css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="../js/common.js"></script>
<script type="text/javascript" src="js/common_switch.js"></script>
<script type="text/javascript" src="js/topo.js"></script>
<style type="text/css">
.lifont{ font-size:12px; font-family:"����", "���Ŀ���", "����"}
</style>
<script language="javascript">
var errorstr="";
<%
var show=Form("show");
if (show == 'yes') aspOutPutGetTopoy();
else write("var maxDepth=0;"); 
%>
//�˸�ʽ���topoNode(parent,mac,ip,name,deviceType,version,isVirtual,warningLevel,parentPort,port,vlanId,desc)
var topo=new topoInit(maxDepth,'level');
topo.start();
function changeShowModel(tagObj,fullScreenTag,normalTag)
{
	this.curMostOuter="70,955,70";//parent.document.getElementById('mostOuterFrameSet').getAttribute("cols");
	this.curMain="88,*,29";//parent.document.getElementById('mainFrameSet').getAttribute("rows");
	this.curMiddle="205,*";//parent.document.getElementById('middleFrameSet').getAttribute("cols");
	this.tagObj=(typeof(tagObj) == 'string') ? document.getElementById(tagObj) : tagObj;
	//�����tagObj���뺬��value���ԡ���������дchange��Ա����
	this.fullScreenTag=fullScreenTag || "ȫ����ʾ";
	this.normalTag=normalTag || "�˳�ȫ��";
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
	ȫ����ʾ
	*******************/
	this.showInFullScreen=function()
	{
		if(window.parent==window.self) return;
		parent.document.getElementById('mostOuterFrameSet').setAttribute("cols","0,*,0");
		parent.document.getElementById('mainFrameSet').setAttribute("rows","0,*,0");
		parent.document.getElementById('middleFrameSet').setAttribute("cols","0,*");
	}
	/**************************
	�˳�ȫ��
	***************************/
	this.showInNormal=function()
	{
		if(window.parent==window.self) return;
		parent.document.getElementById('mostOuterFrameSet').setAttribute("cols",this.curMostOuter);
		parent.document.getElementById('mainFrameSet').setAttribute("rows",this.curMain);
		parent.document.getElementById('middleFrameSet').setAttribute("cols",this.curMiddle);
	}
	/*********
	�ı���ʾģʽ
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
	moveDivObj=moveDiv(obj, TOPOY);/*���ɿ��ƶ���*/
	moveDivObj.init("��������");/*��ʼ��*/
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
		document.getElementById('topoyCaption').innerHTML="<font color=red>��ʾ��</font>�������ͼ�е��豸ͼƬ���ɶԸ��豸����Զ�̹���";
	}
	csm=new changeShowModel("changeShow","ȫ����ʾ","�˳�ȫ��");
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
	var xDiff=divTopo.offsetLeft;//x����ʵ��������svg�Ĳ�ֵ
	var yDiff=divTopo.offsetTop;//y����ʵ��������svg�Ĳ�ֵ
	while(divTopo=divTopo.offsetParent)//��ȡʵ��left��topֵ
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
   <!--*****���ϼ��****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****�м�����****-->
  <tr>
   <td width="15"></td>
  <!--////����-->
  <td><script language="javascript">showSecondTitle("��������");</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr,80);//��ʾ���淵�صĴ�����Ϣ</script>
  <tr valign="top">
    <td>
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
    <td align="center"  class="style7"><table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td>
	<table border="0" width="80%" align='center'><tr><td class="gapTd"></td><td align="center">
    <!--//ֱ�ӽ�ҳ���form���ڴ˴������磺<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
 <form name="TOPOY" id="TOPOY"   method="post">
       <input type="hidden" value="topoy" name="parentPage"> 
       <input type="hidden"  name="ipStr" id="ipStr">
       <input type="hidden"  name="macStr" id="macStr">
       <input type="text" style="display:none"><!--�˿ؼ�����ɾ�������ڽ��ie����input��[ENTER]�Զ��ύ������-->
	<table border="0"  align='center' id="topoTd" width="100%">
    <tr><td height="15" colspan="2"></td></tr>    
    <tr><td width="42%"><input type="button" class="input-button" value="ȫ����ʾ" id="changeShow" onClick="csm.change();">
    <input type="button" class="input-button" value="���˷���"  onClick="save();">
    <input type="button" class="input-button" value="����" name="input2" onClick="window.open('../rhelpinfo.htm#topoy')"></td>
      <td width="58%" align="right" valign="bottom" id="topoyCaption"></td>
    </tr>
    <tr id="noTopoy" style="display:none"><td colspan="2" align="center">�������˷��ְ�ť�������������˷���</td></tr>
    <tr align="center"><td colspan="2">
    <script language="javascript">
	document.write('<div  id="divTopo"><embed   src="other/tu.svg" style="width:'+topo.topoWidth+'px;height:'+topo.topoHeight+'px;margin:0;padding:0;" wmode="transparent" type="image/svg+xml" pluginspage="http://www.adobe.com/svg/viewer/install/"></embed></div>' );
    </script></td></tr>
    <tr><td height="22" colspan="2" align="left" valign="middle"> <a href="http://www.adobe.com/svg/viewer/install/" target="_blank">(�������������ʾ�밲װsvgviewer)</a></td></tr>
    <tr height="20"><td colspan="2"></td></tr>  
    </table>

    <table id="tbPs"  style="display:none">
    <tr><td height="31" colspan="2"  align="center">����������:
      <input type="password" size="13" maxlength="11" value="" onChange="checkPassword1(this,11,1);" class="input" name="passStr" id="passStr"></td></tr>
    <tr><td colspan="2" align="center"><input name="input" type="button" class="input-button" onClick="manageOk()" value="ȷ��">
      <input name="" type="button" class="input-button"  value="ȡ��" onClick="hidePs();"></td></tr>
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

