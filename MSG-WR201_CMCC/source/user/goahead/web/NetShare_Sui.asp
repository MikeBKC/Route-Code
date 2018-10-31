<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<% langTran("common"); %>
<% langTran("FtpServer"); %>
<% langTran("NetShareSui"); %>
<title><% write(nssui_netshare); %></title>
<link href="css/style_new.css" rel="stylesheet" type="text/css" />


</head>
<style type="text/css">
#advoption{ cursor:pointer; font-weight:bold;}
ul{ padding-top:15px;}
ul li{ line-height:30px; height:30px; padding-left:10px;}
#checkbox_span{height:50px; line-height:50px;padding-left:10px;}
#page_span{ height:50px; line-height:50px; padding-left:190px;}
</style>
<body>
<div class="warp"> 
<br>
	<form action="" method="post" name="NetShare_Config">
    <div id="netsharediv">
	<input type="hidden" name="storage_type" id="storage_type"/>
    	<table border="0" cellpadding="0" cellspacing="1" width="600px" id="runstatusInfo" style="background:#5e5e5e;" class="group" >  
    	<tr class="tab_info">
        	<td width="150px"><% write(nssui_netShareEn); %></td>
            <td colspan="2" width="460px"><span class="on-off">&nbsp;<button type="button" class="on"><% write(com_switch); %></button><input type="hidden" name="nershareEn" id="nershareEn" value="" /></span></td>
        </tr>
        <tr class="tab_info" style="word-break: keep-all;white-space:nowrap;">
        	<td><% write(nssui_usedvolume); %></td>
            <td colspan="2"><span id="usage"><% write(nssui_used); %></span>&nbsp;&nbsp;<input type="button" id="scan" class="input-button" value="<% write(nssui_scan); %>" onClick="location.reload()" />&nbsp;&nbsp;<input type="button" id="secpop" class="input-button" value="<% write(nusui_popupDevice); %>" onClick="popStorage()" />&nbsp;&nbsp;&nbsp;</td>
        </tr>
        <tr class="tab_info" id="sd_tr" style="word-break: keep-all;white-space:nowrap;">
        	<td><% write(nssui_usedvolume_sd); %></td>
            <td colspan="2"><span id="sd_usage"><% write(nssui_used); %></span>&nbsp;&nbsp;<input type="button" id="sd_scan" class="input-button" value="<% write(nssui_scan); %>" onClick="location.reload()" />&nbsp;&nbsp;<input type="button" id="sd_secpop" class="input-button" value="<% write(nusui_popupDevice); %>" />&nbsp;&nbsp;&nbsp;</td>
        </tr>
        <tr class="tab_info">
        	<td colspan="3"><span id="advoption"><% write(com_advance); %></span><input type="hidden" value="0" id="adv" />
		<input type="hidden" value="volume0" id="volume_name" name="volume_name"/>
		<input type="hidden" value="" id="ftppwd" name="ftppwd" />
		<input type="hidden" value="" id="ftpconfirmpwd" name="ftpconfirmpwd" />
		<input type="hidden" value="" id="sambaEn" name="sambaEn" />
		<input type="hidden" value="" id="ftpEn" name="ftpEn" />
		<input type="hidden" value="" id="WANEnable" name="WANEnable" />
		<input type="hidden" value="" id="enablePwd" name="enablePwd" />
		<input type="hidden" value="" id="usernameold" name="usernameold" />
		<input type="hidden" value="" id="passwordold" name="passwordold" />
		<input type="hidden" value="" id="oldfilename" name="oldfilename" />
		<input type="hidden" value="" id="olddirpass" name="olddirpass" />
		</td>          
        </tr>
        <tr class="tab_info adv_td" style="display:none;" >
        	<td><% write(nusui_name); %></td>
            <td colspan="2">
            	<input type="text"  class="txt-input" name="resourcename"  id="resourcename" value="" onChange='checkName(this,11,"<% write(nusui_name); %>",0)'/>&nbsp;&nbsp;
            </td>
        </tr>
        <tr class="tab_info adv_td" style="display:none;">
        	<td><%write(nusui_path);%></td>
            <td colspan="2"><input type="text"  class="txt-input" name="resourcepath" readonly id="resourcepath" value="/" onChange='checkName(this,500,"<%write(nusui_path);%>",0)'/>&nbsp;&nbsp;<input type="button" id="choosepath" class="input-button" value="<%write(nusui_setpath);%>" />&nbsp;&nbsp;</td>
        </tr>
        <tr class="tab_info adv_td" style="display:none;">
        	<td><%write(nusui_sambaEn);%></td>
            <td colspan="2"><input type="checkbox"  class="txt-checkbox" checked name="sambaserver"  id="sambaserver" /></td>
        </tr>
		<tr class="tab_info adv_td" style="display:none;">
        	<td><%write(nusui_ftpEn);%></td>
            <td colspan="2"><input type="checkbox"  class="txt-checkbox" checked name="ftpserver"  id="ftpserver" /></td>
        </tr>
        <tr class="tab_info adv_td" style="display:none;" >
        	<td><%write(ftpPort);%></td>
            <td colspan="2">
            	<input type="text" onKeyPress="figureOnly_P(event)" onChange="checkNumber(this,0,65535,'<%write(ftpPort);%>')"  class="txt-input" name="ftpport"  id="ftpport" value="21" style="width:80px;" />&nbsp;&nbsp;
            </td>
        </tr>
        <tr class="tab_info adv_td" style="display:none;">
        	<td><%write(nusui_allowWan);%></td>
            <td colspan="2"><input type="checkbox"  class="txt-checkbox" name="internet"  id="internet"  /></td>
        </tr>
        <tr class="tab_info adv_td" style="display:none;">
        	<td><%write(nusui_needpass);%></td>
            <td colspan="2"><input type="checkbox"  class="txt-checkbox" name="pwdaccess"  id="pwdaccess"  /></td>
        </tr>
        <tr class="tab_info adv_td pwd_td" style="display:none;">
        	<td><%write(com_user);%></td>
            <td colspan="2"><input type="text"  class="txt-input" name="username"  id="username"  /></td>
        </tr>
        <tr class="tab_info adv_td pwd_td" style="display:none;">
        	<td><%write(com_passwd);%></td>
            <td colspan="2" ><span id="pwd_td"><input type="password"  class="txt-input" name="pwd"  id="pwd"  /></span>&nbsp;&nbsp;<%write(com_dispass);%>&nbsp;&nbsp;<input type="checkbox" id="showpwdbox" /></td>
        </tr>
        <tr class="tab_info adv_td pwd_td" style="display:none;">
        	<td><%write(com_confirmpass);%></td>
            <td colspan="2" id="confirmpwd_td"><input type="password"  class="txt-input" name="confirmpwd"  id="confirmpwd"  /></td>
        </tr>
     	 <tr class="tab_savetd_tip"  valign="top"> 
        	<td colspan="3" align="left" style="padding-left:10px; padding-top:4px;">
            	<table cellpadding="0" cellspacing="0" border="0" width="580">
                	<tr valign="top" >
                    	<td colspan="3" align="left"><span id="tips"><%write(nusui_access);%><span id="lanip">192.168.1.1</span>\<span id="uttname">uttShare</span><%write(com_or);%>ftp://<span id="lanip1">192.168.1.1:21</span>（<%write(nusui_external);%>\\<span id="wanip">200.200.200.150</span>\<span id="uttname1">uttShare</span><%write(com_or);%>ftp://<span id="wanip1">200.200.200.150:21</span>)</span></td>
                    </tr>
                	<tr height="45px">
                   
                        <td align="center" width="400px;"><span id="error_msg"  style="color:red;"></span></td>
                        <td align="right"><input type="button" class="btn" name="back" id="savebtn" value="<%write(com_save);%>"/>&nbsp;&nbsp;</td>
                    </tr>
                </table>
           </td>
            
        </tr>

    </table>
    </div>
    <div id="pathdiv" style="display:none;">
    	<table border="0" cellpadding="0" cellspacing="1" width="600px" id="runstatusInfo" style="background:#5e5e5e;" class="group" >
        <tr class="tab_info">
	    <td width="140px"><%write(nusui_curdir);%></td>
            <td colspan="2" width="460px">
            	<span id="currentPath">/Reos</span> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span id="error_msgpath" style="color:red;"></span>
            </td>
        </tr> 
        <tr class="tab_info">
        	<td rowspan="2" width="140px"><%write(nusui_selrecpath);%></td>
            <td colspan="2" width="460px">
            	<a href="#"><span onClick="gobackDir();" id="updirs">&nbsp;&nbsp;&nbsp;<%write(nusui_parentdir);%></span></a>
            	<a href="#"><span onClick="gorootDir();" id="rootdirs">&nbsp;&nbsp;&nbsp;<%write(nusui_goroot);%></span></a>
            </td>
        </tr> 
    	<tr class="tab_info">
        	
            <td colspan="2" width="460px"><span id="filedir">

                </span>
                
              
                 <span id="checkbox_span"><input type="checkbox" name="checkall" id="checkall" onClick="allcheck();"/>&nbsp;&nbsp;<%write(com_checkall);%></a></span>
                 <span id="page_span"><%write(com_at);%><span id="selectpage"></span><%write(com_page);%>&nbsp;&nbsp;
                 <input type="button"  class="input-button" name="back" id="savebtn" onClick="ok();"value="<%write(com_confirm);%>"/>&nbsp;&nbsp;</span>
                 
            </td>
        </tr> 


    </table>
    </div>
	</form>
 </div>

 <br><br>
 <script type="text/javascript" src="script/jquery-1.8.1.min.js"></script>  
