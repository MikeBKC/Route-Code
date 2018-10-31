<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=GBK"/>
<% langTran("MacFilter"); %>
<% langTran("common"); %>
<title><% write(com_utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" src="js/jquery.min.js"></script>
<script type="text/javascript" src="js/highcharts.js"></script>
<script type="text/javascript" ContentType="text/xml">

var lang="zhcn";
//--------------------------------交互开始-------------------------------------
//var totalrecs=22;var max_totalrecs=64;

/*
var ApClients1=new Array(new Array("A1",10),new Array("B1",10),new Array("C1",10),new Array("D1",10),new Array("E1",10),new Array("F1",10),new Array("G1",10),new Array("H1",10),new Array("I1",10),new Array("J1",10),new Array("K1",10),new Array("A5",10),new Array("B5",10),new Array("C5",10),new Array("D5",10),new Array("E5",10),new Array("F5",10),new Array("G5",10),new Array("H5",10),new Array("I5",10),new Array("J5",10),new Array("K5",10));
*/

/*var SSIDClients= {"guset":20,"staff":30,"SSID1":40,"SSID2":50,"SSID3":60};*/



/*
var dateGroup= new Array();
dateGroup[0]= "2014-04-12";
dateGroup[1]= "2014-04-13";
dateGroup[2]= "2014-04-14";
dateGroup[3]= "2014-04-15";
dateGroup[4]= "2014-04-16";
dateGroup[5]= "2014-04-17";
dateGroup[6]= "2014-04-18";


var HistoryClients= new Array();
 for(i=0;i<168;i++)
	    HistoryClients[i]=i;
*/
		


<%
aspOutGetSsidInfo();
%>


//------------------------------交互结束---------------------------------

//recsPerPage=10;
//maxrecs=max_totalrecs;
//currpage=1;
//recscount=totalrecs;






function init(){
	
	
	
//	drawHead();
//	showApClients();
	showSSIDClients();
//	document.getElementById("showNowTime").innerHTML =CurentTime();
//	document.getElementById("showNowTime").innerHTML = SystemTime;
//	showHistoryClients();
//	selectInit();
 }





function dynamicShowGroup(ctrl)
{
	recsPerPage=parseInt(ctrl.value);
	currpage=1;
	drawHead();
	showApClients();
	drawHistogram(options1,Ap,apClients);
}


function drawHistogram(Chart,Aparray,apClientsarray)
{
     Chart.xAxis[0].setCategories(Aparray);
     Chart.series[0].setData(apClientsarray);
}

var pages;
/*function drawHead()
{
	if (recscount == 0) strPages = "0/0";
  		else {
      			pages = Math.floor((recscount - 1) / recsPerPage) + 1; 
       			strPages = currpage.toString() + "/" + pages.toString();
   			}
    strRecs = recscount.toString() + "/" + maxrecs.toString();
	document.getElementById("strGroup").innerHTML = strPages;
    document.getElementById("strRecsGroup").innerHTML = strRecs;
}*/

function firstgroup(){
		if((pages>1)&&(currpage>1)){
			for(i=0;i<recsPerPage;i++){
				Ap[i]=ApClients[i][0];
				apClients[i]=ApClients[i][1];
				}
				
		currpage=1;
		drawHead();
		drawHistogram(options1,Ap,apClients);
		}
		
}

function lastgroup(){
		var k=0;
		if ((pages>1)&&(currpage<pages)){
			for(i=recsPerPage*(pages-1);i<recscount;i++){
					Ap[k]=ApClients[i][0];
					apClients[k]=ApClients[i][1];
					k++;
			}
			for(i=recscount;i<recsPerPage*pages;i++){
					Ap[k]="";
					apClients[k]=null;
					k++;
			}
			
		currpage=pages;
		drawHead();
		drawHistogram(options1,Ap,apClients);
		}
}	



function pregroup(){
		var k=0;
		if((pages>1)&&(currpage>1)){
			for(i=recsPerPage*(currpage-2);i<recsPerPage*(currpage-1);i++){
				Ap[k]=ApClients[i][0];
				apClients[k]=ApClients[i][1];
				k++;
			}
			
		currpage=currpage-1;
		drawHead();
		drawHistogram(options1,Ap,apClients);
		}
        
 }



function nextgroup(){
		var k=0;
		if((pages-currpage)>1){
			for(i=recsPerPage*currpage;i<recsPerPage*(currpage+1);i++){
					Ap[k]=ApClients[i][0];
					apClients[k]=ApClients[i][1];
					k++;
			}
		currpage=currpage+1;
		drawHead();
		drawHistogram(options1,Ap,apClients);
		}
		else if((pages-currpage)==1){
			for(i=recsPerPage*currpage;i<recscount;i++){
					Ap[k]=ApClients[i][0];
					apClients[k]=ApClients[i][1];
					k++;
			}
			for(i=recscount;i<(recsPerPage*(currpage+1));i++){
					Ap[k]="";
					apClients[k]=null;
					k++;
			}
		currpage=currpage+1;
		drawHead();
		drawHistogram(options1,Ap,apClients);
		}
		
		
		
}
					
		
function goanothergroup(){
     	var k=0;
		if(WlanApClientsStatistics.groupnumber.value=="")
		 alert("页面输入错误，只允许输入数字！");
	 	t=parseInt(WlanApClientsStatistics.groupnumber.value);	
		if(t==pages){
				for(i=recsPerPage*(t-1);i<recscount;i++){
					Ap[k]=ApClients[i][0];
					apClients[k]=ApClients[i][1];
					k++;
				}
				for(i=recscount;i<recsPerPage*t;i++){
					Ap[k]="";
					apClients[k]=null;
					k++;
				}
				currpage=t;
				drawHead();
				drawHistogram(options1,Ap,apClients);
		}
		else if(t<pages){
				for(i=recsPerPage*(t-1);i<recsPerPage*t;i++){
				Ap[k]=ApClients[i][0];
				apClients[k]=ApClients[i][1];
				k++;
			}
				currpage=t;
				drawHead();
				drawHistogram(options1,Ap,apClients);
		}
	
}

function gotogroup(e) {
        var code = e.keyCode || e.which;
     	if (code == 13)
				goanothergroup();
		else if (! ((code >= 48) && (code <= 57))) {
                inputless(e);
   		 } else ;
     	
 }
 
function inputless(e) {
   		 e = e || window.event;
    	try{/*ie*/
                e.keyCode=0;
                e.returnValue=false;
        }catch(ex){/*ff*/
         e.preventDefault();
    }
}

	
function searchDataEx(e){

	var code = e.keyCode || e.which;
    if (code != 13) { //当按下的不是回车，则返回
         return;
     }
	
	searchdata = WlanApClientsStatistics.txtSearch.value;
	if(searchdata==null){
		for(i=0;i<totalrecs;i++){
			ApClients[i][0]=ApClients1[i][0];
			ApClients[i][1]=ApClients1[i][1];
		}
		recscount=totalrecs;
		currpage=1;
		recsPerPage=10;
		drawHead();
		showApClients();
		return;
	}	
	
	var searchDatas= new Array(); 
	for(i=0;i<recscount;i++)
	searchDatas[i]=new Array();
	
	var k=0;
	
	for(i=0;i<totalrecs;i++){
		if(ApClients1[i][0].indexOf(searchdata)!= -1){
		    ApClients[k][0]=ApClients1[i][0];
			ApClients[k][1]=ApClients1[i][1];
			k++;
		}
	}
	currpage=1;
	recscount=k;
	drawHead();
		
    
	if(recscount>=recsPerPage){	
	 		 for(i=0;i<recsPerPage;i++){
	     				Ap[i]=ApClients[i][0];
		 				apClients[i]=ApClients[i][1];
			 }
	 }
	else{
		 	 for(i=0;i<recscount;i++){
			 	Ap[i]=ApClients[i][0];
				apClients[i]=ApClients[i][1];
				}
		 	 for(i=recscount;i<recsPerPage;i++){
				 Ap[i]="";
				 apClients[i]=null;
		 		}
		 }
	
     drawHistogram(options1,Ap,apClients);
}






var ApClients= new Array(); 

/*for(i=0;i<ApClients1.length;i++){
		ApClients[i]=new Array();
		ApClients[i][0]=ApClients1[i][0];
		ApClients[i][1]=ApClients1[i][1];
}
	*/

var Ap= new Array();
var apClients= new Array();

function showApClients(){
        Ap = [];
	apClients = [];
	 var k=0;
	 if(recscount>recsPerPage){	
	 		 for(i=0;i<recsPerPage;i++){
	     				Ap[k]=ApClients[i][0];
	 	 				//apClients[k]=parseInt(ApClients[i][1]);
		 				apClients[k]=ApClients[i][1];
						 k++;
		 	}
	}
	 else{
		 	 for(i=0;i<recscount;i++){
			 	Ap[k]=ApClients[i][0];
				apClients[k]=ApClients[i][1];
				k++;
				}
		 	 for(i=recscount;i<recsPerPage;i++){
				 Ap[k]="";
				 apClients[k]=null;
				 k++;
		 		}
                            
		 }
}



var SSID=new Array();
var ssidClients= new Array();

function showSSIDClients(){
	
	 var k=9;	
	 for(var i in SSIDClients) {
	     
		SSID[k]=i;
		ssidClients[k]=SSIDClients[i];
		k--;
	 }
	 while(k >= 0){
	     SSID[k]="";
	     ssidClients[k]=null;
	     k--;
	 }
}



function selectInit()
{
	var option, i;
	var f=HistoryClientsStatistics;
	for (i = f.statisticsDate.length; i > -1; i--)
	{
		f.statisticsDate.remove(i);
	}
	for (i = 0; i < dateGroup.length; i++)
	{
		option = document.createElement("OPTION");

		option.value = i;
		option.text = dateGroup[i];
		f.statisticsDate.options.add(option);
	}
}



var historyClients=new Array();
function showHistoryClients(){
    
	 var dateValue= getListValue(HistoryClientsStatistics.statisticsDate);
   		
        k= dateValue * 24;
	   for(j=0;j<24;j++){
	    historyClients[j]=HistoryClients[k];
	    k++;
	}
}




function changeDate(ct1){
    
    var dateValue= getListValue(ct1);
    k= dateValue * 24;
	for(j=0;j<24;j++){
	    historyClients[j]=HistoryClients[k];
	    k++;
	}
	options3.series[0].setData(historyClients);
}



$(function () {                                                                
    $('#container2').highcharts({                                           
        chart: {                                                           
            type: 'bar'                                                    
        },                                                                 
        title: {                                                           
            text: ' '             
        },                                                                 
                                                                  
        xAxis: {                                                   
            categories: SSID,
			labels: {
                 rotation : -60 ,
				 style : {
				 	fontSize : '11px'
				 }
            },
            title: {                                                       
                text: 'SSID',
		    align: 'high'
            }
			                                             
        },                                                                 
        yAxis: {                                                           
            min: 0,
			allowDecimals:false,                                                        
            title: {                                                       
                text: '用户数',                            
                align: 'high'                                              
            }                                                                                                                  
        },                                                                 
        tooltip: {                                                                        
	   		 headerFormat: '<span style="font-size:10px"></span><table>',
           	 pointFormat: '<tr><td <b>{point.y:.0f}</b></td></tr>',
           	 footerFormat: '</table>',
           	 shared: true,
           	 useHTML: true
                    
        },                                                                 
        plotOptions: {                                                     
                                                                     
        },                                                                 
        legend: {                                                          
            layout: 'vertical',                                            
            align: 'right',                                                
            verticalAlign: 'top',                                          
            x: -40,                                                        
            y: 100,                                                        
            floating: true,                                                
            borderWidth: 1,                                                
            backgroundColor: '#FFFFFF',                                    
            shadow: true,
	    enabled: false

        },                                                                 
        credits: {                                                         
            enabled: false                                                 
        },                                                                 
        series: [{                                                         
            name: '用户数',                                             
            data: ssidClients                                 
        }]                                                                 
    });                                                                    
});    

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
 <!--  <script language="javascript">var errorstr=""; showerror(errorstr,80);</script>-->
  <tr valign="top">
    <td>
    <p>&nbsp;</p>    
          <script language="javascript">
                 var arrayThreeMenu=new Array(
	                                                    "80%",
	                                                     6,
	                                                     
														 new Array("AP用户统计",'firstPage.asp',1,0),
														 new Array("SSID用户统计",'firstWlanSSIDClts.asp',1,1),
														 new Array("历史用户统计",'firstWlanHistoryClts.asp',1,0),
														 new Array("在线用户",'firstWlanCltList.asp',1,0),
														 new Array("AP日志",'firstApLog.asp',1,0),
														 new Array("系统信息",'firstSysInfo.asp',1,0)
														 
	                     );
                     showThreeMenu();
             </script>
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center"><!--白色部分-->
<!--<p>&nbsp;</p>-->
<tr>
    <td align="center"  class="three_td">


       <table width="100%" border="0"  cellspacing="0" cellpadding="0"><!--白色部分里面分隔-->


<tr>
  <td>
        <form name="WlanSSIDClientsStatistics" method="post" action="">
            <table id=grouplist style="display"  border="0" bordercolor="red" align="center" cellpadding="0" cellspacing="0"><!--表单2里面的表格-->
		<tr><td colspan="4" height="20">&nbsp;</td></tr>
		<tr>
		      <td colspan="4" width="600" height="20" background="../<% write(com_imgtablehead); %>" style="background-repeat:no-repeat"><img src="../<% write(com_imgspace); %>" width="10" height="8"><span align="center" valign="middle" class="style4">SSID用户统计</span></td>
	     </tr>
	     <tr>
		  <td colspan="4" width="600" height="20" bgcolor="#efefef"></td>
		</tr>
		<tr>
		      <td align="left" valign="center" colspan="3" height="350">
                       <div id="container2" style="width:550px;height:300px; float:left;"></div>
                        <div id="fresh" style="width:40px;height:300px; float:right; position:relative;">
                       	<input type="button" value="刷新" class="input-button" id="freshbt" onClick="location.reload()" style="position:absolute; top:265px;">
                       </div>

		      </td>
		</tr>
		        
		
		  </table><!--表单2里面表格结束-->
        </form>
  </td>
</tr> 
</table> <!--白色部分里面分隔结束-->
</td></tr><script language="javascript">init();</script>
</table><!--白色部分结束-->
</td></tr></table><!--倒数第二层表格结束-->
</td>
<!--////end-->
  <td width="15"></td>
  </tr>
</table><!--最外层表格结束-->
</body>
</html>
