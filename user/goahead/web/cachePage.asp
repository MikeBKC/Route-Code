<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<%langTran("common");%>
<%langTran("cache");%>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" src="js/cloudFlowBar.js" ></script>
<script type="text/javascript" src="js/dirUtilizationPie.js" ></script>
<title>Cloud</title>
<script language="javascript">

<%getCacheStats();%>

var dirUtilizationPieColors = new Array("none","#e55636", "#19aa5f", "#a8c656");
var dirUtilizationPie = new pieInit(dirUtilizationPieValues,dirUtilizationPieColors, "#677bde");

var cloudFlowBarColors = new Array("#4dbbf6","#e55636", "#19aa5f");
var cloudFlowBar = new BarInit(cloudFlowBarValues,cloudFlowBarColors);


var videoArray = new Array(['video','youkuVideo','<%write(youkuVideo);%>'],
			   ['video','tudouVideo','<%write(tudouVideo);%>'],
			   ['video','tencentVideo','<%write(tencentVideo);%>'],
			   ['video','sinaVideo','<%write(sinaVideo);%>'],
			   ['video','ifengVideo','<%write(ifengVideo);%>'],
			   ['video','letvVideo','<%write(letvVideo);%>'],
			   ['video','v56Video','<%write(v56Video);%>'],
			   ['video','funshionVideo','<%write(funshionVideo);%>'],
			   ['video','baiduMp3','<%write(baiduMp3);%>']
			   );
var webArray = new Array(['web','sina','<%write(all);%>']);



var ajaxObj=ajax();
if(ajaxObj){

	ajaxObj.url="/goform/getCacheData";
	ajaxObj.processHandle=function (str){

		
		dirUtilizationPieValues = null;
		cloudFlowBarValues = null;
		
		eval(str);
		
		dirUtilizationPie.VALUES = dirUtilizationPieValues;
		cloudFlowBar.VALUES = cloudFlowBarValues;
		                                                                            
		try{                                              
		    cloudFlowBar.drawBarFun(); 
			dirUtilizationPie.drawPieFun();  
		}catch(e) {}; 
	}
 }


ajaxObj.start=function() {
	var fun=(ajaxObj.method=='get'?ajaxObj.get:ajaxObj.post);	
	fun();
	var interval = document.getElementById("interval").value * 1000;	
	ajaxObj.ptr=setInterval(fun,interval);
	
	
}	
ajaxObj.Stop=function()
{	
	clearInterval(ajaxObj.ptr);	
}
function init(){
	if(isServerRun&&ajaxObj){		
		ajaxObj.start();
	}else{
		ajaxObj.Stop();
	}
}

function sltOnchg()
{
	if (isServerRun)
	{
		var index = document.getElementById("interval").value;
		clearInterval(ajaxObj.ptr);
		var fun=(ajaxObj.method=='get'?ajaxObj.get:ajaxObj.send);
		fun();
		ajaxObj.ptr=setInterval(fun, (index * 1000));		
	}
}

