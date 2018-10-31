<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
 
 
<title>艾泰科技</title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml"> 
//Butterlate.setTextDomain("firewall");
 
//function initTranslation()
//{
//	var e;
//
//	e = document.getElementById("add");
//	e.value= _("add");
//	e = document.getElementById("delall");
//	e.value= _("delall");
//	e= document.getElementById("del");
//	e.value= _("del");
//
//}
 
var strSecondMenuFocus="基本配置AP列表";
 
//交互变量
/*
var RouteNames=new Array();
var RouteEnables=new Array();
var DesIPs=new Array();
var DesMasks=new Array();
var GateWays=new Array();
var Metrics=new Array();
var Profiles=new Array();
 
*/



var errorstr="";var totalrecs=0;var max_totalrecs=100;

 
//var bssids=new Array();
//var ssids=new Array();
//var signalStrength=new Array();
//var channels=new Array();
//var Encrypts=new Array();
//var choice = new Array();

<%
	aspOutPutApScan();
%>
//bssids[0]="A9-15-4D-FE-bc-94";
//signalStrength[0]="7dB";
//channels[0]="161";
//Encrypts[0]="是";
//choice[0]="选择";

//bssids[1]="A3-15-4D-FE-bc-94";
//signalStrength[1]="15dB";
//channels[1]="114";
//Encrypts[1]="是";
//choice[1]="选择";
//var errorstr="";
//var totalrecs=2;
//var max_totalrecs =32;
var oldDspList=dspList;
dspList=function(){
	oldDspList.apply(this,arguments);
	var tr=frame12.document.getElementById('tableList').rows;
	 for(var i=1;i<tr.length;i++){
		str=trimstr(tr[i].cells[5].innerHTML);
		if(str!="" || str!= undefined)
	 	tr[i].cells[5].innerHTML='<a href="#" onClick="parent.connect('+(i-1)+');" >'+str+'</a>';	
	 }
};
function connect(index){
	
	AP_List.bridgessid.value=ssids[index];
	AP_List.bridgebssid.value=bssids[index];
	AP_List.channel.value=channels[index];
	AP_List.action="WlanBase.asp";
	AP_List.submit();
}

function back(){
	
//	AP_List.bridgessid.value=ssids[index];
//	AP_List.bridgebssid.value=bssids[index];
	AP_List.action="WlanBase.asp";
	AP_List.submit();
}

 function init(){
	AP_List.WrlessEnable.value="<% write(Form('WrlessEnable'));%>";
	AP_List.ssid.value="<% write(Form('ssid'));%>";
	AP_List.APWorkMode.value="<% write(Form('APWorkMode'));%>";
	AP_List.APssid.value="<% write(Form('APssid'));%>";
	AP_List.APCliMacs.value="<% write(Form('APCliMacs'));%>";
	AP_List.APMac0.value="<% write(Form('APMac0'));%>";
	AP_List.APMac1.value="<% write(Form('APMac1'));%>";
	AP_List.APMac2.value="<% write(Form('APMac2'));%>";
	AP_List.APMac3.value="<% write(Form('APMac3'));%>";

	if(AP_List.APMac0.value =="") 
	AP_List.APMac0.value= "000000000000";
	if(AP_List.APMac1.value =="") 
	AP_List.APMac1.value= "000000000000";
	if(AP_List.APMac2.value =="") 
	AP_List.APMac2.value= "000000000000";
	if(AP_List.APMac3.value =="") 
	AP_List.APMac3.value= "000000000000";

	AP_List.wdsSecModes.value="<% write(Form('apclisecmode'));%>";
	AP_List.wdspskPsswds.value="<% write(Form('wdspskPsswd'));%>";

	/* */
	AP_List.wdskeynums.value="<% write(Form('wdskeynums'));%>";

	AP_List.wdswepkey1s.value="<% write(Form('apcliwepkey1'));%>";
	AP_List.wdswepkey2s.value="<% write(Form('apcliwepkey2'));%>";
	AP_List.wdswepkey3s.value="<% write(Form('apcliwepkey3'));%>";
	AP_List.wdswepkey4s.value="<% write(Form('apcliwepkey4'));%>";
	AP_List.wdskeyFormats.value="<% write(Form('apclikeyFormat'));%>";
	AP_List.wdskeyLength1s.value="<% write(Form('apclikeyLength1'));%>";
	AP_List.wdskeyLength2s.value="<% write(Form('apclikeyLength2'));%>";
	AP_List.wdskeyLength3s.value="<% write(Form('apclikeyLength3'));%>";
	AP_List.wdskeyLength4s.value="<% write(Form('apclikeyLength4'));%>";

	AP_List.wrlessMode.value="<% write(Form('wrlessMode'));%>";
	AP_List.chanWidth.value="<% write(Form('chanWidth'));%>";
	AP_List.vhtChanWidth.value="<% write(Form('vhtChanWidth'));%>";
	AP_List.channel.value="<% write(Form('channel'));%>";
	AP_List.broadcastEnable.value="<% write(Form('broadcastEnable'));%>";
	AP_List.wdsEnabled.value="1";
	AP_List.bridgessid.value="<% write(Form('bridgessid'));%>";
	AP_List.bridgebssid.value="<% write(Form('bridgebssid'));%>";
	AP_List.keytype.value="<% write(Form('keytype'));%>";
	AP_List.wepkeynum.value="<% write(Form('wepkeynum'));%>";
	AP_List.certificatetype.value="<% write(Form('certificatetype'));%>";
	AP_List.key.value="<% write(Form('key'));%>";
}

 
//end
 
