<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<%langTran("common");%>
<%langTran("GroupSui");%>
<%langTran("Group");%>
<%langTran("PdbUpdata");%>
<%langTran("Menu");%>
<%langTran("commonWords");%>
<title><% aspGetTitle(); %></title>
<link href="css/style_new.css" rel="stylesheet" type="text/css" />
<link href="css/default.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="script/jquery-1.8.1.min.js"></script> 
<script type="text/javascript" src="js/common.js"></script> 
<script type="text/javascript" src="script/frame.js"></script> 
<script type="text/javascript" src="script/jquery.artDialog.js"></script> 
<script type="text/javascript" src="script/jquery.selectbox.js" ></script>
</head>
<script type="text/javascript">
var RateArr = new Array("64K", "128K", "256K", "512K", "1M", "1.5M", "2M", "3M", "4M", "5M", "6M", "7M", "8M", "9M", "10M", "11M", "12M", "13M", "14M", "15M", "16M", "17M", "18M", "19M", "20M", "25M", "30M", "35M", "40M", "45M", "50M", "90M", "100M");
var RateArrValue = new Array("64", "128", "256", "512", "1000", "1500", "2000", "3000", "4000", "5000", "6000", "7000", "8000", "9000", "10000", "11000", "12000", "13000", "14000", "15000", "16000", "17000", "18000", "19000", "20000", "25000", "30000", "35000", "40000", "40000", "50000", "90000", "100000");
var macadds = new Array();
var policy1=new Array();

<% getArpFromArptable(); %>
<% getCurrentIPMacInfo(); %>
<% aspoutDynConfig();%>
<%coefficient();%>
<% aspOutGroupConfigList_Sui();%>
/*
var totalrecs=1;
GroupNames[0] = "12";
sourceIPs[0] = "1";
FromIPs[0] = "192.168.16.1";
EndIPs[0] = "192.168.16.11";
 policy[0] = "2"; 
 uRate[0] = "6000 "; 
 dRate[0] = "6000 "; 
 macadds[0]="00:12:d2:33";
 
GroupNames[1] = "13";
sourceIPs[1] = "1";
FromIPs[1] = "";
EndIPs[1] = "";
 policy[1] = "2"; 
 uRate[1] = "6 kbit/s"; 
 dRate[1] = "6 kbit/s"; 
macadds[1]="00:12:d2:33"; 

 var totalrecs=2;
 var errorstr="";
 var rightstr="";
*/
var coefficient = 11;
 
var listheadname="<%write(grou_setBW);%>";
var titles = new Array();
var totalTitel = 6;
titles[0] = "ID";
titles[1] ="<%write(PName);%>";
//titles[2] ="< write(com_IP); >";
titles[2] ="<%write(confine_name);%>";
titles[3] ="<%write(grou_limUp);%>";
titles[4] ="<%write(grou_limDown);%>";
titles[5] ="<%write(com_policy);%>";
var LimitObj=new Array();
var uRatew=new Array();
var dRatew=new Array();
var poly;

for(i=0;i<totalrecs;i++){
	if(sourceIPs[i] == 0)
	{
	    LimitObj[i]=FromIPs[i]+"-"+EndIPs[i];
	}
	else
	{
	    LimitObj[i] = macadds[i]; 
	}
		//policy1[i]=(policy[i]=="1"?"<%write(gro_independ);%>":"<%write(gro_share);%>");
	if(policy[i]=="1"){poly="<%write(gro_independ);%>"}
	else if(policy[i]=="2"){poly="<%write(gro_share);%>"}
	else{poly="<%write(mac_rate_limit);%>"}
	policy1[i]=poly;
	
	uRatew[i]=uRate[i]+" kbit/s";
	dRatew[i]=dRate[i]+" kbit/s";
}
var arrayData = new Array(GroupNames,LimitObj,uRatew,dRatew,policy1);

