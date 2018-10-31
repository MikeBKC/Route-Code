// JavaScript Document
<%langTran("JS");%>
<%langTran("WirelessSui");%>
<%langTran("IPMac");%>
function swicth_act(action){
		var switch_b = $(".on-off").children("button");
		var status_now = switch_b.attr("class");
		//alert("status_now:"+status_now);
		if(status_now == "on"){
			//swicth_act("off");
			switch_b.removeClass("on");
			switch_b.addClass("off");
			//$("#ebabledwifi").attr("value","off");
		} else if (status_now == "off"){
			//swicth_act("on");
			switch_b.addClass("on");
			switch_b.removeClass("off");
		//	$("#ebabledwifi").attr("value","on");
		}
	}
function initEnabledButton(switch_b,status){
	//var switch_b = $(".on-off").children("button");
	if(status=="on"){
		switch_b.addClass("on");
		switch_b.removeClass("off");	
	}else{
		switch_b.removeClass("on");
		switch_b.addClass("off");	
	}
}



function checkIP_P(cntrl, tips, allownull) {
    var retips = "";
    var ip_str = trimstr(cntrl.value);
	
    cntrl.value = ip_str;
    if (ip_str == '') {
        if (!eval(allownull)) retips = tips + "<%write(JSAlert61);%>";
        else retips = '';
    } else if (!verifyDottedIP(ip_str)) retips = tips + "<%write(JSAlert14);%>";
	if(retips!=""){
			$("#error_msg").html(retips);
			if (cntrl.type == "text") cntrl.focus();
			return false;
		}
		$("#error_msg").html(retips);
	return true;
	
}

function checkNumber_P(cntrl, minNum, maxNum, tips, allownull, onlycheck) {
    var retips = "";
    var num = trimstr(cntrl.value);
	//alert("num:"+num);
    if (num == '') {
        if (!eval(allownull)) retips = tips + "<%write(JSAlert31);%>";
        else retips = '';
    } else if (!isInt(num)) retips = tips + "<%write(JSAlert39);%>";
    else if (eval(num) < minNum || eval(num) > maxNum) retips = tips + "<%write(JSAlert23);%>" + minNum + "<%write(JSAlert24);%>" + maxNum + "<%write(JSAlert25);%>";
	
    if (retips != '') {
       // if (!eval(onlycheck)) //inputErro(cntrl, retips);
		$("#error_msg").html(retips);
		if (cntrl.type == "text") cntrl.focus();
        return false;
    } else {
      //  if (!eval(onlycheck)) //inputRight(cntrl);
		$("#error_msg").html(retips);
        return true;
    }
}

/*
 * 检查主DNS
 */
