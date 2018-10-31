<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<title></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>

<script language="javascript">
///////////////////////显示的状态变量//////////////
var totalrecs=6;
var ssids=new Array();//SSID
var macs=new Array();//ap mac地址
var secModes=new Array();//安全机制
var authModes=new Array();//认证模式
var channles=new Array();//信道
var signals=new Array();//信号强度
 ssids[0]="atkj";
 macs[0]="00-11-22-33-44-55";
 secModes[0]="WEP";
 authModes[0]=="open";
 channles[0]="5";
 signals[0]="70%";

 ssids[1]="asdd";
 macs[1]="00-11-22-33-44-55";
 secModes[1]="WEP";
 authModes[1]=="open";
 channles[1]="7";
 signals[1]="100%";

 ssids[2]="qwe";
 macs[2]="00-11-22-33-44-55";
 secModes[2]="";
 authModes[2]=="open";
 channles[2]="9";
 signals[2]="80%";
 
 ssids[3]="123_uty";
 macs[3]="00-11-22-33-44-55";
 secModes[3]="WPA";
 authModes[3]=="share";
 channles[3]="10";
 signals[3]="20%";
 
 ssids[4]="erty_cxhu";
 macs[4]="00-11-22-33-44-55";
 secModes[4]="WPA";
 authModes[4]=="share";
 channles[4]="10";
 signals[4]="20%";
 
 ssids[5]="hu.cuanxiang";
 macs[5]="00-11-22-33-44-55";
 secModes[5]="WPA";
 authModes[5]=="share";
 channles[5]="10";
 signals[5]="20%";
//////////////////////////end//////////////////////
<!--
function OnClickAPScan(idx,obj,init_idx)
{
	var i, trobj;
	var apnumObj;

	//apnumObj = document.getElementsByName('apnum');

    for( i=0;i<totalrecs ; i++)
    {
    	trobj = document.getElementById('scan_tr_'+i);
        if(!trobj) break;
        trobj.className = 'big_td';

        textobj = document.getElementById('scan_text_0_'+i);
        if(!textobj) break;
		if( i == init_idx ) textobj.className = 'navy_text';
		//else if(apnumObj[i] && apnumObj[i].value == 0 ) textobj.className = 'item_text';
		else textobj.className = 'gray_text';
		textobj = document.getElementById('scan_text_1_'+i);
        if(!textobj) break;
		if( i == init_idx ) textobj.className = 'navy_text';
		//else if(apnumObj[i] && apnumObj[i].value == 0 ) textobj.className = 'item_text';
		else textobj.className = 'gray_text';
		textobj = document.getElementById('scan_text_2_'+i);
        if(textobj) textobj.className = 'item_text';
    }

    trobj = document.getElementById('scan_tr_'+idx);
    trobj.className = 'big_selected_td';

    textobj = document.getElementById('scan_text_0_'+idx);
    textobj.className = 'white_text';
    textobj = document.getElementById('scan_text_1_'+idx);
    textobj.className = 'white_text';

    textobj = document.getElementById('scan_text_2_'+idx);
	if(textobj) textobj.className = 'white_text';
	obj.value = ssids[idx];
}
function OnDBLClickAPScanNoWizard(idx)
{
	var obj, doc;
    if(idx == -1) 
    {
		idx = parseInt(parent.iframe_scan.document.aplist_fm.apidx.value);
	    doc = parent.iframe_scan.document;
    }
    else
	    doc = document;
 	doc.aplist_fm.apidx.value=ssids[idx];alert(doc.aplist_fm.apidx.value);
	parent.SelAP_Config.selssid.value=doc.aplist_fm.apidx.value;
	parent.SelAP_Config.action="/goform/";
	parent.SelAP_Config.submit();
}

//-->
</script>
<style type="text/css">
.scan_body { margin-left:0; margin-right:0; margin-top:0px; margin-bottom:0; background-color:#FFFFFF;}
.wizard_content_table {border-collapse:collapse; border-style:none; align:left; }
.big_td { height:50px; padding: 3px 5px; border-style:none none solid none; border-collapse:collapse; border-color:#CCCCCC gray #CCCCCC gray; border-width:1 1 1 1; font-size:12px;}
.gray_text { font: normal normal normal 12px arial; color:#888888; }
.item_text {FONT: 12px arial; COLOR: #000000}
.navy_text {FONT: 12px arial; COLOR: navy}
.big_selected_td {
	BORDER-RIGHT: 1px; PADDING-RIGHT: 5px; BORDER-TOP: 1px solid; PADDING-LEFT: 5px; PADDING-BOTTOM: 3px; BORDER-LEFT: 1px; PADDING-TOP: 3px; BORDER-BOTTOM: 1px solid; BORDER-COLLAPSE: collapse; HEIGHT: 0px; BACKGROUND-COLOR: #1c49ac
}
.white_text {FONT-SIZE: 12px; COLOR: #ffffff}
</style>
</head>
<body class="scan_body">
<form method=get action="/goform/" name=aplist_fm>
<table width=465 class=wizard_content_table><input type=hidden name=apidx >
<script language="javascript">
	for(i=0;i<totalrecs;i++){
		document.write('<input type=hidden name=ssid value='+ssids[i]+'><input type=hidden name=mac value='+macs[i]+'><input type=hidden name=secMode value='+secModes[i]+'><input type=hidden name=authMode value='+authModes[i]+'>');
		document.write('<tr id=scan_tr_'+i+' class=big_td onClick="OnClickAPScan('+i+',document.aplist_fm.apidx,0)" onSelectStart="return false;" onDrag="return false;" onDblClick="OnDBLClickAPScanNoWizard('+i+')">');
		document.write('<td class=big_td valign=center><img src="images/signal.gif"></td>');
		document.write('<td class=big_td valign=top><a name=search_ap_'+i+'><span id=scan_text_2_'+i+'><b>'+ssids[i]+'</b></span></a><br><span id=scan_text_0_'+i+' class=gray_text> (信道 '+channles[i]+' - '+macs[i]+')<input type=hidden name=channel value='+channles[i]+'><br>'+(secModes[i]==""?"没有安全保障的无线网络":"<img src=images/lock.gif border=0> 有安全保障的无线网络&nbsp;("+secModes[i]+")")+'</span></td>');
		document.write('<td valign=top align=right class=big_td><span id=scan_text_1_'+i+' class=gray_text> 信号强度 - '+signals[i]+'</span></td>');
		document.write('</tr>');
	}
</script>
</table>
</form>
</body></html>
