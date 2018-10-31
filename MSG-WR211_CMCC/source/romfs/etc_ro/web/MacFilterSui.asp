<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<% langTran("common"); %>
<% langTran("WlanMacFilter");%>
<% langTran("MacFilter");%>
<% langTran("MacFilterSui");%>
<% langTran("PdbUpdata");%>
<title><% write(com_utt); %></title>
<link href="css/style_new.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="script/jquery-1.8.1.min.js"></script> 
<script type="text/javascript" src="js/common.js"></script> 
<script type="text/javascript" src="script/frame.js"></script> 
<script type="text/javascript" src="script/jquery.artDialog.js" ></script>
<script type="text/javascript" src="script/jquery.selectbox.js" ></script>
</head>
<script type="text/javascript">
<% getArpFromArptable(); %>
<% getCurrentIPMacInfo(); %>
<% 
 outputMacFilterConfig();
%>
 

listheadname="<%write(mac_filtList);%>";
var titles=new Array();
titles[0]="ID";
titles[1]="<%write(PName);%>";
titles[2]="<%write(com_MAC);%>";
 
var arrayData=new Array(usernames,filterMacs);
function init(){
	var filterMacArr = i_macs.concat();
	if(filterMacArr.searchData(currentMac) == -1){
	    if (currentMac != ""){
		filterMacArr.push(currentMac);
	    }
	}
	$("#filterMacDiv").selectbox(filterMacArr,"");
	$("#error_msg").html(errorstr);	
	var switch_b = $(".on-off").children("button");
	var status = (MacFilterEnables=="0"?"off":"on");
	initEnabledButton(switch_b,status);
	var f = MacFilterForm;
	$("#MacFilterEnable").val(status);
	if(filterRules=="0") f.filterRule[0].checked=true;
	else f.filterRule[1].checked=true;
    $("#updatebtn").addClass("modifybtn");
	if (currentMac == ""){                                                 
         $("#current_mac").html("<%write(com_currentWarn1);%>");    
        }else{                                                                  
          $("#current_mac").html("<%write(com_currentWarn2);%>"+currentMac);                     
	}
	showSuccessTips(rightstr);
}
function EditConfig(i){
		var f = MacFilterForm;
		$("#updatebtn").attr('disabled',false);
        $("#updatebtn").removeClass("modifybtn");
	    f.oldName.value=usernames[i];
	    f.oldMac.value=filterMacs[i];
		f.username.value=usernames[i];
		f.filterMac.value=filterMacs[i];
		var filterMacArr = i_macs.concat();
		var mac = filterMacs[i].replace(/:/g,"");
		if(filterMacArr.searchData(mac) == -1){
			filterMacArr.push(mac);
		}
		$("#filterMacDiv").empty();
		$("#filterMacDiv").selectbox(filterMacArr,mac);
		
}
function glsave(){
	MacFilterForm.action="/goform/ConfigMacFilterGlobalConfig";
	MacFilterForm.submit();
}
function checkData(){
	var f = MacFilterForm;
	if(!checkName_P(f.username,11,"<%write(PName);%>",0)) return false;
	if(!checkMac_P(f.filterMac,"<%write(com_MAC);%>",0)) return false;
    return true;
}
function save(){
	    var f = MacFilterForm;
		f.filterMac.value = $("#filterMacDiv").selectboxval();
		if(checkData()){
			f.action="/goform/ConfigMacFilter";
	        f.submit();	
		}
	
	}
function add(){
	 MacFilterForm.Action.value="add";
  	 save();	
}
function update(){
	if(MacFilterForm.oldName.value!="")
	{
	    MacFilterForm.Action.value="modify";
  	    save();	
	}
}
function DeleteConfig(index){
	if(confirm("<% write(com_deleteTip); %>")){
		MacFilterForm.delstr.value=usernames[index];
		MacFilterForm.action="/goform/ConfigMacFilterDel";
	    MacFilterForm.submit();
	}
}
function delAll(){
	if(confirm("<% write(com_deleteAllTip); %>")){
		MacFilterForm.action="/goform/ConfigMacFilterDelAll";
		MacFilterForm.submit();
	}
}
$(function(){
	init();
	
	$(".on-off").click(function(){
		var switch_b = $(".on-off").children("button");
		var status_now = switch_b.attr("class");
		if(status_now=="on"){
			$("#MacFilterEnable").val("off");	
		}else{
			$("#MacFilterEnable").val("on");	
		}
		swicth_act(status_now);
	});
	})
