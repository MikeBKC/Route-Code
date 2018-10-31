<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<%langTran("common");%>
<%langTran("commonWords");%>
<%langTran("serviceZone");%>
<%langTran("PdbUpdata");%>
<%langTran("NatStatic");%>
<title>VPS</title>
<link href="css/style_new.css" rel="stylesheet" type="text/css" />
<link href="css/default.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="script/jquery-1.8.1.min.js"></script> 
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="js/common.js"></script> 
<script type="text/javascript" src="script/frame.js"></script> 
<script type="text/javascript" src="script/jquery.artDialog.js" ></script>
</head>
<script type="text/javascript">
var errorstr="";
var changeFlag = "0";
var InnerPortsw = "80";
/*
/////////////////////////////交互变量///////////////// 
*/
 
var edit_Names = "";
tcpBusiness=new Array(21,22,23,25,66,79,80,110,135,139,443,445,1433,1434,1720,1723,1863,3389);
tcpBusinessText=new Array("21(ftp)","22(ssh)","23(telnet)","25(smtp)","66(sql*net)","79(finger)","80(web)","110(pop3)","135(epmap)","139(netbios-ssn)","443(https)","445(ms-ds)","1433(ms-sql-s)","1434(ms-sql-m)","1720(h.323)","1723(pptp)","1863(msn login)","3389(ms-ts)");
udpBusiness=new Array(53,67,68,69,123,137,138,161,162,500,1701);
udpBusinessText=new Array("53(dns)","67(bootps)","68(bootpc)","69(tftp)","123(ntp)","137(netbios-ns)","138(netbios-dgm)","161(snmp)","162(snmptrap)","500(isakmp)","1701(l2tp)");
 
<% aspOutNatStaticMap(); %>
<%getWanIfCount();%>
var configEnables = new Array();
for(i=0;i<totalrecs;i++){
    switch(Protocols[i]){
    case '1':
	Protocols[i] = 'TCP';
	break;
    case '2':
	Protocols[i] = 'UDP';
	break;
    case '3':
	Protocols[i] = 'TCP/UDP';
	break;
    }
    configEnables[i]=(ConfigEnables[i]=="1"?"<%write(enable);%>":"<%write(disable);%>");
}
var listheadname="<%write(vir_list);%>";
var titles=new Array(7);
var totalTitle= 8;
titles[0]="ID";
titles[1]="<%write(PName);%>";
titles[2]="<%write(com_status);%>";
titles[3]="<%write(com_protocol);%>";
titles[4]="<%write(nat_exterFPort);%>";
titles[5]="<%write(com_IP);%>";
titles[6]="<%write(nat_innerFPort);%>";
titles[7]="<%write(nat_portNum);%>";
var innerP="80";
var pro="1";
 
 
var arrayData=new Array(IDs,configEnables,Protocols,OutPorts,IPs,InnerPorts,PortNums);



function clearOptions(cntrl)
{
    while(cntrl.length)	{
	cntrl.options[0] = null;
    }
}
 
function createOptions(cntrl,arrayVal,arrayText)
{
    for(i=0; i<arrayVal.length; i++)
    {
	option=document.createElement("OPTION");
	option.text=arrayText[i];
	option.value=arrayVal[i];
	cntrl.options.add(option);
    }
}
function init(){
    //protocolChange();
    changePro();
    var f=VirtualServer_Config;
    var isSet = 0;
    f.ID.value="";
    selVal(f.Protocol,1);
    f.IP.value="";
    f.OutPort.value="";
    f.PortNum.value="1";
    f.InnerPort.value=innerP;
    $("#edit").attr('disabled',true);
    $("#edit").addClass("modifybtn");
    $("#error_msg").html(errorstr);
    showSuccessTips(rightstr);
}
 
