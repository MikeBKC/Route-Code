<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<title>集中管理配置</title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<link rel="stylesheet" type="text/css" href="css/npe_other.css">
<link rel="stylesheet" type="text/css" href="css/eg_n.css">
<script type="text/javascript" src="js/common.js"></script>
<script language="javascript">
//------------交互开始---------------
//　　var cwmpEnable="1";
//　　var cwmpType="1";
//　　var cwmpServerIp="112.124.31.88";
//　　var cwmpUsername="123"; 
//　　var cwmpPassword="123";

　　<%aspOutputCwmpConfig();%>
//-------------交互结束--------------------　
function enableCLK() {
	var f = form1;
	if (f.cwmpEnable.checked) {
		f.cwmpType.disabled = false;
		f.cwmpServerIp.disabled = false;
		f.cwmpUsername.disabled = false;
		f.cwmpPassword.disabled = false;

        /*
           f.cwmpAcsAuthEnable.checked = false;
           f.cwmpCpeAuthEnable.checked =false;
           f.cwmpStunEnable.checked = false;
           f.cwmpStunAuthEnable.checked = false;
         */
	} else {
		f.cwmpType.disabled = true;
		f.cwmpServerIp.disabled = true;
		f.cwmpUsername.disabled = true;	
		f.cwmpPassword.disabled = true;	

        /*
           f.cwmpAcsAuthEnable.checked =true;
           f.cwmpCpeAuthEnable.checked =true;
           f.cwmpStunEnable.checked = true;
           f.cwmpStunAuthEnable.checked = true;
         */
	}
}


function init(){
	var f=form1;
	f.cwmpEnable.checked = (cwmpEnable == '1')? true:false;
	f.cwmpType.value = cwmpType;
	f.cwmpServerIp.value = cwmpServerIp;
    
	f.cwmpAcsAuthEnable.checked = (cwmpAcsAuthEnable == '1')? true:false;
	f.cwmpUsername.value = cwmpUsername;
	f.cwmpPassword.value = cwmpPassword;

	f.cwmpCpeAuthEnable.checked = (cwmpCpeAuthEnable == '1')? true:false;
	f.cwmpCpeUsername.value = cwmpCpeUsername;
	f.cwmpCpePassword.value = cwmpCpePassword;

	f.cwmpStunEnable.checked = (cwmpStunEnable == '1')? true:false;
	f.cwmpStunIp.value = cwmpStunIp;
	f.cwmpStunAuthEnable.checked = (cwmpStunAuthEnable == '1')? true:false;
	f.cwmpStunUsername.value = cwmpStunUsername;
	f.cwmpStunPassword.value = cwmpStunPassword;
	enableCLK();

	
	}


function checkData(){
	if (form1.cwmpServerIp.value=="0.0.0.0"){ alert("服务端IP地址不能为0.0.0.0");return false};
	if (!checkIP(form1.cwmpServerIp,"服务端IP地址",0)) return false;

    /*
    if(!checkName1(form1.cwmpUsername,31,"用户名",1)) return false;  //检查用户名：不能超31个字符、应为除:< > , % \' \\ \" & ; 以及空格之外的字符� 
    if(!checkPassword1(form1.cwmpPassword, 31, 1, 0)) return false;  //检查密码：不能超过31个字符、应为除% \' \\ \"  , ；; 以及空格之外
	else
	return true;
    */
    return true;
	}
	
	
	
function save(){
	if(checkData()){
		var f=form1;
		form1.action = "/goform/formCwmpConfig";
		f.submit();
                alert("保存成功");
	}	
	}

function Reset(){
	form1.reset();
	init();
	
	}

</script>
</head>

<body class="rac" align="center">
<h2 class="tab">集中管理配置</h2>

