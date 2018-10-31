<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<title>Web Authentication</title>
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<!-- css3-mediaqueries.js for IE8 or older -->
<!--[if lt IE 9]>
	<script src="js/css3-mediaqueries.js"></script>
<![endif]-->
</head>
<style type="text/css">
html,body{ width:100%; height:100%; margin:0; padding:0; /*background-color: #ecf8f8;*/ color:#656565;font-family: Microsoft Yahei, Verdana, Arial, Sans-Serif; }
.header{ width:100%; height:38%;  }
.main{ margin:-3% auto; text-align: center; width:450px; /*border:1px solid #CF3; */z-index:150;background-color: #ecf4f5;}
.tabs{ width:100%; z-index:170;background-color:#ffffff;width:450px; }
#phonelogin,#weixinlogin{ margin-right:30px;}
a{ padding:5px; width:50px; cursor:pointer;}
a#phonelogin{ background:url(images/phone.png) no-repeat; background-position:left center; padding-bottom:5px; padding-left:15px; padding-top:5px;}
a#weixinlogin{ background:url(images/weixin.png) no-repeat; background-position:left center; padding-bottom:5px; padding-left:30px;padding-top:5px;}
a#accountlogin{ background:url(images/account.png) no-repeat; background-position:left center; padding-bottom:5px; padding-left:32px;padding-top:5px;}
.header_tr{ height:50px;}
input[type='text'],input[type='password']{ width:130px; height:15px;}
.button{ padding-bottom:3px; padding-top:4px;text-align: center;font-weight: bold;color: #656565;background-color:#DAECEE;border: none;border-radius: 5px; width:100px; }
a#phonelogin:hover, a.selected a#phonelogin{background-color: #fff; color:#008893; }
a#weixinlogin:hover,  a.selected a#weixinlogin{background-color: #fff;color:#008893;}
a#accountlogin:hover, a.selected a#accountlogin{background-color: #eaeceb;color:#008893;}
a:hover,  a.selected a{background-color: #000000;-moz-border-radius: 5px;-webkit-border-radius: 5px;border-radius: 5px;}
#errormsg{ color:red;}	

@media screen and (max-width: 600px){
.main,.tabs{ width:300px;}
#phonelogin,#weixinlogin{ margin-right:15px;}
a#phonelogin{ background: none;  padding-bottom:5px; padding-left:0px; padding-top:5px;}
a#weixinlogin{ background:none; padding-bottom:5px; padding-left:0px;padding-top:5px;}
a#accountlogin{ background:none; padding-bottom:5px; padding-left:0px;padding-top:5px;}
#phone_td{ vertical-align:top;}	
#phonenum{ margin-bottom:6px;}
}
</style>
<script type="text/javascript">
<% aspOutputWebAuthMessage(); %>/*������������������޸�*/
<%aspOutputTips();%>/*������������������޸�*/
//var idcardauth = "0";
//var smsauth  = "1";
//var weixinauth = "1";/*������������������޸�*/
//var currentType="0";//��ǰĬ����ʾ�ĵ�¼��ʽ0���˺������¼ 1���ֻ���������֤��2��΢����֤
</script>
<body style=" z-index:-1;background-color: #f7f7f7;" onLoad="init()">
<div class="header" style="position:; ">
	<script type="text/javascript">
	//activePic=1;en_picture =0;picturedir="images/5.jpg";pictureUrl="";
    if(activePic == 1){
		if((en_picture == 1 || en_picture == 3)&&(picturedir != ""))
		{
		document.write('<img src="'+picturedir+'" width="100%" width="100%" height="38%" style="position:absolute; z-index:-9999;" />');
		}else if((en_picture == 0 || en_picture == 2)&&(pictureUrl != "")){
		document.write('<img src="'+pictureUrl+'" width="100%" width="100%" height="38%" style="position:absolute; z-index:-9999;" />');
		}else{
		document.write('<a width="100%" height="38%" style="position:absolute; z-index:-9999;">&nbsp;</a>');
		}
	}
    
    </script>
	<!--<img src="images/5.jpg" width="100%" height="38%" style="position:absolute; z-index:-9999;"/>-->
</div>
<div class="main" style="z-index:-9999;">
<form action="" method="post" name="webAuth">
	<table cellpadding="0" cellspacing="0" border="0" class="tabs" style=" border: 1px #000 solid;">
    	<tr style="background-color:#ecf4f9" class="header_tr">
		<td>&nbsp;</td>
        	<td align="center">
            	<a id="phonelogin" onClick="phone();" >�ֻ���¼</a><a id="weixinlogin" onClick="weixin();" >΢�ŵ�¼</a><a id="accountlogin" onClick="account();">�˺ŵ�¼</a>
		</td>
		<td>&nbsp;</td>
        </tr>
        <tr>
        	<td colspan="3" id="errormsg" align="center"></td>
        </tr>
         <tr>
        	<td>&nbsp;</td>
            <td align="center">
            	<table cellpadding="0" cellspacing="0" border="0" id="accountTab" style="display:;">
                	<tr height="30px">
                    	<td>&nbsp;</td>
                        <td align="right">�˺�&nbsp;</td>
                        <td><input type="text" name="userName" id="userName"/></td>
                    </tr>
                    <tr height="30px">
                    	<td>&nbsp;</td>
                        <td align="right">����&nbsp;</td>
                        <td><input type="password" name="userPasswd" id="userPasswd"/></td>
                    </tr>
                    <tr>
                    	<td colspan="3">&nbsp;</td>
                    </tr>
                    <tr height="30px">
                    	<td colspan="3" align="center"><input type="button" value="��¼" onClick="accountLogin();" class="button" /></td>
                    </tr>
                </table>
                <table cellpadding="0" cellspacing="0" border="0" id="phoneTab" style="display:none;">
                	<tr height="30px">
                    	<td>&nbsp;</td>
                        <td width="30%" id="phone_td" align="right">�ֻ���&nbsp;</td>
                        <td align="left"><input type="text" name="phonenum"  id="phonenum"/>&nbsp;<input type="button" id="phonebtn" value="��ȡ��֤��" class="button" onClick="getCode();" /></td>
                    </tr>
                    <tr height="30px">
                    	<td>&nbsp;</td>
                        <td align="right">��֤��&nbsp;</td>
                        <td align="left"><input type="text" name="phonecode" id="phonecode"/></td>
                    </tr>
                    <tr>
                    	<td colspan="3">&nbsp;</td>
                    </tr>
                    <tr height="30px">
                    	<td colspan="3" align="center"><input type="button" value="��¼" onClick="phoneLogin();" class="button" /></td>
                    </tr>
                </table>
                <table cellpadding="0" cellspacing="0" border="0" id="weixinTab" style="display:none;">
                
                	<tr>
                    	<td colspan="3" align="center"><img name="weixinpic" id="weixinpic" src="images/weixinpic.jpg" width="180px" /></td>
                       
                    </tr>
                	<tr height="30px">
                    	<td>&nbsp;</td>
                        <td width="30%" align="right">΢�ź�&nbsp;</td>
                        <td align="left"> <label id="weixinname"></label></td>
                    	<td>&nbsp;</td>
                    </tr>
                    <tr height="30px">
                    	<td>&nbsp;</td>
                        <td align="right">��֤��&nbsp;</td>
                        <td align="left"><input type="text" name="weixincode" id="weixincode"/></td>
                    </tr>
                    <tr>
                    	<td colspan="3">&nbsp;</td>
                    </tr>
                    <tr height="30px">
                    	<td>&nbsp;</td>
			<td colspan="3" align="center"><input type="button" value="��¼" onClick="weixinLogin();" class="button" /></td>
                    	<td>&nbsp;</td>
                    </tr>
                </table>
            </td>
            <td>&nbsp;</td>
        </tr>
        <tr height="25px">
        	<td colspan="3">&nbsp;</td>
        </tr>
         <tr height="25px" id="contact" >
         	<td></td>
        	<td style="word-break:break-all" ><% aspOutputContactData("html");%></td>
            <td></td>
        </tr>
	<tr height="25px" style="background-color:#ecf4f9">
	        <td colspan="3">&nbsp;</td>
	</tr>
    </table>
    </form>
</div>
<script type="text/javascript" >

function init(){

	if(smsauth == "1")
	       document.getElementById("phonelogin").style.display ="";
	else
	       document.getElementById("phonelogin").style.display ="none";
        if(weixinauth == "1")
	       document.getElementById("weixinlogin").style.display =""; 
	else
	       document.getElementById("weixinlogin").style.display ="none";
	document.getElementById("accountlogin").style.display =""; 

	 document.getElementById("phonelogin").style.backgroundColor="#ecf4f9";
	 document.getElementById("weixinlogin").style.backgroundColor="#ecf4f9";
	 document.getElementById("accountlogin").style.backgroundColor="#ecf4f9";
	 if (weixinPicExist == "1")
	     document.getElementById("weixinpic").style.display="";
	 else
	     document.getElementById("weixinpic").style.display="none";
	 if( currentType == "1"){
		phone();
	 }else if( currentType == "0"){
		account();
	 }else{
		weixin();
	 }

	document.getElementById("phonebtn").disabled=false;
	document.getElementById("weixinname").innerHTML=wechatNames;

/*	  var message_web = "http://" + ip + "/WebAuth_message.asp"
//	if (authResult == "��֤�ɹ���")
//	{       
//	    window.open(message_web, '_blank', 'height=200, width=400, toolbar=no, menubar=no, scrollbars=no, resizable=no, location=no, status=no, top=0, left=0'); 
//	}
	else if (authResult == "��֤ʧ�ܣ�")
	{
		document.getElementById("errormsg").innerHTML=authResult; 
	}else if (authResult != ""){
		document.getElementById("errormsg").innerHTML=authResult; 	
	}*/
	
	if(en_picture == 2 || en_picture == 3){
		document.getElementById("contact").style.display="none";	
	}	
}


/***************************
ajax�������ɺ���
***************************/
function ajax(){
	var aj=new Object();
    aj.url = null;//Զ��ҳ��
	aj.erroInfo="Զ���������ɴ�";//����ʱ����ʾ��Ϣ
	aj.isAutoRefresh=1;//�Ƿ��Զ�ˢ��(�Զ��Ӻ�̨��ȡ����)
	aj.refreshTime=2;//ˢ��ʱ�������룩
	aj.ptr=null;//�Զ�ˢ�� ����id
	aj.method='get';//ax���̨�������ݵķ�ʽpost or get
	aj.xmlHttp=null;//xmlHttp��
	aj.sendString=null;//POST��ʽʱPOST������
	aj.start=function() {
		var fun=(aj.method=='get'?aj.get:aj.post);
		fun();
        if(aj.isAutoRefresh) aj.ptr=setInterval(fun,aj.refreshTime*1000);
	}	
	aj.Stop=function()
	{
		clearInterval(aj.ptr);
	}

	/******************************
	post��ʽ
	*******************************/
	aj.post = function(newUrl, sendString) {
	    /*ȥ���Ƿ�������������һ������Ϊ�¼�����*/
	    if(typeof(newUrl)!="string") newUrl = "";
	    if(typeof(sendString)!="string") sendString = "";
	    aj.xmlHttp.open('POST', newUrl || aj.url,true);
		aj.xmlHttp.onreadystatechange =  aj.xmlHttpStateChange ;/*ie 7/6�汾ajaxÿ��ִ�ж�������ָ��onreadystatechange*/
	    aj.xmlHttp.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
	    aj.xmlHttp.send(sendString||aj.sendString);
	}
	/********************************************
	��������:������Զ���������ɴ�
	********************************************/
   	aj.erroProcessHandle= function() {
       // alert(aj.erroInfo);
		return;
    }
	/*******************************************
	ajax�����ı�������
	********************************************/
    aj.processHandle=function(str) {
    }
	/******************************
	xmlHttp��ʼ��
	*******************************/
	aj.creatXmlHttp=function()
	{
        if (window.XMLHttpRequest) { // For Mozilla, Safari, ...
            aj.xmlHttp = new XMLHttpRequest();
        } else if (window.ActiveXObject) { // IE�����
            try {
                aj.xmlHttp = new ActiveXObject("Msxml2.XMLHTTP");
            } catch(e) {
                try {
                    aj.xmlHttp = new ActiveXObject("Microsoft.XMLHTTP");
                } catch(e) {}
            }
        }
        if (!aj.xmlHttp) { // �쳣����������ʵ��ʧ��
            alert("�����쳣");
            return false;
        }
		/*ie 7/6�汾ajaxÿ��ִ�ж�������ָ��onreadystatechange
		�ʲ����ڴ�ֱ��ָ��onreadystatechange����
		*/
		aj.xmlHttpStateChange = function() {
    //        if (aj.xmlHttp.overrideMimeType) { //����MiME���
    //            aj.xmlHttp.overrideMimeType("text/plain");
    //        }
            if (aj.xmlHttp.readyState == 4) {
                if (aj.xmlHttp.status == 200) {
                    aj.processHandle(aj.xmlHttp.responseText);
                } else {
                    aj.erroProcessHandle();
                }
            }
        }
	}
	aj.creatXmlHttp();
	if(!aj.xmlHttp) return false;	
	return aj;
}
var aj = ajax();

var wait = 60;

function timeout()
{
    if (wait <= 0) {
	document.getElementById("phonebtn").disabled=false;
	document.getElementById("phonebtn").value="��ȡ��֤��";
	wait=60;
    } else {
	document.getElementById("phonebtn").value=wait+"�����ط�";
	wait--;
	setTimeout('timeout();',1000);
    }

}

//��ȡ��֤��
function getCode(){
    var tbl = webAuth;
    if(tbl.phonenum.value==""){
	document.getElementById("errormsg").innerHTML="�ֻ����벻��Ϊ�գ�";
	return;	 
    }
    var mobileReg =/^1[3458]\d{9}$/; 
    if (!mobileReg.test(tbl.phonenum.value)){  
	document.getElementById("errormsg").innerHTML="�ֻ������ʽ����ȷ�����������룡";
	return ;
    }
	//var aj = ajax();
	var num = document.getElementById("phonenum").value;
	var sendString="phonenum="+num;
	
    var url = "http://" + ip + "/goform/formWebAuthGetPhoneSubmit";
	aj.post(url,sendString);
	 aj.processHandle=function(str){
	  //  alert("str:"+str);
	     eval(str);
	     document.getElementById("errormsg").innerHTML=errorstr;
	 }
   // tbl.submit();

	document.getElementById("phonebtn").disabled=true;
	timeout();
}

function copyWeixin(){
    if (window.clipboardData) {
	window.clipboardData.clearData();
	window.clipboardData.setData("Text", wechatNames);
    } else if (navigator.userAgent.indexOf("Opera") != -1) {
	window.location = wechatNames;
    } else if (window.netscape) {
	try {
	    netscape.security.PrivilegeManager
		.enablePrivilege("UniversalXPConnect");
	} catch (e) {
	    alert("��ʹ�õ�FireFox�����,���ƹ��ܱ�������ܾ���\n�����������ַ�����롰about:config�����س���\nȻ�󽫡�signed.applets.codebase_principal_support��˫��������Ϊ��true��");
	    return;
	}
	var clip = Components.classes['@mozilla.org/widget/clipboard;1']
	    .createInstance(Components.interfaces.nsIClipboard);
	if (!clip)
	    return;
	var trans = Components.classes['@mozilla.org/widget/transferable;1']
	    .createInstance(Components.interfaces.nsITransferable);
	if (!trans)
	    return;
	trans.addDataFlavor('text/unicode');
	var str = new Object();
	var len = new Object();
	var str = Components.classes["@mozilla.org/supports-string;1"]
	    .createInstance(Components.interfaces.nsISupportsString);
	var copytext = wechatNames;
	str.data = copytext;
	trans.setTransferData("text/unicode", str, copytext.length * 2);
	var clipid = Components.interfaces.nsIClipboard;
	if (!clip)
	    return false;
	clip.setData(trans, null, clipid.kGlobalClipboard);
    }
    else{
	alert("����������֧��һ�����ƹ��ܣ��ֻ����Գ���Ҫ���Ƶ�����ѡ�к���и���");
	return;
    }
    //alert("���Ƴɹ�");
}

function accountLogin(){
    var tbl = webAuth;
    if(tbl.userName.value==""){
	document.getElementById("errormsg").innerHTML="�û�������Ϊ��";
	return;  
    }
    if(tbl.userPasswd.value==""){
	document.getElementById("errormsg").innerHTML="���벻��Ϊ��";
	return ;	
    }
    //tbl.action = "http://" + ip + "/goform/formWebAuthUserSubmit";
    //tbl.submit();		
//	var aj = ajax();
	var acct = document.getElementById("userName").value;
	var pswd = document.getElementById("userPasswd").value;
	var sendString="userName="+acct+"&userPasswd="+pswd;
	
    var url = "http://" + ip + "/goform/formWebAuthUserSubmit";
	aj.post(url,sendString);
	 aj.processHandle=function(str){
	//     alert("str:"+str);
	     eval(str);
	     document.getElementById("errormsg").innerHTML=errorstr;
	     var message_web = "http://" + ip + "/WebAuth_message.asp";
	  //alert("test:"+(errorstr == "��֤�ɹ���")+"errorstr:"+errorstr);
            if (errorstr == "��֤�ɹ���"){       
	    //window.open(message_web, '_blank','width=500,height=400 ,scrollbars=yes'); 
	    window.location.href=message_web;
	}
	 }
}
function weixinLogin(){
    var tbl = webAuth;
    if(tbl.weixincode.value==""){
	document.getElementById("errormsg").innerHTML="��֤�벻��Ϊ�գ�";
	return;  
    }

//	var aj = ajax();
	var num = document.getElementById("weixincode").value;
	var sendString="weixincode="+num;
	
    var url = "http://" + ip + "/goform/formWebAuthCaptchaSubmit";
	aj.post(url,sendString);
	 aj.processHandle=function(str){
	     eval(str);
	    document.getElementById("errormsg").innerHTML=errorstr;
	  var message_web = "http://" + ip + "/WebAuth_message.asp";
	  //alert("test:"+(errorstr == "��֤�ɹ���")+"errorstr:"+errorstr);
	if (errorstr == "��֤�ɹ���")
	{       
	    //window.open(message_web, '_blank', 'height=200, width=400, toolbar=no, menubar=no, scrollbars=no, resizable=no, location=no, status=no, top=0, left=0'); 
	    window.location.href=message_web;
	}
	 }
}
function phoneLogin(){
    var tbl = webAuth;
    if(tbl.phonecode.value==""){
	document.getElementById("errormsg").innerHTML="��֤�벻��Ϊ�գ�";
	return ;	
    }
//	var aj = ajax();
	var num = document.getElementById("phonecode").value;
	var sendString="phonecode="+num;
	
    var url = "http://" + ip + "/goform/formWebAuthCaptchaSubmit";
	aj.post(url,sendString);
	 aj.processHandle=function(str){
//	     alert("str:"+str);
	     eval(str);
	    document.getElementById("errormsg").innerHTML=errorstr;
	 
	  var message_web = "http://" + ip + "/WebAuth_message.asp";
	  //alert("test:"+(errorstr == "��֤�ɹ���")+"errorstr:"+errorstr);
	if (errorstr == "��֤�ɹ���")
	{       
	    //window.open(message_web, '_blank', 'height=200, width=400, toolbar=no, menubar=no, scrollbars=no, resizable=no, location=no, status=no, top=0, left=0'); 
	    window.location.href=message_web;
	}
    }
}
function phone(){
    document.getElementById("accountTab").style.display="none";	
    document.getElementById("phoneTab").style.display="";	
    document.getElementById("weixinTab").style.display="none";		
    document.getElementById("errormsg").innerHTML="";
    document.getElementById("phonelogin").style.backgroundColor="#daecee";
    document.getElementById("weixinlogin").style.backgroundColor="#ecf4f9";
    document.getElementById("accountlogin").style.backgroundColor="#ecf4f9";
    document.getElementById("errormsg").height="30px";
}

function account(){
    document.getElementById("accountTab").style.display="";	
    document.getElementById("phoneTab").style.display="none";	
    document.getElementById("weixinTab").style.display="none";	
    document.getElementById("errormsg").innerHTML="";			
    document.getElementById("phonelogin").style.backgroundColor="#ecf4f9";
    document.getElementById("weixinlogin").style.backgroundColor="#ecf4f9";
    document.getElementById("accountlogin").style.backgroundColor="#daecee";
    document.getElementById("errormsg").height="30px";
}
function weixin(){
    document.getElementById("accountTab").style.display="none";	
    document.getElementById("phoneTab").style.display="none";	
    document.getElementById("weixinTab").style.display="";	
    document.getElementById("errormsg").innerHTML="";		
    document.getElementById("phonelogin").style.backgroundColor="#ecf4f9";
    document.getElementById("weixinlogin").style.backgroundColor="#daecee";
    document.getElementById("accountlogin").style.backgroundColor="#ecf4f9";
    if (weixinPicExist == "1")
	document.getElementById("errormsg").height="10px";
    else
	document.getElementById("errormsg").height="30px";
}
</script>
</body>
</html>
