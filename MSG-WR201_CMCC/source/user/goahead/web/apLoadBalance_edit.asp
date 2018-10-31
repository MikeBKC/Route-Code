<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<%langTran("common");%>
<%langTran("apLoadBalance");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript">
	var errorstr="";
/////////////////////////////交互变量/////////////////////
/*
var enables=1;//1：开启  0：未开启
var loadBalanceNames="ad";
var loadThresValues="20";
var loadBalDiffs="6";
var apLists = new Array();
var loadBalanceList = {"AP1 1234563 002124f921":0,"AP2 1234563 002124f921":1};//0： 掉线  1：正常
apLists[0]="AP1 1234563 002124f921";
apLists[1]="AP2 1234563 002124f921";
apLists[2]="AP3 1234563 002124f921";
apLists[3]="AP4 1234563 002124f921";
apLists[4]="AP6 1234563 002124f921";
apLists[5]="AP5 1234563 002124f921";
*/
var enables;//1：开启  0：未开启
var loadBalanceNames="";
var loadThresValues="";
var loadBalDiffs="";
var apLists = new Array();
//var loadBalanceList = new Array;//0： 掉线  1：正常
var loadBalanceList = { };//0： 掉线  1：正常
//var loadBalanceList = {"AP1 1234563 002124f921":0,"AP2 1234563 002124f921":1};//0： 掉线  1：正常

<% 
    aspOutputApEnvInfo();
	var edit_Name="";
	edit_Name=Form("id");
	if ( edit_Name!="") aspOutputApLbOneInfo(edit_Name);//需要修改
%> 
var edit_Names = "<% write(Form("id"));%>";


function createLoadBal(){
	var f = LoadBalance_Config;
	for(var i in loadBalanceList){
			
		if(loadBalanceList[i]==0){
			var option1=document.createElement("OPTION");
			option1.text=i;
			option1.value=i+"_"+loadBalanceList[i];
			option1.style.color="red";
			f.loadBalList.options.add(option1);
		}else{
			var option1=document.createElement("OPTION");
			option1.text=i;
			option1.value=i+"_"+loadBalanceList[i];
			option1.style.color="";
			f.loadBalList.options.add(option1);
		}
	}
}

function createApList(){

	var f = LoadBalance_Config;

	if(apLists.length>0){
		for(var i =0;i<apLists.length;i++){
			var option1=document.createElement("OPTION");
			option1.text=apLists[i];
			option1.value=apLists[i]+"_1";
			f.apList.options.add(option1);
	
		}
	}
}
function checkContain(text,obj)
{
	
	for(var i=0;i<obj.options.length;i++)
	{
		if(text==obj.options[i].text)
		{
			alert("<%write(use_incSame);%>");
			return false;
		}
	}
	return true;
}
function fromApListToLoadBal(){

		
	  var loadBalOPtions = document.getElementById("loadBalList");
	  

	  var apOption =document.getElementById("apList");
	
	  if(apOption.value==""){
	  	alert("<%write(add_require);%>");
		return;
	  }
	  var option = document.createElement("option");
	  option.text = apOption.value.substring(0,apOption.value.length-2);
	 
	  option.value= apOption.value;
	  var apstate = apOption.value.substring(apOption.value.length-1);
	 
	  if(apstate==0){
		  option.style.color="red";
	  }else{
	  		option.style.color="";
	  }

	  if(checkContain(option.text,loadBalOPtions)==false)return;
	  loadBalOPtions.add(option);
	  deleteListOption(apOption);
}
function fromLoadBalToApList(){
	  var loadBalOPtions = document.getElementById("loadBalList");
	  

	  var apOption =document.getElementById("apList");
	 
	  if(loadBalOPtions.value==""){
	  	alert("<%write(add_require);%>");
		return;
	  }
	  var option = document.createElement("option");
	  option.text = loadBalOPtions.value.substring(0,loadBalOPtions.value.length-2);
	
	  option.value= loadBalOPtions.value;
	 
	var loadstate = loadBalOPtions.value.substring(loadBalOPtions.value.length-1);

		if(loadstate==0){
			option.style.color="red";	
		}else{
			option.style.color="";	
		}
	  if(checkContain(option.text,apOption)==false)return;
	  apOption.add(option);
	  
	  deleteListOption(loadBalOPtions);
}
function deleteListOption(opt){
	 //var loadBalOPtions = document.getElementById("loadBalList");
	 for(var i=0;i<opt.length;i++){
	 	if(opt.value==opt[i].value){
			opt.options.remove(i);
		}
	 }
}