function checkMainDns_P(cntrl, tips, allownull) {
    var retips = "";
    var ip_str = trimstr(cntrl.value);
    cntrl.value = ip_str;
    if (ip_str == '') {
        if (!eval(allownull)) retips = tips + "<%write(JSAlert31);%>";
        else retips = '';
    } else if (!verifyDottedIP(ip_str) || (ip_str == '0.0.0.0')) {
		retips = tips + "<%write(JSAlert14);%>";
	}
    if (retips != '') {
        $("#error_msg").html(retips);
        return false;
    } else {
       $("#error_msg").html(retips);
        return true;
    }
}
/*
 *检查名称
*/
function checkName_P(cntrl, maxlen, tips, allownull) {
    var retips = "";
	var name = typeof(cntrl) == 'object' ? cntrl.value: cntrl;
    name = trimstr(name);
    cntrl.value = name;
    if (name == "") {
        if (!eval(allownull)) retips = tips + "<%write(JSAlert31);%>";
        else retips = '';
    } else if (name == 0) {
        retips = tips + "<%write(JSAlert36);%>";
    } else if (name.lstrLen() > maxlen) {
        retips = tips + "<%write(JSAlert37);%>" + maxlen + "<%write(JSAlert29);%>";
    } else if (name.indexOf(";") != -1 || name.indexOf(",") != -1 || name.indexOf("%") != -1 || name.indexOf("\"") != -1 || name.indexOf("&") != -1 || name.indexOf("\'") != -1 || name.indexOf("\\") != -1 || name.indexOf("<") != -1 || name.indexOf(">") != -1) {
        retips = tips + "<%write(JSAlert19);%>";
    }
   // alert(retips);
    if (retips != '') {
       // typeof(cntrl) == 'object'?inputErro(cntrl, retips):inputErro(cntrl, retips,2);
	   $("#error_msg").html(retips);
        return false;
    } else {
       // inputRight(cntrl);
	   $("#error_msg").html(retips);
        return true;
    }
}
/*
 *检查名称,允许空格
*/
function checkNameAllowSpace_P(cntrl, maxlen, tips, allownull) {
    var retips = "";
	var name = typeof(cntrl) == 'object' ? cntrl.value: cntrl;
    name = trimstr(name);
    cntrl.value = name;
    if (name == "") {
        if (!eval(allownull)) retips = tips + "<%write(JSAlert31);%>";
        else retips = '';
    } else if (name == 0) {
        retips = tips + "<%write(JSAlert36);%>";
    } else if (name.lstrLen() > maxlen) {
        retips = tips + "<%write(JSAlert37);%>" + maxlen + "<%write(JSAlert29);%>";
    } else if (name.indexOf(";") != -1 || name.indexOf(",") != -1 || name.indexOf("%") != -1 || name.indexOf("\"") != -1 || name.indexOf("&") != -1 || name.indexOf("\'") != -1 || name.indexOf("\\") != -1 || name.indexOf("<") != -1 || name.indexOf(">") != -1) {
        retips = tips + "<%write(JSAlert62);%>";
    }
   // alert(retips);
    if (retips != '') {
       // typeof(cntrl) == 'object'?inputErro(cntrl, retips):inputErro(cntrl, retips,2);
	   $("#error_msg").html(retips);
        return false;
    } else {
       // inputRight(cntrl);
	   $("#error_msg").html(retips);
        return true;
    }
}
///////////////////////////////////////
//功能：检查Lan口是否合法ip地址（3110vf新增加的函数）
//参数说明:
//cntrl--控件名
//tips--提示信息
//allownull--1允许为空，0不允许为空
//返回值：true合法，false非法
//////////////////////////////////////
function checkLanIP_P(cntrl, tips, allownull) {
    ip_str = trimstr(cntrl.value);
    if (ip_str == "0.0.0.0") {
        alert(tips + "<%write(JSAlert32);%>");
        return false;
    }
    if (ip_str == "") {
        if (!eval(allownull)) {
            $("#error_msg").html(tips + "<%write(JSAlert31);%>");
            cntrl.focus();
            return false;
        } else return true;
    } else if (!verifyDottedIP2(ip_str)) {
        $("#error_msg").html(tips + "<%write(JSAlert34);%>");
        cntrl.focus();
        return false;
    } else return true;
}
///////////////////////////////////////
//功能：检查是否合法的子网掩码（3110vf新增加）
//参数说明:
//cntrl--控件名
//tips--提示信息
//allownull--1允许为空，0不允许为空
//返回值：true合法，false非法
//////////////////////////////////////
function checkMask_P(cntrl, tips, allownull) {

    var ip_str = trimstr(cntrl.value);

    var retips = '';
    if (ip_str == "") {
        if (!eval(allownull)) {
            retips = tips + "<%write(JSAlert31);%>";
        }
    } else if (!validateMask(ip_str)) {
        retips = tips + "<%write(JSAlert34);%>";
    }
    if (retips != '') {
        $("#error_msg").html(retips);
        return false;
    } else {
       $("#error_msg").html(retips);
        return true;
    }
}

///////////////////////////////////////
//功能：检查是否合法MAC地址
//参数说明:
//cntrl--控件名
//tips--提示信息
//allownull--1允许为空，0不允许为空
//noChange--1改变 ,0不改变(是否改变cntrl的值)
//返回值：true合法，false非法
//////////////////////////////////////
function checkMac_P(cntrl, tips, allownull, noChange) {
	var retips = '';
    var num = cntrl.value.toUpperCase();
    num = trimstr(num);
    if (num == "") {
        if (!eval(allownull)) {
            retips = tips + "<%write(JSAlert31);%>";
        } 
    } else {
        var length = num.length;
        for (var i = 0; i < length; i++) {
            onenum = num.substring(i, i + 1);
            twonum = num.substring(i + 1, i + 2);
            if ((onenum < "0" || onenum > "9") && (onenum < "a" || onenum > "f") && (onenum < "A" || onenum > "F")) {
                if (onenum == "-" || onenum == ":") {
                    if (twonum == "-" || twonum == ":") {
                        length = 0;
                        break;
                    } else {
                        num = num.substring(0, i) + num.substring(i + 1, length);
                        length--;
                    }
                } else {
                    length = 0;
                    break;
                }
            }
        }
        if (length != 12) {
            retips = tips + "<%write(JSAlert35);%>";
        } else {
            if (num == "000000000000" || num == "FFFFFFFFFFFF") {
                retips = tips + "<%write(JSAlert35);%>";
            }
        }
    }
	if (retips != '') {
       // inputErro(cntrl, retips);
	   $("#error_msg").html(retips);
        return false;
    } else {
       // inputRight(cntrl);
	   $("#error_msg").html(retips);
        if (noChange != 1) cntrl.value = num;
        return true;
    }
}