recsPerPage[0] =10;
lineButton[0] =10;
currpage[0]=1;
recscount[0]=totalrecs;
maxrecs[0]=max_totalrecs;
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]="AP列表";
var titles1=new Array(6);
titles1[0]="BSSID";
titles1[1]="SSID";
titles1[2]="信号强度";
titles1[3]="信道";
titles1[4]="是否加密";
titles1[5]="选择";
 
 
arrayData1=new Array(bssids,ssids,signalStrength,channels,Encrypts,choice);
maxDspLen[0]=new Array(0,0,60);
sortable[0]=new Array(1,1,1,1,1,1);
sortType[0]=new Array(TYPE_STR,TYPE_STR,TYPE_STR,TYPE_STR,TYPE_STR,TYPE_STR);
arrayCount[0]=6;
 
<!--
function dsplist(index,list)
{
	if(index==0) dspList(index,list,600,titles1,6,arrayData1,0,0);
}

function fresh(){
	AP_List.action="WlanBase_list_wait_apScan.asp";
	AP_List.submit();	
}
-->
</script>
</head>
<body onLoad="init();" background="images/body_main.png">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle("AP列表");</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td ><form name="AP_List" method="post" action="">
	<table width="80%" border="" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
    <td align="center" class="style7">
   <table id=grouplist style="display" width="600"  border="0" align="center" cellpadding="0" cellspacing="0"><tr><td colspan="4" height="20">&nbsp;</td></tr>
		     <tr bgcolor="c0ddff">
		      <td colspan="4"><iframe src="_listhead1.htm" name="frame11" scrolling="no" align="MIDDLE" width="600" height="45" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden ></iframe></td>
		    </tr>
		    <tr bgcolor="c0ddff">
		      <td colspan="4"><iframe src="_list1.htm" name="frame12" id="framelist" align="MIDDLE" width="600" height="260" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe></td>
		    </tr>
		    <tr>
		      <td  colspan="4" valign="bottom"><table id=dellist  width="100%"  height="19" border="0" cellspacing="0" cellpadding="1">
			  <tr>
			  <td width="5"></td>
			  <td  class="style6"><div align="left">
		</td>
			  <td  class="style6" ><input type="hidden" name="delstr" size=10 ><input type="hidden" name="id"></td> 
			   <td width="560" valign="top" align="right"><input type="button" value="刷新" class="input-button" onClick="fresh();" id="add"></td>
			  <td valign="top" align="right"><input type="button" value="返回" class="input-button" onClick="back();" id="delall"></td>
			
			  </tr>
		      </table></td>
		    </tr><tr><td colspan="4" height="20">&nbsp;</td></tr>
		  </table>
		  </td></tr></table>
          <input type="hidden" name="AP_LIST" value="aplist"/>
          <input type="hidden" name="WrlessEnable" value=""/>
          <input type="hidden" name="ssid" value=""/>
          <input type="hidden" name="APWorkMode" value=""/>
          <input type="hidden" name="wrlessMode" value=""/>
          <input type="hidden" name="channel" value=""/>
          <input type="hidden" name="chanWidth" value=""/>
          <input type="hidden" name="vhtChanWidth" value=""/>
          <input type="hidden" name="broadcastEnable" value=""/>
          <input type="hidden" name="wdsEnabled" value=""/>
          <input type="hidden" name="APssid" value=""/>
          <input type="hidden" name="APCliMacs" value=""/>
          <input type="hidden" name="APMac0" value=""/>
          <input type="hidden" name="APMac1" value=""/>
          <input type="hidden" name="APMac2" value=""/>
          <input type="hidden" name="APMac3" value=""/>
          <input type="hidden" name="wdsSecModes" value=""/>
          <input type="hidden" name="wdspskPsswds" value=""/>
          <input type="hidden" name="wdskeynums" value=""/>
          <input type="hidden" name="wdswepkey1s" value=""/>
          <input type="hidden" name="wdswepkey2s" value=""/>
          <input type="hidden" name="wdswepkey3s" value=""/>
          <input type="hidden" name="wdswepkey4s" value=""/>
          <input type="hidden" name="wdskeyFormats" value=""/>
          <input type="hidden" name="wdskeyLength1s" value=""/>
          <input type="hidden" name="wdskeyLength2s" value=""/>
          <input type="hidden" name="wdskeyLength3s" value=""/>
          <input type="hidden" name="wdskeyLength4s" value=""/>

          <input type="hidden" name="bridgessid" value=""/>
           <input type="hidden" name="bridgebssid" value=""/>
          <input type="hidden" name="keytype" value=""/>
          <input type="hidden" name="wepkeynum" value=""/>
          <input type="hidden" name="certificatetype" value=""/>
          <input type="hidden" name="key" value=""/>
          
         
  </form></td>
  </tr>
</table></td>
  <td width="15"></td>
  </tr>
</table>
</body>
</html>

