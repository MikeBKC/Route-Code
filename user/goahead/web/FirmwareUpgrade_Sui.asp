<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<%langTran("common");%>
<%langTran("Menu");%>
<%langTran("SoftwareUpdate");%>
<%langTran("DeviceConfig");%>
<%langTran("DeviceConfigSui");%>
<title><%write(FirmwareUpgrade);%></title>
<link href="css/style_new.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="script/jquery-1.8.1.min.js"></script> 
<script type="text/javascript" src="script/jquery.timers-1.2.js" ></script>

</head>
<script type="text/javascript">
<% outputSystemVersion();%>

var oneClick=0;
var errorstr="";
<% aspoutDynConfig(); %>
<% outputTheErrorMsg(); %>
<% outputProductID();%>
<% outputLinkID();%>
<% getFlashMtdSize("Kernel");%>
<% getFirmWareAppType();%>
function init(){
	var f=SoftWare_Config;
    var revisions = "MSG WR201-W.00-T-v1.8.13-171223"
	$("#softwareversion").html(revisions);
	
	/* 硬件版本初始化 */
	if (hardwareID != "") {
		hwmodeltr.style.display = "";
		
	} else {
		hwmodeltr.style.display = "none";
		
	}
	$("#hardwareversion").html(hardwareID);
}
function update()
{
  if(checkData())
  {
	$("input[type='button']").prop("disabled",true);
	$("#error_msg").html("<%write(upg_warn1);%>");
	SoftWare_Config.reBoot.disabled=false;
	$("#error_msg").html("<%write(upg_warn1);%>");
	SoftWare_Config.action = "/goform/UpdateFirmware";
	SoftWare_Config.submit();
  }
}
function autoUpdate()
{
    $.ajax({
    url: "/goform/formAppUpdate?appType=SOFT_UDTYPE",
    cache: false,
    dataType: "json",
    async:false,
    success: function(rsp){
	if(rsp){
	    if (rsp.updateStatus != 7) {
		location.href = "progress.asp";
	    }
	}
    },
    error:function(data){
    
    }
    });
}
function checkData()
{
    /*
    var file = $("input[type='file']").get(0).files[0].files;
    var fileName;
    var extStart;
    var ext;

    if (SoftWare_Config.updatesoftware.value==""){
	 $("#error_msg").html("请指定升级软件");return false;
    }

    fileName = file.name;
    extStart = fileName.lastIndexOf("."); 
    ext = fileName.substring(extStart,fileName.length).toLowerCase(); 
    if (ext != ".bin") {
	$("#error_msg").html("文件格式错误");return false;
    }

    if (mtdKernelSize < file.size) {
	$("#error_msg").html("软件非法，无法升级");return false;
    }
    */
    var fileName = $("input[type='file']").get(0).value;
    var extStart;
    var ext;

    if (SoftWare_Config.updatesoftware.value==""){
	 $("#error_msg").html("<%write(SpecifyUpgradeSW);%>");return false;
    }

    extStart = fileName.lastIndexOf("."); 
    ext = fileName.substring(extStart,fileName.length).toLowerCase(); 
    /*
    if (ext != ".bin") {
	$("#error_msg").html("<%write(fileErr);%>");return false;
    } */
    
    if (SoftWare_Config.reBoot.checked && !confirm("<%write(upg_warn2);%>")) return false;
    return true;
}
function printlinklogo()
{
    document.getElementById("downloadnewversion").href=ProductLinkID;
}
var glflag = 0;
function appStatusCheck(appType,action)
{
    var statusInfo = "";
    var descriptionInfo = "";

    if (action == "MANUAL") {
	glflag = 1;
    } 
    $.ajax({
    url: "/goform/formAppCheck",
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
	    oneClick = rsp.updateStatus;
	    if ((rsp.updateStatus == 2) || (rsp.updateStatus == 3)) {
		statusInfo +="&nbsp;&nbsp;&nbsp;&nbsp;"
		statusInfo +="<a onclick = 'appStatusCheck("+appType+","+"\"MANUAL\""+")'><%write(com_statusInfo8);%></a>";
		$("#appVersion").html("");
		$("#appVersion").hide();
		$("#description").hide();
	    } else {
		if (oneClick == 4) {
		     $("#oneClickUpdate").show();
		} else {
		     $("#oneClickUpdate").hide();
		}
		if (rsp.updateStatus >= 4 && rsp.updateStatus != 6) {
		    $("#appVersion").html(rsp.st+"v"+rsp.sv+"-"+rsp.bt);
		    $("#appVersion").show();
		    descriptionInfo = rsp.st+"v"+rsp.sv +"-"+rsp.bt + " <%write(com_statusInfo11);%>:\n" + rsp.rn;
		    $("#description").show().attr("value",descriptionInfo);

		} else {
		    $("#appVersion").html("");
		    $("#appVersion").hide();
		    $("#description").hide();
		}
	    }
	    $('body').stopTime('Timer1');
	    $('body').everyTime('3s','Timer1',function(){
		appStatusCheck(appType,"AUTO");
		    },0,true);
	}
    },
    error:function(data){
    
    }
    });
    $("#appStatus").html(statusInfo);
}
$(function(){
    $("#error_msg").html(errorstr);
    init();
    if(lang_en == 1){
	$("#hidde").hide();
    }else{
		if(galaxywind_ctrl ==1){//银河风云品牌控制
			$("#hidde").hide();
			$("#downloadnewversion").hide();
		}else if(middlesoft == 1){
            $("#hidde").hide();
			$("#downloadnewversion").hide();
        }
        else{
			$("#hidde").show();
		}
    }
    appStatusCheck(SOFT_UDTYPE,"AUTO");
    if(oneClick != 4)
    {
	$("#oneClickUpdate").hide();
    }
    else
    {
	 $("#oneClickUpdate").show();
    }
    parent.closeTimer();
});
</script>
<body scroll="no">
<div class="warp">

