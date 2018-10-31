<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("IPMac");%>
<title><% aspGetTitle(); %></title>
<link href="css/style_new.css" rel="stylesheet" type="text/css" />
<link type="text/css" rel="stylesheet" href="css/jquery-ui-1.8.12.custom.css"/>
<script type="text/javascript" src="script/jquery-1.8.1.min.js"></script> 
<script type="text/javascript" src="js/common.js"></script>  
<script type="text/javascript" src="script/jquery.timers-1.2.js" ></script>
<script type="text/javascript" src="script/jquery.corner.js" ></script> 
<script type="text/javascript" src="script/frame.js"></script>  
<script type="text/javascript" ContentType="text/xml">
<% aspOutArpBindGlobalConfig();
	aspOutArpBindList();
%>

var strSecondMenuFocus="<%write(com_bandList);%>";
recsPerPage[0]=10;
lineButton[0]=10;
currpage[0]=1;
recscount[0]=totalrecs;
maxrecs[0]=max_totalrecs;
listname="<%write(ipm_bandInfo);%>";
var titles1=new Array(3);
titles1[0]="ID";
titles1[1]="<%write(com_user);%>";
titles1[2]="<%write(com_IP);%>";
titles1[3]="<%write(com_MAC);%>";
titles1[4]="<%write(com_allow);%>";

arrayData1=new Array(UserNames,arp_IPs,arp_Macs,Allows);
sortable[0]=new Array(1,1,1,1);
sortType[0]=new Array(TYPE_STR,TYPE_IP,TYPE_STR,TYPE_STR);
arrayCount[0]=4;

<!--
function dsplist(index,list)
{
	if(index==0) dspList(index,list,600,titles1,4,arrayData1,1,1);
}
/******************list***********************/
function SaveUserAllow(i)
{
	if (Allows[i].toLowerCase()=="no")　//复选框原来的值是"DENY"，即不选中，那么点击后调用本函数时状态应该是选中
	{
		str="<%write(com_allow);%>";
		ARPBind_List.Allow.value="yes";
	}
	else
	{
		str="<%write(com_ban);%>";
		ARPBind_List.Allow.value="no";
	}
	ARPBind_List.AllowID.value=UserNames[i];
	ARPBind_List.AllowIP.value=arp_IPs[i];
	ARPBind_List.AllowMAC.value=arp_Macs[i].replace(/:/g,"");
	if (confirm("<%write(com_really);%>"+str+"<%write(com_relNet);%>"))
	{
		 ARPBind_List.action ="/goform/formArpBindAllow";
		 ARPBind_List.submit();
	}
	else{
	   window.location.href=window.location.href; 
	}
}
function init(){
	var f=ARPBindGloable_Config;
	f.AllowOtherEnable.checked=(AllowOtherEnables=="on"?true:false);
        $("#updatebtn").addClass("modifybtn");
	$("#error_msg").html(errorstr);
	showSuccessTips(rightstr);
}
function save(){
	ARPBindGloable_Config.action="/goform/formArpBindGlobalConfig";
	ARPBindGloable_Config.submit();
}
function EditConfig(i){
		if(eval(max_totalrecs-totalrecs)<1)	{
			alert("<%write(com_editAlert);%>");
			return;
		}
	var f = ARPBind_Config;
	$("#updatebtn").attr('disabled',false);
        $("#updatebtn").removeClass("modifybtn");
	 f.UserNameold.value=UserNames[i];
        f.IPold.value=arp_IPs[i];
        f.Macold.value=arp_Macs[i];
	f.UserName.value=UserNames[i];
	f.IP.value=arp_IPs[i];
	f.Mac.value=arp_Macs[i];
}
function checkData(){
	var f = ARPBind_Config;
	var tips;
	if(!checkName_P(f.UserName,11,"<%write(com_user);%>",0))
	{
	    return false;
	}
	if(!checkIP_P(f.IP,"<%write(com_IP);%>",0)) 
	{
	return false;
	}
	if(!checkMac_P(f.Mac,"<%write(com_MAC);%>",0))
	{ 
	    return false;
	}
       return true;
}

function update()
{
    	var f= ARPBind_Config;
	f.Action.value="modify";
	if(checkData())
	{
	 ARPBind_Config.action="/goform/formArpBindConfig";
         ARPBind_Config.submit();
        }
}
function DeleteConfig(index){
	if(confirm("<%write(com_deleteTip);%>")){
		ARPBind_List.delstr.value=UserNames[index];
		ARPBind_List.action="/goform/formArpBindDel";
	    ARPBind_List.submit();
		
	}
}
function delAll(){
	if(confirm("<%write(com_deleteAllTip);%>")){
		ARPBind_List.action="/goform/formArpBindDelAll";
		ARPBind_List.submit();
	}
}

function addconfig(){
	location.href='IPMacConfigSui.asp';
}

function delIpMac(){
	if (genData(ARPBind_List,0,frame12,UserNames,'<%write(ipm_IPMacInfo);%>') == true){
	    ARPBind_List.action="/goform/formArpBindDel";
	    ARPBind_List.submit();
	}
}