function checkData(){
	var f=LoadBalance_Config;
//	if(f.enabledLoadBal.checked)
    {
		if (!checkName(f.loadBalanceName,12,"<%write(load_balance_g_n);%>",0)) return false;
/*
		if(!checkNumber(f.loadThresValue, 1, 8, "负载均衡阀值", 0)) return false;
		if(!isNum(f.loadBalDiff.value)) {
		alert("请填写数字");
		return false;
		}
*/
		if( f.loadBalList.length <2 || f.loadBalList.length >10){
			alert("<%write(max_allow_num);%>");
			return false;
		}
	}
    return true;
}
function save(){
    var f=LoadBalance_Config;
	if(checkData()){
	    f.temp1.value="";
        for(var i=0;i<f.loadBalList.options.length;i++)
		{
			f.temp1.value+=f.loadBalList.options[i].value;
			f.temp1.value+=";";
		}
        if(f.loadBalList.options.length > max_apnum_in_lb_group ){
            alert("AP数超过负载均衡最大AP数");
            return false;
        }

		f.action="/goform/formApLbConfig";
		f.submit();
	}
}
function init(){
	var f=LoadBalance_Config;
	f.enabledLoadBal.checked=enables;
	f.loadBalanceName.value=loadBalanceNames;
//	f.loadThresValue.value=loadThresValues;
//	f.loadBalDiff.value=loadBalDiffs;

	if(edit_Names!=""&&edit_Names!=null){//alert();
		f.Action.value="modify";
	    f.loadBalanceNameOld.value=loadBalanceNames;
	}

	createApList();
	createLoadBal();
	
}

</script>
</head>

<body onLoad="init();" background="images/body_main.png">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
	<!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle('AP<%write(load_balance);%>');</script>
  	<table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
   		 <tr valign="top">
    		<td>
			<form action="/goform/"  name="LoadBalance_Config" method="post">
				<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center" style="display:">
				<p>&nbsp;</p>
					<tr><td>&nbsp;&nbsp;</td></tr>
					 <tr>
    					<td align="center" class="style7">
						
								  <table width="100%"	 border="0" cellspacing="0" cellpadding="0">
								  <p>&nbsp;</p>
									  <tr>
									  <td align="center"><table width="100%"  border="0" cellspacing="0" cellpadding="0">
									  <tr>
									  <td width="43%" height="30" align="right"><%write(start_load_balance);%></td>
									  <td width="1%">&nbsp;</td>
									  <td width="56%"><input name="enabledLoadBal" id="enabledLoadBal" type="checkbox"   class="input-checkbox"  > </td>
									  </tr>
									  <tr>
									  <td width="43%" height="30" align="right"><%write(load_balance_g_n);%></td>
									  <td width="1%">&nbsp;</td>
									  <td width="56%"><input type="text"  class="input" name="loadBalanceName" id="loadBalanceName" maxlength="11" size="15" > 
									  </td></tr><input type="hidden" name="loadBalanceNameOld">

<!--
									  <tr>
									  <td width="43%" height="30" align="right">负载均衡阀值</td>
									  <td width="1%">&nbsp;</td>
									  <td width="56%"><input type="text" name="loadThresValue" id="LoadThresValue" class="input" value="10" onKeyPress="figureOnly()"/> (取值范围：1-8)
									  </td></tr>
