<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<%langTran("common");%>
<%langTran("Route");%>
<%langTran("JS");%>
<title><%write(rou_routeConf);%></title>
<link href="css/style_new.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="script/jquery-1.8.1.min.js"></script> 
<script type="text/javascript" src="js/common.js"></script> 
<script type="text/javascript" src="script/frame.js"></script> 
<script type="text/javascript" src="script/jquery.artDialog.js" ></script>
</head>
<script type="text/javascript">
var RouteStatus = new Array();
<%aspOutputPPTPAccInfo();%>
<% aspOutStaticRouteList();%>
<%getWanIfCount();%>
for(i=0;i<totalrecs;i++){
	if (Profiles[i] == "LAN0") {
		Profiles[i] = "LAN";
	}
	else if(wanIfCount==1 && Profiles[i] == "WAN1") {
		Profiles[i] = "WAN";
	}
	

	RouteStatus[i]=(RouteEnables[i]=="1"?"<%write(JSAlert64);%>":"<%write(JSAlert53);%>");	
}

var listheadname="<%write(rou_routeList);%>";
var titles=new Array();
var editNameBak="";
titles[0]="ID";
titles[1]="<%write(rou_routeName);%>";
titles[2]="<%write(com_status);%>";
titles[3]="<%write(rou_destNet);%>";
titles[4]="<%write(com_NetMask);%>";
titles[5]="<%write(com_GateWayAddr);%>";
titles[6]="<%write(com_priority);%>";
titles[7]="<%write(com_Interface);%>";

var arrayData = new Array(RouteNames,RouteStatus,DesIPs,DesMasks,GateWays,Metrics,Profiles);

function init(){
		var f = StaticRouter_Config;
		var switch_b = $(".on-off").children("button");
		initEnabledButton(switch_b,"on");
		$("#RouteEnable").val("1");
		$("#error_msg").html(errorstr);
	    $("#updatebtn").addClass("modifybtn");
	    showSuccessTips(rightstr);
}
function checkData(){
	var f = StaticRouter_Config;
	if(!checkName_P(f.RouteName,11,"<%write(rou_routeName);%>",0)) return false;
	if(!checkIP_P(f.DesIP,"<%write(rou_destNet);%>",0)) return false;
	if (!checkMask_P(f.DesMask,"<%write(com_NetMask);%>",0)) return false;
	if(!checkIP_P(f.GateWay,"<%write(com_GateWayAddr);%>",0)) return false;
	if(! checkNumber_P(f.Metric,0,7,"<%write(com_priority);%>",0)) return false;
	return true;
}

function save(){
	var f=StaticRouter_Config;
	if(checkData()){
		f.action="/goform/formStaticRoute";
		f.submit();
	}
	
}
function add(){
	StaticRouter_Config.Action.value="add";
  	save();
}
function update(){
    if(StaticRouter_Config.RouteName.value!=editNameBak)
	{
		$("#error_msg").html("<%write(rou_routeName);%><%write(rou_noModify);%>");
		return;
	}
	StaticRouter_Config.Action.value="modify";
  	save();
}
function EditConfig(i){
		var f = StaticRouter_Config;
		editNameBak=RouteNames[i];
		f.RouteName.value=RouteNames[i];		
		f.DesIP.value=DesIPs[i];
		f.DesMask.value=DesMasks[i];
		f.GateWay.value=GateWays[i];
		f.Metric.value=Metrics[i];
		if(wanIfCount==1 && Profiles[i] == "WAN")
			selVal(f.Profile,"WAN1");
		else selVal(f.Profile,Profiles[i]);
		var switch_b = $(".on-off").children("button");
		var status = (RouteEnables[i]=="1"?"on":"off");
		initEnabledButton(switch_b,status);
		$("#RouteEnable").val(RouteEnables[i]);
		$("#updatebtn").attr('disabled',false);
        $("#updatebtn").removeClass("modifybtn");
}
function DeleteConfig(index){
	if(confirm("<%write(com_deleteTip);%>")){
		StaticRouter_Config.delstr.value=RouteNames[index];
		StaticRouter_Config.action="/goform/formStaticRouteDel";
	    StaticRouter_Config.submit();
	}
}
function delAll(){
	if(confirm("<%write(com_deleteAllTip);%>")){
		StaticRouter_Config.action="/goform/formStaticRouteDelAll";
		StaticRouter_Config.submit();
	}
}


$(function(){
	init();
	
	$(".on-off").click(function(){
		var switch_b = $(".on-off").children("button");
		var status_now = switch_b.attr("class");
		if(status_now=="on"){
			$("#RouteEnable").val("0");	
		}else{
			$("#RouteEnable").val("1");	
		}
		swicth_act(status_now);
	});
	})
