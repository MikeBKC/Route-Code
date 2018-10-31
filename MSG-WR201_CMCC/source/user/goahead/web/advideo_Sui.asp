<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("advideo");%>
<%langTran("advideoSui");%>
<title><%write(com_utt);%></title>
<link href="css/style_new.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="js/common.js"></script> 
<script type="text/javascript" src="script/jquery-1.8.1.min.js"></script> 
<script type="text/javascript" src="script/frame.js"></script> 
<script type="text/javascript" src="script/jquery.timers-1.2.js" ></script>
<script type="text/javascript">
var advideo = 0 ;
var act_en = 1 ;
<%aspAdvideo();%> 
<%getAdFilterAppType();%>
function save(){
    var f=formadvideo;
    var advideoEnable = $("#advideo").val();
    if (advideoEnable == "1") {
        f.action="/goform/ConfigAdvideo";
    } else {
        f.action="/goform/ConfigAdvideoDel";
    }
    f.submit();
}  

function init()
{
    var switch_b = $(".on-off").children("button");
    var en = "off";
    if (advideo == 1)
    {
	en = "on";
    }
    initEnabledButton(switch_b,en);
    $("#advideo").val(advideo);	
    $("#error_msg").html(errorstr);
    showSuccessTips(rightstr);
}

function appUpdate(appType)
{
    $.ajax({
    url: "/goform/formAppUpdate?appType="+appType,
    cache: false,
    dataType: "json",
    async:false,
    success: function(rsp){
    /*
	if(rsp && rsp.appType == appType){
	    if (rsp.updateStatus >= 4) {
		statusInfo = "有更新";
		action = "<a onclick = 'appUpdate("+appType+")'>立即更新</a>";
	    } else {
		action = "<a href='/goform/formAppCheck?appType="+appType+"'>更新检测</a>";
	    }
	}
	*/
    },
    error:function(data){
    
    }
    });
}
var glflag = 0;
function appStatusCheck(appType,action)
{
    var statusInfo = "";
    var actionInfo = "";

    if (action == "MANUAL") {
	glflag = 1;
    } 
    $.ajax({
    url: "/goform/formAppCheck?_="+new Date().getTime(),
    cache: false,
    dataType: "json",
    data:{
	"appType":appType,
	"action":action
    },
    async:false,
    success: function(rsp){
	if(rsp && rsp.appType == appType){
	    switch(rsp.updateStatus){
	    case 1:
		statusInfo = "<%write(com_statusInfo1);%>";
		break;
	    case 2:
		statusInfo = "<%write(com_statusInfo2);%>";
		break;
	    case 3:
		if(glflag == 0) {
		statusInfo = "<%write(com_statusInfo3);%>";
		} else {
		statusInfo = "<%write(com_statusInfo10);%>";
		}
		break;
	    case 4:
		statusInfo = "<%write(com_statusInfo4);%>";
		break;
	    case 5:
		statusInfo = "<%write(com_statusInfo5);%>";
		break;
	    case 6:
		statusInfo = "<%write(com_statusInfo6);%>";
		break;
	    default:
		statusInfo = "<%write(com_statusInfo7);%>";
		break;
	    }
	    if (rsp.updateStatus == 4) {
		actionInfo = "<a onclick = 'appUpdate("+appType+")'><%write(com_statusInfo9);%></a>";
	    } else {
		actionInfo = "<a onclick = 'appStatusCheck("+appType+","+"\"MANUAL\""+")'><%write(com_statusInfo8);%></a>";
	    }
	    $('body').stopTime('Timer1');
	    //if (rsp.stopAjax == 0) {
		$('body').everyTime('3s','Timer1',function(){
		    appStatusCheck(ADFILTER_UDTYPE,"AUTO");
			},0,true);
	    //}
	}
    },
    error:function(data){
    
    }
    });
    $("#appStatus").html(statusInfo);
    $("#appAction").html(actionInfo);
}
$(function(){
    init();
    $(".on-off").click(function(){
	var switch_b = $(".on-off").children("button");
	var status_now = switch_b.attr("class");
	if(status_now=="on"){
	    $("#advideo").val("0");	
	}else{
	    $("#advideo").val("1");	
	}
	swicth_act(status_now);
    });
    var f = formadvideo;
    if (updateType == 0) {/*自动*/
	f.updateType[0].checked = true;
	$("#appStatus").hide();
	$("#appAction").hide();
    } else {/*手动*/
	f.updateType[1].checked = true;
	$("#appStatus").show();
	$("#appAction").show();
    }
    $("form input:radio").click(function(){
	if (this.value == 0) {
	    $("#appStatus").hide();
	    $("#appAction").hide();
	} else {
	    actionInfo = "<a onclick = 'appStatusCheck("+ADFILTER_UDTYPE+","+"\"MANUAL\""+")'><%write(com_statusInfo8);%></a>";
	    $("#appAction").html(actionInfo);
	    $("#appStatus").show();
	    $("#appAction").show();
	}
    });

    if (updateType == 1) {/*手动更新的情况下，才会启动*/
	appStatusCheck(ADFILTER_UDTYPE,"AUTO");/*页面首次加载时,启动定时器*/
    }
    parent.closeTimer();
});
</script>
</head>
<body>
<div class="warp">