function writeVideo() {
	var les = videoArray.length;
	//document.write('<tr>');
	for ( i = 0 ; i < les ; i++){ //{
	if(i%4==0)
	document.write('<tr><td width="2%" height="27"><div align="left"><input type="checkbox" name="'+videoArray[i][1]+'" id="'+videoArray[i][1]+'"  ></div></td><td width="3%">&nbsp;</td><td width="19%">'+videoArray[i][2]+'</td>');
	else if(i%4==3)
	document.write('<td width="2%" height="27"><div align="left"><input type="checkbox" name="'+videoArray[i][1]+'" id="'+videoArray[i][1]+'"  ></div></td><td width="3%">&nbsp;</td><td width="19%">'+videoArray[i][2]+'</td></tr>');
	else
	document.write('<td width="2%" height="27"><div align="left"><input type="checkbox" name="'+videoArray[i][1]+'" id="'+videoArray[i][1]+'"  ></div></td><td width="3%">&nbsp;</td><td width="19%">'+videoArray[i][2]+'</td>');
	}
	if(les % 4 ==1)
	   document.write('<td colspan="9" width="74%"></td></tr>');
	else if(les % 4 ==2)
	   document.write('<td colspan="6" width="49%"></td></tr>');
	else if(les % 4 ==3)
	   document.write('<td colspan="3" width="24%"></td></tr>');
}
function writeWeb() {
	var les = webArray.length;
	//document.write('<tr>');
	for ( i = 0 ; i < les ; i++){ //{
	if(i%4==0)
	document.write('<tr><td width="2%" height="27"><div align="left"><input type="checkbox" name="'+webArray[i][1]+'" id="'+webArray[i][1]+'" ></div></td><td width="3%">&nbsp;</td><td width="19%">'+webArray[i][2]+'</td>');
	else if(i%4==3)
	document.write('<td width="2%" height="27"><div align="left"><input type="checkbox" name="'+webArray[i][1]+'" id="'+webArray[i][1]+'" ></div></td><td width="3%">&nbsp;</td><td width="19%">'+webArray[i][2]+'</td></tr>');
	else
	document.write('<td width="2%" height="27"><div align="left"><input type="checkbox" name="'+webArray[i][1]+'" id="'+webArray[i][1]+'"  ></div></td><td width="3%">&nbsp;</td><td width="19%">'+webArray[i][2]+'</td>');
	}
	if(les % 4 ==1)
	   document.write('<td colspan="9" width="74%"></td></tr>');
	else if(les % 4 ==2)
	   document.write('<td colspan="6" width="49%"></td></tr>');
	else if(les % 4 ==3)
	   document.write('<td colspan="3" width="24%"></td></tr>');
}




function setCacheSize(){
	document.getElementById("cacheSize").value = cacheSize;
}

function isRun(){
	if(isServerRun==1){
		document.cacheConfig.elements["serverEnable"].checked=true;
	}
}
function checked(){
	var les = checklist.length;
	
	for(i=0;i<les;i++){
		document.cacheConfig.elements[checklist[i]].checked=true;
	}
	
}


function clearCache(){
//	alert("save");
	var answer = confirm("<%write(cac_conmclean);%>");
	if(answer){
		document.cacheConfig.action="/goform/clearCacheForm";
		document.cacheConfig.submit();
	}
	
}

function save(){
	var cacheSize = document.cacheConfig.elements["cacheSize"];
	if(cacheSize.value<=0){
		alert("<%write(cac_sizeTip);%>");
		return;
	}
	if(cacheSize.value>400){
		alert("<%write(cac_sizeTip1);%>");
		return;
	}
	document.cacheConfig.submit();
}
</script>
<style type="text/css">
body {
  padding:0px; 
  margin:0px;
  width:100%;
  height:100%;}
.CacheSetPara{
	border:1px solid #FFFFFF;
    background:#84c6ce;
	color : #FFFFFF;
}

.CacheSetContent{
     border:1px solid #FFFFFF;
	 background:#eeeeed;
}
</style>
</head>
<body onbeforeunload="logOut()" background="images/body_main.png" onload="init()">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
<!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle(_("<%write(cloudServer);%>"));</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
  <td><form name="cacheConfig" action="/goform/cacheConfigForm" method="POST">
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
    <td align="center" class="style7">
