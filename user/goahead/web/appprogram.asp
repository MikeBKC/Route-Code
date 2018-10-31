<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="x-ua-compatible" content="IE=9" />
<meta name="format-detection" content="telephone=no" />
<meta http-equiv="pragma" content="no-cache">
<meta http-equiv="Cache-Control" content="no-cache, must-revalidate">
<meta http-equiv="expires" content="Wed, 26 Feb 1997 08:21:57 GMT">
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<% langTran("HomePage"); %>
<% langTran("common"); %>
<title><% aspGetTitle(); %></title>
<link href="css/style_new.css" rel="stylesheet" type="text/css" />
<link href="css/default.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="script/curvycorners.js" ></script>
<script type="text/javascript" src="script/jquery-1.8.1.min.js" ></script>
<!--
<script type="text/javascript" src="script/popwin.js" ></script>
-->
<script type="text/javascript" src="script/contextMenu.js" ></script>
<script type="text/javascript" src="script/jquery.corner.js" ></script>
<script type="text/javascript" src="script/jquery.artDialog.js" ></script>
<script type="text/javascript" src="script/iframeTools.js" ></script>
<script type="text/javascript" src="script/jquery.timers-1.2.js" ></script>
<script type="text/javascript" src="script/menu.js" ></script>
<style type="text/css">
a{ behavior:url(images_Sui/blur.htc);} /* ����ȥ�����ӽ��� */
</style>
<script type="text/javascript">
<%aspoutDynConfig();%>
<%aspOutputShortCutItemNum();%>
	$(function() {
	var sWidth = $("#focus").width(); //��ȡ����ͼ�Ŀ�ȣ���ʾ�����
	var len = $("#focus .app div").length; //��ȡ����ͼ����
	//alert("len:"+len);
	var index = 0;
	var picTimer;
    if(galaxywind_ctrl ==1){//���ӷ���Ʒ�ƿ���
		$("#logo").css({"background":"url(../images_Sui/galaxywindlogo.png) no-repeat"});
		$("#logo").attr("href","");
		$("#logo").attr("target","");
	}else if(middlesoft == 1 && skyworth == 0){
        $("#logo").css({"background":"url(../images_Sui/chinatelecom.png) no-repeat"});
    }else if(skyworth == 1){
        $("#logo").css({"background":"url(../images_Sui/sky.png) no-repeat"});
    }
    else{
        $("#logo").css({"background":"url(../images_Sui/<%write(com_imglogo1);%>) no-repeat"});
		}
	if(LEVELONE_LOGO==1){
		$("#logo").attr("href","http://global.level1.com/");
	}
        $(".app_pro li a").css({"width": "<%write(com_width);%>","height": "<%write(com_height);%>","padding-top": "<%write(com_padding_top);%>"});
	if (lang_en == 1)
	{
	    $(".app_pro li a").css({"padding-top": "60px", "padding-bottom":"10px", "background-position":"27px 3px"});
	}	
	//���´���������ְ�ť�Ͱ�ť��İ�͸������������һҳ����һҳ<div class='btnBg'></div>������ť
	var btn = "<div class='btnBg'></div><div class='btn'>";
	for(var i=0; i < len; i++) {
		btn += "<span>"+(i+1)+"</span>";
	}
	btn += "</div><div class='preNext pre'></div><div class='preNext next'></div>";
	$("#focus").append(btn);
	$("#focus .btnBg").css("opacity",0.5);

	//ΪС��ť�����껬���¼�������ʾ��Ӧ������
	$("#focus .btn span").css("opacity",0.4).mouseover(function() {
		index = $("#focus .btn span").index(this);
		showPics(index);
	}).eq(0).trigger("mouseover");

	//��һҳ����һҳ��ť͸���ȴ���
	$("#focus .preNext").css("opacity",0.2).hover(function() {
		$(this).stop(true,false).animate({"opacity":"0.5"},300);
	},function() {
		$(this).stop(true,false).animate({"opacity":"0.2"},300);
	});

	//��һҳ��ť
	$("#focus .pre").click(function() {
		index -= 1;
		if(index == -1) {index = len - 1;}
		showPics(index);
	});

	//��һҳ��ť
	$("#focus .next").click(function() {
		index += 1;
		if(index == len) {index = 0;}
		showPics(index);
	});

	//����Ϊ���ҹ�����������liԪ�ض�����ͬһ�����󸡶�������������Ҫ�������ΧulԪ�صĿ��
	$("#focus .app").css("width",sWidth * (len));
	
	//��껬�Ͻ���ͼʱֹͣ�Զ����ţ�����ʱ��ʼ�Զ�����
/*	$("#focus").hover(function() {
		clearInterval(picTimer);
	},function() {
		picTimer = setInterval(function() {
			showPics(index);
			index++;
			if(index == len) {index = 0;}
		},4000); //��4000�����Զ����ŵļ������λ������
	}).trigger("mouseleave");*/
	
	//��ʾͼƬ���������ݽ��յ�indexֵ��ʾ��Ӧ������
	function showPics(index) { //��ͨ�л�
		var nowLeft = -index*sWidth; //����indexֵ����ulԪ�ص�leftֵ
		$("#focus .app").stop(true,false).animate({"left":nowLeft},300); //ͨ��animate()����ulԪ�ع������������position
		//$("#focus .btn span").removeClass("on").eq(index).addClass("on"); //Ϊ��ǰ�İ�ť�л���ѡ�е�Ч��
		$("#focus .btn span").stop(true,false).animate({"opacity":"0.4"},300).eq(index).stop(true,false).animate({"opacity":"1"},300); //Ϊ��ǰ�İ�ť�л���ѡ�е�Ч��
	}


    //�˳�
    $("#logout").click(function(){
	window.location.href="logoutSuccess_Sui.asp";
    });
});

