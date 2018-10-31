<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<%langTran("common");%>
<%langTran("Menu");%>
<%langTran("DeviceConfig");%>
<%langTran("DeviceConfigSui");%>
<title><%write(BackupRecovery);%></title>
<link href="css/style_new.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="script/jquery-1.8.1.min.js"></script> 
</head>
<script type="text/javascript">
<% aspoutErrorMsg(); %>
//<% getFlashMtdSize("xmlConfig");%>
$(function(){
    $("#error_msg").html(errorstr);	
});

function saveConfig(){
	Device_Config.action="/goform/formExportSettings";
	Device_Config.submit();
}
function saveImport(){
    var fileName;
    var extStart;
    var ext;

    if (Device_Config.importConfig.value=="")
	$("#error_msg").html("<%write(warning1);%>");
    else if(confirm("<%write(warning5);%>")) {
	/*
	try{
	    fileName = $("input:file").get(0).files[0].name;
	    extStart = fileName.lastIndexOf("."); 
	    ext = fileName.substring(extStart,fileName.length).toLowerCase(); 
	    if (ext != ".xml") {
		$("#error_msg").html("文件格式错误");return false;
	    }

	    if (mtdKernelSize < file.size) {
		$("#error_msg").html("文件非法，无法导入");return false;
	    }
	} catch(e) {
	    fileName = $("input[type='file']").get(0).value;
	    extStart = fileName.lastIndexOf("."); 
	    ext = fileName.substring(extStart,fileName.length).toLowerCase(); 
	    if (ext != ".xml") {
		$("#error_msg").html("文件格式错误");return false;
	    }
	}
	*/
	fileName = $("input[type='file']").get(0).value;
	extStart = fileName.lastIndexOf("."); 
	ext = fileName.substring(extStart,fileName.length).toLowerCase(); 
	if (ext != ".xml") {
	    $("#error_msg").html("<%write(fileErr);%>");return false;
	}
	$("#error_msg").html("");
	Device_Config.action = "/goform/formUploadSettings";

	Device_Config.submit();
    }
}
function saveReset(){
	if(confirm("<%write(warning3);%>")) {
		Device_Config.action="/goform/formResetSettings";
		Device_Config.submit();
		/*if(confirm("<%write(RebootNow);%>")) {
		    setTimeout(function(){
			window.location.href="reload_Sui.asp";
		    },500);
		}*/
	}
}


</script>
<body>
<div class="warp">
<br> 
	<form action="/goform/" method="post" name="Device_Config" enctype="multipart/form-data">

	<table border="0" cellpadding="0" cellspacing="1" id="runstatusInfo" class="group" style="background:#5e5e5e;" >
   
         <tr class="tab_title">
        	<td colspan="3"><%write(file2);%></td>
           
        </tr>
        <tr class="tab_info">
        	<td width="220px"><%write(file3);%></td>
            <td colspan="2" width="380px"><input name="saveconfig" type="button" size="18" value="<%write(com_save);%>" class="input-button" onClick="saveConfig()" id="save"/></td>
        </tr>
        <tr class="tab_title">
        	<td colspan="3"><%write(restorFile);%></td>
           
        </tr>
        <tr class="tab_info">
        	<td><%write(SelConfFile);%></td>
            <td colspan="2" ><input name="importConfig" type="file" />&nbsp;&nbsp;&nbsp;&nbsp;<input name="saveimport" type="button" class="input-button"  value="<%write(Import);%>" onClick="saveImport()" id="import"/></td>
        </tr>
        <tr class="tab_info">
        	<td><%write(routerImpConf);%></td>
            <td colspan="2"><input name="chkset"  type="checkbox" checked="checked" /></td>
        </tr>
        <tr class="tab_info">
        	<td></td>
            <td colspan="2"><span id="error_msg" style=" color:red;"></span></td>
        </tr>
        <tr class="tab_info">
        	<td><%write(reConfig);%></td>
            <td colspan="2" >
				<input name="savereset" type="button" class="input-button"  value="<%write(com_confirm);%>" onClick="saveReset()" id="recover" />
			</td>
        </tr>
		<tr class="tab_info">
			<td colspan=3 style="text-align: center;">
				<span style="color:red;font-size:13px;"><%write(RebootNow);%></span>
			</td>
		</tr>
        
       
    </table>
    </form>
<br>
<br>
</div>
</body>
</html>