<table cellpadding="0px" cellspacing="0px" width="100%" height="90%" border="0">
 <tr>
		<td height="2%">&nbsp;</td>
		<td>&nbsp;</td>
		<td>&nbsp;</td>
  </tr>
    <tr>
		<td height="6%" width="10%">&nbsp;</td>
		<td width="80%"><table cellpadding="0px" cellspacing="0px" border="0" width="100%">
			<tr>
			<td  width="70%" class="style6" align="left"><%write(ulr_autoFresh);%>
                  <select name="interval" id="interval" class="input" onChange="sltOnchg()">
                      <option value="1">1</option>
                      <option value="2">2</option>
                      <option value="3">3</option>
                      <option value="4">4</option>
                      <option value="5" selected>5</option>
                  </select><%write(com_second);%>
    		</td> 
				<td width="30%"><%write(runCache);%>&nbsp;&nbsp;<input
				 type="checkbox" id="serverEnable" name="serverEnable"/></td>
			<tr>
		</table></td>
		<td width="10%">&nbsp;</td>
	</tr>
	  <tr>
		<td height="2%">&nbsp;</td>
		<td>&nbsp;</td>
		<td>&nbsp;</td>
	</tr>
	  <tr>
		<td height="30%">&nbsp;</td>
		<td>
			<table cellpadding="0" cellspacing="0" width="100%" border="0">
				<tr>
					<td width="50%" align="left"><%write(cacheStatistics);%></td>
					<td width="50%" align="left"><%write(cacheStorage);%></td>
				</tr>
				<tr>
					<td><embed id="cloudFlowBar" name="cloudFlowBar" src="other/cloudFlowBar.svg" type="image/svg+xml" width="320px" height="200px" style="background: #eeeeed"/></td>
					<td><embed id="dirUtilizationPie" name="dirUtilizationPie" src="other/dirUtilizationPie.svg" type="image/svg+xml" width="340px" height="200px"  style="background: #eeeeed" /></td>
				</tr>
			</table>
		</td>
		<td>&nbsp;</td>
	</tr>
	 <tr>
		<td height="2%">&nbsp;</td>
		<td align="left" valign="middle"><a href="http://download.adobe.com/pub/adobe/magic/svgviewer/win/3.x/3.03/en/SVGView.exe" target="_blank">(<%write(ulr_showErr);%>)</a></td>
		<td>&nbsp;</td>
	</tr>
		 <tr>
		<td height="2%">&nbsp;</td>
		<td>&nbsp;</td>
		<td>&nbsp;</td>
	</tr>
	  <tr>
		<td height="48%">&nbsp;</td>
		<td>
			<table cellpadding="0" cellspacing="0" width="100%" border="0" height="100%">
				<tr>
					
					<td  height="20px" align="left" valign="middle" colspan="2" style="font-size:14px; font-weight:bold" ><%write(cacheSetting);%></td>
				</tr>
				<tr>
					<td align="center" height="80px" valign="middle" width="15%" class="CacheSetPara" ><%write(cacheSize);%></td>
					<td width="85%" class="CacheSetContent" id="SpaceCapacVal">
					<table cellpadding="0" cellspacing="0" width="100%" border="0" height="100%">
					<tr>
						<td align="left" width="96">&nbsp;<input type="text" name="cacheSize" id="cacheSize" style="width:50px;" value="" onkeyup="this.value=this.value.replace(/\D/g,'')" ></input><span id="CacheUnit">GB</span></td>
						<script type="text/javascript">setCacheSize();</script>
					</tr>
					</table>
					</td>
				</tr>
								<tr>
					<td align="center" height="80px" valign="middle" width="15%" class="CacheSetPara"><%write(webVideo);%></td>
					<td  class="CacheSetContent" id="web">
						<table cellpadding="0" cellspacing="0" width="100%" border="0" height="100%">
						<script type="text/javascript">writeVideo();</script>
						</table>
					</td>
				</tr>
				<tr>
					<td align="center" height="80px" valign="middle" width="15%" class="CacheSetPara"><%write(webPage);%></td>
					<td class="CacheSetContent" id="HttpDownload">
						<table cellpadding="0" cellspacing="0" width="100%" border="0" height="100%">
						<script type="text/javascript">isRun();</script>
						<script type="text/javascript">writeWeb();</script>
						<script type="text/javascript">checked();</script>
						</table>
					</td>
				</tr>
			</table>
		</td>
		<td>&nbsp;</td>
	</tr>
	</tr>
	  <tr>
		<td height="2%">&nbsp;</td>
		<td>&nbsp;
			
		</td>
		<td>&nbsp;</td>
	</tr>
	</tr>
	  <tr>
		<td height="6%">&nbsp;</td>
		<td align="right"><table cellpadding="0" cellspacing="0" border="0">
				<tr>
					<td width="100%" align="right"><input type="button" value="<%write(save);%>" id="savebtn" name="savebtn" onclick="save();"/></td>
					<td>&nbsp;&nbsp;<input type="hidden" name="temp" value=""/></td>
					<td><input type="button" value="<%write(clearCache);%>" id="ClearAll" name="ClearAll" onclick="clearCache();"/></td>
					<td> <input type="button" value="<%write(com_help);%>" class="inpu
					    t-button" onClick="window.open('<% write(com_helppage); %>#Cache')"> </td>
				</tr>
			</table></td>
		<td>&nbsp;</td>
	</tr>
	
	 <tr>
		<td height="2%">&nbsp;</td>
		<td>&nbsp;</td>
		<td>&nbsp;</td>
	</tr>
</table>
</td></tr></table>
</form></td></tr></table>
<center</body>
</html>
