<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<%langTran("common");%>
<%langTran("commonWords");%>
<%langTran("DnsFilter");%>
<%langTran("ParentsControlSui");%>
<%langTran("PdbUpdata");%>
<title><%write(com_parentscontrol);%></title>
<link href="css/style_new.css" rel="stylesheet" type="text/css" />
<link href="css/default.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="script/jquery-1.8.1.min.js"></script> 
<script type="text/javascript" src="js/common.js"></script> 
<script type="text/javascript" src="script/frame.js"></script> 
<script type="text/javascript" src="script/jquery.artDialog.js" ></script>
<script type="text/javascript" src="script/jquery.selectbox.js" ></script>
</head>
<script type="text/javascript">
var urlCount=10;
maxDspLen[0]=new Array(0,0,20,10);

var EffectTimeRngs=new Array();
var parentCount=9;


<% getArpFromArptable(); %>
<% getCurrentIPMacInfo(); %>
<% getParentsControlGlobalInfo(); %>
<% getParentsControlConf_Sui(); %>

var result = new Array();
for (var i = 0; i < i_totalrecs; i++) {
    for (var j = 0; j < i_totalrecs; j++) {
	if (i_macs[i] != result[j]) {
	    result.push(i_macs[i]);//如果result数组中不存在，则加入result数组中
	} else {//result数组中已存在该值，说明重复,移除i_macs中该元素
	    i_macs.splice(i,1);
	}
    
    }
}

for(var i=0;i<totalrecs;i++){
	EffectTimeRngs[i]="";
	if(fw_days[i]==effectDay)
		EffectTimeRngs[i]="<%write(com_every);%>";
	else{
		for(j=0;j<fw_days[i].length;j++){
			if(fw_days[i].charAt(j)=="1"){
				if(EffectTimeRngs[i].length) EffectTimeRngs[i]+="，";
					EffectTimeRngs[i]+=DateArray[j];
			}
		}
	}
	
	if(fw_timestarts[i]!=effectTime){
		EffectTimeRngs[i]+="；"+fw_timestarts[i]+"-"+fw_timestops[i];
	}
}
var listheadname="<%write(par_childList);%>";
var titles = new Array();
var totalTitle = 5;
titles[0] = "ID";
titles[1] ="<%write(PName);%>";
titles[2] ="<%write(par_address);%>";
titles[3] ="<%write( par_domain);%>";
titles[4] ="<%write(par_effTime);%>";
var arrayData = new Array(UserNames,arp_Macs,userUrls,EffectTimeRngs);



function checkArrayRep(arr,val){
	
	for(var i=0;i<arr.length;i++){
		if(arr[i]==val){
			return false;
		}	
	}	
	return true;
}
function init(){
	var f = ParentControl_Config;
	var switch_b = $(".on-off").children("button");
	initEnabledButton(switch_b,AllowControlEnables);
	$("#AllowControlEnable").val(AllowControlEnables);	
	
	var option_html1 ="";
	var option_html2 ="";
	var option_html3 ="";
	var option_html4 ="";
	var option_html5 ="";
	var parentsMacArr;

   for(var i =1; i<= parentCount;i++)
   {
	   var mac = eval('parentsMac'+i);
	   parentsMacArr=[];
	   if (mac == "000000000000")
	   {   
	 	 $("#parentmac"+i).selectbox(i_macs,"");
	   }
	   else
	   {
	       mac= mac.replace(/:/g,"");

		   parentsMacArr = i_macs.concat();
		   if(checkArrayRep(i_macs,mac)){
			  parentsMacArr.push(mac);
			
		     }
		   $("#parentmac"+i).selectbox(parentsMacArr,mac);
	    }
   }
	
		/**init middle */
	$("#username").val("");
	var childMacArr = i_macs.concat();
	if(checkArrayRep(i_macs,currentMac)){
	    if (currentMac != ""){
		childMacArr.push(currentMac);
	    }
	}
	$("#selMacAddress").selectbox(childMacArr,"");
	
	$("#updatebtn").attr('disabled',true);
	$("#updatebtn").addClass("modifybtn");
	seldate(f, true);
	f.fromhour.disabled = f.fromminute.disabled = f.endhour.disabled = f.endminute.disabled = true;
	$("#error_msg").html(errorstr);
	if (currentMac == "") {
	    $("#current_mac").html("<font color='red'><%write(par_lanToMac);%></font>");
	} else {
	    $("#current_mac").html(currentMac);
	}
}

