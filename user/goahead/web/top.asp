<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<link rel=stylesheet type="text/css" href="css/style.css">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<% langTran("top"); %>
<% langTran("common"); %>
<% langTran("Link"); %>
<% langTran("commonWords"); %>
<title>Top Image</title>
<base target="_self" />
</head>
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="switch/js/common_switch.js"></script>
<style type="text/css">
.fz24{font-size:24px;}
.fz10{font-size:10px;}
.c_white{color:#fff;}
.c_grey{}
/*img{
	cursor:pointer;
	}*/
</style>
<script language="javascript">
//交互
<%getSysLang();%>
<%aspOutTopPageInfo();%>
<% outputSystemVersion();%>	//var revisions="nv3520Gv1.0.0-110908";
<% outputProductID();%>	//var ProductID="3520G";
//var ProductID="3520G";//型号
var sysVersion=revisions;//版本

//end交互
var replacePort=new Array();
var combNo = new Array(23,24);//SFP复用端口号
var ajaxObj= numPorts||wanIfCount?ajax():null;
var portImageAreaW = 200;
if(ajaxObj)//初始化成功
{
	ajaxObj.url="/goform/formStatForTopImage";
	ajaxObj.refreshTime=8;
	ajaxObj.processHandle=function (str)
	{
		//eval('curSpeed=new Array("Disabled","Down","Down","1Gfdx","Down","Down","Down","Disabled","Disabled","1Gfdx","Down","Down","Down","1Gfdx","1Gfdx","Down","Down","Down","Down","Down","Down","1Gfdx","1Gfdx","1Gfdx");isFiber=new Array(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1);');//后台发过来的数据形式
		eval(str);
		chgPortImage();
	}
}
 
 /*
function init_replacePort()
{
	if(!curSpeed) return;
	for(var i=0 ;i <combNo.length;i++)
	{
		replacePort[i] = curSpeed[combNo[i]-1].toLowerCase().indexOf("fiber") >=0 ? combNo[i] :0;
	}
}*/
function showPort(port)
{
	if(!curSpeed || !replacePort) return;
	var j=0;
	if(port <= 0) return;
	if(port<=numPorts)
	{
		if(curSpeed[port-1]!='Down' && curSpeed[port-1]!='Disabled' && replacePort.searchData(port)<0)
		{
			parent.main.location="switch/stat_detailed.asp?port="+port;
		}
	}
	else
	{
		if(curSpeed[port-1]!='Down' && curSpeed[port-1]!='Disabled')
		{
				parent.main.location="switch/stat_detailed.asp?port=WAN" + (port-numPorts);
		}
		/*
		else
		{
			j=replacePort[port-24-1]-1;
			if(j>=0 && curSpeed[j]!='Down' && curSpeed[j]!='Disabled')
			{
				parent.main.location="stat_detailed.asp?port="+replacePort[port-24-1];
			}
		}
		*/
	}
}
/**
根据服务器发送的数据更改端口图片以及提示信息
**/
function chgPortImage()
{
//	init_replacePort();
	var up=0,i=0,j=0;
	var portImages=document.getElementsByName("portImage");
	for(i=0;i<numPorts;i++)
	{
		up=i%2;
		j=replacePort.searchData(i+1);
		if( j>=0)//被复用SFP（SFP复用端口）
		{
			if(up) portImages[i].src="switch/images/jack_copper_disabled_top.png";
			else portImages[i].src="switch/images/jack_copper_disabled_bottom.png";
			portImages[i].title="Port "+(i+1)+": Dsiabled(<% write(TopBySFP); %> "+(j+1)+"<% write(TopMultiplex); %>)";
			portImages[i].alt="Port "+(i+1)+": Dsiabled(<% write(TopBySFP); %> "+(j+1)+"<% write(TopMultiplex); %>)";
			continue;
		}
		portImages[i].title="Port "+(i+1)+": "+curSpeed[i];
		portImages[i].alt="Port "+(i+1)+": "+curSpeed[i];
		if(curSpeed[i]=="Down")		
		{	
			if(up) portImages[i].src="switch/images/jack_copper_down_top.png";
			else portImages[i].src="switch/images/jack_copper_down_bottom.png";
		}
		else if(curSpeed[i]=="Disabled")		
		{	
			if(up) portImages[i].src="switch/images/jack_copper_disabled_top.png";
			else portImages[i].src="switch/images/jack_copper_disabled_bottom.png";
		}
		else	
		{	
			if(up) portImages[i].src="switch/images/jack_copper_link_top.png";
			else portImages[i].src="switch/images/jack_copper_link_bottom.png";
		}
	}
	for(i=0;i<wanIfCount;i++)
	{
		portImages[i+numPorts].title="WAN " + (i+1) + ": " + curSpeed[(i+numPorts)];
		portImages[i+numPorts].alt="WAN " + (i+1) + ": " + curSpeed[(i+numPorts)];
		if(curSpeed[(i+numPorts)]=="Down")		
		{	
			portImages[i+numPorts].src="switch/images/jack_copper_down_bottom.png";
		}
		else if(curSpeed[i+numPorts]=="Disabled")		
		{	
			portImages[i+numPorts].src="switch/images/jack_copper_disabled_bottom.png";
		}
		else	
		{	
			portImages[i+numPorts].src="switch/images/jack_copper_link_bottom.png";
		}
	}
	
/*
		for(i=0;i<replacePort.length;i++)
		{
			j=replacePort[i]-1;
			if(j>=0)
			{
				portImages[i+24].title="SFP "+(i+1)+": "+curSpeed[j]+"(<% write(TopMultiplexPort); %>  "+(j+1)+")";
				portImages[i+24].alt="SFP "+(i+1)+": "+curSpeed[j]+"(<% write(TopMultiplexPort); %>  "+(j+1)+")";
				if(curSpeed[j]=="Down")		
				{	
					portImages[i+24].src="switch/images/jack_sfp_down_top.png";
				}
				else if(curSpeed[j]=="Disabled")		
				{	
					portImages[i+24].src="switch/images/jack_sfp_disabled_top.png";
				}
				else	
				{	
					portImages[i+24].src="switch/images/jack_sfp_link_top.png";
				}
			}
			else
			{
				j=combNo[i]-1;
				if(curSpeed[j]=="Disabled")		
				{	
					portImages[i+24].title="SFP "+(i+1)+": Disabled";
					portImages[i+24].alt="SFP "+(i+1)+": Disabled";
					portImages[i+24].src="switch/images/jack_sfp_disabled_top.png";
				}
				else
				{
					portImages[i+24].title="SFP "+(i+1)+": Down";
					portImages[i+24].alt="SFP "+(i+1)+": Down";
					portImages[i+24].src="switch/images/jack_sfp_down_top.png";
				}
			}
		}
		*/
}
/**
绘制端口图片并自动适应分辨率
**/
function writeAllPorts()
{
	var scrW = parseInt(screen.width) >= 1280 ? 1280:parseInt(screen.width);
	var imageSrc;
	var diff=32; //端口图片宽度
	var oldHei=23;//端口图片的高度
	var spW=10;//图片分割空白的宽度
	var hei=oldHei;
	var iPort=1, i=0;
	var portTop=0;
	var numTop=0;
	diff = eval (scrW / 1280 *diff);
	hei = eval (scrW / 1280 *hei);
	spW = eval (scrW / 1280 *spW);
	if(parseInt(diff)<16)
	{ 
		spW = 16 / diff * spW;
		hei = 16 / diff * hei;
		diff=16;//最小16的宽度
	}

	var dis=0;
	/*绘制lan口相关*/
	for(iPort = 1;iPort<=numPorts;iPort+=2)//修改端口图片位置大小以及端口数字
	{
		if(iPort==1)
		{
		        dis = 0;
		}
		else if((iPort-1)%8 == 0)
		{/*每8个图片显示一组*/
		        dis=parseInt(dis)+spW;
		}
		for(i=0; i<2;i++)
		{
			if(i==0)/*下面一排图片。编号为奇数*/
			{
				portTop = 46;
				numTop  = 69 - (oldHei-hei);/*高度变化后，下面的数字上移*/
				imageSrc = "switch/images/jack_copper_down_bottom.png";
			}
			else/*上面一排图片。编号为偶数*/
			{
				portTop = 23 + oldHei-hei;/*图片高度变化后上面的图片应下移*/
				numTop = 7 + oldHei-hei;/*高度变化后，上面的数字下移*/
				imageSrc = "switch/images/jack_copper_down_top.png";
			}
			document.write('<a href="javascript:showPort('+(iPort+i)+');"><img name="portImage" title="Port '+(iPort+i)+': Down" alt="Port '+(iPort+i)+': Down" style="position: absolute; top: '+ portTop +'px; left: '+dis+'px; width: '+diff+'px; height: '+hei+'px;" src="'+imageSrc+'" id="state_1" border="0"></a>');  /*由于字体大小不变，故数字高度不用调整*/             
			document.write('<div classname="portlabel" class="portlabel" style="position: absolute; z-index: 2; top: '+numTop + 'px; left: '+dis+'px; width: '+diff+'px; height: 23px;" align="center">'+(iPort+i)+'</div>');
		}
		
		dis=parseInt(dis)+parseInt(diff);

	}
	/*绘制wan口相关*/
	for(iPort=1;iPort<=wanIfCount;iPort++)
	{
		if(iPort==1)
		{
		        dis=parseInt(dis)+spW;
		}
		portTop = 46;
		numTop  = 69 - (oldHei-hei);/*高度变化后，下面的数字上移*/
		document.write('<a href="javascript:showPort('+(iPort+numPorts)+');"><img name="portImage" title="WAN '+iPort+': Down" alt="WAN '+iPort+': Down" style="position: absolute; top: '+ portTop +'px; left: '+dis+'px; width: '+diff+'px; height: '+hei+'px;" src="switch/images/jack_copper_down_bottom.png" id="state_1" border="0"></a>');  /*由于字体大小不变，故数字高度不用调整*/             
		document.write('<div classname="portlabel" class="portlabel" style="position: absolute; z-index: 2; top: '+numTop + 'px; left: '+dis+'px; width: '+diff+'px; height: 23px;" align="center">W'+iPort+'</div>');
		dis=parseInt(dis)+parseInt(diff);
	}
	portImageAreaW = dis;
	
}
function init()
{
	if(dispUser==1){
	    initization();
        }
	fixPNG(document.getElementById("logoImg"));
	if(ajaxObj) ajaxObj.start();
	//自适应分辨率设置
	document.getElementById('portImageArea').style.width=portImageAreaW + 'px';
	if(portImageAreaW<300) return;
	if( parseInt(screen.width) >= 1280) return;
	
	//logo宽度自适应设置
	var logoWidth=document.getElementById('logoImg').getAttribute('width');
	logoWidth = eval (screen.width / 1280 *logoWidth);
	var logoHeight =document.getElementById('logoImg').getAttribute('height');
	logoHeight = eval (screen.width / 1280 *logoHeight);
	if(screen.width<1024)
	{/*1024以下先采取以下模式*/
		logoHeight=100/logoWidth*logoHeight;
		logoWidth=100;
	}
	document.getElementById('logoImg').style.width=logoWidth+'px';
	document.getElementById('logoImg').style.height=logoHeight+'px';
	//end
	
	
	if(screen.width<1024)
	{
		document.getElementById('assTd1').style.display="none";
	}	
	else if(screen.width<1280)
	{
		document.getElementById('assTd1').setAttribute("width",'5%');	
	}
	
	
}
/**
使背景透明PNG支持IE6
**/
function fixPNG(myImage) { 
var arVersion = navigator.appVersion.split("MSIE"); 
var version = parseFloat(arVersion[1]); 
if ((version >= 5.5) && (version < 7) && (document.body.filters)) 
{
     var imgID = (myImage.id) ? "id='" + myImage.id + "' " : ""; 
     var imgClass = (myImage.className) ? "class='" + myImage.className + "' " : ""; 
     var imgTitle = (myImage.title) ? "title='" + myImage.title   + "' " : "title='" + myImage.alt + "' "; 
     var imgStyle = "display:inline-block;" + myImage.style.cssText; 
     var strNewHTML = "<span " + imgID + imgClass + imgTitle 
 
   + " style=\"" + "width:" + myImage.width 
 
   + "px; height:" + myImage.height 
 
   + "px;" + imgStyle + ";" 
 
   + "filter:progid:DXImageTransform.Microsoft.AlphaImageLoader" 
 
   + "(src=\'" + myImage.src + "\', sizingMethod='scale');\"></span>"; 
	 myImage.outerHTML=strNewHTML;
} 
} 


/***显示在线ap***/
var onlineap = 0;
var offlineap = 0;
var onlineuser = 0;
var refresh = 5;
var aj=ajax();
function showAPnum(){
var str ='<div style="border:0px solid blue; position:relative; width:100%; margin:5px 0 0 0;">'+
		'<div style="width:100%; height:70px; border:0px red solid; position:absolute; left:0px; top:0px;">'+
			'<table border="0" bordercolor="green" width="100%" cellpadding="0" cellspacing="0">'+
				'<tr valign="bottom">'+
					'<td align="right" width="15%"><img id="APImg" src="images/apico.png" style="cursor:pointer;" /></td>'+
					'<td width="14%" align="center" valign="middle"><span class="fz24 c_white" id="online">'+onlineap+'</span></td>'+
					'<td width="14%" align="center" valign="middle"><span class="fz24 c_white" id="offline">'+offlineap+'</span></td>'+
					'<td width="2%" rowspan="2" align="center"><img src="images/banner.png" /></td>'+
					'<td width="16%" align="center"><div id="showInfo" style="cursor:pointer;z-index:999999;position:relative; border:0px red solid; width:100%;height:100%;"><img src="images/userico.png" /></div></td>'+
					'<td  align="center" width="16%" valign="middle"><span id="onlineuser" class="fz24 c_white">'+onlineuser+'</span></td>'+
					'<td width="22%"></td>'+
				'</tr>'+
				'<tr valign="middle">'+
					'<td class="c_grey fz10" align="right"><table border="0" bordercolor="red" width="100%" cellpadding="0" cellspacing="0"><tr valign="bottom"><td width="45%"></td><td width="55%" align="center">AP</td></tr></table></td>'+
					'<td class="c_grey fz10" align="center">在线</td>'+
					'<td class="c_grey fz10" align="center">离线</td>'+
					'<td class="c_grey fz10" align="right"><table style="text-align:center;" border="0" bordercolor="red" width="100%" cellpadding="0" cellspacing="0"><tr valign="bottom"><td>无线用户</td></tr></table></td>'+
					'<td class="c_grey fz10" align="center">在线</td>'+
					'<td width="20%"></td>'+
				'</tr>'+
			'</table>'+
		'</div>'+
	'<div style="border:0px solid green; width:100%;height:10px; position:absolute;z-index:999999; left:0px; top:60px;">'+
	'<table border="0" width="100%" cellpadding="0" cellspacing="0">'+
		'<tr valign="bottom">'+
			'<td width="82%" align="right"><div  id="freshIco" style="background:#fff; height:20px; width:65px;cursor:pointer; z-index:999999;" ><a onClick="getApInform()"><img src="images/freshico.png"/></a></div></td>'+
			'<td align="right">'+
			
				'<select id="refreshInterval" onChange="refreshAp();">'+
				'	<option value="0">手动</option>'+
				'	<option value="5">5秒一次</option>'+
				'	<option value="10">10秒一次</option>'+
				'	<option value="30">30秒一次</option>'+
				'</select>'+
			
			'</td>'+
		'</tr>'+
	'</table>	'+
	'	</div>'+
	'</div>';
return str;	
}
function getApInform(){
	//alert("test..");
	$.ajax({
				cache : false,
				async:false,
				url : "/goform/formStatForTopDis",
				type : "post",
				dataType : "json",
				success : function(data) {	
				 // var jsonData = eval("("+data+")");
				//alert("test...success"+data.onlineap);
					$("#online").html(data.onlineap);
					$("#offline").html(data.offlineap);
					$("#onlineuser").html(data.onlineuser);
				}
		}); 
		//alert("refresh");
}
var apfresh ;
function refreshAp(){
	var interval = $("#refreshInterval").val();
	//alert("interval:"+interval);
	
	if(interval!=0){
		clearInterval(apfresh);
		apfresh = setInterval("getApInform()",interval*1000);
		///alert("interval!=0");
	}else{
		//alert("interval==0");
		clearInterval(apfresh);

	}

}
function initization(){
	//alert("test");
	var str = showAPnum();
	$("#showapnum").html(str);
    var oAPImg = document.getElementById("APImg");
	oAPImg.onclick = function(){
		parent.main.location = "apManage.asp";
		}
	var oShowInfo = document.getElementById("showInfo");
	//alert("oShowInfo:"+oShowInfo);
	oShowInfo.onclick = function(){
		//alert("123");
		parent.main.location = "wlanCltList.asp";
		}

	var oFreshIco = document.getElementById("freshIco");
	var aImgFresh = oFreshIco.getElementsByTagName("img");
	oFreshIco.onmousedown = function(){
		aImgFresh[0].style.opacity = 0.5;
		aImgFresh[0].style.filter = "alpha(opacity = "+50+")";
		
		}
	oFreshIco.onmouseup = function(){
		aImgFresh[0].style.opacity = 1;
		aImgFresh[0].style.filter = "alpha(opacity = "+100+")";
		
		}
	$("#refreshInterval").val(refresh);
	apfresh = setInterval("getApInform()",refresh*1000);
	
}
</script>
<style type="text/css"> 
A:link { text-decoration: none; color: #ffffff;  font-weight:bold;}
A:active { text-decoration: underline; color: #ffffff;font-weight:bold}
A:visited { text-decoration: none; color: #ffffff; font-weight:bold}
A:hover { text-decoration: underline; color: #ffffff; font-weight:bold;}
</style>
<body bgcolor="#ffffff"> 
 
<table border="0" width="100%" cellpadding="0" cellspacing="0">
  <tr height="85">
	<td width="15" align="left" valign="top"><img src="<% write(com_imgtopl); %>" width="15" height="85"></td>
    <TD height="85" vAlign=center background="<% write(com_imgtopbg); %>" style="background-repeat:repeat-x; "><a href="<% write(LinkLogo); %>" target="_blank"><img id="logoImg" src="images/logo.png" width="206" height="45" border="0"></a></TD>
     <td id="assTd1"  width="9%" background="<% write(com_imgtopbg); %>" style="background-repeat:repeat-x;"></td>
	 <TD   width="50%" background="<% write(com_imgtopbg); %>"   style="background-repeat:repeat-x; display:" align="center" vAlign="top">
		<table width="95%" height="85" style="overflow:hidden;" border="0" bordercolor="red" cellpadding="0" cellspacing="0">
		<tr>
     <td style="background-repeat:repeat-x;position:relative;z-index:10;"    valign="top" align="center">
	 <div id="showapnum" style="margin:0; position:relative;z-index:90;"></div>

<div id="portImageArea" style="margin:2px; position: relative; left:1px">
<img src="<% write(com_imgtopbg); %>" id="switch_1" border="0"  style="background-repeat:;opacity:0;filter:alpha(opacity=0)">
     
 <script langugae="javascript"> 
writeAllPorts();
</script>
</div>
</td>
</tr>
</table>
    </TD>
	 <TD  width="30%"height=85 vAlign=bottom  align="right"  background="<% write(com_imgtopbg); %>" style="background-repeat:repeat-x">
	 <table width="100%" border="0" bordercolor="blue" cellspacing="0" cellpadding="0"><tr><td>&nbsp;</td>
	 
	 <td width="200"><table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr>
    
    <td  height="20" align="left" nowrap id="modeltd"></td>
  </tr>
  <tr id="hwmodeltr" syle="display:none">
    <td height="20" align="left" nowrap id="hwmodeltd"></td>
  </tr>
  <tr>
    <td height="20" align="left" nowrap id="revisiontd"></td>
  </tr>
</table></td></tr></table>

<table width="100%" border="0" cellspacing="0" cellpadding="0" align="right"   style="padding-top:5px;" >
  <tr bgcolor="#008893" >
	<td width="60" height="22" align="center" valign="middle"><a title="<% write(TopUttarea); %>" target="_blank" href="<% write(LinkProduct); %>"  id="taolun"><font color=""><% write(ProductDiscu); %></font></a></td>
	<td width="3"><img src="<% write(com_imgline); %>" width="1" height="16"></td>
	<td width="60" height="22" align="center" valign="middle"><a href="<% write(LinkKnowLedgeTopFo); %>"  target="_blank"  title="<% write(TopKnowledgeBase); %>" id="zhishiku"><font color=""><% write(TopKnowledge); %></font></a></td>
	<td width="3"><img src="<% write(com_imgline); %>" width="1" height="16"></td>
	<td width="60" align="center" valign="middle" height="22"><a href="<% write(LinkReservationTopFe); %>"  target="_blank" title="<% write(ReservationMsg); %>" id="yuyue"><font color=""><% write(Reservation); %></font></a></td>
  </tr>
</table>
<table><tr><td height="25">&nbsp; </td></tr></table>
</TD>
<td width="15" align="right" valign="top"><img src="<% write(com_imgtopr); %>" width="15" height="85"></td>
  </tr>
</table>
<script language="javascript">modeltd.innerHTML="<font style=font-size:12px><% write(Model); %>"+ProductID+"</font>";
/* 硬件版本 */
if (hardwareID != "") {
	hwmodeltr.style.display = "block";
    hwmodeltd.innerHTML="<font style=font-size:12px><% write(HardwareVer); %>"+hardwareID+"</font>";
}
revisiontd.innerHTML="<font style=font-size:12px><% write(SoftwareVer); %>"+sysVersion+"</font>";
init();


	


</script>
</body>
</html>