function ExportIPMac(){
	ARPBind_List.action="/goform/formIPMacExport";
	ARPBind_List.submit();
}
function delIPMac(){
	genData(ARPBind_List,0,frame12,UserNames,'<%write(ipm_IPMacInfo);%>');
	ARPBind_List.action="/goform/formArpBindDel";
	ARPBind_List.submit();
}
var num=0;
$(function(){
       init();
       $("li").corner("10px top");
 })
//-->
</script>
</head>
 <style type="text/css">
ul li {  float:left; margin-left:2px; margin-top:10px; height:32px; padding-top:3px;  padding-left:10px; padding-right:10px; color:#fff;cursor:pointer;}
#tabs{width:650px;}
#firstli{background:#fff;color:#0099D2;}
#secondli{/*background:#0099D2;*/color:#fff;}
#arpBindSeting_box,#list_box{text-align:center; margin:0 auto; width:600px;}
</style>
<body  scroll="no">
<br>
<div style="width:650px; text-align:center;margin-left:25px;margin-right:25px;border:1px solid;" >
    <div style=" height:42px;background-color:#0099D2;margin:0px;padding:0px;width:650px;" id="tabstitle">
	<ul>
		<li style="margin-left:25px;" id="firstli" onclick="window.parent.open_windows(null,'arpList',null)" ><%write(com_bandList);%></li>
		<li  id="secondli" onclick="window.parent.open_windows(null,'arpConf',null)" ><%write(com_bandConf);%></li>
	    </ul>
    </div>
   <div style="width:650px; text-align:left;">&nbsp; 
    <form action="" method="post" name="ARPBindGloable_Config">
    <input type="hidden" id="profIndex" name="profIndex" value="0"/>
	<table  border="0" cellpadding="0" cellspacing="0"  width="100%" id="runstatusInfo" style="background:#5e5e5e;" >
	  	    <tr class="tab_info" align="left">
		<td  colspan="3"  ><%write(ipm_allow);%>&nbsp;&nbsp;<input type="checkbox" name="AllowOtherEnable" >&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<input type="button" class="btn"   value="<%write(com_save);%>" onClick="save()" id="uttsave">&nbsp;&nbsp;&nbsp;
		</td>
	   </tr>
	    	   </table>
	  </form>
    </div>
	     <table border="0" cellpadding="0" cellspacing="1" frame="hsides" rules="all" bordercolor="#fff" width="650px" id="runstatusInfo" style="background:#5e5e5e;" class="group">
	    <form action=""  name="ARPBind_Config" method="post">
            <tr class="tab_title">
              <td colspan="3"> <%write(ipm_modify);%><input type="hidden" name="UserNameold" value=""><input type="hidden" name="Action"><input type="hidden" name="IPold">  <input type="hidden" name="Macold"> </td>
            </tr>
	<tr class="tab_info" >
	    <td ><%write(com_user);%></td>
            <td  colspan="2"align="center"><input name="UserName" type="text" size="18" maxlength="11" class="txt-input" />
            </td>
            </tr>
	<tr class="tab_info" >
	    <td ><%write(com_IP);%></td>
            <td colspan="2" ><input name="IP" type="text" size="18" maxlength="15"   class="txt-input" />
	    </td>
	    </tr>
         <tr class="tab_info"style="border=1px solid;">
	    <td  ><%write(com_MAC);%></td>
            <td colspan="2"><input name="Mac" type="text" size="18" maxlength="17"  class="txt-input" />
         </td>
	</tr> 
	<tr height="60px" class="tab_savetd">
	<td colspan="3" align="center">
         <table cellpadding="0" cellspacing="0" border="0" width="550">
         <tr>
	    <td align="center" colspan="2" ><span id="error_msg"  style="color:red;"></span></td>
            <td align="right" width="250px"><input type="button" id="updatebtn" class="btn"  name="back" onclick="update();"  value="<%write(com_modify);%>" disabled/></td>
          </tr>
	</table>
        </td>    
        </tr>
	</form>    
	</table>
	
	<form name="ARPBind_List" method="post" action="/goform/formArpBindDel">
         <script type="text/javascript">document.write(dspListTab(listname,titles1,titles1.length,arrayData1,1,"650px"));</script>
         <table border="0" cellpadding="0" cellspacing="0" width="650px" id="runstatusInfo" style="background:#5e5e5e;" class="group">	
	<tr style="border=1px solid;">
		
	    <div>
	    <td colspan="3" class="tab_savetd" align="right">
	    <input type="hidden" name="delstr" size=10 /><input type="hidden" name="id"/><input type="hidden" name="AllowID" /><input type="hidden" name="AllowIP"/><input type="hidden" name="AllowMAC"><input type="hidden" name="Allow"/>
	    <span><input type="button" class="btn"   value="<%write(ipm_export);%>" onClick="ExportIPMac()"/></span>
	     <span><input type="button" class="btn"   value="<%write(com_addNew);%>" onClick="addconfig()"/></span>
	    <span><input type="button"class="btn" value="<%write(com_delAll);%>" onClick="delAll()"/></span>&nbsp;&nbsp; </td></div>
        </tr>
    </table>
</form>
</div>
</body>   
<br><br>
</html>