-->
                                      <!--
									  <tr>
									  <td width="43%" height="30" align="right">负载均衡差值</td>
									  <td width="1%">&nbsp;</td>
									  <td width="56%"><input type="text" name="loadBalDiff" id="loadBalDiff" class="input" value="5" onKeyPress="figureOnly()"/> 
									  </td></tr>
                                      -->
									  </table>
									  <table style="display:" width="100%"  border="0" align="center" cellpadding="0" cellspacing="0" id="AddrGroupTab" name="AddrGroupTab">
												<tr>
													<td width="16%" height="30">&nbsp;</td>
													<td width="23%">&nbsp;</td>
													<td width="2%">&nbsp;</td>
													<td width="10%">&nbsp;</td>
													<td width="2%">&nbsp;</td>
													<td width="23%">&nbsp;</td>
													<td width="24%">&nbsp;</td>
												</tr>
												<tr>
													<td>&nbsp;</td>
													<td align="center"><font style="FONT-FAMILY: "Arial", ",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",","Helvetica", ",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",","sans-serif";FONT-SIZE: 12px;margin-left:5px;"><%write(left_show);%></font></td>
													<td>&nbsp;</td>
													<td>&nbsp;</td>
													<td>&nbsp;</td>
													<td align="center"><font style="FONT-FAMILY: "arial", ",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",","helvetica", ",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",",","sans-serif";font-size: 12px"><%write(load_balance_list);%></font></td>
													<td>&nbsp;</td>
												</tr>
												<tr>
													<td>&nbsp;</td>
													<td align="right">
													<select class="input" name="apList" size="7" multiple  id="apList" ondblclick="fromApListToLoadBal()"style=" background-color: #ffffff; SCROLLBAR-FACE-COLOR: #ffffff;  SCROLLBAR-HIGHLIGHT-COLOR: #ffffff;	#c0ddff 1px solid; SCROLLBAR-SHADOW-COLOR: #ffffff; LINE-HEIGHT: 20px; SCROLLBAR-ARROW-COLOR: #c0ddff; SCROLLBAR-TRACK-COLOR: #ffffff; SCROLLBAR-DARKSHADOW-COLOR: #c0ddff; SCROLLBAR-BASE-COLOR: #c0ddff;border:1px solid #c0ddff; width:250px" border=0>
														 
    
                  									</select></td>
													<td>&nbsp;</td>
													<td align="center">
														<table id="idImg">
														  <tr>
														<td><input name="m1" type="button" value="==>" onClick="fromApListToLoadBal();" style="width:65px"/></td>
														  </tr>
														  <tr><td>&nbsp;</td></tr>
														  <tr>
														<td height="30"><input name="m2" type="button" value="<==" onClick="fromLoadBalToApList()" style="width:65px" /> </td>
														  </tr>

														 </table>
													</td>
													<td>&nbsp;</td>
													<td>    
													<select class="input" name="loadBalList" size=7  id="loadBalList" ondblclick="fromLoadBalToApList()"style="width:250px" border=0>
               										 </select></td>
													<td>&nbsp;</td>
												</tr>
												<tr>
													<td colspan="7" height="30">&nbsp;</td>
			                                        <input type="hidden" name="temp1" id="fillId" size="10">
												</tr>
													</tr>

  <tr><td colspan="3" height="20"><input type="hidden" name="Action" value="add">&nbsp;</td></tr>
													<tr>
													<td>&nbsp;</td>
													<td colspan="5" align="center">
														<input type="button" class="input-button" value="<%write(com_save);%>"  onClick="save()">&nbsp;&nbsp;&nbsp;<input type="reset" class="input-button" value="<%write(com_reset);%>">&nbsp;&nbsp;&nbsp;<input type="button" value="<%write(com_help);%>" class="input-button" onClick="window.open('<% write(com_helppage); %>#UserGroup')">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_back);%>" onClick="GoBack_Page('apLoadBalanceList.asp')">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
													</td>
													
													<td>&nbsp;</td>
												</tr>
												<tr>
													<td colspan="7" height="30">&nbsp;</td>
								
												</tr>
												
						</table>
			
									  
									  
							</td>
						</tr>
		
	    </table>
						
						</td>
					</tr>
				</table>
				
			</form>
			
			</td>
			
		 </tr>
   
    </table>
  	
  </td>
  </tr>

</table>
</body>
</html>