function protocolChange(index)
{
		
		var f=VirtualServer_Config;
		//alert("protocolChange index:"+index+";f.Protocol.value:"+f.Protocol.value);
		clearOptions(f.CommService);
		if (f.Protocol.value=="1")
		{
			createOptions(f.CommService,tcpBusiness,tcpBusinessText);
			custom=document.createElement("OPTION");
			custom.text="<%write(com_defSelf);%>";
			custom.value="custom";
			f.CommService.options.add(custom);
			f.CommService.selectedIndex=18;
			tbservice.style.display= dis;
			//alert("startdsdsd bussiness");
			//businessChange(index);
		
			f.CommService.value=InnerPorts[index];
			if(f.CommService.value==""){
			    f.CommService.value="custom";
			}
		}
		else if (f.Protocol.value=="2")
		{
			createOptions(f.CommService,udpBusiness,udpBusinessText);
			custom=document.createElement("OPTION");
			custom.text="<%write(com_defSelf);%>";
			custom.value="custom";
			f.CommService.options.add(custom);
			f.CommService.selectedIndex=11;
			tbservice.style.display= dis;
			//businessChange(index);
			
			f.CommService.value=InnerPorts[index];
			if(f.CommService.value==""){
			    f.CommService.value="custom";
			}
		}
		else
		{
			tbservice.style.display="none";
			f.InnerPort.value=InnerPorts[index];
			changeFlag = "1";
		}
}

function changePro(){
	var f=VirtualServer_Config;

	
		clearOptions(f.CommService);
		if (f.Protocol.value=="1")
		{
			createOptions(f.CommService,tcpBusiness,tcpBusinessText);
			custom=document.createElement("OPTION");
			custom.text="<%write(com_defSelf);%>";
			custom.value="custom";
			f.CommService.options.add(custom);
			f.CommService.selectedIndex=18;
			tbservice.style.display= dis;
			//alert("startdsdsd bussiness");
			//businessChange(index);
			if(pro==1){
				f.InnerPort.value=innerP;
			}else{
				f.InnerPort.value="";	
			}
			portChange();
		}
		else if (f.Protocol.value=="2")
		{
			createOptions(f.CommService,udpBusiness,udpBusinessText);
			custom=document.createElement("OPTION");
			custom.text="<%write(com_defSelf);%>";
			custom.value="custom";
			f.CommService.options.add(custom);
			f.CommService.selectedIndex=11;
			tbservice.style.display= dis;
			if(pro==2){
				f.InnerPort.value=innerP;
			}else{
				f.InnerPort.value="";	
			}
			portChange();
			
			
		}
		else
		{
			tbservice.style.display="none";
			if(pro==3){
				f.InnerPort.value=innerP;
			}else{
				f.InnerPort.value="";	
			}
			
			changeFlag = "1";
		}
	
}
function portChange(){
	  var f=VirtualServer_Config;

		//	 f.InnerPort.value=innerP;  

	
	//alert("portChange:"+f.CommService.value );
	if (f.CommService.value != "custom")
	{
		f.InnerPort.value=f.CommService.value;
	}
}

function EditConfig(i){
	$("#error_msg").html("");
    if(IDs[i].toLowerCase()=="admin"){
	alert("<%write(vir_warn);%>");
	return ;
    }

    var switch_b = $(".on-off").children("button");
    $("#ebabledvirtual").val(ConfigEnables[i]);
    var stauts = (ConfigEnables[i]=="1"?"on":"off");
    initEnabledButton(switch_b,stauts);
    var f=VirtualServer_Config;

    var protocol;
    switch(Protocols[i]){
    case 'TCP':
	protocol = '1';
	break;
    case 'UDP':
	protocol = '2';
	break;
    case 'TCP/UDP':
	protocol = '3';
	break;
    }
    f.ID.value=IDs[i];
    f.IDold.value=IDs[i];
    selVal(f.Protocol,protocol);
	pro = protocol;
	innerP =InnerPorts[i];
    f.InnerPort.value=InnerPorts[i];
    f.IP.value=IPs[i];
    f.OutPort.value=OutPorts[i];
    f.PortNum.value=PortNums[i];
	
    protocolChange(i);
    $("#edit").attr('disabled',false);
    $("#edit").removeClass("modifybtn");
}
function DeleteConfig(index){
    if(IDs[index].toLowerCase()=="admin"){
	alert("<%write(nat_editAlert2);%>");
	return;
    }
    if(confirm("<%write(com_deleteTip);%>")){
	VirtualServer_Config.delstr.value=IDs[index];
	VirtualServer_Config.action="/goform/formNatStaticMapDel";
	VirtualServer_Config.submit();
    }
}
function delAll(){
    if(confirm("<%write(com_deleteAllTip);%>")){
	VirtualServer_Config.action="/goform/formNatStaticMapDelAll";
	VirtualServer_Config.submit();
    }
}

