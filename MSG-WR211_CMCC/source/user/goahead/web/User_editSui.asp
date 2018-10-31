<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<% langTran("User_edit"); %>
<% langTran("common"); %> 
<title><%write(com_notitle);%></title>
<link href="css/style_new.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="script/jquery-1.8.1.min.js"></script> 

<script type="text/javascript" src="js/common.js"></script> 
<script type="text/javascript" src="script/frame.js"></script> 
</head>
<script type="text/javascript">
var usernames= "admin";var passwd1s= "admin";var passwd2s= "admin";var errorstr="";
function init(){
	$("#error_msg").html(errorstr);
	
	
}
$(function(){
    $("input[name='passwd1']").get(0).focus();
});

function checkData(){
	var f=PasswordSet_Config;
	if(!checkPassword_P(f.passwd1, f.passwd2, 31, 0, 0)) return false;	//检查2次的密码输入是否一致。
    //if(!checkPassword1_P(f.passwd1, 31, 0, 0)) return false;  //之前的密码检测方法，以下是瑞斯的新的密码检测方法

	if(!checkPassword1_Password(f.passwd1, 31, 0, 0)) return false;	//检查密码：不能超过31个字符、不能为空、应为除% \' \\ \"  , ；; 以及空格之外的字符。
	if(!checkPassword1_Password(f.passwd2, 31, 0, 0)) return false;	//检查密码：不能超过31个字符、不能为空、应为除% \' \\ \"  , ；; 以及空格之外的字符。
	if(!checkPassword_num_letters(f.passwd1))  return false;
    return true;
} 
function save(){

	if(checkData()){
		PasswordSet_Config.action="/goform/formUser";
		PasswordSet_Config.submit();	
	}
}

</script>
<body>
<div style="width:500px; margin:0 auto; text-align:center;">

<br>
	<form action="" method="post" name="PasswordSet_Config">
    
	<table border="0" cellpadding="0" cellspacing="1" style="background:#5e5e5e;" class="group" width="500px">
   
    	<tr class="tab_info">
        	<td width="160px"><%write(NPass);%></td>
            <td width="360px" colspan="2"><input name="passwd1"  type="password" style="width:138px" maxlength="31" class="txt-input" /></td>
        </tr>
        <tr class="tab_info">
        	<td><%write(ConfirmNewpasswd);%></td>
            <td colspan="2"><input name="passwd2" type="password" style="width:138px" maxlength="31" class="txt-input" /></td>
           
        </tr>
       
       
          <tr class="tab_savetd"> 
        	<td colspan="3" align="left" style="padding-left:10px; padding-top:4px;">
            	<table cellpadding="0" cellspacing="0" border="0" width="450">
                	
                	<tr height="45px">
                    	<td width="150px">&nbsp;</td>
                        <td align="left"><span id="error_msg"  style="color:red;"></span></td>
                        <td align="right"><input type="button" class="btn" name="back" id="savebtn" onClick="save();"value="<%write(com_save);%>"/></td>
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
