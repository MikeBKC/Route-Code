<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv='content-type' content='text/html;charset=gb2312'>
<meta name='robots' content='noindex,nofollow'>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<link rel='stylesheet' type='text/css' href='css/uttwlan.css'>
<style type='text/css'> 
#txt {
	width: 475px;
	white-space: nowrap;
}
#bwm-controls {
	width:145px;
	text-align: right;
	margin-right: 5px;
	margin-top: 5px;
	float: right;
	visibility: hidden;
}
#tabs td a{ width:20px;px; padding-left:3px; padding-right:3px}
#tabs td.portTd a{ width:20px }
#tabs .newLine { margin-top:2px;}
</style>
<%langTran("common");%>
<%langTran("InterfaceFlow");%>
<%langTran("JS");%>
<title><%write(InterfaceFlow);%></title>
<script type='text/javascript' src='js/config.js'></script>
<script type='text/javascript' src='js/uttwlan.js'></script>
<script type='text/javascript' src='js/bwm-common.js'></script>
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" ContentType="text/xml"> 
nvram = {
        web_svg: '1',
        rstats_colors: '',
        http_id: 'TID65929cbc61a4d7d7'
        };
		
<%getSysLang();%>
var cprefix = 'bw_r';
var updateInt = 2;
var updateDiv = updateInt;
var updateMaxL = 300;
var updateReTotal = 1;
var prev = [];
var debugTime = 0;
var avgMode = 0;
var wdog = null;
var wdogWarn = null;
 
_tabCreate=function (tabs)
{
	var buf = [];
	var tdCl = '';
	buf.push('<table id="tabs" cellpadding="0" cellspacing="0" align="left"><tr><td><table cellpadding="0" cellspacing="0"><tr>');
	for (var i = 0; i < arguments.length; ++i)
	{
		/*当显示内容太长的时候。则新起一行显示lan口信息*/
		if(arguments[i][0].toUpperCase().indexOf('LAN')>=0 && arguments.length>18)
		{//新起一行
			buf.push('</tr></table>'//终结该行表格
					 +'</td></tr>'//终结表格tabs第一行
					 +'<tr><td><table cellpadding="0" cellspacing="0" class=newLine><tr>');//开启新的一行
		}
		if(isNum(arguments[i][0])){
			tdCl=' class=portTd';
		}
		else {
				tdCl='';
		}
		
		buf.push('<td '+tdCl+'><a href="javascript:tabSelect(\'' + arguments[i][0] + '\')" id="' + arguments[i][0] + '">' + arguments[i][1] + '</a></td>');
	}
	buf.push('</tr></table></td></tr></table><div id="tabs-bottom"></div>');
	return buf.join('');
}
 
var ref = new TomatoRefresh('/goform/formGraphShowForSvg', '', 2);
//交互数据：具体形式请参考
 
ref.stop = function() {
	this.timer.start(1000);
}
 /***
 ajax方式请注释掉uttwlan.js里面的this.http.onCompleted执行项
 **/
ref.refresh = function(text) {
	var c, i, h, n, j, k;
	watchdogReset();
 
	++updating;
	try {
		netdev = null;
		eval(text);
 
		n = (new Date()).getTime();
		if (this.timeExpect) {
			if (debugTime) E('dtime').innerHTML = (this.timeExpect - n) + ' ' + ((this.timeExpect + 2000) - n);
			this.timeExpect += 2000;
			this.refreshTime = MAX(this.timeExpect - n, 500);
		}
		else {
			this.timeExpect = n + 2000;
		}
 
		for (i in netdev) {
			c = netdev[i];
			if ((p = prev[i]) != null) {
				h = speed_history[i];
				h.rx.splice(0, 1);
			    h.rx.push((c.rx < p.rx) ? 0 : (c.rx - p.rx));
			    //h.rx.push(c.rx );
			    h.tx.splice(0, 1);
			    h.tx.push((c.tx < p.tx) ? 0 : (c.tx - p.tx));
			    //h.tx.push(c.tx );
		    }
		    else if (!speed_history[i]) {
			    speed_history[i] = {};
			    h = speed_history[i];
			    h.rx = [];
			    h.tx = [];
			    for (j = 300; j > 0; --j) {
				    h.rx.push(0);
				    h.tx.push(0);
			    }
			    h.count = 0;
		    }
    
		    
		    prev[i] = c;
	    }
	    loadData();
    }
    catch (ex) {
    }
    --updating;
}
function watchdog()
{
    watchdogReset();
    ref.stop();
    wdogWarn.style.display = '';
}