<script type="text/javascript" src="js/common.js"></script> 
<script type="text/javascript" src="script/frame.js"></script> 
<script type="text/javascript">
var error = 0;
var exist = 1;
var flag = 0;
var usb ="0";//1：插入USB；0：未插入USB
var useage="10M";
var totalage="100M";
var enabledShare = "off";
var sambaservers ="0";
var ftpservers = "0";
var ftpports ="21";
var allowInternet ="0";
var allowPwd ="0";
var usernames="admin";
var passwords="admin";
var usernameolds="admin";
var passwordolds="admin";
var resourcenames = "uttShare";
var resourcespaths ="/";
<%aspoutDynConfig();%>
<% aspOutputNetShare_Sui(); %>
$(function(){
	function init(){
		var checkalldirs = "0";//目录是否全选 1:全选
		var showpwds="0";//显示密码
		
		var f = NetShare_Config;
		var en = enabledShare=="on"?"1":"0";
		$("#nershareEn").val(en);
		var switch_b = $(".on-off").children("button");
		initEnabledButton(switch_b,enabledShare);
		parent.art.dialog.list["func_box"].size(700,$("#netsharediv").height()+70);
		if(usb==0){
			$("#usage").html("<%write(nusui_insertusb);%>");
		}else{
			$("#usage").html("<%write(nusui_usedvol);%>"+useage+" <%write(nusui_suiplus);%>"+totalage);	
		}
		if (sd_surpport==1) {
			$("#sd_tr").show();
		} else {
			$("#sd_tr").hide();
		}
		if(sd==0){
			$("#sd_usage").html("<%write(nusui_insertsd);%>");
		}else{
			$("#sd_usage").html("<%write(nusui_usedvol);%>"+sd_useage+" <%write(nusui_suiplus);%>"+sd_totalage);	
		}
		$("#resourcename").val(resourcenames);
		f.oldfilename.value = resourcenames;
		$("#resourcepath").val(resourcespaths);
		f.olddirpass.value = resourcespaths;
		checkboxfun($("#sambaserver"),sambaservers);
		checkboxfun($("#ftpserver"),ftpservers);
		$("#ftpport").val(ftpports);
		checkboxfun($("#internet"),allowInternet);
		checkboxfun($("#pwdaccess"),allowPwd);
		$("#username").val(usernames);	
		$("#pwd").val(passwords);
		$("#confirmpwd").val(passwords);
		checkboxfun($("#showpwdbox"),showpwds);
		showpassword();
		$("#currentPath").html(resourcespaths);
		
		checkboxfun($("#checkall"),checkalldirs);
		f.usernameold.value=usernameolds;
		f.passwordold.value=passwordolds;
		$("#error_msg").html(errorstr);
		var lan_ip="<%getLanIp();%>";
		$("#lanip").html(lan_ip);
		$("#lanip1").html(lan_ip + ":" + ftpports);
		var internet_ip="<%getWanIp(1);%>";
		if (internet_ip == "")
		{
		    internet_ip = "<%write(nusui_exteraddr);%>";
		}
		$("#wanip").html(internet_ip);
		$("#wanip1").html(internet_ip + ":" + ftpports);
		$("#uttname").html(resourcenames);
		$("#uttname1").html(resourcenames);
		if (lang_en == 1)
		{
		    $("#secpop").css({"width":"115px"});
			$("#sd_secpop").css({"width":"115px"});
		    $("#choosepath").css({"width":"100px"});
		}
		showSuccessTips(rightstr);
	}
	init();
	function checkboxfun(obj,val){
		if(val=="1"){
			obj.attr("checked",true);	
		}else{
			obj.attr("checked",false);	
		}
	}
	function showpassword(){
		var p = $("#pwd").val();
		if($("#showpwdbox").attr("checked")){
			
			$("#pwd_td").html('<input type="text"  class="txt-input" name="pwd"  id="pwd" value="'+p+'" />');
			$("#confirmpwd_td").html('<input type="text"  class="txt-input" name="confirmpwd"  id="confirmpwd" value="'+p+'" />');	
		}else{
			$("#pwd_td").html('<input type="password"  class="txt-input" name="pwd"  id="pwd" value="'+p+'" />');
			$("#confirmpwd_td").html('<input type="password"  class="txt-input" name="confirmpwd"  id="confirmpwd" value="'+p+'" />');		
		}	
	}
	$("#showpwdbox").click(function(){
		showpassword();	
	});
	
	//高级选项
	$("#advoption").click(function(){
		 
			var adv = $("#adv").val();
			if(adv==0){
				$(".adv_td").show();
				$("#adv").val("1");	
				showUser();				
				
			}else{
				$(".adv_td").hide();
				$("#adv").val("0");		
			}
				
			parent.art.dialog.list["func_box"].size(700,$("#netsharediv").height()+70);	
	});
	//允许密码访问
	$("#pwdaccess").click(function(){
			if($("#internet").attr("checked")){
					$("#pwdaccess").attr("checked",true);
				}
			showUser();	
	});
	//显示和隐藏用户名 密码
	function showUser(){
		var temp = $("#pwdaccess").attr("checked");
	
		if(temp){
			$(".pwd_td").show();	
		}else{
			$(".pwd_td").hide();	
		}
		parent.art.dialog.list["func_box"].size(700,$("#netsharediv").height()+70);	
	} 
	//点击选择路径的方法
	$("#choosepath").click(function(){
			$("#netsharediv").hide();
			$("#pathdiv").show();
			getAllDir($("#resourcepath").val());
			parent.art.dialog.list["func_box"].size(700,$("#pathdiv").height()+70);
			
	});
	//点击允许外网访问的事件
	$("#internet").click(function(){
			allowInternetbtn();
	});
	//开启按钮事件
	$(".on-off").click(function(){
	    var switch_b = $(".on-off").children("button");
	    var status_now = switch_b.attr("class");
	    if(status_now=="on"){
		$("#nershareEn").val("0");	
	    }else{
		$("#nershareEn").val("1");	
	    }
	    swicth_act(status_now);
    });
	//保存按钮事件
	$("#savebtn").click(function(){
		if(checkData()){
			NetShare_Config.action="/goform/formNetShare_Sui";
			NetShare_Config.submit();	
		}	
	});
	//允许外网访问 必须使用密码访问
	function allowInternetbtn(){
		var temp = $("#internet").attr("checked");
		if(temp){
			$("#pwdaccess").attr("checked",true);
			showUser();	
		}	
	}
	//验证数据合法性
	function checkData(){
		var f = NetShare_Config;
		//var a = re_checkName(f.resourcename,31,"共享资源名称",0);
		f.ftppwd.value = $("#pwd").val();
		f.ftpconfirmpwd.value = $("#confirmpwd").val();
		if($("#sambaserver").attr("checked")) {
		    f.sambaEn.value="on";
		}else {
		    f.sambaEn.value="off";
		}
		if($("#ftpserver").attr("checked")) {
		    f.ftpEn.value="on";
		}else {
		    f.ftpEn.value="off";
		}
		if($("#internet").attr("checked")){
		    f.WANEnable.value="on";
		}else{
		    f.WANEnable.value="off";
		}
		if($("#pwdaccess").attr("checked")){
		    f.enablePwd.value="on";
		}else{
		    f.enablePwd.value="off";
		}
	
		if(!checkName_P(f.resourcename,31,"<%write(nusui_name);%>",0)) return false;
		if(!checkNameAllowSpace_P(f.resourcepath,500,"<%write(nusui_path);%>",0)) return false;
		var status = $("#nershareEn").val();
		if(status=="1"){
			if(!$("#sambaserver").attr("checked") && !$("#ftpserver").attr("checked")){
				$("#error_msg").html("<%write(nusui_needonefun);%>");
				return false;	
			}	
		}
		if($("#pwdaccess").attr("checked")){
			if ((f.username.value == "admin") || (f.username.value == "guest"))
			{
			    $("#error_msg").html("<%write(nusui_invalidname);%>");
			    return false;
			}
			if(!checkName_P(f.username,32,"<%write(com_user);%>",0)) return false;
			if(!checkPassword1_P(f.ftppwd,"31",0,1)) return false;
			if(!checkPassword1_P(f.ftpconfirmpwd,"31",0,1)) return false;
			if(f.ftppwd.value != f.ftpconfirmpwd.value)
			{
			    $("#error_msg").html("<%write(nusui_passerror);%>");
			    return false;
			}

		}
		if($("#ftpport").val()<1 || $("#ftpport").val()>65535)
		{
		    $("#error_msg").html("<%write(nusui_porterror);%>");
		    return false;
		}
		return true;	
	}
	
})
	var directors = new Array();
	//*********************选择路径div中的js**********************************
	function getAllDir(path){
	    flag = 1;
	    var ajaxObj=ajax();
	    if(ajaxObj)//初始化成功
	    {
		ajaxObj.url="/goform/formFtpServerLocalDirSui?path="+$("#currentPath").html();
		ajaxObj.refreshTime=2000;
		ajaxObj.processHandle=function (str)
		{
		    eval(str);
		    if (error == 1)
		    {
			$("#error_msgpath").html("<%write(nusui_invaliddir);%>");
			error = 0;
		    }		
		    else
		    {
			directors = filedirectories;
			goPage(1,10,filedirectories);
			$("#checkall").attr("checked", false);
			allcheck();
		    }
		    flag = 0;
		}
	    }else
	    {
		flag = 0;
	    }
	    
	    ajaxObj.start();
/*
		$.ajax({
				//url:"/goform/form?currentpath="+path,
				url:"/goform/formFtpServerLocalDirSui",
				dataType:"json",
				cache: false,
				success:function(data){
				    directors = filedirectories;
				    goPage(1,10,filedirectories);
				},
				error:function(){
					
				}
			});*/
/*
var data = {"filedirectories" : ["演讲","ReOS1","工作","NetFilter","演讲2","ReOS2","工作4","NetFilter3","演讲3","ReOS3","工作5","NetFilter4"]};
		var s = data.filedirectories;
*/
		
			
	}
	
