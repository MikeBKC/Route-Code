<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<title>���й�������</title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<link rel="stylesheet" type="text/css" href="css/npe_other.css">
<link rel="stylesheet" type="text/css" href="css/eg_n.css">
<script type="text/javascript" src="js/common.js"></script>
<script language="javascript">
//------------������ʼ---------------
//����var cwmpEnable="1";
//����var cwmpType="1";
//����var cwmpServerIp="112.124.31.88";
//����var cwmpUsername="123"; 
//����var cwmpPassword="123";

����<%aspOutputCwmpConfig();%>
//-------------��������--------------------��
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
	if (form1.cwmpServerIp.value=="0.0.0.0"){ alert("�����IP��ַ����Ϊ0.0.0.0");return false};
	if (!checkIP(form1.cwmpServerIp,"�����IP��ַ",0)) return false;

    /*
    if(!checkName1(form1.cwmpUsername,31,"�û���",1)) return false;  //����û��������ܳ�31���ַ���ӦΪ��:< > , % \' \\ \" & ; �Լ��ո�֮����ַ�� 
    if(!checkPassword1(form1.cwmpPassword, 31, 1, 0)) return false;  //������룺���ܳ���31���ַ���ӦΪ��% \' \\ \"  , ��; �Լ��ո�֮��
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
                alert("����ɹ�");
	}	
	}

function Reset(){
	form1.reset();
	init();
	
	}

</script>
</head>

<body class="rac" align="center">
<h2 class="tab">���й�������</h2>

<form name="form1" method="post">
<div class="root_box ">

    <div class="form-norm">
        <div class="form-group" relDom="RAC_box">
            <div class="form-label">���й���(WMC)��</div>
            <div class="form-controls">
                <input type="checkbox" name="cwmpEnable" onclick="enableCLK()"/>
                <label for="RAC_checkbox">�������й���</label>
                <label class="help">����WMC�󣬿���ͨ��WMCϵͳ���й����豸��</label>
            </div>
        </div>

        <div class="form-group" relDom="RAC">
            <div class="form-label">�������ͣ�</div>
            <div class="form-controls">
                <select name="cwmpType">
                    <option value="1">WMC����</option>
                </select>
            </div>
        </div>

        <div class="form-group" relDom="RAC">
            <div class="form-label">�����Url��</div>
            <div class="form-controls">
                <input type="text" name="cwmpServerIp" class="input-txt" onKeyPress="ipOnly()" size="18" maxlength="50" onChange="showfiledtip(re_checkIP(this,'IP��ַ'),yutd1)" />
                <em class="noem">*</em><label id="yutd1"></label>
            </div>
        </div>

        <div class="form-group" relDom="RAC_box">
            <div class="form-label">ACS ��֤���أ�</div>
            <div class="form-controls">
                <input type="checkbox" name="cwmpAcsAuthEnable"/>
            </div>
        </div>

        <div class="form-group" relDom="RAC">
            <div class="form-label">ACS �û�����</div>
            <div class="form-controls">
                <input type="text" class="input-txt" name="cwmpUsername" onChange="showfiledtip(re_checkSpecialName(this,'�û���'),usernametd)"/>
                <label id="usernametd"></label>
            </div>
        </div>

         <div class="form-group" relDom="RAC">
            <div class="form-label">ACS ���룺</div>
            <div class="form-controls">
                <input type="password" class="input-txt" name="cwmpPassword" onChange="showfiledtip(re_checkPassword1(this, 31, 1),passwd1tb)"/>
                <label id="passwd1tb"></label>
            </div>
        </div>

        <div class="form-group" relDom="RAC_box">
            <div class="form-label">CPE ��֤���أ�</div>
            <div class="form-controls">
                <input type="checkbox" name="cwmpCpeAuthEnable"/>
            </div>
        </div>
        <div class="form-group" relDom="RAC">
            <div class="form-label">CPE �û�����</div>
            <div class="form-controls">
                <input type="text" class="input-txt" name="cwmpCpeUsername" onChange="showfiledtip(re_checkSpecialName(this,'�û���'),usernametd)"/>
                <label id="usernametd"></label>
            </div>
        </div>

         <div class="form-group" relDom="RAC">
            <div class="form-label">CPE ���룺</div>
            <div class="form-controls">
                <input type="password" class="input-txt" name="cwmpCpePassword" onChange="showfiledtip(re_checkPassword1(this, 31, 1),passwd1tb)"/>
                <label id="passwd1tb"></label>
            </div>
        </div>

        <div class="form-group" relDom="RAC_box">
            <div class="form-label">STUN ���أ�</div>
            <div class="form-controls">
                <input type="checkbox" name="cwmpStunEnable"/>
            </div>
        </div>
        <div class="form-group" relDom="RAC">
            <div class="form-label">STUN ��ַ��</div>
            <div class="form-controls">
                <input type="text" class="input-txt" name="cwmpStunIp"/>
                <label id="usernametd"></label>
            </div>
        </div>

        <div class="form-group" relDom="RAC_box">
            <div class="form-label">STUN ��֤���أ�</div>
            <div class="form-controls">
                <input type="checkbox" name="cwmpStunAuthEnable"/>
            </div>
        </div>
        <div class="form-group" relDom="RAC">
            <div class="form-label">STUN �û�����</div>
            <div class="form-controls">
                <input type="text" class="input-txt" name="cwmpStunUsername" onChange="showfiledtip(re_checkSpecialName(this,'�û���'),usernametd)"/>
                <label id="usernametd"></label>
            </div>
        </div>

         <div class="form-group" relDom="RAC">
            <div class="form-label">STUN ���룺</div>
            <div class="form-controls">
                <input type="password" class="input-txt" name="cwmpStunPassword" onChange="showfiledtip(re_checkPassword1(this, 31, 1),passwd1tb)"/>
                <label id="passwd1tb"></label>
            </div>
        </div>

        <div class="form-group">
            <div class="form-controls">
                 <button type="button" class="button_bdeep" onclick="save()">����</button>&nbsp;&nbsp;<button type="button" class="button_bdeep" onclick="Reset()">����</button>
            </div>
        </div>
    </div>

</div>
</form>
<script language="javascript" type="text/javascript">init();</script>
</body>

</html>