function EditConfig(i){
	var f = ParentControl_Config;
	var len = f.date.length;
	for (var j = 0; j < len; j++) {
	    f.date[j].checked = false;
	} 
	$("#updatebtn").attr('disabled',false);
	$("#updatebtn").removeClass("modifybtn");
	$("#username").val(UserNames[i]);
	f.usernameOld.value = UserNames[i];
	//alert("arp_Mac:"+arp_Macs[i].replace(/:/g,""));
	//$("#parentmac1").selectbox("12");
	//$("#selMacAddress").empty();
	var mac = arp_Macs[i].replace(/:/g,"");

	
		var childMacArr = i_macs.concat();
		if(checkArrayRep(i_macs,mac)){
			childMacArr.push(mac);
			
		}
	$("#selMacAddress").empty();
	$("#selMacAddress").selectbox(childMacArr,mac);
	
	var domainsArray = new Array();

	if(userUrls[i].indexOf(",")!=-1){
	domainsArray =userUrls[i].split(",");
	}else{
		domainsArray.push(userUrls[i]);	
	}
	
	make_input_boxs(domainsArray);
	setTimeValue(f,fw_days[i],fw_timestarts[i],fw_timestops[i]);
		
}
var domInputBclor="";
function checkDomain(){
	var doms_val =  $("input[name='doms[]']");
	var checkRet=true;
		if(doms_val){
			var newDommains = new Array();
			doms_val.each(function(index){
				if(domInputBclor=="") domInputBclor=$(this).css("border-color");
			    else $(this).css("border-color",domInputBclor);
				var tips = checFilterName(this,49,"<%write(dns_domain);%>",1);
				if(tips==""){
					var dom_one = this.value;
					if(dom_one!=""){
						this.value = dom_one;
						for(var i=0;i<newDommains.length;i++){
							if(this.value==newDommains[i]){
								//art.dialog({icon:"error",title:false,content:"有相同的域名&nbsp;&nbsp;&nbsp;&nbsp;"}).lock().time(3);
								$("#error_msg").html("<%write(par_sameDom);%>");
								$(this).css("border-color","#F00");
								checkRet=false;
								return false;	
							}	
						}
						newDommains.push(dom_one);
						return true;
					}
				}else{
					//art.dialog({icon:"error",title:false,content:tips}).lock().time(3);
					$("#error_msg").html(tips);
					$(this).css("border-color","#F00");
					checkRet=false;
					return false;
				}
			});	
		}
	if(checkRet) ParentControl_Config.SiteVals.value = newDommains;
	return checkRet;
	
}
function add(value){
	inputErro=function(obj,tips){$("#error_msg").html(tips);}
	var tbl = ParentControl_Config;
	if (value == 1)
	{
	    tbl.Action.value = "add";
	}
	else
	{
	    tbl.Action.value = "modify";
	}
	tbl.writeMacAddress.value = $("#selMacAddress").selectboxval();

	var  returnval = checkName_P(ParentControl_Config.username, 11, "<%write(PName);%>", 0);
	if(!returnval){
		return;
	}

	if(!checkMac(tbl.writeMacAddress,"<%write(com_MAC);%>",0)) return;
	
	getTimeValue(tbl);
	if(tbl.day.value=="0000000"&&!tbl.everyday.checked){
		alert("<%write(com_selectDate);%>");
		tbl.day.value="";
		return;
	}
	if(checkDomain()&& returnval){
		tbl.action="/goform/formParentsControlConf_Sui";
		tbl.submit();
	}	
}
function checFilterName(cntrl,maxlen,tips,allownull)
{
	var name=cntrl.value;
	var msg ="";
	if(name=="")
	{
		if(!eval(allownull))
		{
		 msg = tips+"<%write(com_null);%>";
		 cntrl.focus();   
		  return msg;
		}
		else
			return msg;
	}
  else if(name==0){
		 msg = tips+"<%write(com_zero);%>";
		return msg;
	}
	else if (name.length > maxlen)
	{
		
		 msg = tips+"<%write(com_chkLenTip);%>"+maxlen+"<%write(com_lenAppend);%>";
		cntrl.focus();
		return msg;
	}
	else
	{
		allowChar="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890.-*";
		for (i=0;i<name.length;i++)
		{
			if (allowChar.indexOf(name.substring(i,i+1))==-1)
			{
				 msg = tips+ "<%write(par_onlySymbol);%>";
				return msg;
			}
		}
	}
	return msg;
}