function pro_open_window(index){
    open_windows(index,null,menus);
}

function writeApp(){
    var total = 0;
    var innerli = "";
	var l =0,t=0;
	var appUpdate = set_upgrade_count();
    for(var i=0;i<menus.length;i++){
	
	if(menus[i][3]==1){
		
	    total ++;
	    if(total%16==1){
		innerli +='<div><ul class="app_pro">'; 
	    }
		
		
	    innerli +='<li><a  class="contextMenu"  onClick="pro_open_window('+i+');" id="'+i+'" style="background: url(images_Sui/'+menus[i][2]+');  background-position:20px 20px; background-repeat:no-repeat;cursor: pointer; position:relative;"><span>'+menus[i][0]+'</span>';
		if(appUpdate){
			for(var n =0,len = appUpdate.length; n< len ;n++){
				if(menus[i][7]==appUpdate[n]){
					innerli +='<span style="background:url(images_Sui/upgrade_icon.png) no-repeat; width:30px;height:30px;display:inline-block;position:absolute;top:16px;"></span>';
				}
			}
		}
		
		innerli += '</a></li>';
	    if(total%16==0){
		innerli +='</ul></div>'; 
	    }	
						 
	}
	
    }
    if((total%16)!=0){
	innerli +='</ul></div>'; 
    }
	
   // $(".app").append(innerli);
    document.write(innerli);
}
//������Ҫ���������Ŀ
function set_upgrade_count(){
if(auto_update==1){

	var appList = new Array();
	$.ajax({
	url: "/goform/formAppCheck",
	cache: false,
	async:false,
	dataType: "json",
	success: function(rsp){
		if(rsp){
		//rsp.countUpdate=2;
			if(rsp.countUpdate > 0){
				var str = '<span style="background:url(images_Sui/upgradeIcon1.png) no-repeat; width:24px;height:24px;display:inline-block;position:absolute; top:-12px;left:46px;color:#fff;text-align:center;font-size:12px;padding-top:2px;">'+rsp.countUpdate+'</span>';
				$("#updateCount").html(str);
			}
			if(rsp.appList){
			$.each(rsp.appList,function(i,item){
				
				appList.push(item.appType);
			});
			}
		}
		$('body').stopTime('TimerB');
	    $('body').everyTime('3s','TimerB',function(){
		   set_upgrade_count();
		    },0,true);

	},
	error:function(data){
	
	}
});

return appList;
}
}