function watchdogReset()
{
    if (wdog) clearTimeout(wdog)

    wdog = setTimeout(watchdog, 10000);
    wdogWarn.style.display = 'none';
}

function init()
{
    speed_history = [];

    initCommon(2, 1, 1);

    wdogWarn = E('warnwd');
    watchdogReset();
    ref.start();
}

var oldTabSelect= tabSelect;
tabSelect = function()
{
    oldTabSelect.apply(this,arguments);/*运行之前的 tabSelect函数*/
    arrayThreeMenu[3][1]='switch/stat_detailed.asp?port='+	(cookie.get(cprefix + 'tab')||1);/*更改页面顶部的tab选项卡链接*/
    
}
</script>

</head>
<body onload='init()' onbeforeunload="logOut()" background="<%write(com_bodyMain);%>" style="background-repeat:repeat-x">
<table width="100%" border="0" cellspacing="0" cellpadding="0" >
<!--*****顶上间隔****-->
<tr>
<td colspan="3">&nbsp;</td>
</tr>

<!--****中间内容****-->
<tr valign="top">
<td width="15"></td>
<!--////内容-->
<td class="table_bg"><script language="javascript">showSecondTitle('<%write(InterfaceFlow);%>');//显示页面名称</script>
<table width="95%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  >
<script language="javascript">//showerror(errorstr,100);//显示保存返回的错误信息</script>
<tr valign="top">
<p>&nbsp;</p>
<td>
    <!--//三级菜单-->
   <!--  
   <script language="javascript">
	    var arrayCount = stat_detailed + InterfaceFlow;
	    var arrayThreeMenu=new Array(
		    "100%",
		    arrayCount,
		    new Array('<%write(InterfaceFlow);%>','InterfaceFlow.asp',1,1),
		    new Array('<%write(InterfaceDetail);%>','switch/stat_detailed.asp?port='+	(cookie.get(cprefix + 'tab')||1),stat_detailed,0)
	    );
	    showThreeMenu();
    </script>
    <!--//end-->
<table width="100%" border="0" cellspacing="0" cellpadding="0" align="center"><tr>
<td align="center"  class="style7"><table width="100%" border="0" cellspacing="0" cellpadding="0">
<tr  valign="top"><td><table border="0" width="100%" align='center'><tr><td class="gapTd"></td><td align="center">
    <!--//直接将页面的form填在此处，例如：<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
<form>
<br>
<table id='container' border="0">
<td id='content' align="center">
<div id='rstats' style="text-align:center;">
<table width="100%" border="0">
<tr>
    <td align="center">
<div><!--<span class="tabs-caption">端口</span>--><span id='tab-area'></span></div>
<div style='border-top:1px solid #f0f0f0;border-bottom:1px solid #f0f0f0;visibility:hidden;padding:0;margin:0' id='graph'><embed src='other/bwm-graph.svg' style='width:620px;height:200px;margin:0;padding:0' type='image/svg+xml' pluginspage='http://www.adobe.com/svg/viewer/install/'></embed></div>
</td>
</tr>
<tr> 
	    <td align="right" valign="middle"><%write(Interval);%></td>
</tr>
<tr><td height="1"></td></tr>
<tr>
    <td>
    <table width="100%" border="0">