///////////////////////////////////////
//功能：检查是否合法小时
//参数说明:
//cntrl--控件名
//tipsObj--存放提示信息的空间id
//返回值：true合法，false非法
//////////////////////////////////////
function checkHour_P(cntrl, tips, tipsObj) {
	if(!tips) tips="<%write(JSAlert42);%>";
    var retips = '';
    var obj = tipsObj || cntrl;
    if (cntrl.value.length == 1) cntrl.value = "0" + cntrl.value;
    if (cntrl.value == "") {
        retips = tips + "<%write(JSAlert31);%>";
    } else if (!isNum(cntrl.value)) {
        retips = tips + "<%write(JSAlert14);%>";

    } else {
        var hour = toNumber(cntrl.value, 0, 2);
        if (hour < 0 || hour > 23) {
            retips = tips + "<%write(JSAlert14);%>";
        }
    }
    if (retips != '') {
       // inputErro(obj, retips);
	    $("#error_msg").html(retips);
        return false;
    } else {
       // inputRight(obj);
	    $("#error_msg").html(retips);
        return true;
    }
}
///////////////////////////////////////
//功能：检查是否合法分钟（秒）
//参数说明:
//cntrl--控件名
//tipsObj--存放提示信息的空间id
//返回值：true合法，false非法
//////////////////////////////////////
function checkMinute_P(cntrl, tips, tipsObj) {
	if(!tips) tips="<%write(JSAlert43);%>";
    var retips = '';
    var obj = tipsObj || cntrl;
    if (cntrl.value.length == 1) cntrl.value = "0" + cntrl.value;
    if (cntrl.value == "") {
        retips = tips + "<%write(JSAlert31);%>";
    } else if (!isNum(cntrl.value)) {
        retips = tips + "<%write(JSAlert34);%>";

    } else {
        var minute = toNumber(cntrl.value, 0, 2);
        if (minute < 0 || minute > 59) {
            retips = tips + "<%write(JSAlert34);%>";
        }
    }
    if (retips != '') {
        //inputErro(obj, retips);
		 $("#error_msg").html(retips);
        return false;
    } else {
        //inputRight(obj);
		 $("#error_msg").html(retips);
        return true;
    }
}
///////////////////////////////////////
//功能：检查是否合法秒钟
//参数说明:
//cntrl--控件名
//tipsObj--存放提示信息的空间id
//返回值：true合法，false非法
//////////////////////////////////////
function checkSecond_P(cntrl, tips, tipsObj) {
	if(!tips) tips="<%write(JSAlert59);%>";
    var retips = '';
    var obj = tipsObj || cntrl;
    if (cntrl.value.length == 1) cntrl.value = "0" + cntrl.value;
    if (cntrl.value == "") {
        retips = tips + "<%write(JSAlert31);%>";
    } else if (!isNum(cntrl.value)) {
        retips = tips + "<%write(JSAlert34);%>";

    } else {
        var minute = toNumber(cntrl.value, 0, 2);
        if (minute < 0 || minute > 59) {
            retips = tips + "<%write(JSAlert34);%>";
        }
    }
    if (retips != '') {
      // inputErro(obj, retips, 2, 0);
	   $("#error_msg").html(retips);
        return false;
    } else {
        //inputRight(obj);
		$("#error_msg").html(retips);
        return true;
    }
}


///////////////////////////////////////
//功能：检查是否合法密码
//参数说明:
//cntrl1、cntrl2--控件名
//maxlen--允许的最大长度
//allownull--1允许为空，0不允许为空
//返回值：true合法，false非法
//////////////////////////////////////
function checkPassword_P(cntrl1, cntrl2, maxlen, allownull, alertFlag) {
    if (!checkPassword1_Password(cntrl1, maxlen, allownull, alertFlag)) {
        return false;
    } else if (cntrl1.value != cntrl2.value) {
        //inputErro(cntrl2, "两次输入的密码不一致。", alertFlag);
		$("#error_msg").html("<%write(JSAlert30);%>");
        return false;
    }
   // inputRight(cntrl2);
   $("#error_msg").html("");
    return true;
}

function checkPassword_num_letters(cntrl1)
{ 
    if(cntrl1.value.length < 8)    
    {
        $("#error_msg").html("<%write(passwdless8);%>");
        return false;
    }
    if(cntrl1.value.length > 16)    
    {
        $("#error_msg").html("<%write(passwdlong16);%>");
        return false;
    }
    var p = /[0-9]/; var b = p.test(cntrl1.value);//true,说明有数字
    if(!b){
        $("#error_msg").html("<%write(passwdneedNumAndLet);%>");
        return false;        
    }
    var q = /[a-z]/i; var c = q.test(cntrl1.value);//true,说明有英文字母
    if(!c){
        $("#error_msg").html("<%write(passwdneedNumAndLet);%>");
        return false;
    }
    $("#error_msg").html("");
    return true;

}



