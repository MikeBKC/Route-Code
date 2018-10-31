
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=GBK"/>


<title>tomax</title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" src="js/jquery.min.js"></script>
<script type="text/javascript" src="js/highcharts.js"></script>
<script type="text/javascript" ContentType="text/xml">

//----------------------------交互开始-------------------------
<%aspOutGetHistoryFLowInfo();%>
//----------------------------交互结束-------------------------
var data1Len=0;
var data2Len=0;
/*
for (var i = 0;i< data1.length;i++){
	if(data1[i]==0)
	data1Len++;
	}
	if(data1Len == data1.length){
		for(var i=0;i<data1.length;i++){
			data1[i]=null;
			}
	}
	
for (var i = 0;i< data2.length;i++){
if(data2[i]==0)
data2Len++;
}
if(data2Len == data2.length){
	for(var i=0;i<data2.length;i++){
		data2[i]=null;
		}
}*/
/*aps[0] = "ap11";
aps[1] = "ap22";
aps[2] = "ap33";
aps[3] = "ap44";*/
var lang="zhcn";
//var apMac = new Array();
if(currenttime == "最近1小时"){
}else{    
    var myDate = new Date();
	//alert("myDate:"+myDate);
    var myHour = myDate.getHours();
    //alert("myhour:"+myHour);
	var myYear = myDate.getFullYear()
	//alert("myYear:"+myYear);
	var myMonth = myDate.getMonth()+1;
	//alert("myMonth:"+myMonth);
	var myDay = myDate.getDate();
	//alert("myDay:"+myDay);
	var aCurrenttime = currenttime.split("-");
	aCurrenttime[0] = parseInt(aCurrenttime[0]);
	aCurrenttime[1] = parseInt(aCurrenttime[1]);
	aCurrenttime[2] = parseInt(aCurrenttime[2]);
	//alert("aCurrenttime:"+aCurrenttime[0]+": "+aCurrenttime[1]+": "+aCurrenttime[2])
	if((myYear == aCurrenttime[0])&&(myMonth == aCurrenttime[1])&&(myDay ==aCurrenttime[2])){
		//alert("same")
     myHour = myHour + 1;
	   for(i=myHour;i<24;i++){
		    data1[i]=null;
	       	    data2[i]=null;
	   }
	}
  }


function init(){
	var str="";
	for(var i=0;i< time.length;i++){
		if(time[i]==currenttime){
			str +="<option selected value='"+time[i]+"'>"+time[i]+"</option>";
		}else{
			str +="<option value='"+time[i]+"'>"+time[i]+"</option>";
		}
		
	}
	$("#statsticsTime").html(str);
	$("#systime").html(currentsystime);
	var apstr = "<option selected value=''>请选择</option>";
	$("#apNames1").html(apstr);
	for(var i = 0;i<aps.length;i++){
		if(aps[i]==currAP1){
		apstr +="<option selected value='"+aps[i]+"'>"+aps[i]+"</option>";
		}else{
		apstr +="<option value='"+aps[i]+"'>"+aps[i]+"</option>";
		}		
	}
	$("#apNames1").html(apstr);
	var apstr1 = "<option selected value=''>请选择</option>";
	$("#apNames2").html(apstr1);
	for(var i = 0;i<aps.length;i++){
		if(aps[i]==currAP2){
		apstr1 +="<option selected value='"+aps[i]+"'>"+aps[i]+"</option>";
		}else{
		apstr1 +="<option value='"+aps[i]+"'>"+aps[i]+"</option>";
		}		
	}
	$("#apNames2").html(apstr1);

}


