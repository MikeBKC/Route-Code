<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<%langTran("common");%>
<%langTran("SysInfo");%>
<title><%write(SysInfo);%></title>
<link href="css/style_new.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="script/jquery-1.8.1.min.js"></script> 
<script type="text/javascript" src="js/common.js"></script> 
</head>
<script type="text/javascript">
/*输出现在的系统时间*/
<% aspoutDynConfig();%>
<% outputSystemNowTime();%>
/*输出路由器运行时间*/
<% outputSystemUpTime();%>
/*输出系统软件版本*/
<% outputSystemVersion();%>
/*输出产品型号和硬件版本*/
<% outputProductID();%>	
/*输出序列号*/
<% outputSystemSn();%>
/*输出CPU使用信息*/
<% outputSystemCpu();%>
/*输出内存使用信息*/
<% outputSystemMemory();%>
/*输出USB外存使用信息*/
<%outputFlashStat();%>
//进行时间处理
sysDate=new Date(UTCtoLocal(eval(sysDateTimes),0) * 1000);
Year= sysDate.getFullYear();
Month= sysDate.getMonth();
Day= sysDate.getDate();
if (10 > sysDate.getHours()) {
    Hour="0"+sysDate.getHours();
} else {
    Hour=""+sysDate.getHours();
}
if (10 > sysDate.getMinutes()) {
    Minute="0"+sysDate.getMinutes();
} else {
    Minute=""+sysDate.getMinutes();
}
if (10 > sysDate.getSeconds()) {
    Second="0"+sysDate.getSeconds();
} else {
    Second=""+sysDate.getSeconds();
}
Month1=Month+1;//路由器内部0表示1月，1表示2月……
var systimes=Year.toString()+"-"+Month1.toString()+"-"+Day.toString()+"   "+Hour.toString()+":"+Minute.toString()+":"+Second.toString();


/*处理外存使用率*/
var outerFlash=null;
var spaceStat=0;
var outerFlashUsedSpace=0;
var http_request = false;
function init_globalData()
{
    outerFlashUnusedSpace=convertToByte(outerFlashUnusedSpace);
    outerFlashTotalSpace=convertToByte(outerFlashTotalSpace);
    outerFlashUsedSpace=outerFlashTotalSpace-outerFlashUnusedSpace;
    if(outerFlashTotalSpace)
    {
	if(!(parseInt(outerFlashTotalSpace)))/*总空间大小为0*/
	{
		outerFlashTotalSpace =false;
	}
    }
    if(outerFlashTotalSpace)
    {
    /*使用率*/
	spaceStat= Math.round(outerFlashUsedSpace/outerFlashTotalSpace*100);
    } else {
	$("#trUsbSpace").hide();
    }
}
$(document).ready(function(){
    //var systime="2013-10-12 12:34:12";		
    $("#sys_time").html(systimes);
    
    //var runtime="X天X时X分X秒";

    $("#run_time").html(runtimes);
    
    //var serialnum="XXXXXX";
    if(ctrlserialnum == 0)
    {
        $("#serialnum").html(productIDs);
    }
    
    //var mode="XXXXXX";
    var ProductID = "MSG WR201"
    $("#mode").html(ProductID);
    
    //var software="XXXXXX";
    var revisions="MSG WR201-W.00-M-v1.8.13-171228";
    $("#software").html(revisions);
    
    //var hardware="XXXXXX";
    $("#hardware").html(hardwareID);
    
    init_globalData();
    if (cpuStat < 0) {
	cpuStat = 0;
    }
    if (memStat <0) {
	memStat = 0;
    }
    //var spaceStat=50;
    var flag1 = false;
    var flag2 = false;
    var flag3 = false;

    if (cpuStat == 0) {
	cpu1.bgColor = "#FFFFFF";
	flag1 = true;
    }

    if (memStat == 0) {
	mem.bgColor = "#FFFFFF";
	flag2 = true;
    }
    
    if (spaceStat == 0) {
	space.bgColor = "#FFFFFF";
	flag3 = true;
    }
    if (!flag1) {
	cpu1.width = cpuStat + "%";
    }

    if (!flag2) {
	mem.width = memStat + "%";
    }
    if (!flag3) {
	space.width = spaceStat + "%";
    }

    chgColor();
    $("#cpuval").html(cpuStat+"%");
    $("#memval").html(memStat+"%");
    $("#spaceval").html(spaceStat+"%");
    function chgTableColor(td) {
	if (td == null) return;
	var color = "";
	var width = eval((td.width.substring(0, td.width.length - 1)));

	if (width >= 70) {
	    td.bgColor = "#19a5e3";
	}
	else if (width >= 50 && width < 70) {
	    td.bgColor = "#19a5e3";
	}
	else if (width >= 0 && width < 50) {
	    td.bgColor = "#19a5e3";
	}
    }
 
    function chgColor() {
	var natSess = 0;

	if (cpuStat != 0) {
		chgTableColor(cpu1);
	}
	if (memStat != 0) {
		chgTableColor(mem);
	}
	if (spaceStat != 0) {
		chgTableColor(space);
	}
    }


});
</script>
<body>
<div class="warp">