function DeleteConfig(index){
	
	if(confirm("<%write(com_deleteTip);%>")){
	    ParentControl_Config.delstr.value = UserNames[index];
	    ParentControl_Config.action="/goform/formParentsControlDel";
	    ParentControl_Config.submit();
	}
}
function delAll(){
	if(confirm("<%write(com_deleteAllTip);%>")){
		ParentControl_Config.action="/goform/formParentsControlDelAll";
		ParentControl_Config.submit();
	}
}
function make_input_boxs(datas){
	/*	var left_box=8; 	//补充 空的 input box 个数
		if (datas.length) {
			if(datas.length>0 && datas.length<7){
				left_box = 8-datas.length;
			} else {
				var yu = datas.length%3;
				var bu = 3-yu + 3 - 1;
				left_box=bu;
			}
		}*/
		$("#macs_box").empty();
		$("#macs_box").html('<li id="add_botton" class="stop_disabled"><a><img border="0" src="<%write(com_addDomain);%>"></a></li>');
		for(var i=0;i<datas.length;i++){
			//datas[i] = mac_format(datas[i]);
			
			$("#add_botton").before('<li><input type="text" name="doms[]" class="txt-input stop_disabled" value="'+datas[i]+'" /></li>');
		}
		/*for(var i=0;i<left_box;i++){
			$("#add_botton").before('<li><input type="text" name="macs[]" class="txt-input stop_disabled" value="" /></li>');
		}*/
	}


var parentMacBcolor="";
function save(){
	var f = ParentControl_Config;
	var i,obj;
	var tmpMacArr=[];
	if(parentMacBcolor=="") parentMacBcolor=$("#parentmac1divinput").css("border-color");
	inputErro=function(obj,tips){$("#error_msg1").html(tips);}
	for(i=1;i<=parentCount;i++)
	{
		obj = eval('f.parentpc'+i);
		obj.value = $("#parentmac"+i).selectboxval().toLowerCase();
		if(obj.value!="") {
			if(!checkMac(obj,"<%write(com_MAC);%>",1)) {
				$("#parentmac"+i+"divinput").css("border-color","#F00");
				return;
			} 
	
			if(tmpMacArr.searchData(obj.value)!=-1)
			{
				$("#parentmac"+i+"divinput").css("border-color","#F00");
				$("#error_msg1").html("<%write(par_samemac);%>");
				return;
			}
			else 	tmpMacArr.push(obj.value);
		}
		$("#parentmac"+i+"divinput").css("border-color",parentMacBcolor);
	}
	var switch_b = $(".on-off").children("button");
	f.AllowControlEnable.value = switch_b.attr("class");
    f.action="/goform/formParentsControlGlobalConfig";
	f.submit();	
}

$(function(){
			
			/*$("#parentmac1").selectbox(i_macs,"");
			$("#parentmac2").selectbox(i_macs,"");
			$("#parentmac3").selectbox(i_macs,"");
			$("#parentmac4").selectbox(i_macs,"");
			$("#selMacAddress").selectbox(i_macs,"");*/
	init();
	$(".on-off").click(function(){
		var switch_b = $(".on-off").children("button");
		var status_now = switch_b.attr("class");
		if(status_now=="on"){
			$("#AllowControlEnable").val("off");	
		}else{
			$("#AllowControlEnable").val("on");	
		}
		swicth_act(status_now);
	});
	$("#domain").click(function(){
			var domainshows = $("#domainshow").val();
			if(domainshows=="0"){
				$("#allowDomain_box").hide();
				$("#domainshow").val("1");	
				var box_cnt = $("input[name='doms[]']").length;
				if(box_cnt==0){
					box_cnt=1;
				}
				parent.art.dialog.list["func_box"].size(800,$(document).height()-box_cnt*20);	
			}else{
				$("#allowDomain_box").show();	
				$("#domainshow").val("0");	
				parent.art.dialog.list["func_box"].size(800,$(document).height());	
			}
		
	});
	$("#effectTime").click(function(){
			var timeshows = $("#timeshow").val();
			
			if(timeshows=="0"){
				$("#date_box").hide();
				$("#time_box").hide();
				$("#timeshow").val("1");	
				
				parent.art.dialog.list["func_box"].size(800,$(document).height()-150);	
			}else{
				$("#date_box").show();
				$("#time_box").show();		
				$("#timeshow").val("0");
				parent.art.dialog.list["func_box"].size(800,$(document).height()+50);	
			}
		
	});	
	
	//添加输入框
	var add_cl = 0;
	$("#macs_box").html('<li id="add_botton" class="stop_disabled"><a><img border="0" src="<%write(com_addDomain);%>"></a></li>');
	$("#add_botton").live("click",function(){
		var add_cnt = 3;
		var box_cnt = $("input[name='doms[]']").length;
		if (box_cnt >= urlCount) {
			art.dialog({icon:"error",title:false,content:"<%write(par_maxWrite);%> "+urlCount+"<%write(par_manyDomain);%>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"}).lock().time(4);
			return false;
		}
		var next_max = $("input[name='doms[]']").length+3;
		if (next_max >= urlCount) {
			add_cnt = urlCount-box_cnt;
		}
		//var status_now = $("input[name='status']:checked").val();
		//if (status_now != "stop"){
			for(var i=0;i<add_cnt;i++){
				$(this).before('<li><input type="text" name="doms[]" class="txt-input stop_disabled"/></li>');
		//	}
			var top_he = 100+add_cl*72;
			$('.setup_box').animate({scrollTop:top_he},1000);
			add_cl++;
		}
		parent.art.dialog.list["func_box"].size(800,$(document).height()+20);	
	})
	
	//make input boxs
	
			//$("#parentmac1").selectbox();
		
			//$("#parentmac2").selectbox();
			//$("#parentmac3").selectbox();
			//$("#parentmac4").selectbox();
			
			//$("#selMacAddress").val("");
			//$("#selMacAddress").selectbox();
			
	
})