var CHART; 
$(function() { 
init();
//alert("(new Date()).getTime():"+(new Date()).getTime())
Highcharts.setOptions({ 
    global: { 
        useUTC: false 
    } 

})
    CHART = new Highcharts.Chart({ 
        chart: { 
            renderTo: 'container', //图表放置的容器，DIV 
            defaultSeriesType: 'spline'//, //图表类型为曲线图 
        }, 
        title: { 
            text: ''  //图表标题 
        }, 
        xAxis: { //设置X轴 
            type: 'datetime',  //X轴为日期时间类型 
            tickPixelInterval: 80, //X轴标签间隔 
			title:{
		    text: '',
		    layout: 'vertical',
		    align: 'high'
	    }
        }, 
        yAxis: { //设置Y轴 
            title : {
					useHTML : true,
					align: 'high',
					text : "流量（单位"+unit+"）"
			},
            max: maxNum, //Y轴最大值 
            min: 0  //Y轴最小值 
        }, 
        tooltip: {//当鼠标悬置数据点时的提示框 
            formatter: function() { //格式化提示信息 
                return '时间：'+ 
                Highcharts.dateFormat('%H:%M:%S', this.x) +'；流量： '+  
                Highcharts.numberFormat(this.y, 2)+unit; 
            },
			shared : false
        }, 
        legend: { 
            enabled: true  //设置图例不可见 
        }, 
        exporting: { 
            enabled: false  //设置导出按钮不可用 
        }, 
        series: [{ 
			color : '#89A54E',
			tooltip : {
				valuePrefix :currAP1
			},
			name: currAP1,
            data: disData(data1)
        },
		{ 
			yAxis : 0,
			color : '#4572A7',
			tooltip : {
				valuePrefix :currAP2
			},
			name: currAP2,
            data: disData(data2)
        }] 
    }); 
	if(currenttime == "最近1小时"){
		CHART.xAxis[0].setTitle( { text:'时间（分钟）'}, true);
	}else{
		CHART.xAxis[0].setTitle( { text:'时间（小时）'}, true);
	}
}); 
var disData = function(apdata){
 var data = [],i,str="",d="",time=0,j=0;
				if(currenttime == "最近1小时"){
						
					  d   =   new   Date(Date.parse(currentsystime.replace(/-/g,   "/")));
					  time = d.getTime();
					   for (i =-59; i <= 0; i++) { 
					   			
							data.push({ 
								x: time + i * 60 * 1000,  
								y: apdata[j] 
							});
							j++; 
						} 
				
				}else{
					str = currenttime+" 00:00:00";
						var   d   =   new   Date(Date.parse(str.replace(/-/g,   "/")));
						var time = d.getTime();
						for (i = 0; i <= 23; i++) { 
							data.push({ 
								x: time + i * 3600 * 1000,  
								y: apdata[i]
							}); 
						} 
				}
			

                return data; 
}

function changeText(){
    var selectIndex1 = document.getElementById("apNames1").selectedIndex;
    var selectIndex2 = document.getElementById("apNames2").selectedIndex; 
	var selectTimeIndex = document.getElementById("statsticsTime").selectedIndex;
	if(selectIndex1==0)
		document.getElementById("mac1").value="";
	else
    	document.getElementById("mac1").value = apMac[selectIndex1-1];
		
	if(selectIndex2==0)
		document.getElementById("mac2").value="";
	else
    	document.getElementById("mac2").value = apMac[selectIndex2-1];
		
	document.getElementById("statsticsTime").value =time[selectTimeIndex];
	
	historyFlowStatistics.action="";
	historyFlowStatistics.submit();	
}