function EditConfig(i){
	var f = Bandwidth_Config;
	
	if((policy[i] !=1) && (policy[i] !=2)){
			var selOpt = $("#policy option");
			selOpt.remove();
			$("#policy").append("<option value='3'><%write(mac_rate_limit);%></option>");
		}else{
			var selOpt = $("#policy option");
			selOpt.remove();
			$("#policy").append("<option value='1'><%write(gro_inContent);%></option>");
			$("#policy").append("<option value='2'><%write(gro_shContent);%></option>");
		}

	$("#updatebtn").attr('disabled',false);
	$("#updatebtn").removeClass("modifybtn");
		f.GroupName.value = GroupNames[i];
		f.GroupNameOld.value = GroupNames[i];
		//var n = sourceIPs[i];
		//f.sourceIP[n].checked = true;
		if(sourceIPs[i] == 0)
        {	
			if($("#ipDis").hide())
			{
				$("#macDis").hide();
				$("#ipDis").show();
			}
			f.sourceIP[0].checked=true;
			//f.FromIP.disabled =false;
			//f.EndIP.disabled=false;		
			f.FromIP.value=FromIPs[i];
			f.EndIP.value=EndIPs[i];
			//f.macaddress.disabled=true;
			selObject(f,"iprange");						
        }
   	    else
    	{
			if($("#macDis").hide())
			{
				$("#ipDis").hide();
				$("#macDis").show();
			}
			f.sourceIP[1].checked = true;
			f.writeMacAddress.value=macadds[i];
			//f.macaddress.value=macadds[i];
			
			var MacArr =  i_macs.concat();
			var mac = macadds[i].replace(/:/g,"");
			if(MacArr.searchData(mac) == -1){
				MacArr.push(mac);
			}
			$("#selMacAddress").empty();
			$("#selMacAddress").selectbox(MacArr,mac);
			
			selObject(f,"macaddress");
    	}	
		//alert("policy["+i+"]:"+policy[i]);
		
		f.policy.value=policy[i];
		
		//alert("f.policy.value:"+f.policy.value);
		f.MaxDownRatetxt.value=dRate[i];
		f.uMaxDownRatetxt.value=uRate[i];
		
		changeSelect(f.MaxDownRatetxt, f.MaxDownRatew);

		changeSelect(f.uMaxDownRatetxt, f.uMaxDownRatew);
	
//	}
	
}


function DeleteConfig(index){
/*
	art.dialog({icon:"warning",id:"wating_box",title:false,content:'确定删除？&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;',
				ok:function(){
					Bandwidth_Config.delstr.value=GroupNames[index];
					Bandwidth_Config.action="/goform/formGroupConfigDel";
	   				Bandwidth_Config.submit();
					this.close();
				},
				
				okVal: "确定", 
				cancel: function(){
					
			        this.close();
			        return false;
			    }
	}).lock();

*/
	if(confirm("<%write(com_deleteTip);%>")){
	    Bandwidth_Config.delstr.value=GroupNames[index];
	    Bandwidth_Config.action="/goform/formGroupConfigDel";
	    Bandwidth_Config.submit();
	}
}
function delAll(){
/*
	art.dialog({icon:"warning",id:"wating_box",title:false,content:'确定删除所有的条目？&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;',
				ok:function(){
					Bandwidth_Config.action="/goform/formGroupConfigDelAll";
					Bandwidth_Config.submit();
				},
				
				okVal: "确定", 
				cancel: function(){
					
			        this.close();
			        return false;
			    }
	}).lock();
*/
	
	if(confirm("<%write(com_deleteAllTip);%>")){
		Bandwidth_Config.action="/goform/formGroupConfigDelAll";
		Bandwidth_Config.submit();
	}
}