/**
 * 分页函数
 * pno--页数
 * psize--每页显示记录数
 * 分页部分是从真实数据行开始，因而存在加减某个常数，以确定真正的记录数
 * 纯js分页实质是数据行全部加载，通过是否显示属性完成分页功能
 **/
function goPage(pno,psize,dirs){
	var temp =dirs;
	if(dirs=='undefined' || dirs==null){
		temp = directors;	
	}
	var num = temp.length;//总记录
	var totalPage = 0;//总页数
	var pageSize = psize;//每页显示行数
	if((num-1)/pageSize > parseInt((num-1)/pageSize)){   
   		totalPage=parseInt((num-1)/pageSize)+1;   
   	}else{   
   		totalPage=parseInt((num-1)/pageSize);   
   	}   
	var currentPage = pno;//当前页数	
	var startRow = (currentPage - 1) * pageSize+1;//开始显示的行   
   	var endRow = currentPage * pageSize+1;//结束显示的行   
   	endRow = (endRow > num)? num : endRow;
	writefile(startRow,temp);
	writePage(totalPage,temp);
	$("#filenamepage").val(currentPage);	
	parent.art.dialog.list["func_box"].size(700,$("#pathdiv").height()+70);	
}
//选中全选 不能再选择radio按钮
function allcheck(){
	if($("#checkall").attr("checked"))	{	
		$("input[type=radio]").attr("disabled",true);			
	}else{
		
		$("input[type=radio]").attr("disabled",false);		
	}	
}
//显示目录，每页显示10个
function writefile(pno,dirs){
	//if(dirs.length!=0){
		var html_str = "<ul>";
			for(var i=pno;i<=dirs.length;i++){
				html_str += '<li><a><input type="radio" name="filename" value="'+dirs[i-1]+'" />&nbsp;&nbsp;<span onClick="linkdir('+(i-1)+')">'+dirs[i-1]+'</span></a></li>';
				if(i%10==0)break;	
			}
		html_str +="</ul>";	
		$("#filedir").html(html_str);
	//}
	allcheck();	
}
//翻页的下拉框
function writePage(totalPage,dirs){
	var sel = '<select id="filenamepage" onchange="goPage(this.value,10,null);">';
	for(var i=1;i<=totalPage;i++){
		sel+='<option value='+i+'>'+i+'</option>';	
	}
	if (totalPage == 0)
	{
		sel+='<option value="1">1</option>';	
	}
	sel +='</select>';
	$("#selectpage").html(sel);
}
//点击每个目录，显示该目录的子目录
function linkdir(index){
	if (flag == 1) return;
	$("#error_msgpath").empty();
	if(directors.length==0)return false;
	var filename = 	directors[index];
	//var path = $("#resourcepath").val();
	var path = $("#currentPath").html();//当前目录
	var currentpath = "";
	var reg =/^\/$/;
	var result = reg.test(path);
	if(result){
		currentpath = path+filename;	
	}else{
		currentpath = path+"/"+filename;	
	}
	
	$("#currentPath").html(currentpath);
	
	getAllDir(currentpath);
}
//上一级目录的方法
function gobackDir(){
	if (flag == 1) return;
	$("#error_msgpath").empty();
	var path = $("#currentPath").html();//当前目录
	//获得上一级目录
	var reg =/^\/$/;
	var result = reg.test(path);
	if(	result){
		if (exist == 0)
		{
		    $("#error_msgpath").html("<%write(nusui_invaliddir);%>");
		}
		return;
	}
	var backdir = path.substr(0,path.lastIndexOf('/'));
	if(backdir==""){
		backdir="/";
	}
	$("#currentPath").html(backdir);
	$("#resourcepath").val(backdir);
	getAllDir(backdir);
	
}
//返回根目录的方法
function gorootDir(){
	if (flag == 1) return;
	$("#error_msgpath").empty();
	var backdir="/";
	$("#currentPath").html(backdir);
	$("#resourcepath").val(backdir);
	getAllDir(backdir);
	
}
//点击确定按钮
function ok(){
	$("#error_msgpath").empty();
	var val =  $('input[type="radio"]:checked').val();
	if(!val && !$("#checkall").attr("checked")){
		$("#error_msgpath").html("<%write(nusui_notchoosedir);%>");
		return ;	
	}
	if($("#checkall").attr("checked")){
		$("#resourcepath").val($("#currentPath").html());
		
	}else{
		if(val){
		var reg =/^\/$/;
		var path =$("#currentPath").html();
		var result = reg.test(path);
		if(result){
			path =$("#currentPath").html()+val;	
		}else{
			path =$("#currentPath").html()+"/"+val;		
		}
		 $("#resourcepath").val(path);	
		}	
	}
	
		$("#netsharediv").show();
			$("#pathdiv").hide();			
			parent.art.dialog.list["func_box"].size(700,$("#netsharediv").height()+70);
	
}
function popStorage()
{
	$("#storage_type").val("usb");
	NetShare_Config.action="/goform/formStoragePop";
	NetShare_Config.submit();
}
$(function(){
	$("#sd_secpop").click(function(){
	    $("#storage_type").val("sd");
	    NetShare_Config.action="/goform/formStoragePop";
	    NetShare_Config.submit();
	});
})
  </script>
</body>
</html>