function checkPassword1_P(cntrl1, maxlen, allownull, alertFlag) {
    var retips = "";
    pas1 = cntrl1.value;
    maxlen = toNumber(maxlen, 0, maxlen.length);
    if (pas1.length > maxlen) {
        retips = "<%write(JSAlert27);%>" + maxlen + "<%write(JSAlert29);%>";
    } else if (pas1 == "") {
        if (!eval(allownull)) {
            retips = "<%write(JSAlert28);%>";
        } else retips = "";
    } 
    else if (pas1.indexOf(",") != -1 || pas1.indexOf(";") != -1 || pas1.indexOf("；") != -1 || pas1.indexOf("%") != -1 || pas1.indexOf("\"") != -1 || pas1.indexOf("\'") != -1 || pas1.indexOf("\\") != -1 || 　pas1.indexOf(" ") != -1 || pas1.indexOf("&") != -1) {
        retips = "<%write(JSAlert19);%>";
    } 
    if (retips != '') {
        //inputErro(cntrl1, retips, alertFlag);
		//alert("retips:"+retips);
		$("#error_msg").html(retips);
        return false;
    } else {
        //inputRight(cntrl1);
		$("#error_msg").html(retips);
        return true;
    }
}
function checkPassword1_Password(cntrl1, maxlen, allownull, alertFlag) {
    var retips = "";
    //var reg = /^.*(?=.{8,})(?=.*\d)(?=.*[A-Z])(?=.*[a-z])(?=.*[!@#$%^*?:,+-]).*$/
    var reg = /^(?=.*[a-z])(?=.*[A-Z])(?=.*\d)(?=.*[!@#$%^*?:,+-])[A-Za-z\d!@#$%^*?:,+-]{8,}$/
    pas1 = cntrl1.value;
    maxlen = toNumber(maxlen, 0, maxlen.length);
    if (pas1.length > maxlen) {
        retips = "<%write(JSAlert27);%>" + maxlen + "<%write(JSAlert29);%>";
    } else if (pas1 == "") {
        if (!eval(allownull)) {
            retips = "<%write(JSAlert28);%>";
        } else retips = "";
    } 
    /*else if (pas1.indexOf(",") != -1 || pas1.indexOf(";") != -1 || pas1.indexOf("；") != -1 || pas1.indexOf("%") != -1 || pas1.indexOf("\"") != -1 || pas1.indexOf("\'") != -1 || pas1.indexOf("\\") != -1 || 　pas1.indexOf(" ") != -1 || pas1.indexOf("&") != -1) {
        retips = "<%write(JSAlert19);%>";
    } */
    else if(!reg.test(pas1)){
        retips = "修改密码需八位以上包含大小字母、数字、特殊字符(!@#$%^*?:,+-)";
    }
    if (retips != '') {
        //inputErro(cntrl1, retips, alertFlag);
		//alert("retips:"+retips);
		$("#error_msg").html(retips);
        return false;
    } else {
        //inputRight(cntrl1);
		$("#error_msg").html(retips);
        return true;
    }
}
function checkEnglishName_P(cntrl, maxlen, tips, allownull) {
    var name = cntrl.value;
    if (name == "") {
        if (!eval(allownull)) {
            $("#error_msg").html(tips + "<%write(JSAlert17);%>");
            cntrl.focus();
            return false;
        } else return true;
    } else if (name == 0) {
        $("#error_msg").html(tips + "<%write(JSAlert17);%>");
        return false;
    } else if (name.length > maxlen) {
        $("#error_msg").html(tips + "<%write(JSAlert37);%>" + maxlen + "<%write(JSAlert29);%>");
        cntrl.focus();
        return false;
    } else {
        allowChar = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890.<>/?[]{}-_+=|`~!@$^&*()";
        for (i = 0; i < name.length; i++) {
            if (allowChar.indexOf(name.substring(i, i + 1)) == -1) {
                $("#error_msg").html(tips + "<%write(JSAlert39);%>");
                return false;
            }
        }
    }
	$("#error_msg").html("");
    return true;
}


function checkWEP_P(key1, key2, key3, key4, Index) {
    var str = "<%write(JSAlert3);%>";
    if (key1dis && key2dis && key3dis && key4dis) {
        $("#error_msg").html("<%write(JSAlert4);%>");
        return false;
    }
    //alert("keylength1="+keylength1+"key1.value.length="+key1.value.length);
    if (!key1dis) {
        if (key1.value.length != keylength1) {
            $("#error_msg").html(str + "<%write(JSAlert5);%>" + keylength1 + "<%write(JSAlert6);%>");
            return false;
        } else {
            if (Index == 1) {
                if (!checkEnglishName(key1, keylength1, "<%write(JSAlert7);%>", 0)) return false;
            } else {
                if (!checkHex(key1, "<%write(JSAlert7);%>", 0)) return false;
            }
        }
    }
    if (!key2dis) {
        if (key2.value.length != keylength2) {
            $("#error_msg").html(str + "<%write(JSAlert8);%>" + keylength2 + "<%write(JSAlert6);%>");
            return false;
        } else {
            if (Index == 1) {
                if (!checkEnglishName(key2, keylength2, "<%write(JSAlert9);%>", 0)) return false;
            } else {
                if (!checkHex(key2, "<%write(JSAlert9);%>", 0)) return false;
            }
        }
    }
    if (!key3dis) {
        if (key3.value.length != keylength3) {
            $("#error_msg").html(str + "<%write(JSAlert10);%>" + keylength3 + "<%write(JSAlert6);%>");
            return false;
        } else {
            if (Index == 1) {
                if (!checkEnglishName(key3, keylength3, "<%write(JSAlert11);%>", 0)) return false;
            } else {
                if (!checkHex(key3, "<%write(JSAlert11);%>", 0)) return false;
            }
        }
    }
    if (!key4dis) {
        if (key4.value.length != keylength4) {
            $("#error_msg").html(str + "<%write(JSAlert12);%>" + keylength4 + "<%write(JSAlert6);%>");
            return false;
        } else {
            if (Index == 1) {
                if (!checkEnglishName(key4, keylength4, "<%write(JSAlert13);%>", 0)) return false;
            } else {
                if (!checkHex(key4, "<%write(JSAlert13);%>", 0)) return false;
            }
        }
    }
	$("#error_msg").html("");
    return true;
}

//文本框改变后的验证信息

//obj：对象 msg 提示信息 val:是否为空的
function validateName(obj,maxlen,msg,val){
	obj.focus();
	checkName_P(obj,maxlen,msg,val);	
}
function validateIP(obj,msg,val){
	obj.focus();
	checkIP_P(obj,msg,val);	
}
function validateMask_P(obj,msg,val){
	obj.focus();
	checkMask_P(obj,msg,val);	
}

function validateMAC(obj,msg,val){
	obj.focus();
	checkMac_P(obj,msg,val);	
}
function validateNumber(obj,minval,maxval,msg,val){
	obj.focus();
	checkNumber_P(obj,minval,maxval,msg,val)	
}

///////////////////////////////////////
//功能：只允许输入合法数字，不包括小数点和负号
//////////////////////////////////////
function figureOnly_P(evt) {
	var e = window.event || evt ;
	//var keyCode=window.event?e.keyCode:e.which;
	var keycode = e.which;
    if (! ((e.keyCode >= 48) && (e.keyCode <= 57))) {
        e.keyCode = 0;
    }
	
}

function ipOnly_P(evt) {
	var e = window.event || evt;
    if (! (((e.keyCode >= 48) && (e.keyCode <= 57)) || (e.keyCode == 46))) {
        e.keyCode = 0;
    }
}
///////////////////////////////////////
//功能：只允许输入MAC地址
//////////////////////////////////////
function macOnly_P(evt) {
	var e = window.event || evt
    if (! (((e.keyCode >= 48) && (e.keyCode <= 57)) || ((e.keyCode >= 97) && (e.keyCode <= 102)) || (e.keyCode == 45) || ((e.keyCode >= 65) && (e.keyCode <= 70)))) {
       e.keyCode = 0;
    }
}
/*
*列表函数
* listheadname : 列表名称
* titles: 列表标题
* arrayData ：数据
* edit : 是否显示管理 1：显示管理（编辑和删除）0：不显示  

*/
function dspListTab(listheadname,titles,totalTitle,arrayData,edit,tabwidth,total){
	
	if(typeof(tabwidth)=="undefined"){
		tabwidth = "600px";
	}else{
		tabwidth =tabwidth;	
	}

	if(typeof(total)!="undefined"){
			totalrecs = total;
	}
	//alert("totalrecs:"+totalrecs);
	var tab_html='<table border="0" cellpadding="0" cellspacing="1" style="background:#5e5e5e;width:'+tabwidth+'" class="tab_list" id="tablelist" >';
	var titlelen =totalTitle;
	if(edit){
		titlelen = titlelen+1;
	}
	if(listheadname=="<%write(wire_apList);%>"){
		titlelen = titlelen+1;;		
	}else{
		tab_html +='<tr class="tab_title"><td colspan="'+titlelen+'" align="left">'+listheadname+'</td></tr>';
	}
	tab_html +='<tr class="tab_info_list">';
	for(var i=0;i<totalTitle;i++){
		tab_html +='<th align="center">'+titles[i]+'</th>';
	}
	if(edit){
		tab_html +='<th align="center"><%write(admin);%></th>';	
	}	
	if(listheadname=="<%write(wire_apList);%>"){
		tab_html +='<th align="center"><%write(JSAlert63);%></th>';		
	}
	tab_html +='</tr>'; 
	for(var i =0;i<totalrecs;i++){
		tab_html +='<tr class="tab_info_list"><td align="center">'+(i+1)+'</td>';
		for(var j=0;j<arrayData.length;j++){
			//alert("arrayData["+i+"]["+j+"]:"+arrayData[j][i]);
		       if((listheadname== "<%write(ipm_bandInfo);%>")&&(j==3)){
			    var  strTmp = "";
			     if (arrayData[j][i].toLowerCase() == "yes")　 { //允许上网
			        strTmp = "checked";
			     }
                              tab_html += '<td align="center"><input type="checkbox" name="chkAllow" ' + strTmp + ' onClick= "javascript:SaveUserAllow('+i.toString()+')" value='+ arrayData[j][i]+'/></td>';
			}
			else{
			tab_html +='<td align="center">'+shortShow(arrayData[j][i],j)+'</td>';
			}
		}
		if(edit){
			tab_html +='<td style="text-align:center;text-valign:middle;"><a href="javascript:EditConfig(' + i + ')"  class="ablack"><img src="images_Sui/edit.png" /></a>&nbsp;&nbsp;&nbsp;&nbsp;<a href="javascript:DeleteConfig(' + i + ')"  class="ablack"><img src="images_Sui/del.png" /></a></td>';
		}
		if(listheadname=="<%write(wire_apList);%>"){
			tab_html +='<td style="text-align:center;text-valign:middle;"><a href="javascript:EditConfig(' + i + ')"  class="ablack"><%write(JSAlert63);%></a></td>';		
		}
		tab_html +='</tr>';	
	}
	
	tab_html +='</table>';
	return tab_html;
}

function getdisable_wep(keyLength1, keyLength2, keyLength3, keyLength4) {
    wep1dis = (keyLength1.selectedIndex == 0);
    wep2dis = (keyLength2.selectedIndex == 0);
    wep3dis = (keyLength3.selectedIndex == 0);
    wep4dis = (keyLength4.selectedIndex == 0);
}
function auto_disable_wep(key1, key2, key3, key4, keynum, keyLength1, keyLength2, keyLength3, keyLength4) {
    var keychoosed;
    key1.disabled = keynum[0].disabled = wep1dis;
    key2.disabled = keynum[1].disabled = wep2dis;
    key3.disabled = keynum[2].disabled = wep3dis;
    key4.disabled = keynum[3].disabled = wep4dis;
    for (i = 0; i < keynum.length; i++) {
        if (keynum[i].checked) {
            keychoosed = keynum[i].value;
            break;
        }
    }
    if ((0 == wep1dis) || (0 == wep2dis) || (0 == wep3dis) || (0 == wep4dis)) {
        if ((keychoosed == 1 && keyLength1.selectedIndex == 0) || (keychoosed == 2 && keyLength2.selectedIndex == 0) || (keychoosed == 3 && keyLength3.selectedIndex == 0) || (keychoosed == 4 && keyLength4.selectedIndex == 0)) {
            if (0 == wep1dis) keynum[0].checked = 1;
            else if (0 == wep2dis) keynum[1].checked = 1;
            else if (0 == wep3dis) keynum[2].checked = 1;
            else keynum[3].checked = 1;
        }
    }
}

function auto_length_wep(key1, key2, key3, key4, Length1, Length2, Length3, Length4, Index) {

    weplength1 = (1 == wep1dis) ? weplength1: (((3 == Length1.selectedIndex) ? 16 : ((2 == Length1.selectedIndex) ? 13 : 5)) * ((0 == Index) ? 2 : 1));
    weplength2 = (1 == wep2dis) ? weplength2: (((3 == Length2.selectedIndex) ? 16 : ((2 == Length2.selectedIndex) ? 13 : 5)) * ((0 == Index) ? 2 : 1));
    weplength3 = (1 == wep3dis) ? weplength3: (((3 == Length3.selectedIndex) ? 16 : ((2 == Length3.selectedIndex) ? 13 : 5)) * ((0 == Index) ? 2 : 1));
    weplength4 = (1 == wep4dis) ? weplength4: (((3 == Length4.selectedIndex) ? 16 : ((2 == Length4.selectedIndex) ? 13 : 5)) * ((0 == Index) ? 2 : 1));

    if ((key1.value.length > weplength1) && (wep1dis != 1)) key1.value = key1.value.substring(0, weplength1);
    if ((key2.value.length > weplength2) && (wep2dis != 1)) key2.value = key2.value.substring(0, weplength2);
    if ((key3.value.length > weplength3) && (wep3dis != 1)) key3.value = key3.value.substring(0, weplength3);
    if ((key4.value.length > weplength4) && (wep4dis != 1)) key4.value = key4.value.substring(0, weplength4);
    key1.maxLength = weplength1;
    key2.maxLength = weplength2;
    key3.maxLength = weplength3;
    key4.maxLength = weplength4;
}
function checkWEP_PP(key1, key2, key3, key4, Index) {
    var str = "<%write(JSAlert3);%>";
    if (wep1dis && wep2dis && wep3dis && wep4dis) {
        $("#error_msg").html("<%write(JSAlert4);%>");
        return false;
    }
    //alert("weplength1="+weplength1+"key1.value.length="+key1.value.length);
    if (!wep1dis) {
        if (key1.value.length != weplength1) {
            $("#error_msg").html(str + "<%write(JSAlert5);%>" + weplength1 + "<%write(JSAlert6);%>");
            return false;
        } else {
            if (Index == 1) {
                if (!checkEnglishName(key1, weplength1, "<%write(JSAlert7);%>", 0)) return false;
            } else {
                if (!checkHex(key1, "<%write(JSAlert7);%>", 0)) return false;
            }
        }
    }
    if (!wep2dis) {
        if (key2.value.length != weplength2) {
            $("#error_msg").html(str + "<%write(JSAlert8);%>" + weplength2 + "<%write(JSAlert6);%>");
            return false;
        } else {
            if (Index == 1) {
                if (!checkEnglishName(key2, weplength2, "<%write(JSAlert9);%>", 0)) return false;
            } else {
                if (!checkHex(key2, "<%write(JSAlert9);%>", 0)) return false;
            }
        }
    }
    if (!wep3dis) {
        if (key3.value.length != weplength3) {
            $("#error_msg").html(str + "<%write(JSAlert10);%>" + weplength3 + "<%write(JSAlert6);%>");
            return false;
        } else {
            if (Index == 1) {
                if (!checkEnglishName(key3, weplength3, "<%write(JSAlert11);%>", 0)) return false;
            } else {
                if (!checkHex(key3, "<%write(JSAlert11);%>", 0)) return false;
            }
        }
    }
    if (!wep4dis) {
        if (key4.value.length != weplength4) {
            $("#error_msg").html(str + "<%write(JSAlert12);%>" + weplength4 + "<%write(JSAlert6);%>");
            return false;
        } else {
            if (Index == 1) {
                if (!checkEnglishName(key4, weplength4, "<%write(JSAlert13);%>", 0)) return false;
            } else {
                if (!checkHex(key4, "<%write(JSAlert13);%>", 0)) return false;
            }
        }
    }
	$("#error_msg").html("");
    return true;
}

/*ssid2*/
function checkWEP_PP_2(key1, key2, key3, key4, Index) {
    if (typeof(AuthModes_2) != 'undefined') {
	var str = "<%write(JSAlert3);%>";
	if (wep1dis_2 && wep2dis_2 && wep3dis_2 && wep4dis_2) {
	    $("#error_msg").html("<%write(JSAlert4);%>");
	    return false;
	}
	//alert("weplength1="+weplength1+"key1.value.length="+key1.value.length);
	if (!wep1dis_2) {
	    if (key1.value.length != weplength1_2) {
		$("#error_msg").html(str + "<%write(JSAlert5);%>" + weplength1 + "<%write(JSAlert6);%>");
		return false;
	    } else {
		if (Index == 1) {
		    if (!checkEnglishName(key1, weplength1_2, "<%write(JSAlert7);%>", 0)) return false;
		} else {
		    if (!checkHex(key1, "<%write(JSAlert7);%>", 0)) return false;
		}
	    }
	}
	if (!wep2dis_2) {
	    if (key2.value.length != weplength2_2) {
		$("#error_msg").html(str + "<%write(JSAlert8);%>" + weplength2_2 + "<%write(JSAlert6);%>");
		return false;
	    } else {
		if (Index == 1) {
		    if (!checkEnglishName(key2, weplength2_2, "<%write(JSAlert9);%>", 0)) return false;
		} else {
		    if (!checkHex(key2, "<%write(JSAlert9);%>", 0)) return false;
		}
	    }
	}
	if (!wep3dis_2) {
	    if (key3.value.length != weplength3_2) {
		$("#error_msg").html(str + "<%write(JSAlert10);%>" + weplength3_2 + "<%write(JSAlert6);%>");
		return false;
	    } else {
		if (Index == 1) {
		    if (!checkEnglishName(key3, weplength3_2, "<%write(JSAlert11);%>", 0)) return false;
		} else {
		    if (!checkHex(key3, "<%write(JSAlert11);%>", 0)) return false;
		}
	    }
	}
	if (!wep4dis_2) {
	    if (key4.value.length != weplength4_2) {
		$("#error_msg").html(str + "<%write(JSAlert12);%>" + weplength4_2 + "<%write(JSAlert6);%>");
		return false;
	    } else {
		if (Index == 1) {
		    if (!checkEnglishName(key4, weplength4_2, "<%write(JSAlert13);%>", 0)) return false;
		} else {
		    if (!checkHex(key4, "<%write(JSAlert13);%>", 0)) return false;
		}
	    }
	}
	    $("#error_msg").html("");
    }
    return true;
}


/*ssid2加密*/
function getdisable_wep_2(keyLength1, keyLength2, keyLength3, keyLength4) {
    if (typeof(AuthModes_2) != 'undefined') {
	wep1dis_2 = (keyLength1.selectedIndex == 0);
	wep2dis_2 = (keyLength2.selectedIndex == 0);
	wep3dis_2 = (keyLength3.selectedIndex == 0);
	wep4dis_2 = (keyLength4.selectedIndex == 0);
    }
}
function auto_disable_wep_2(key1, key2, key3, key4, keynum, keyLength1, keyLength2, keyLength3, keyLength4) {
    if (typeof(AuthModes_2) != 'undefined') {
	var keychoosed;
	key1.disabled = keynum[0].disabled = wep1dis_2;
	key2.disabled = keynum[1].disabled = wep2dis_2;
	key3.disabled = keynum[2].disabled = wep3dis_2;
	key4.disabled = keynum[3].disabled = wep4dis_2;
	for (i = 0; i < keynum.length; i++) {
	    if (keynum[i].checked) {
		keychoosed = keynum[i].value;
		break;
	    }
	}
	if ((0 == wep1dis_2) || (0 == wep2dis_2) || (0 == wep3dis_2) || (0 == wep4dis_2)) {
	    if ((keychoosed == 1 && keyLength1.selectedIndex == 0) || (keychoosed == 2 && keyLength2.selectedIndex == 0) || (keychoosed == 3 && keyLength3.selectedIndex == 0) || (keychoosed == 4 && keyLength4.selectedIndex == 0)) {
		if (0 == wep1dis_2) keynum[0].checked = 1;
		else if (0 == wep2dis_2) keynum[1].checked = 1;
		else if (0 == wep3dis_2) keynum[2].checked = 1;
		else keynum[3].checked = 1;
	    }
	}
    }
}

function auto_length_wep_2(key1, key2, key3, key4, Length1, Length2, Length3, Length4, Index) {
    if (typeof(AuthModes_2) != 'undefined') {
	weplength1_2 = (1 == wep1dis_2) ? weplength1_2: (((3 == Length1.selectedIndex) ? 16 : ((2 == Length1.selectedIndex) ? 13 : 5)) * ((0 == Index) ? 2 : 1));
	weplength2_2 = (1 == wep2dis_2) ? weplength2_2: (((3 == Length2.selectedIndex) ? 16 : ((2 == Length2.selectedIndex) ? 13 : 5)) * ((0 == Index) ? 2 : 1));
	weplength3_2 = (1 == wep3dis_2) ? weplength3_2: (((3 == Length3.selectedIndex) ? 16 : ((2 == Length3.selectedIndex) ? 13 : 5)) * ((0 == Index) ? 2 : 1));
	weplength4_2 = (1 == wep4dis_2) ? weplength4_2: (((3 == Length4.selectedIndex) ? 16 : ((2 == Length4.selectedIndex) ? 13 : 5)) * ((0 == Index) ? 2 : 1));

	if ((key1.value.length > weplength1_2) && (wep1dis_2 != 1)) key1.value = key1.value.substring(0, weplength1_2);
	if ((key2.value.length > weplength2_2) && (wep2dis_2 != 1)) key2.value = key2.value.substring(0, weplength2_2);
	if ((key3.value.length > weplength3_2) && (wep3dis_2 != 1)) key3.value = key3.value.substring(0, weplength3_2);
	if ((key4.value.length > weplength4_2) && (wep4dis_2 != 1)) key4.value = key4.value.substring(0, weplength4_2);
	key1.maxLength = weplength1_2;
	key2.maxLength = weplength2_2;
	key3.maxLength = weplength3_2;
	key4.maxLength = weplength4_2;
    }
}
function checkWEP_PP_2(key1, key2, key3, key4, Index) {
    if (typeof(AuthModes_2) != 'undefined') {
	var str = "<%write(JSAlert3);%>";
	if (wep1dis_2 && wep2dis_2 && wep3dis_2 && wep4dis_2) {
	    $("#error_msg").html("<%write(JSAlert4);%>");
	    return false;
	}
	//alert("weplength1_2="+weplength1_2+"key1.value.length="+key1.value.length);
	if (!wep1dis_2) {
	    if (key1.value.length != weplength1_2) {
		$("#error_msg").html(str + "<%write(JSAlert5);%>" + weplength1_2 + "<%write(JSAlert6);%>");
		return false;
	    } else {
		if (Index == 1) {
		    if (!checkEnglishName(key1, weplength1_2, "<%write(JSAlert7);%>", 0)) return false;
		} else {
		    if (!checkHex(key1, "<%write(JSAlert7);%>", 0)) return false;
		}
	    }
	}
	if (!wep2dis_2) {
	    if (key2.value.length != weplength2_2) {
		$("#error_msg").html(str + "<%write(JSAlert8);%>" + weplength2_2 + "<%write(JSAlert6);%>");
		return false;
	    } else {
		if (Index == 1) {
		    if (!checkEnglishName(key2, weplength2_2, "<%write(JSAlert9);%>", 0)) return false;
		} else {
		    if (!checkHex(key2, "<%write(JSAlert9);%>", 0)) return false;
		}
	    }
	}
	if (!wep3dis_2) {
	    if (key3.value.length != weplength3_2) {
		$("#error_msg").html(str + "<%write(JSAlert10);%>" + weplength3_2 + "<%write(JSAlert6);%>");
		return false;
	    } else {
		if (Index == 1) {
		    if (!checkEnglishName(key3, weplength3_2, "<%write(JSAlert11);%>", 0)) return false;
		} else {
		    if (!checkHex(key3, "<%write(JSAlert11);%>", 0)) return false;
		}
	    }
	}
	if (!wep4dis_2) {
	    if (key4.value.length != weplength4_2) {
		$("#error_msg").html(str + "<%write(JSAlert12);%>" + weplength4_2 + "<%write(JSAlert6);%>");
		return false;
	    } else {
		if (Index == 1) {
		    if (!checkEnglishName(key4, weplength4_2, "<%write(JSAlert13);%>", 0)) return false;
		} else {
		    if (!checkHex(key4, "<%write(JSAlert13);%>", 0)) return false;
		}
	    }
	}
	    $("#error_msg").html("");
    }
    return true;
}