<br>
	<form action="" method="post" name="formadvideo">
    
	<table border="0" cellpadding="0" cellspacing="1" id="runstatusInfo" style="background:#5e5e5e;" class="group" >
   
    	<tr class="tab_info">
        	<td width="120px"><%write(adv_open);%></td>
            <td width="480px" colspan="2"><span class="on-off">&nbsp;<button type="button" class="on"><%write(com_switch);%></button><input type="hidden" name="advideo" id="advideo" value="" /></span>
	    </td>
        </tr>
    	<tr class="tab_info">
        	<td width="120px"><%write(updateWay);%></td>
            <td width="480px" colspan="2"><span>&nbsp;<input type="radio" name="updateType" value=0 checked><%write(auto);%></input>
	    &nbsp;&nbsp;&nbsp;<input type="radio" name="updateType" value=1 /><%write(manual);%></span>
	    &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
	    <span id="appAction"></span>
	    &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
	    <span style="color:red;" id="appStatus"></span>
	    </td>
        </tr>
        <tr class="tab_title">
        	<td colspan="3"><%write(adv_filt);%></td>
        </tr>
        <tr class="tab_info" height="70px">
            <td colspan="3">
		<div style="padding-bottom:5px">
		    <input type="checkbox" disabled checked style="display:none;" ><%write(adv_youku);%>&nbsp;&nbsp;&nbsp;&nbsp;
		    <input type="checkbox" checked disabled style="display:none;" ><%write(adv_tudou);%>&nbsp;&nbsp;&nbsp;&nbsp;
		    <input type="checkbox" disabled checked style="display:none;" ><%write(adv_leshi);%>&nbsp;&nbsp;&nbsp;&nbsp;
		    <input type="checkbox" disabled checked style="display:none;" ><%write(adv_tx);%>&nbsp;&nbsp;&nbsp;&nbsp;
		    <input type="checkbox" disabled checked style="display:none;" ><%write(adv_iqiyi);%>&nbsp;&nbsp;&nbsp;&nbsp;
		    <input type="checkbox" disabled checked style="display:none;" >PPS&nbsp;&nbsp;&nbsp;&nbsp;
		    <input type="checkbox" disabled checked style="display:none;" >PPTV&nbsp;&nbsp;&nbsp;&nbsp;
		    <input type="checkbox" disabled checked style="display:none;" ><%write(adv_sohu);%>
		</div>
            </td>
        </tr>
         <tr  height="60px" class="tab_savetd"> 
	    <td colspan="3" align="center">
		<table cellpadding="0" cellspacing="0" border="0" width="550">
		     <tr valign="top" style="background:#FFF;">
			<td colspan="3" align="left">
			    <span id="tips"><%write(adv_warn1);%><br>
				    <%write(adv_warn2);%><br>
				    <%write(adv_warn3);%>
			    </span>
			</td>
		    </tr>
		    <tr>
			<td align="center" colspan="2" ><span id="error_msg"  style="color:red;"></span>
			    <input name="GroupName" type="hidden" maxlength="12" size="18" value="utt_action">
			    <input type="hidden" name="Action" value="add">
			    <input type="hidden" name="pdbPolicy" value="143">
			    <input type="hidden" name="sourceIP" value="groupSel">
			    <input type="hidden" name="day" value="1111111">
			    <input type="hidden" name="timestart" value="All">
			    <input type="hidden" name="timestop"  value="0">
			</td>
			<td align="right" width="200px"><input type="button" class="btn" onclick="save();"  value="<%write(com_save);%>"/></td>
		    </tr>
			</table>
		   </td>
		</tr>
	    </table>
	    </form>
    <br>
    <br>
    </div>
</body>
</html>