function checkData(){
    var f = VirtualServer_Config;
    if(!checkName_P(f.ID,11,"<%write(nat_staMapName);%>",0)) return false;
    if(f.ID.value.toLowerCase()=="admin"){
	alert("<%write(nat_checkTip);%>");
	return false;
    }
    if (L2TPList == 1) {
	if(f.ID.value.toLowerCase()=="l2tp"){
	    alert("<%write(nat_checkTip);%>");
	    return false;
	}
    }
    if (PPTPClientList == 1) {
	if(f.ID.value.toLowerCase()=="pptp"){
	    alert("<%write(nat_checkTip);%>");
	    return false;
	}
    }

    if(!checkIP_P(f.IP,"<%write(com_IP);%>",0)) return false;
    if(!checkNumber_P(f.OutPort,1,65535,"<%write(nat_exterFPort);%>",0)) return false;
    if(!checkNumber_P(f.InnerPort,1,65535,"<%write(nat_innerFPort);%>",0)) return false;
    if(!checkNumber_P(f.PortNum,1,500,"<%write(nat_portNum);%>",0)) return false;
    var outnum=parseInt(f.OutPort.value)+parseInt(f.PortNum.value);
    var innum=parseInt(f.InnerPort.value)+parseInt(f.PortNum.value);
    if(outnum>65536||innum>65536){
	alert("<%write(nat_ckPorTip);%>");
	return false;
    }
    return true;
}
function save(){
    var f=VirtualServer_Config;
    if(checkData()){
	if(f.Action.value=="add") {
	    f.IDold.value=f.ID.value;
	}
	/*
	alert("ID="+f.ID.value);
	alert("IDold="+f.IDold.value);
	alert("Protocal="+f.Protocol.value);
	alert("InnerPort="+f.InnerPort.value);
	alert("IP="+f.IP.value);
	alert("OutPort="+f.OutPort.value);
	alert("PortNum="+f.PortNum.value);
	alert("Action="+f.Action.value);
	*/
	VirtualServer_Config.action="/goform/formNatStaticMap";
	VirtualServer_Config.submit();
    }	
}
$(function(){
    init();
    $(".on-off").click(function(){
	var switch_b = $(".on-off").children("button");
	var status_now = switch_b.attr("class");
	if(status_now=="on"){
	    $("#ebabledvirtual").val("0");	
	}else{
	    $("#ebabledvirtual").val("1");	
	}
	swicth_act(status_now);
    });
    $("#add").click(function(){
	if(eval(max_totalrecs-totalrecs)<1) {
	    //alert("配置已经达到最大条数,不能添加新配置");
	    $("#error_msg").html("<%write(vir_warn1);%>");
	    return;
	}
	$("#Action").val("add");
	save();
    });
    $("#edit").click(function(){
	$("#Action").val("modify");
	if ($("#IDold").val() == "") {/*如果点击修改按钮，但是IDold又不存在，那么认为此为误操作*/
	    $("#error_msg").html("<%write(vir_sorryno);%>");
	    return false;
	}
	save();
    });
});