<tr>
    <td width="60%" colspan="2" valign="top">
	    <table border=0 cellspacing=2 id='txt' width="100%">
    <tr>
	    <td width='7%' align='right' valign='top'><b style='border-bottom:blue 1px solid' id='rx-name'><%write(receive);%></b></td>
		    <td width='16%' align='right' valign='top'><span id='rx-current'></span></td>
	    <td width='10%' align='right' valign='top'><b><%write(average);%></b></td>
		    <td width='16%' align='right' valign='top' id='rx-avg'></td>
	    <td width='10%' align='right' valign='top'><b><%write(maximum);%></b></td>
		    <td width='16%' align='right' valign='top' id='rx-max'></td>
	    <td width='8%' align='right' valign='top'><b><%write(total);%></b></td>
		    <td width='16%' align='right' valign='top' id='rx-total'></td>
    </tr>
    <tr>
	    <td height="5"></td>
    </tr>
    <tr>
	    <td width='7%' align='right' valign='top'><b style='border-bottom:blue 1px solid' id='tx-name'><%write(send);%></b></td>
		    <td width='16%' align='right' valign='top'><span id='tx-current'></span></td>
	    <td width='10%' align='right' valign='top'><b><%write(average);%></b></td>
		    <td width='16%' align='right' valign='top' id='tx-avg'></td>
	    <td width='10%' align='right' valign='top'><b><%write(maximum);%></b></td>
		    <td width='16%' align='right' valign='top' id='tx-max'></td>
	    <td width='8%' align='right' valign='top'><b><%write(total);%></b></td>
		    <td width='16%' align='right' valign='top' id='tx-total'></td>
    </tr>
    </table>	</td>
    <td  width="40%" valign="top">	<div id='bwm-controls' style=" margin-top:0px; padding-top:0px">
	    <%write(timeShaft);%>&nbsp;
		    <a href='javascript:switchAvg(1)' id='avg1'>1x</a>,
		    <a href='javascript:switchAvg(2)' id='avg2'>2x</a>,
		    <a href='javascript:switchAvg(4)' id='avg4'>4x</a>,
		    <a href='javascript:switchAvg(6)' id='avg6'>6x</a>,
		    <!--<a href='javascript:switchAvg(8)' id='avg8'>8x</a>--><br>
	    <%write(flowShaft);%>&nbsp;
		    <a href='javascript:switchScale(0)' id='scale0'><%write(standard);%></a>,
		    <a href='javascript:switchScale(1)' id='scale1'><%write(maximize);%></a><br>
	    <%write(show);%>&nbsp;
		    <a href='javascript:switchDraw(0)' id='draw0'><%write(solid);%></a>,
		    <a href='javascript:switchDraw(1)' id='draw1'><%write(hollow);%></a><br>
	    <%write(color);%>&nbsp; <a href='javascript:switchColor()' id='drawcolor'>-</a><br>
	    <a href='javascript:switchColor(1)' id='drawrev'>[<%write(turn);%>]</a>

    
    </div></td>
</tr>
</table>	</td>
</tr>
    <tr><td align="left" valign="middle"> <a href="http://download.adobe.com/pub/adobe/magic/svgviewer/win/3.x/3.03/en/SVGView.exe" target="_blank"><%write(showTip);%></a></td>
        <!--  <td align="right" valign="middle"><a onClick="seeFlow()" target="main" style="cursor :hand; color:#0000FF; text-decoration: underline">查看端口详情</a></td>-->
    	</tr>
</table>
</div>
 
<!-- / / / -->
 
</td></tr>
<tr><td id='footer' colspan=1>
	<span id='warnwd' style='display:none'><%write(attention);%>&nbsp;</span>
	<span id='dtime'></span>
	<img height="10" src='' id='refresh-spinner'  style="display:none" onclick='javascript:debugTime=1'>
</td></tr>
</table>
</form>
<!--//直接将页面的form填在此处，例如：<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
</td><td class="gapTd"></td></tr>
  </table>
 </td>
  </tr>
</table> 
  </td></tr>
</table></td></tr></table></td>
<!--////end-->
  <td width="15">&nbsp;</td>
  </tr>
</table>
</body>
</html>