<br>
	<table border="0" cellpadding="0" cellspacing="1" width="500px" id="runstatusInfo" class="group" style="background:#5e5e5e;" >
    	<tr class="tab_info">
        	<td width="150px"><%write(systemnow);%></td>
            <td colspan="2" id="sys_time"></td>
        </tr>
        <tr class="tab_info">
        	<td><%write(sys_runTime);%></td>
            <td colspan="2" id="run_time"></td>
        </tr>
         <tr class="tab_title">
        	<td colspan="3"></td>
           
        </tr>
        <script>
        if(ctrlserialnum == 0)
            {   
                document.write('<tr class="tab_info">');
                document.write('<td><%write(sys_serialnum);%></td>');
                document.write(' <td colspan="2" id="serialnum"></td>');
                document.write('</tr>');
            }
         </script>

        <tr class="tab_info">
        	<td><%write(ProMod);%></td>
            <td colspan="2" id="mode"></td>
        </tr>
        <tr class="tab_info">
        	<td><%write(SofVer);%></td>
            <td colspan="2" id="software"></td>
        </tr>
        <tr class="tab_info">
        	<td><%write(HardVer);%></td>
            <td colspan="2" id="hardware"></td>
        </tr>
        <tr class="tab_title">
        	<td colspan="3"></td>
           
        </tr>
        <!-- style="background:#19a5e3;" -->
         <tr style=" background:#fff; font-size:14px; color: #383838;">
        	<td height="30px;" style="padding-left:10px; text-align:left" >CPU               <%write(sys_usedvolume);%></td>
            <td colspan="2" style="padding-left:10px; text-align:left"><table class="style24" width="135" height="10px"align="left" >
						<tr><td id="cpu1" height="15" width="1%" ></td><td ></td></tr>
							</table><span style="padding-left:8px;" id="cpuval"></span></td>
   
        </tr>
        <tr style=" background:#fff; font-size:14px; color: #383838;">
        	<td height="30px;" style="padding-left:10px; text-align:left"><%write(sys_ramusage);%></td>
            <td colspan="2" style="padding-left:10px; text-align:left"><table class="style24" width="135" align="left" height="10px" >
	<tr><td id="mem" height="15" width="1%"></td><td ></td></tr>
	</table><span style="padding-left:8px;" id="memval"></span></td>
   
        </tr>
        <tr style=" background:#fff; font-size:14px; color: #383838;" id="trUsbSpace">
        	<td height="30px;" style="padding-left:10px; text-align:left"><%write(sys_memusage);%></td>
            <td colspan="2" style="padding-left:10px; text-align:left"><table class="style24" width="135" align="left" height="10px">
	<tr><td id="space" height="15" width="1%"></td><td ></td></tr>
	</table><span style="padding-left:8px;" id="spaceval"></span></td>
    	
        </tr>
    </table>
    <table border="0" cellpadding="0" cellspacing="1" style="background:#5e5e5e;" class="group" >
	<tr><td  class="tab_savetd" align="right"><span><input type="button" class="btn" onClick="location.reload();"  value="<%write(com_fresh);%>"/></span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td></tr>
    </table>
<br>
<br>
</div>
</body>
</html>