function exportData(){
	historyFlowStatistics.exportstr.value = "onlineUsers.tar";
	historyFlowStatistics.action = "/goform/formExportCltFlowInfo";
	historyFlowStatistics.submit();
	}

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
  <td><script language="javascript">showSecondTitle("无线统计");</script>
  <table width="100%" border="0"  cellspacing="0" cellpadding="0" align="center"  class="table_bg">
  <!-- <script language="javascript">var errorstr=""; showerror(errorstr,80);</script>-->
  <tr valign="top">
    <td>
    <p>&nbsp;</p>    
          <script language="javascript">
                 var arrayThreeMenu=new Array(
	                                                     "80%",
	                                                     6,
	                                                     
														 new Array("AP用户统计",'WlanApClientsStatistics.asp',1,0),
														 new Array("SSID用户统计",'WlanSSIDClientsStatistics.asp',1,0),
														 new Array("历史用户统计",'WlanHistoryClientsStatistics.asp',1,0),
														 new Array("历史流量统计",'historyFlowStatistics.asp',1,1),
														 new Array("用户列表",'wlanCltList.asp',1,0),
														 new Array("黑名单",'wlanCltMacFilter.asp',1,0)
														
	                     );
                     showThreeMenu();
             </script>
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center"><!--白色部分-->
<!--<p>&nbsp;</p>-->
<tr>
    <td align="center"  class="three_td">


       <table width="100%" border="0" bordercolor="red" cellspacing="0" cellpadding="0"><!--白色部分里面分隔-->



  <tr>
    <td>
        <form name="historyFlowStatistics" method="post" action="">
            <table style="display:"  border="0" bordercolor="green" align="center" cellpadding="0" cellspacing="0"><!--表单1里面的表格-->

         <input type="hidden" name="mac1" value="" id="mac1"/><input type="hidden" name="mac2" value="" id="mac2"/>
    	<tr><td colspan="4" height="20">&nbsp;</td></tr>   
		<tr>
		      <td colspan="4">
			  <table width="100%" height="20" border="0" bordercolor="blue" cellpadding="0" cellspacing="0"><!--1-->
				 	<tr>
						<td>
							<table width="100%" height="20" border="0" cellspacing="0" cellpadding="0" bgcolor=""><!--2-->
								<tr>
									<td width="300" background="images/tablehead.gif" style="background-repeat:no-repeat"><span class="style4">&nbsp;流量统计</span></td>
			                        <td   width="300"align="right"  class="style5" ></td><td width="50" align="right"><span id="strRecsGroup"></span></td>
			                     </tr>
							</table><!--2-->
						</td>
					</tr>
					<tr class="style18">
					    <td height="24" colspan="4"  ><table width="650"  border="0" bordercolor="yellow" cellspacing="0" cellpadding="0"><!--3-->
							<tr>
							   <td width="10"><img src="images/space.gif" width="10" height="8"></td>
							   <td class="style6" width="70">统计时间</td>
							   <td>
								   <select id="statsticsTime" name="statsticsTime"   onChange="changeText()">
								  
								   </select>
							   </td>
							   <td align="right" width="130">比较AP流量</td>
							   <td>
									<select id="apNames1" name="apNames1"  onChange="changeText()">
								 
								   </select>
							   </td>
							   <td align="left">
								   <select id="apNames2" name="apNames2" onChange="changeText()">
								   
								   </select>
							   </td>
	<td align="left">
							   <td align="right" width="180">当前时间&nbsp;&nbsp;<span id="systime"><span></td>
							   
							  
							</tr>
							<tr>
								<td colspan="8">
								<div id="container" style="min-width: 500px; height: 400px; margin: 0 auto"></div> 
								</td>
							</tr>
                            <tr valign="top"><td colspan="8" align="right" style="background-color:white;" height="20" valign="top"><input type="button" class="input-button" onClick="exportData();" value="导出数据" id="mange_div2" name="mange_div2"><input type="hidden" id="exportstr" name="export"> <input type="button" class="input-button" id="freshbt" value="刷新" onClick="changeText()"></td></tr>
						</table><!--3-->
								
								
				 </table><!--1-->
			
			</td>
		 </tr>
		
                        
		  </table><!--表单1里面表格结束-->
        </form>
  </td>
 </tr>

  
  </table> <!--白色部分里面分隔结束-->
</td></tr><!--<script language="javascript">init();</script>-->
</table><!--白色部分结束-->
</td></tr></table><!--倒数第二层表格结束-->
</td>
<!--////end-->
  <td width="15"></td>
  </tr>
</table><!--最外层表格结束-->
</body>
</html>