</script>
<body>
<div class="warp">

<br>

	<form action="" method="post" name="MacFilterForm">
	<table border="0" cellpadding="0" cellspacing="1" style="background:#5e5e5e;" class="group" >
   		
   		 <tr class="tab_info">
        	<td width="140px"><%write(mac_openFilt);%></td>
            <td width="360px"  colspan="2">
            	<span class="on-off">&nbsp;<button type="button" class="on"><%write(com_switch);%></button><input type="hidden" name="MacFilterEnable" id="MacFilterEnable" value="off" /></span>
            </td>
           
        </tr>
        <tr class="tab_info">
        	<td rowspan="2"><%write(FilterRule);%></td>
            <td colspan="2">
            	<input type="radio" value="0" name="filterRule"  checked="checked" /> <font style="color:#666666"><%write(mac_whiteList);%></font>
            </td>
           
        </tr>
       	 <tr class="tab_info">
        	
            <td colspan="2">
            		<input type="radio"  value="1" name="filterRule" />  <font style=" color:#666666"><%write(mac_blackList);%></font>
            </td>
           
        </tr>
       	  <tr  height="60px" class="tab_savetd"> 
        	<td colspan="3" align="center">
            	<table cellpadding="0" cellspacing="0" border="0" width="550">
                	<tr>
                    	<!--<td width="70px">&nbsp;</td>-->
                        <td align="center" colspan="2" ></td>
                        <td align="right" width="200px"><input type="button" class="btn" name="back" onclick="glsave();"  value="<%write(com_save);%>"/>
                    </tr>
                </table>
           </td>
            
        </tr> 
      	<tr class="tab_title">
        	<td colspan="3"><%write(mac_setFilt);%><input type="hidden" name="oldName" value=""><input type="hidden" name="Action"><input type="hidden" name="oldMac"></td>           
        </tr>
         <tr class="tab_info">
        	<td><%write(PName);%></td>
            <td colspan="2">
            		<input name="username" type="text" size="18" maxlength="11" onChange="validateName(this,11,'<%write(PName);%>',0);" class="txt-input" />
            </td>
           
        </tr>
        <tr class="tab_info">
        	<td><%write(com_MAC);%></td>
            <td colspan="2">
	    <table>
             <td> <div id="filterMacDiv" style="position:relative;"></div></td>
            <td> <input type="hidden" name="filterMac" id="filterMac"></td>
	    <td><span id="current_mac"></span></td>
	    </table>
            </td>
        </tr>
        
         <tr  height="60px" class="tab_savetd"> 
        	<td colspan="3" align="center">
            	<table cellpadding="0" cellspacing="0" border="0" width="550">
                	<tr>
                    	<!--<td width="70px">&nbsp;</td>-->
                        <td align="center" colspan="2" ><span id="error_msg"  style="color:red;"></span></td>
                        <td align="right" width="250px"><input type="button" class="btn" name="back" onclick="add();"  value="<%write(com_save);%>"/>&nbsp;&nbsp;&nbsp;&nbsp;<input type="button" id="updatebtn" class="btn"  name="back" onclick="update();"  value="<%write(com_modify);%>" disabled/></td>
                    </tr>
                </table>
           </td>
            
        </tr>
    </table>
    <script type="text/javascript">document.write(dspListTab(listheadname,titles,titles.length,arrayData,1));</script>
   <table border="0" cellpadding="0" cellspacing="1" style="background:#5e5e5e;" class="group" >
   		<tr><td  class="tab_savetd" align="right"><span><input type="button" class="btn" onclick="delAll();" value="<%write(com_delAll);%>"/><input type="hidden" name="delstr"></span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td></tr>
       
   </table>
</form>
<br>
<br>
</div>
</body>
</html>