function selObject(f,val)
{	
	if(val=="iprange")
	{
		
		f.FromIP.disabled=false;
		f.EndIP.disabled=false;
		//f.macaddress.disabled=true;
		f.writeMacAddress.disabled=true;
	}
	if(val=="macaddress")
	{
		f.FromIP.disabled=true;
		f.EndIP.disabled=true;
		//f.macaddress.disabled=false;
		f.writeMacAddress.disabled=false;
	}
}   
function changeSelect(ctrl1, ctrl2)
{	

	if (ctrl1.value == "")
	{
		var selectIndex = 0;
	} else {
		var selectIndex = getSelIndexByValue(ctrl2, ctrl1.value);
	}

	if (selectIndex != -1)
	{
		ctrl2.selectedIndex = selectIndex;
	} else {
		ctrl2.selectedIndex = 1;
	}
}   
function changeRate(ctl,ctl2){	
	var selValue = getListValue(ctl);
	selValue=(selValue=="zidingyi"?"":selValue);
	ctl2.value = selValue;	
}

function changePolicy(){
	var macVal=$("#policy option:selected").val();
	if(macVal==3){
		$("#ipDis").hide();
		$("#macDis").show();
	}else{
		$("#ipDis").show();
		$("#macDis").hide();
	}
}

function checkData(){
	
	var f = Bandwidth_Config;
	if(!checkName_P(f.GroupName,12,"<%write(com_group);%>",0)) return false;
	
        //if (f.FromIP.value=="0.0.0.0"){ alert("起始IP地址不能为0.0.0.0");return false};
	var macVal=$("#policy option:selected").val();
	if(macVal!=3){
		/*if (f.sourceIP[0].checked == true)
		{
			if(!checkIP_P(f.FromIP,"<%write(com_startIP);%>",0)) return false;
			if(!checkIP_P(f.EndIP,"<%write(com_endIP);%>",0)) return false;
		}*/
		if (f.FromIP.value=="0.0.0.0")
		{
			$("#error_msg").html("<%write(The_startingIP_address);%>");
			f.FromIP.focus();
			return false;
		}
		if (f.EndIP.value=="0.0.0.0")
		{
			$("#error_msg").html("<%write(The_endIP_address);%>");
			f.FromIP.focus();
			return false;
		}

		if(!checkIP_P(f.FromIP,"<%write(com_startIP);%>",0)) return false;
		if(!checkIP_P(f.EndIP,"<%write(com_endIP);%>",0)) return false;
		
		if (compareIP(f.FromIP.value,f.EndIP.value)==1)
		{
			$("#error_msg").html("<%write(starting_address_than_end);%>");
			return false;
		}  
	}
	//验证输入的MAC地址
	else{
		/*if (f.sourceIP[1].checked == true)
		{
			if(!checkMac_P(f.macaddress,"<%write(com_MAC);%>",0)) return false;
			f.writeMacAddress.value = $("#selMacAddress").selectboxval();
			if(!checkMac_P(f.writeMacAddress,"MAC地址",0)) return false;
		}*/
		f.writeMacAddress.value = $("#selMacAddress").selectboxval();
		if(!checkMac_P(f.writeMacAddress,"<%write(MACAddress);%>",0)) return false;
	}
	
	

	if(f.MaxDownRatetxt.value!="0")
	    if(!checkNumber_P(f.MaxDownRatetxt,coefficient,102400,"<%write(grou_bandLimDown);%>",0)) return false;

	if(f.uMaxDownRatetxt.value!="0")
	    if(!checkNumber_P(f.uMaxDownRatetxt,coefficient,102400,"<%write(grou_bandLimUp);%>",0)) return false;

	var down_rate = parseInt(f.MaxDownRatetxt.value);

	var up_rate = parseInt(f.uMaxDownRatetxt.value);

	return true;
}
function save(value){
	if(checkData()){
	    var f = Bandwidth_Config;
	    if (value == 1)
	    {
		f.Action.value="add";
	    }
	    else
	    {
		f.Action.value="modify";
	    }
		
		f.writeMacAddress.value = $("#selMacAddress").selectboxval();
	    f.dRate.value=f.MaxDownRatetxt.value;
	    f.uRate.value=f.uMaxDownRatetxt.value;
	    f.action="/goform/formGroupConfig_Sui";
	    f.submit();
	}
}
$(function(){
	var MacArr = i_macs.concat();
	if(MacArr.searchData(currentMac) == -1){
	    if (currentMac != ""){
		MacArr.push(currentMac);
	    }
	}
	$("#selMacAddress").selectbox(MacArr,"");
	
	/*var macAddressArr = new Array();
	macAddressArr[0]="00e04ca01108";
	$("#selMacAddress").selectbox(macAddressArr,"");*/
	if(mac_limit_menu==0)//不显示MAC选项
	{
		$("#policy option[value='3']").remove();
		$("#ipDis").show();
		$("#macDis").hide();
	}
	else
	{
		$("#policy").append("<option value='3'><%write(mac_rate_limit);%></option>");
		$("#policy").val(3);
		$("#ipDis").hide();
		$("#macDis").show();
	}
	//$("#policy").val(3);
	//$("#ipDis").hide();
	//$("#macDis").show();
	
	$("#GroupName").val("");
	//selObject(Bandwidth_Config,"ipRange");
	$("#FromIP").val("");
	$("#EndIP").val("");
	$("#macaddress").val("");
	$("#uMaxDownRatetxt").val("");
	$("#MaxDownRatetxt").val("");
	$("#error_msg").html(errorstr);		
	$("#updatebtn").attr('disabled',true);
	$("#updatebtn").addClass("modifybtn");
	var f = Bandwidth_Config;
	f.MaxDownRatetxt.value=0;
	f.uMaxDownRatetxt.value=0;
	showSuccessTips(rightstr);
})             
</script>
<body>
<div class="warp">