</script>
<body  scroll="no">
<div class="warp">

<br>
	<form action="" method="post" name="VirtualServer_Config">
    <!-- <div id="error_msg" style="color:red;"></div> -->
	<table border="0" cellpadding="0" cellspacing="1" style="background:#5e5e5e;" class="group" >
   		  <tr class="tab_title">
        	<td colspan="3"><%write(vir_set);%></td>
           
        </tr>
    	<tr class="tab_info">
        	<td width="140px"><%write(PName);%></td>
            <td width="460px" colspan="2"><input name="ID" type="text" maxlength="11" size="18" class="txt-input" /></td>
        </tr>
        <tr class="tab_info">
        	<td><%write(enable);%></td>
            <td colspan="2"><span class="on-off">&nbsp;<button type="button" class="on"><%write(com_switch);%></button><input type="hidden" name="ConfigEnable" id="ebabledvirtual" value="1" /></span></td>
	   <input type="hidden" id="IDold" name="IDold" >
	   <input type="hidden" name="NatBind" value="WAN1">
           
        </tr>
       	 <tr class="tab_info">
        	<td><%write(com_protocol);%></td>
            <td colspan="2"><select  name="Protocol" class="select" onChange="changePro()" />
	   <option value="1">TCP</option>
	   <option value="2">UDP</option>
	   <option value="3">TCP/UDP</option>
	   </select></td>
           
        </tr>
        <tr class="tab_info">
        	<td><%write(nat_exterFPort);%></td>
            <td colspan="2">
            	<input name="OutPort" type="text" size="18" maxlength="5" onKeyPress="figureOnly()"  class="txt-input"  value="1" />

            </td>
        </tr>
        <tr class="tab_info">
        	<td><%write(vir_servIP);%></td>
            <td colspan="2"><input name="IP" type="text" size="18" maxlength="15" onKeyPress="ipOnly()" class="txt-input" /></td>
        </tr>
        <tr class="tab_info" id="tbservice">
        	<td width="140px"><%write(vir_comPort);%></td>
            <td width="460px" colspan="2"><select name="CommService" class="select" onChange="portChange();"></select></td>
        </tr>
       <tr class="tab_info">
        	<td><%write(nat_innerFPort);%></td>
            <td colspan="2"><input name="InnerPort" type="text" size="18" maxlength="5" onKeyPress="figureOnly()" class="txt-input"  value="1"></td>
        </tr>
        <tr class="tab_info">
        	<td><%write(vir_setNum);%></td>
            <td colspan="2"><input name="PortNum" type="text" size="18" maxlength="5" onKeyPress="figureOnly()" class="txt-input"  value="1"></td>
        </tr>
       	 <tr  height="60px" class="tab_savetd"> 
        	<td colspan="3" align="center">
            	<table cellpadding="0" cellspacing="0" border="0" width="550">
                	<tr>
                    	<td width="170px">&nbsp;</td>
                        <td align="left"><span id="error_msg"  style="color:red;"></span></td>
                        <td align="right"><input type="button" class="btn" id="add" name="add"  value="<%write(com_add);%>"/>&nbsp;&nbsp;&nbsp;&nbsp;<input type="button" class="btn" id="edit" name="edit" value="<%write(com_modify);%>"/><input type="hidden" id="Action" name="Action" value="add"></td>
                    </tr>
                </table>
           </td>
            
        </tr>
      
    </table>
    <input type="hidden" name="delstr" size=10 >
    <script type="text/javascript">document.write(dspListTab(listheadname,titles,totalTitle,arrayData,1));</script>
   <table border="0" cellpadding="0" cellspacing="1" style="background:#5e5e5e;" class="group" >
   		<tr><td  class="tab_savetd" align="right"><span><input type="button" class="btn" onclick="delAll();" value="<%write(delAll);%>"/></span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td></tr>
   </table>
    </form>
</div>
<br>
<br>
</body>
</html>