<br>
	<form action="" method="post" name="SoftWare_Config"  enctype="multipart/form-data">
     
	<table border="0" cellpadding="0" cellspacing="1" id="runstatusInfo" style="background:#5e5e5e;" class="group" >
   
    	<tr class="tab_info">
        	<td width="140px"><%write(SWVer);%></td>
            <td width="460px" colspan="2" id="softwareversion"></td>
        </tr>
        <tr class="tab_info" id="hwmodeltr">
        	<td><%write(HDVer);%></td>
            <td colspan="2" id="hardwareversion"></td>
           		
        </tr>
<tr class="tab_info">
                 <td><%write(UpgradeWarn);%></td>
                 <td colspan="2">
                 &diams;&nbsp;<%write(upg_warn4);%><br>
                 &diams;&nbsp;<%write(upg_warn5);%><br>
                 &diams;&nbsp;<%write(upg_warn3);%>
                 </td>
        </tr>
        <tr class="tab_info" id="hidde">
        	<td><%write(OneClick);%></td>
            <td colspan="2">
            	<table cellpadding="0" cellspacing="0" border="0">
                	<tr valign="bottom">
                    	<td>
			<span style="color:red" id="appStatus"></span>&nbsp;&nbsp;&nbsp;&nbsp;
			<span style="color:red" id="appVersion"></span>&nbsp;&nbsp;&nbsp;&nbsp;
		      <span id="oneClickUpdate"><input type="button" class="btn" name="onClickUpdate" onClick="autoUpdate();"value="<%write(OneClick);%>"/></span>&nbsp;&nbsp;
			<textarea id="description" style="width:430px; height:100px;resize:none;border:0;outline:none" readonly></textarea>
			</td>
                    </tr>
	       </table>
            </td>
        </tr>
       
               <tr class="tab_info">
        	<td><%write(ManUpgrade);%></td>
            <td colspan="2">
            	<table cellpadding="0" cellspacing="0" border="0">
                <tr style="height:10px;">
			<br>
			<td align="left"><%write(ChoseUpgradeSW);%><input name="updatesoftware"  type="file"/></td>
			<td align="right"><a href="#" onclick="printlinklogo()" target="_blank" id="downloadnewversion"><%write(DownSoft);%></a></td>
                    </tr>
		     <tr>
                    <td  align="left"><span id="error_msg"  style="color:red;"></span></td>
		    <td align="right"><input type="button" class="btn" name="back" id="savebtn" onClick="update();"value="<%write(Upgrade);%>"/></td>
		    </tr>   
		    <tr valign="top">
                    	<td><input type="checkbox" name="reBoot"  disabled="disabled" style="display:none;" checked="checked"/></td>
                    </tr>
	     </table>
            </td>
        </tr>
    </table>
    </form>
<br>
</div>
</body>
</html>