<br>
	<form action="" method="post" name="Bandwidth_Config">
    <!-- <div id="error_msg" style="color:red;"></div> -->
	<table border="0" cellpadding="0" cellspacing="1" style="background:#5e5e5e;" class="group" >
   		  <tr class="tab_title">
        	<td colspan="3"><%write(BandSet_t);%></td>
           
        </tr>
    	<tr class="tab_info">
        	<td width="140px"><%write(PName);%></td>
            <td width="460px" colspan="2"><input type="text" onChange="validateName(this,11,'<%write(com_group);%>',0);"class="txt-input"  name="GroupName" style="width:130px;" id="GroupName" value="" /></td>
        </tr>
         <tr class="tab_info">
        	<td><%write(gro_speed);%></td>
            <td colspan="2">
            	<select name="policy" id="policy" class="select" style="width:260px;" onchange="changePolicy()">
	   			<option value="1"><%write(gro_inContent);%></option>
	   			<option value="2"><%write(gro_shContent);%></option>
                
	   			</select>
            </td>
        </tr>
        <tr class="tab_info" id="ipDis">
        	<td><%write(grou_setObj);%></td>
            <td colspan="2"><input type="radio" style="display:none;" name="sourceIP" checked value="iprange" onClick="selObject(this.form,this.value)" />IP&nbsp;<%write(grou_dow);%>&nbsp;<%write(grou_addr);%>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<input type="text" onChange="validateIP(this,'<%write(com_startIP);%>',0);" class="txt-input" name="FromIP" style="width:130px;" id="FromIP"  value="" />&nbsp;&nbsp;-&nbsp;&nbsp;<input type="text" onChange="validateIP(this,'<%write(com_endIP);%>',0);"  class="txt-input" name="EndIP" style="width:130px;" id="EndIP" value="" />&nbsp;&nbsp; </td>
           
        </tr>
        <tr class="tab_info" style="display:none;" id="macDis">
         	<td><%write(grou_setObj);%></td>
            <td colspan="2">
            <input type="radio" style="display:none;" name="sourceIP" onClick="selObject(this.form,this.value)" value="macaddress" /><%write(com_MAC);%>
            &nbsp;&nbsp;
            <input type="hidden" name="writeMacAddress" id="writeMacAddress">
            <span style="position:relative">
                  <div id="selMacAddress" style="display:inline-block;"></div>
            </span>
            </td>
        </tr>
        <tr class="tab_info">
        	<td><%write(grou_bandLimUp);%></td>
            <td colspan="2"><input type="text" name="uMaxDownRatetxt" onchange="validateNumber(this,11,102400,'<%write(<gro_shContent);%>',0)" id="uMaxDownRatetxt" style="width:70px;" onKeyPress="figureOnly_P(event)" onBlur="changeSelect(this,Bandwidth_Config.uMaxDownRatew)" size="8" maxlength="6" class="txt-input" />&nbsp;kbit/s&nbsp;&nbsp;&nbsp;&nbsp;<b style="font-size:12px">&lt;==</b>&nbsp;<select name="uMaxDownRatew" id="uMaxDownRatew"  class="select" onChange="changeRate(this,Bandwidth_Config.uMaxDownRatetxt)" style="width:100px;" >
	  <option value="0"><%write(com_noLimit);%></option>
	   <option value="zidingyi"><%write(com_defSelf);%></option>
		<script language="javascript">
		for(i=0;i<RateArr.length;i++)
			
			document.write('<option value="'+RateArrValue[i]+'">'+RateArr[i]+'</option>');
		</script>