<form name="form1" method="post">
<div class="root_box ">

    <div class="form-norm">
        <div class="form-group" relDom="RAC_box">
            <div class="form-label">集中管理(WMC)：</div>
            <div class="form-controls">
                <input type="checkbox" name="cwmpEnable" onclick="enableCLK()"/>
                <label for="RAC_checkbox">开启集中管理</label>
                <label class="help">配置WMC后，可以通过WMC系统集中管理设备。</label>
            </div>
        </div>

        <div class="form-group" relDom="RAC">
            <div class="form-label">管理类型：</div>
            <div class="form-controls">
                <select name="cwmpType">
                    <option value="1">WMC管理</option>
                </select>
            </div>
        </div>

        <div class="form-group" relDom="RAC">
            <div class="form-label">服务端Url：</div>
            <div class="form-controls">
                <input type="text" name="cwmpServerIp" class="input-txt" onKeyPress="ipOnly()" size="18" maxlength="50" onChange="showfiledtip(re_checkIP(this,'IP地址'),yutd1)" />
                <em class="noem">*</em><label id="yutd1"></label>
            </div>
        </div>

        <div class="form-group" relDom="RAC_box">
            <div class="form-label">ACS 认证开关：</div>
            <div class="form-controls">
                <input type="checkbox" name="cwmpAcsAuthEnable"/>
            </div>
        </div>

        <div class="form-group" relDom="RAC">
            <div class="form-label">ACS 用户名：</div>
            <div class="form-controls">
                <input type="text" class="input-txt" name="cwmpUsername" onChange="showfiledtip(re_checkSpecialName(this,'用户名'),usernametd)"/>
                <label id="usernametd"></label>
            </div>
        </div>

         <div class="form-group" relDom="RAC">
            <div class="form-label">ACS 密码：</div>
            <div class="form-controls">
                <input type="password" class="input-txt" name="cwmpPassword" onChange="showfiledtip(re_checkPassword1(this, 31, 1),passwd1tb)"/>
                <label id="passwd1tb"></label>
            </div>
        </div>

        <div class="form-group" relDom="RAC_box">
            <div class="form-label">CPE 认证开关：</div>
            <div class="form-controls">
                <input type="checkbox" name="cwmpCpeAuthEnable"/>
            </div>
        </div>
        <div class="form-group" relDom="RAC">
            <div class="form-label">CPE 用户名：</div>
            <div class="form-controls">
                <input type="text" class="input-txt" name="cwmpCpeUsername" onChange="showfiledtip(re_checkSpecialName(this,'用户名'),usernametd)"/>
                <label id="usernametd"></label>
            </div>
        </div>

         <div class="form-group" relDom="RAC">
            <div class="form-label">CPE 密码：</div>
            <div class="form-controls">
                <input type="password" class="input-txt" name="cwmpCpePassword" onChange="showfiledtip(re_checkPassword1(this, 31, 1),passwd1tb)"/>
                <label id="passwd1tb"></label>
            </div>
        </div>

        <div class="form-group" relDom="RAC_box">
            <div class="form-label">STUN 开关：</div>
            <div class="form-controls">
                <input type="checkbox" name="cwmpStunEnable"/>
            </div>
        </div>
        <div class="form-group" relDom="RAC">
            <div class="form-label">STUN 地址：</div>
            <div class="form-controls">
                <input type="text" class="input-txt" name="cwmpStunIp"/>
                <label id="usernametd"></label>
            </div>
        </div>

        <div class="form-group" relDom="RAC_box">
            <div class="form-label">STUN 认证开关：</div>
            <div class="form-controls">
                <input type="checkbox" name="cwmpStunAuthEnable"/>
            </div>
        </div>
        <div class="form-group" relDom="RAC">
            <div class="form-label">STUN 用户名：</div>
            <div class="form-controls">
                <input type="text" class="input-txt" name="cwmpStunUsername" onChange="showfiledtip(re_checkSpecialName(this,'用户名'),usernametd)"/>
                <label id="usernametd"></label>
            </div>
        </div>

         <div class="form-group" relDom="RAC">
            <div class="form-label">STUN 密码：</div>
            <div class="form-controls">
                <input type="password" class="input-txt" name="cwmpStunPassword" onChange="showfiledtip(re_checkPassword1(this, 31, 1),passwd1tb)"/>
                <label id="passwd1tb"></label>
            </div>
        </div>

        <div class="form-group">
            <div class="form-controls">
                 <button type="button" class="button_bdeep" onclick="save()">保存</button>&nbsp;&nbsp;<button type="button" class="button_bdeep" onclick="Reset()">重置</button>
            </div>
        </div>
    </div>

</div>
</form>
<script language="javascript" type="text/javascript">init();</script>
</body>

</html>
