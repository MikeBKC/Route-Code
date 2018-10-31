<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<%langTran("common");%>
<script type="text/javascript">
<% aspGetScheduler(); %>

</script>
<link href="css/style_new.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="script/jquery-1.8.1.min.js"></script> 
<script type="text/javascript" src="script/frame.js"></script>
<script type="text/javascript" src="js/common.js"></script> 
<style>
	.demotable>tr>td{
			padding-top:10px;
			padding-bottom:10px;
		
		}
		
	
	</style>
</head>
<script type="text/javascript">


function save()
{
	var tname = $('#name').val();
//	var tinter = $('#task_interval').val();
//	var tweek = $('#weeks').val();
	var thours = Number($('#hours').val());
	var tmins = Number($('#mins').val());

	
	if(tname == '' || !(tname.match(/^[^ |<|>|,|%|'|&|"|;|\\|:]{0,}$/))){
		alert('�������Ʋ���Ϊ�գ���������:<>,%\'\\"&;|�Ϳո��ַ�');
		return false;
	}
	var namelength = 0; // ���Ƶĳ���
	var sarr = tname.split('');
	sarr.forEach(function(obj){
		if(obj.match(/[^\u0000-\u00FF]/)){
			namelength += 2;
		}else{
			namelength ++;
		}
	});
	
	if(namelength>31){
		alert(�������Ƴ������Ϊ31�ַ�);
		return false;
	}
	
	
	if(!thours || thours<0 || thours>23){
		alert('ʱ���ʽ����Сʱ��Χ0-23�����ӷ�Χ0-59');
		return false;
	}
	if(!tmins || tmins<0 || tmins>59){
		alert('ʱ���ʽ����Сʱ��Χ0-23�����ӷ�Χ0-59');
		return false;
	}
  	knowifi_config.action="/goform/formTaskEdit";
  	knowifi_config.submit();
	
}

</script>
<body>

<div style="width:500px; margin:0 auto; text-align:center;">
<br>
<form action="" method="post" name="knowifi_config" id="knowifi_config">
<table border="0" cellpadding="0" cellspacing="1" id="knowifiConfig" style="background:#5e5e5e;"class="group" width="500px">
    <tr class="tab_info">
	    <td width="140px">��������</td>
	    <td colspan="2" width="360px">
	    	<input type="text" style="width:160px;height:25px" name="ID" id="name"/>
	    </td>
    </tr>
    <tr class="tab_info">
	    <td width="140px">������</td>
	    <td colspan="2" width="360px">
	    	<select style="width:160px;height:25px" name="selDateType" id="task_interval">
	    		<option value="01">ÿ����</option>
	    		<option value="02">ÿ��</option>
	    		
	    		
	    	</select>
	    </td>
    </tr>
    <tr class="tab_info" id="choose_time">
	    <td width="140px">����ʱ��</td>
	    <td colspan="2" width="360px">
	    	<select style="width:160px;height:25px" name="selDay" id="weeks">
	    		<option value="01">����һ</option>
	    		<option value="02">���ڶ�</option>
	    		<option value="03">������</option>
	    		<option value="04">������</option>
	    		<option value="05">������</option>
	    		<option value="06">������</option>
	    		<option value="00">������</option>
	    	</select> 
	    	<input type="number" id="hours" name="txtHour1" max="23" min="00" style="width:40px;height:20px;position:relative;top:-1" /> : 
	    	<input type="number" id="mins"  name="txtMin1" max="59" min="00"style="width:40px;height:20px;position:relative;top:-1" /> : 
	    	<input type="number" id="secs" name="txtSec1" max="59" min="00" style="width:40px;height:20px;position:relative;top:-1" />
	    </td>
    </tr>
	<tr class="tab_info">
	    <td width="140px">��������</td>
	    <td colspan="2" width="360px">
	    	<select style="width:160px;height:25px" name="selContent" id="selContent">
	    		<option value="reboot">����</option>
	    	</select>
	    </td>
    </tr>

    <tr  height="60px" class="tab_savetd" valign="top"> 
	    <td colspan="3" align="left" style="padding-left:10px; padding-top:20px;">
		    <table cellpadding="0" cellspacing="0" border="0" width="450">
			    <tr valign="top">
					<td colspan="3" align="left"><span id="tips"></span>
				    </td>
			    </tr>
			    <tr>
				    <td colspan="2">&nbsp;<span id="error_msg"  style="color:red;">    </span></td>
				    <td align="right"><input type="button" class="btn" name="back" id="savebtn" onClick="save();"Value="����"/></td>
			    </tr>
   			</table>
    	</td>
    </tr>
</table>
</form>
<br>
<br>
</div>

<script>
	/*��ֵ*/

	$('#name').val((flag == 1)?ID:'');
	//alert(selDateType)
	$('#task_interval').val((flag == 1)?selDateType:'');
	$('#weeks').val((flag == 1)?selDay:'');
	$('#hours').val((flag == 1)?txtHour1:'');
	$('#mins').val((flag == 1)?txtMin1:'');
//	$('#secs').val(secs);
//	$('#selContent').val(selContent);
	
	
	/* ʱ���� ���� ����ʱ�� */
	$('#task_interval').change(function(){
		taskIntervalChange()
	});
	taskIntervalChange()
	function taskIntervalChange(){
		var tval = $('#task_interval').val();
		var $p =  $('#choose_time');
		var $p1 = $('#weeks');
		var $p2 = $('#hours');
		var $p3 = $('#mins');
		var $p4 = $('#secs');
		if(tval== '01'){
			$p.show();
			$p1.show();
			$p2.removeAttr('disabled').attr('name','txtHour1');
			$p3.removeAttr('disabled').attr('name','txtMin1');
			$p4.attr('disabled','disabled').val('00').attr('name','txtSec1');
		}else if(tval== '02'){
			$p.show();
			$p1.hide();
			$p2.removeAttr('disabled').attr('name','txtHour2');
			$p3.removeAttr('disabled').attr('name','txtMin2');
			$p4.attr('disabled','disabled').val('00').attr('name','txtSec2');
		}else if(tval== '3'){
			$p.show();
			$p1.hide();
			$p2.attr('disabled','disabled').val('00');
			$p3.removeAttr('disabled');
			$p4.attr('disabled','disabled').val('00');
		}else if(tval== '4'){
			$p.hide();
			$p1.hide();
			$p2.attr('disabled','disabled').val('00');
			$p3.attr('disabled','disabled').val('00');
			$p4.attr('disabled','disabled').val('00');
		}
	}
</script>

</body>
</html>