</select>&nbsp;<%write(com_limitTip);%><input type="hidden" name="page" value="limit"><input type="hidden" name="dRate"><input type="hidden" name="dCeil" value="0"><input type="hidden" name="uRate"><input type="hidden" name="uCeil" value="0"><input type="hidden" name="policy_name"><input type="hidden" name="delstr" size=10><input type="hidden" name="Action" value="add"><input type="hidden" name="GroupNameOld" id="GroupNamesOld" value=""></td>
        </tr>
        <tr class="tab_info">
        	<td><%write(grou_bandLimDown);%></td>
            <td colspan="2"><input type="text" name="MaxDownRatetxt" id="MaxDownRatetxt" onchange="validateNumber(this,11,102400,'<%write(grou_bandLimDown);%>',0)" onKeyPress="figureOnly_P(event)" onBlur="changeSelect(this,Bandwidth_Config.MaxDownRatew)"   style="width:70px;" size="8" maxlength="6" class="txt-input" />&nbsp;kbit/s&nbsp;&nbsp;&nbsp;&nbsp;<b style="font-size:12px">&lt;==</b>&nbsp;<select name="MaxDownRatew" id="MaxDownRatew" onChange="changeRate(this,Bandwidth_Config.MaxDownRatetxt)" class="select" style="width:100px;">
	  <option value="0"><%write(com_noLimit);%></option>
	   <option value="zidingyi"><%write(com_defSelf);%></option>
		<script language="javascript">
		for(i=0;i<RateArr.length;i++)
			document.write('<option value="'+RateArrValue[i]+'">'+RateArr[i]+'</option>');
		</script>
</select>&nbsp;<%write(com_limitTip);%></td>
        </tr>
       
       	 <tr  height="60px" class="tab_savetd"> 
        	<td colspan="3" align="center">
            	<table cellpadding="0" cellspacing="0" border="0" width="550">
                	<tr>
                    	<!--<td width="70px">&nbsp;</td>-->
                        <td align="center" colspan="2" ><span id="error_msg"  style="color:red;"></span></td>
                        <td align="right" width="250px"><input type="button" class="btn" name="back" onclick="save(1);"  value="<%write(com_add);%>"/>&nbsp;&nbsp;&nbsp;&nbsp;<input type="button" class="btn" name="updatebtn" id="updatebtn"  value="<%write(com_modify);%>" onclick="save(2);"/></td>
                    </tr>
                </table>
           </td>
            
        </tr>

    </table>
    <script type="text/javascript">document.write(dspListTab(listheadname,titles,totalTitel,arrayData,1));</script>
   <table border="0" cellpadding="0" cellspacing="1" style="background:#5e5e5e;" class="group" >
   		<tr><td  class="tab_savetd" align="right"><span><input type="button" class="btn" onclick="delAll();" value="<%write(delAll);%>"/></span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td></tr>
   </table>
    </form>
<br>
<br>
</div>
</body>
</html>