</script>
<body>

<div class="warp" style="width: 700px;">
<br>
	<form action="" method="post" name="StaticRouter_Config">
    <!-- <div id="error_msg" style="color:red;"></div> -->
	<table border="0" cellpadding="0" cellspacing="1" style="background:#5e5e5e;width:700px;" class="group" >
   		  <tr class="tab_title">
        	<td colspan="3"><%write(rou_set);%><input type="hidden" name="Action"><input type="hidden" name="delstr"></td>
           
        </tr>
   		 <tr class="tab_info">
        	<td width="140px"><%write(rou_routeName);%></td>
            <td width="360px"  colspan="2">
            	<input name= "RouteName" type="text" maxlength="11" size="16" class="txt-input" onChange="validateName(this,11,'<%write(rou_routeName);%>',0);" />
            </td>
           
        </tr>
        <tr class="tab_info">
        	<td><%write(com_startUse);%></td>
            <td colspan="2"><span class="on-off">&nbsp;<button type="button" class="on"><%write(com_switch);%></button><input type="hidden" name="RouteEnable" id="RouteEnable" value="" /></span></td>
           
        </tr>
       	 <tr class="tab_info">
        	<td><%write(rou_destNet);%></td>
            <td colspan="2">
            		<input name="DesIP" type="text" size="16" maxlength="15" onKeyPress="ipOnly_P(event)" onChange="validateIP(this,'<%write(rou_destNet);%>',0);" class="txt-input" value="0.0.0.0" />
            </td>
           
        </tr>
        <tr class="tab_info">
        	<td><%write(com_NetMask);%></td>
            <td colspan="2">
            	<input name="DesMask" type="text" size="16" maxlength="15"   onKeyPress="ipOnly_P(event)"  onChange="validateMask_P(this,'<%write(com_NetMask);%>',0);" class="txt-input" value="255.255.255.0" />

            </td>
        </tr>
        <tr class="tab_info">
        	<td><%write(com_GateWayAddr);%></td>
            <td colspan="2">
            	<input name="GateWay" type="text" size="16" maxlength="15" onChange="validateIP(this,'<%write(com_GateWayAddr);%>',0);" onKeyPress="ipOnly_P(event)" class="txt-input" value="0.0.0.0" />
            </td>
        </tr>
        <tr class="tab_info" id="tbservice">
        	<td><%write(com_priority);%></td>
            <td colspan="2"><input name="Metric" type="text" size="16" onChange="validateNumber(this,0,7,'<%write(com_priority);%>',0);" style="width:50px" maxlength="1" onKeyPress="figureOnly_P(event)" class="txt-input"  value="0" />&nbsp;<%write(rou_priTip);%></td>
        </tr>
      
        <tr class="tab_info">
        	<td><%write(com_Interface);%></td>
            <td colspan="2">
          		<select name="Profile" class="select" style="width:80px">         
	  <option value='LAN0'> LAN</option>
        	<script language="javascript"> 
		 for(var i=1;i<=wanIfCount;i++){
					document.write('<option value="WAN'+i+'" '+ (i==1? 'selected="selected"':'') +'>WAN'+(wanIfCount==1?"":i)+'</option>');
		 }
                 if(PptpClientNames.length>0)
                 {
                    for(var i=0;i<PptpClientNames.length;i++)
                    {
                        document.write('<option value='+PptpClientNames[i]+'>'+PptpClientNames[i]+'</option>');
                    }
                 }
		</script> 	    
	   </select>
            </td>
        </tr>
       	  <tr  height="60px" class="tab_savetd"> 
        	<td colspan="3" align="center">
            	<table cellpadding="0" cellspacing="0" border="0" width="550">
                	<tr>
                    	<!--<td width="70px">&nbsp;</td>-->
                        <td align="center" colspan="2" ><span id="error_msg"  style="color:red;"></span></td>
                        <td align="right" width="250px"><input type="button" class="btn" name="back" onclick="add();"  value="<%write(com_add);%>"/>&nbsp;&nbsp;&nbsp;&nbsp;<input type="button"  id="updatebtn"  class="btn" name="back" onclick="update();"  disabled value="<%write(com_modify);%>"/></td>
                    </tr>
                </table>
           </td>
            
        </tr>
      
    </table>
    <script type="text/javascript">document.write(dspListTab(listheadname,titles,titles.length,arrayData,1,"700px"));</script>
   <table border="0" cellpadding="0" cellspacing="1" style="background:#5e5e5e;width:700px;" class="group" >
   		<tr><td  class="tab_savetd" align="right"><span><input type="button" class="btn" onclick="delAll();" value="<%write(com_delAll);%>"/></span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td></tr>
   </table>
    </form>
<br>
<br>
</div>
</body>
</html>