</script>
<style type="text/css">

/*.group .item-list .txt-input{  height: 24px; line-height: 24px; border: 1px solid #cdcdcd; background: url(../images/input_txt_bg.png) repeat-x; padding: 0 2px; font-size: 14px; color: #333; font-family: Microsoft Yahei;}*/
/*#parentmac1,#parentmac2,#parentmac3,#parentmac4,#selMacAddress{height:20px; width:140px; color: }*/

</style>
<body scroll="no">



<div class="warp" style="width:700px;" >
<br>

<form action="" method="post" name="ParentControl_Config">
	<table border="0" cellpadding="0" cellspacing="1" style="background:#5e5e5e;width:700px;" class="group" >
   		<tr class="tab_title">
        	<td colspan="3"><%write(par_setControl);%></td>           
        </tr>
       
   		 <tr class="tab_info">
        	<td width="180px"><%write(par_open);%></td>
            <td width="460px"  colspan="2">
            	<span class="on-off">&nbsp;<button type="button" class="on"><%write(com_switch);%></button><input type="hidden" name="AllowControlEnable" id="AllowControlEnable" value="" /></span>

            </td>
           
        </tr>
          <tr class="tab_info">
        	<td width="140px"><%write(par_nowMac);%></td>
            <td width="460px"  colspan="2">
            	<span id="current_mac"></span>
               
            </td>
           
        </tr>
     	<tr class="tab_info" height="120px">
        	<td><%write(par_PCMac);%></td>
            <td colspan="2" align="center">
            	<div style="padding-bottom:10px;">
                	<script language="javascript">
					for(var i=1;i<=parentCount;i++)
						document.write('<div id="parentmac'+i+'" style="padding-bottom:5px;padding-top:5px; z-index:'+(500-i)+';position:relative; float:left;"></div>');
                    </script>
                </div>
            </td>
           
        </tr>
       
       	  <tr  height="60px" class="tab_savetd"> 
        	<td colspan="3" align="center">
            	<table cellpadding="0" cellspacing="0" border="0" width="650">
                	<tr>
                    	<!--<td width="70px">&nbsp;</td>-->
                        <td align="center" colspan="2" ><span id="error_msg1" style="color:red;"></span></td>
                        <td align="right" width="200px"><input type="button" class="btn" name="back" onclick="save();"  value="<%write(com_save);%>"/>
                        <script language="javascript">
						for(var i=1; i<=parentCount;i++) document.write('<input type="hidden" name="parentpc'+i+'" id="parentpc'+i+'">');
                        </script>
			<input type="hidden" name="writeMacAddress" id="writeMacAddress"><input type="hidden" name="SiteVals" id="SiteVals">
			<input type="hidden" name="Action" id="Action" value="add"><input type="hidden" name="delstr" size=10>
                    </tr>
                </table>
           </td>
            
        </tr>

      	<tr class="tab_title">
        	<td colspan="3"><%write( par_childPC);%></td>           
        </tr>
         <tr class="tab_info">
        	<td><%write(PName);%></td>
            <td colspan="2">
            		<input type="text" name="username" id="username" class="txt-input" /><input type="hidden" name="usernameOld" id="usernameOld">

            </td>
           
        </tr>
        <tr class="tab_info">
        	<td><%write(par_childMac);%></td>
            <td colspan="2">
            			  <span style="position:relative">
                          <div id="selMacAddress"></div>
                        <!--  <select name="selMacAddress"  id="selMacAddress" class="txt-input select">   
                               <div id="dropdown"></div>
                          </select> -->
                          </span>
                    

            </td>
           
        </tr>
        <tr class="tab_info">
        	<td><a id="domain" style="cursor:pointer"><%write(par_setDomain);%></a><input type="hidden" id="domainshow" value="1"/></td>
			 <td colspan="2"><%write(You_can_use);%></td>
           
        </tr>
        <tr class="tab_info" id="allowDomain_box" style="display:none;">
        	<td width="140px"><%write(par_allowDomain);%></td>
            <td width="460px" colspan="2">
            		<ul id="macs_box" class="item-list" style="margin-top:10px; " >
					
					
				</ul>

            </td>
           
        </tr>
        <tr class="tab_info">
        	<td><a id="effectTime" style="cursor:pointer"><%write(par_effTime);%></a><input type="hidden" id="timeshow" value="1"/></td>
            <td colspan="2">
            </td>
           
        </tr>
        
         <tr class="tab_info" id="date_box" style="display:none;" height="70px">
        	<td><%write(par_date);%></td>
            <td colspan="2">
            	<div style="padding-bottom:10px;"><input type="checkbox" name="everyday" onClick="seldate(this.form,this.checked)" checked="checked" value="everyday2"><%write(com_every);%></div>
                <div>
                	<input type="checkbox" name="date" value="1"><%write(com_monday);%>&nbsp;<input type="checkbox" name="date" value="2"><%write(com_tuesday);%>&nbsp;<input type="checkbox" name="date" value="3"><%write(com_wednesday);%>&nbsp;<input type="checkbox" name="date" value="4"><%write(com_thursday);%>&nbsp;<input type="checkbox" name="date" value="5"><%write(com_friday);%>&nbsp;<input type="checkbox" name="date" value="6"><%write(com_saturday);%>&nbsp;<input type="checkbox" name="date" value="7"><%write(com_sunday);%>
                </div>
            </td>
           
        </tr>
         <tr class="tab_info" id="time_box" style="display:none;" height="80px">
        	<td><%write(par_time);%></td>
            <td colspan="2">
            	<div style="padding-bottom:10px;">
                	<input type="radio" name="effecttime" value="All" onClick="seltime(this.form,this.value)" checked>&nbsp;<%write(com_allDay);%>
                </div>
                <div>
                	<input type="radio" name="effecttime" value="timerange" onClick="seltime(this.form,this.value)">&nbsp;<%write(com_from);%>&nbsp;<select name="fromhour" class="list"><script language="javascript">CreateHOp(24);</script></select>&nbsp;:&nbsp;<select name="fromminute" class="list"><script language="javascript">CreateHOp(60);</script></select>&nbsp;&nbsp;<%write(com_to);%>&nbsp;<select name="endhour" class="list"><script language="javascript">CreateHOp(24);</script></select>&nbsp;:&nbsp;<select name="endminute" class="list"><script language="javascript">CreateHOp(60);</script></select></td></tr><input type="hidden" name="day"><input type="hidden" name="timestart"><input type="hidden" name="timestop">

                </div>
            </td>
           
        </tr>
         <tr  height="60px" class="tab_savetd"> 
        	<td colspan="3" align="center">
            	<table cellpadding="0" cellspacing="0" border="0" width="650">
                	<tr>
                    	<!--<td width="70px">&nbsp;</td>-->
                        <td align="center" colspan="2" ><span id="error_msg"  style="color:red;"></span></td>
                        <td align="right" width="250px"><input type="button" class="btn" name="back" onclick="add(1);"  value="<%write(com_add);%>"/>&nbsp;&nbsp;&nbsp;&nbsp;<input type="button" class="btn" id="updatebtn" name="updatebtn" onclick="add(2);"  value="<%write(com_modify);%>"/></td>
                    </tr>
                </table>
           </td>
            
        </tr>
    </table>
    <script type="text/javascript">document.write(dspListTab(listheadname,titles,totalTitle,arrayData,1,"700px"));</script>
   <table border="0" cellpadding="0" cellspacing="1" style="background:#5e5e5e; width:700px;" class="group" >
   		<tr><td  class="tab_savetd" align="right"><span><input type="button" class="btn" onclick="delAll();" value="<%write(delAll);%>"/></span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td></tr>
   </table>
	<form>
</div>
<br>
<script language="javascript">showSuccessTips(rightstr);</script>
</body>
</html>