function closeTimer()
{
    $('body').stopTime('TimerB');
}
$(document).ready(function() {
    $(".contextMenu").hover(function(){
	   $(this).css({"background-color":"#4bc2ec"});
	   $(this).corner("10px");
	},function(){
	   $(this).css({"background-color":"#00a1e1"});
	}
    ); 
    // ����״̬
    $("#runstatus").click(function(){
	open_windows(null,"runstatus",menus);
    });
    //������	
    $("#system_guide").click(function(){
	open_windows(null,"guide",menus);
    });
    //��������
    $("#setting_wireless").click(function(){
	open_windows(null,"wifi",menus);
    });
    //��������
    $("#set_network").click(function(){
	open_windows(null,"internet",menus);
    });	
	if(auto_update ==1)
	{
		set_upgrade_count();
	}
});

</script>

</head>

<body id="appprogram">
<div id="header">
	<div class="wrap">
		<p class="top_menu">
			<a href="home.asp" id="traffic_info"><%write(homePage);%></a>
			<span class="pipe">|</span><a  id="runstatus"><%write(runningStatus);%></a>
			<span class="pipe">|</span><a  id="system_guide"><%write(setupWizard);%></a>
			<!--<span class="pipe">|</span><a  id="setting_wireless">��������</a>-->
            <span class="pipe">|</span><a href="appprogram.asp" id="app" style="position:relative;"><%write(App);%><span id="updateCount"></span></a>
			<span class="pipe">|</span><a href="javascript:void();" id="logout"><%write(Logout);%></a>
		</p>
		<h1><a target="_blank"  id="logo">admin</a></h1>

	</div>
</div>
<div class="wrapper">
    <div id="focus">
	<div class="app">
	<script type="text/javascript"> writeApp();</script>
    </div>
</div>
</div>
 <div id="bmenu" style="position:absolute;display:none;top:10px;left:100px;width:160px;margin:0px;padding:2px;border:1px solid #cccccc;background-color:#fff;">
  <ul>
  <li id="open" style="text-align:left; padding-left:7px;"><%write(Open);%></li>
  <li id="add" style="text-align:left; padding-left:7px;"><%write(AddToShortCut);%></li>
  </ul>
 </div>
</body>
<script type="text/javascript">
//�Զ����Ҽ�������
var cmenu = new contextMenu(
    {
	menuID : "bmenu",
	targetEle : "contextMenu"
    },
    {
	bindings:{
	    'open' : function(o){
		    pro_open_window(o.id);
		},
	    'add' : function(o){
		if (itemNum==5) {
		    if (confirm("<%write(confirmRepalceShortcutMsg);%>")) {
			$.ajax({ 
			    type: "post",  
			    url : "/goform/formShortCutMenuAdd", 
			    asnyc:false,
			    dataType:'html',
			    data:{instName:menus[o.id][6]},
			    success: function(data, textStatus){
				if (data == 1) {
				    alert("<%write(AddSuccess);%>");
				} else if (data == 0) {
				    alert("<%write(getInstanceFail);%>");
				} else {
				    alert(menus[o.id][0] +"<%write(shortCutMenuExist);%>");
				}
			    }
			});
		    }
		}
		if (itemNum < 5) {
		    $.ajax({ 
			type: "post",  
			url : "/goform/formShortCutMenuAdd", 
			asnyc:false,
			dataType:'html',
			data:{instName:menus[o.id][6]},
			success: function(data, textStatus){
			    if (data == 1) {
				if (itemNum < 5) {
				    itemNum++;
				}

				alert("<%write(AddSuccess);%>");
			    } else if (data == 0) {
				alert("<%write(getInstanceFail);%>");
			    } else {
				alert(menus[o.id][0] +"<%write(shortCutMenuExist);%>");
			    }
			}
		    });
		}
	    }
    }
   }
  );
 
 cmenu.buildContextMenu();
	
</script>
</html>
