<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<title>tomax</title>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<meta http-equiv="pragma" content="no-cache">
<meta http-equiv="Cache-Control" content="no-cache">
<meta http-equiv="expires" content="0">
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/contextMenu.js" ></script>
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" src="js/jquery.min.js"></script>
<!--<script type="text/javascript" src="js/jquery-ui.min.js"></script>-->
</head>
<style type="text/css">
div,a,span,img,ul,li{margin:0;padding:0;}
</style>
<script type="text/javascript">

jQuery.noConflict();
var isIE = (document.all) ? true : false;

var $ = function (id) {
	return "string" == typeof id ? document.getElementById(id) : id;
};
var id="";
var m=0;
var Drawings = new Array();
//--------------------������ʼ-------------------------------
<% 
	var filename = Form("oldName");//Զ���豸mac��ַ
	aspOutPutApDrawingEdit();
%>

var filename = "<%write(Form("oldName"));%>";

var positionAp = "<%write(Form("serialno"));%>";
//alert("positionAp"+ positionAp);
//if((typeof positionAp))
//alert("positionAp"+typeof positionAp);
//alert("filename: " + filename);

//------------------------��������-----------------------------------

var downX = 0;
var downY = 0;
var upX = 0;
var upY = 0;

var uapnum = userialnum.length;
var u = uapnum; 
function fresh(){

	self.location='DrawingPreview.asp?oldName='+filename;
}

function goBack(){
    csm.curMostOuter="70,955,70";
	if(parent.document.getElementById('frameset1').getAttribute("cols") != csm.curMostOuter){
		csm.change();
	}
	GoBack_Page('DrawingList.asp');
}

var apnum = serialnum.length;
var m = apnum-1;

//alert("init"+m);
var csm=null;
 /**��ʼ��****/
var ustr = ""; 
var toptop = 121;
function init(){
	if(path_obj == ""){
		//alert("����ѡ��ͼֽ������ѵ���ͼֽ��ѡ��ͼֽ�������ͼֽ�뵼��ͼֽ��");
		var IMG = document.createElement("img");
		IMG.id = "bgimg";
		IMG.width ="700px";
		IMG.height = "500px";
		//IMG.src="";
		$("idContainer").appendChild(IMG);
		
		var coverDiv = document.createElement("div");
		coverDiv.style.width = 700+"px"
		coverDiv.style.height = 520+"px";
		coverDiv.style.background="white";
		coverDiv.style.position="absolute";
		coverDiv.style.left = 0;
		coverDiv.style.top = 0;
		coverDiv.id = "cover";
		$("idContainer").appendChild(coverDiv);
		
		
		}
	else{
		var IMG = document.createElement("img");
		IMG.id = "bgimg";
		IMG.width ="700px";
		IMG.width = "500px";
		IMG.src="images/"+path_obj+"?temp="+Math.random();
		$("idContainer").appendChild(IMG);
		}
	
	var oSelect= document.getElementById("oldName");
//	alert("123"+oSelect.value);
	document.getElementById("path_obj").value= oSelect.value;
	var bgimg_h =500;
	var bgimg_w =700;
	if(width > 700){
		 bgimg_w =width ;
	}
	if(height>500){
		bgimg_h = height;
	}
	document.getElementById("bgimg").width=bgimg_w;
	document.getElementById("bgimg").height=bgimg_h;	
	//alert("bgimg width:"+document.getElementById("bgimg").width+";;bgimg height:"+document.getElementById("bgimg").height);
	//alert("apnum = "+ apnum);
	var scrollButton = document.getElementById("scrollBt");
	 toptop = toptop -((bgimg_w-700)/50)*10;
	 scrollButton.style.top = toptop + "px";
	
	
	for(var i=0;i<uapnum;i++){//��ʼ��ap�б�
		var apList=document.getElementById("apList");
		var li = document.createElement("li");
		li.innerHTML="<img src='images/normal.png' /> "+uapmac[i]+"("+userialnum[i]+")";
		li.style.cursor="pointer";
		jQuery(li).attr('id','uap'+i);
		apList.appendChild(li);
		/*ustr += '<li style="cursor:pointer;" id="uap"'+i+'><img src="images/normal.png">'+uapmac[i]+'('+userialnum[i]+')'+'</li>'
		jQuery(apList).html(ustr);
		alert(jQuery(apList).html());*/
   		}
		
		
	for(var n = 0;n<apnum;n++){//��ʼ����ͼֽ�ϵ�ap
		var num = n;
		var divobj =document.createElement("div");
		var divid ="apdiv_"+num;
			divobj.id=divid;
			divobj.style.position="absolute";
			divobj.style.left=x[n]+"px";
			divobj.style.top=y[n]+"px";
			//jQuery(divobj).draggable();
			$("idContainer").appendChild(divobj);
		//var drag = new Drag(divid, { mxContainer: "idContainer", Handle: id, Limit: true});	;
		var imgobj =document.createElement("img");
		   if(statue[n] == "0"){
				imgobj.src="images/offline.png";
			}else if(statue[n] == "1"){
				imgobj.src="images/unknow.png";	
			}else if(statue[n] == "2"){
				imgobj.src="images/normal.png";	
			}else{
				imgobj.src="images/unknow.png";	
			}
		var status_h = document.createElement("input");
			status_h.type="hidden";
			status_h.id="apstatus_"+n	;
			status_h.value=statue[n];
			document.body.appendChild(status_h);	
			var id = "ap_"+num;
			imgobj.id=id;	
			imgobj.className="contextMenu";
			imgobj.style.position="absolute";	
			imgobj.style.left=25+"px";
			imgobj.style.top=21+"px";
			imgobj.style.cursor = "pointer";
			divobj.appendChild(imgobj);
			
	   var delobj=document.createElement("div");
	   		delobj.innerHTML="<img src='images/DPdelete.png' width='20' height='20' />";
			delobj.id="apdel_"+n;
			delobj.style.width=20+"px";
			delobj.style.position="absolute";
			delobj.style.top=20+"px";
			delobj.style.left=55+"px";
			delobj.style.display="none";
			delobj.style.cursor = "pointer";
	  		divobj.appendChild(delobj);
		
			 
	   var statusobj=document.createElement("div");
	   		statusobj.innerHTML="<img src='images/DPstatus.png' width='20' height='20' />";
			statusobj.id="apstatus_"+n;
			statusobj.style.width=20+"px";
			statusobj.style.position="absolute";
			statusobj.style.left=28+"px";
			statusobj.style.top=0+"px";
			statusobj.style.display="none";
			statusobj.style.cursor = "pointer";
			divobj.appendChild(statusobj);
	   
	   
	   var manageobj=document.createElement("div");
	 	    manageobj.innerHTML="<img src='images/DPmanage.png' width='20' height='20' />";
			manageobj.id="apmanage_"+n;
			manageobj.style.width=20+"px";
			manageobj.style.position="absolute";
			manageobj.style.left=0+"px";
			manageobj.style.top=20+"px";
			manageobj.style.display="none";
			manageobj.style.cursor = "pointer";
			divobj.appendChild(manageobj);
	  
	  
	 var summaryobj=document.createElement("div");
	 	  summaryobj.id="apsummary_"+num;
		  summaryobj.style.position="absolute";
		  summaryobj.style.left=40+"px";
		  summaryobj.style.top=40+"px";
		  summaryobj.align="left";
	  var titlemsg="";//��ϸ
	  var titlesum="";//��Ҫ
	  var imgtitle = "";
	  
	  var apname_h = document.createElement("input");
		  apname_h.type="hidden";
		  apname_h.id="apname_"+num	;
		  document.body.appendChild(apname_h);
		  
		  imgtitle +="��  �ƣ�"+apname[n]+'\n';
	  if(apname[n]!=""){
		  titlemsg +="��  �ƣ�"+apname[n]+'<br />';
		  apname_h.value=apname[n];
		}else{apname_h.value="";}
		
	  
	  var apmodel_h = document.createElement("input");
		  apmodel_h.type="hidden";
		  apmodel_h.id="apmodel_"+num;
		  document.body.appendChild(apmodel_h);		
	  if(apmodel[n]!=""){
		  titlemsg +="��  �ţ�"+apmodel[n]+'<br />';
		  apmodel_h.value=apmodel[n];
	  }else{apmodel_h.value="";}
	  
	 var apip_h = document.createElement("input");
		  apip_h.type="hidden";
		  apip_h.id="apip_"+num	;
		  document.body.appendChild(apip_h);	
	  if(apip[n]!=""){
		  titlemsg +="IP ��ַ��"+apip[n]+'<br />';
		  titlesum +="IP ��ַ��"+apip[n]+'<br />';
		  //imgtitle +="IP ��ַ��"+apip[n]+'\n';
		  //alert(apip_h.id);
		  apip_h.value=apip[n];
		  }else{apip_h.value="";}
		  
	 var apmac_h = document.createElement("input");
		  apmac_h.type="hidden";
		  apmac_h.id="apmac_"+num	;	
		  document.body.appendChild(apmac_h);
	  if(apmac[n]!=""){
		  titlemsg +="MAC��ַ��"+apmac[n]+'<br />';
		  titlesum +="MAC��ַ��"+apmac[n]+'<br />';
		  imgtitle +="MAC��ַ��"+apmac[n]+'\n';
		  apmac_h.value=apmac[n];	
	  }else{ apmac_h.value="";}
	  
	  var APMAC_H = document.createElement("input");
	  		APMAC_H.type="hidden";
			APMAC_H.id="APMAC_"+num;
			APMAC_H.value=MAC[n];
			document.body.appendChild(APMAC_H);
		        document.getElementById('APMAC_'+num).value=MAC[n];
	  
	  
	  if(managed_enable[n]!=""){
		  var managed_enable_h = document.createElement("input");
		  managed_enable_h.type="hidden";
		  managed_enable_h.id="managed_enable_"+num	;
		  managed_enable_h.value=managed_enable[n];		
		  document.body.appendChild(managed_enable_h);
	  }
  
  
     var hidden = document.createElement("input");
		  hidden.type="hidden";
		  hidden.id="apserial_"+num;	
		  document.body.appendChild(hidden);
   	  if(serialnum[n]!="0"){//0
		  titlemsg +="���к�:"+serialnum[n]+'<br />';	
		  titlesum +="���к�:"+serialnum[n]+'<br />';
		  //imgtitle +="���к�:"+serialnum[n]+'\n';
		  hidden.value=serialnum[n];
		  }else{hidden.value="0";}
	 
	 var clientCt_h = document.createElement("input");
		  clientCt_h.type = "hidden";
		  clientCt_h.id = "clientCt_"+ num;
		  document.body.appendChild(clientCt_h); 
	  if(clienCt[n]!=""){
		  titlemsg +="�ͻ�������"+clienCt[n]+'<br />';	
		  imgtitle +="�ͻ�������"+clienCt[n]+'\n';
		  clientCt_h.value = clienCt[n];
		  }else{clientCt_h.value = "";}
		  
	 var channel_h = document.createElement("input");
		  channel_h.type = "hidden";
		  channel_h.id = "channel_"+ num;	
		  document.body.appendChild(channel_h);	  
	 if(channel[n]!=""){
		  titlemsg +="�ŵ���"+channel[n]+'<br />';
		  imgtitle +="�ŵ���"+channel[n]+'\n';
		  channel_h.value = channel[n];
		  }else{channel_h.value ="";}
		  
		  
	var ssid_h = document.createElement("input");
		  ssid_h.type = "hidden";
		  ssid_h.id = "ssid_" + num;	
		  document.body.appendChild(ssid_h);  
	  if(ssid[n]!=""){
		  titlemsg +="SSID��"+ssid[n];
		  imgtitle +="SSID��"+ssid[n]+'\n';
		  ssid_h.value = ssid[n];
		  }
	  //alert(titlemsg);
	  
	  summaryobj.style.width=150+"px";
	  summaryobj.style.backgroundColor="white";
	  summaryobj.innerHTML=titlesum;
	  summaryobj.style.display="none";
	  summaryobj.style.border="black 1px solid";
	  divobj.appendChild(summaryobj);
	
 var detailobj=document.createElement("div");
	  detailobj.id="apdetail_"+num;
	  detailobj.style.position="absolute";
	  detailobj.style.left=40+"px";
	  detailobj.style.top=40+"px";
	  detailobj.style.width=150+"px";
	  detailobj.style.backgroundColor="white";
	  detailobj.innerHTML=titlemsg;
	  detailobj.style.border="black 1px solid";
	  detailobj.style.display="none";
	  detailobj.align="left";
	  divobj.appendChild(detailobj);
	 imgobj.title = imgtitle;
	
if(positionAp == serialnum[n] ){
		delobj = document.getElementById("apdel_"+n);
	  	statusobj = document.getElementById("apstatus_"+n);
	 	manageobj = document.getElementById("apmanage_"+n);
		delobj.style.display="";
		statusobj.style.display="";
		manageobj.style.display="";
		
		}
	
  imgobj.onmousedown=function(ev){
		  var oEvent = ev||event;
		  downX = oEvent.clientX;
		  downY = oEvent.clientY;
	  }
	  imgobj.onmouseup =function(up){
		   var oEventUp = up||event;
		   upX = oEventUp.clientX;
		   upY = oEventUp.clientY;
		   
		   if((downX==upX)&&(downY==upY)){
			   var b = this.id.split("_")[1];
			   var delobj = document.getElementById("apdel_"+b);
			   var statusobj = document.getElementById("apstatus_"+b);
			   var manageobj = document.getElementById("apmanage_"+b);
			   //alert("manageobj:"+manageobj);
				if(delobj.style.display=="none"){
				  delobj.style.display="";
				  statusobj.style.display="";
				  manageobj.style.display="";
				  }else{
				  delobj.style.display="none";
				  statusobj.style.display="none";
				  manageobj.style.display="none";
				}
			}
			//alert("downX"+downX)
		   //alert(" upX"+ upX);
	  }
		  
		  
   delobj.onclick=function(){
	   var j= this.id.split("_")[1];
	    var divobj = document.getElementById("apdiv_"+j); 
		if(confirm("ȷ��ɾ����")){
			  $("idContainer").removeChild(divobj);
			  var li = document.createElement("li");
			  li.id = "uap" + u;
			  var b = this.id.split("_")[1];
			  userialnum.push(document.getElementById("apserial_"+b).value);
			  uapname.push(document.getElementById("apname_"+b).value);
			  uapmodel.push(document.getElementById("apmodel_"+b).value);
			  uapip.push(document.getElementById("apip_"+b).value);
			  uapmac.push(document.getElementById("apmac_"+b).value);
			  uclienCt.push(document.getElementById("clientCt_"+b).value);
			  umanaged_enable.push(document.getElementById("managed_enable_"+b).value);
			  uchannel.push(document.getElementById("channel_"+b).value);
			  ussid.push(document.getElementById("ssid_"+b).value);
			  UNMAC.push(document.getElementById("APMAC_"+b).value);
			  //alert(userialnum);
			  li.innerHTML="<img src='images/normal.png' /> "+ uapmac[u] + "("+ userialnum[u]+")";
			  li.style.cursor = "pointer";
			  document.getElementById("apList").appendChild(li);
			 // alert(li.id);
			  u++;
			  LTC();
			  }
		   };
		   
	statusobj.onclick = function(){
		var j= this.id.split("_")[1];
		    var mac = document.getElementById("APMAC_"+j).value;
			APCONFIG.macStr.value= mac;
		   	openForm(APCONFIG,"system_summary.asp",750,470);
		    };	 
		
	manageobj.onclick = function(){
		var j= this.id.split("_")[1];
			document.getElementById("errorS").innerHTML="";
				var left = this.offsetLeft;
				var top = this.offsetTop;
				//alert("divobj.offsetLeft"+ this.offsetLeft);
				//alert("divobj.offsetTop"+ this.offsetTop);
				left = document.getElementById("idContainer").offsetLeft+this.parentNode.offsetLeft+left+40;//���ڵ������к���������������
				top = document.getElementById("idContainer").offsetTop+this.parentNode.offsetTop+top+15;	
				//alert(this.parentNode.offsetLeft);
				//alert("pwd"+ left);
				//alert("pwd" + top);
				var tab = document.getElementById("tbPwd");
				tab.style.display="none";
				var newDiv;
				var s = document.getElementById("DivS");
				if(s){
					s.style.display="";
					newDiv = document.getElementById("DivS");
				}else{
					newDiv = document.createElement("div");
				}
				newDiv.id = "DivS";
				newDiv.style.position = "absolute";
				newDiv.style.zIndex = "9999";
				newDivWidth = 250;
				newDivHeight = 100;
				newDiv.style.width = newDivWidth + "px";
				newDiv.style.height = newDivHeight + "px";
				newDiv.style.top = top + "px";
				newDiv.style.left = left + "px";
				newDiv.style.background = "#ffffff";
				tab.style.border = "#888 1px solid";
				newDiv.style.padding = "5px"; 
				newDiv.appendChild(tab);		
				document.body.appendChild(newDiv);
				document.getElementById("img_id").value=j;
				//alert("document.getElementById(img_id).value"+document.getElementById("img_id").value);
				document.getElementById("password").value="";//Ϊ0ʱ���ap��ֱ���������к� �ı������� 0
				//document.getElementById("tbPwd").style.display="none";
				manageAPOk();
			};
			
			
	var drag = new Drag(divid, { mxContainer: "idContainer", Handle: id, Limit: true});
        	 
	};//for(var n = 0;n<apnum;n++)
	
		csm=new changeShowModel("changeShow","ȫ��","�˳�ȫ��");		
}



/**ȡ����ť**/
function close(){
	document.getElementById("DivS").style.display="none";
	 
}
function cancel(){
	close();
}



function save(){
	var img = document.getElementsByTagName("img");
	var bgimg = document.getElementById("bgimg");
	var width = bgimg.width;
	var height =  bgimg.height;
	//alert("width:"+width+";height:"+height);
	document.getElementById("bgimg_w").value=width;
	document.getElementById("bgimg_h").value=height;
	var arr = new Array();
	var str="";
	for(var i =0;i<img.length;i++){
		var imgid = img[i].id;
		//alert("save imgid = " + imgid);
		var imgobj = img[i];
		//alert("imgobj = " + imgobj );
		
		//alert("span id:"+spanid);
		if(imgid.indexOf("ap_")!=-1){//�Ǳ���ͼƬ
			var num =  imgid.split("_")[1];
			var divobj = document.getElementById("apdiv_"+num);
			//alert("save divobj = "+ divobj);
			var serialobj = document.getElementById("apserial_"+num);//hidden  type=input hidden.id= apserial_"+num hidden.value=serialnum[n]/serial
			//alert("save serialobj = " + serialobj);
			//alert("save serialobj.value = " + serialobj.value);
			var serialnum=0;
			if(serialobj){
				serialnum=serialobj.value;
			}
			//var serialnum = document.getElementById("apserial_"+num).value;
			var left = divobj.offsetLeft;
			var top = divobj.offsetTop;
			//alert("div left:"+divobj.offsetLeft+";div top:"+divobj.offsetTop);
			str = left+"-"+top+"-"+serialnum;	
			
			arr.push(str);
		}	
	}
	//alert("arr:"+arr);
	$("apsinfo").value=arr.join(",");
	//alert("apinforr:"+$("apsinfo").value);//x -y -serialnum
  
	APCONFIG.submit();
}

	//�Ŵ���СͼƬ
 function imgToSize(size) {	
		var img = document.getElementById("bgimg")
				var idContainer = document.getElementById("idContainer");
		var idDrag = document.getElementById("idDrag");
		
		var containerleft = idContainer.offsetLeft;
		var containerTop = idContainer.offsetTop;
		var idDragleft =idDrag.offsetLeft;
		var idDragtop = idDrag.offsetTop; 
		var imgleft = img.offsetLeft;
		var imgtop =img.offsetTop;
		//alert("containerleft:"+containerleft+";containerTop:"+containerTop+";idDragleft:"+idDragleft+";idDragtop:"+idDragtop+";imgleft:"+imgleft+";imgtop:"+imgtop);
		//alert("size:"+size);
		
		var oWidth=img.width; //ȡ��ͼƬ��ʵ�ʿ��
		var oHeight=img.height; //ȡ��ͼƬ��ʵ�ʸ߶�
		img.width=oWidth + size;
		img.height=oHeight + size/oWidth*oHeight;
		if(path_obj == ""){
			var oCoverDiv = document.getElementById("cover");
			oCoverDiv.style.width = oWidth + size;
			
			}
		var idHandle = document.getElementById("idHandle");
		//alert("idHandle offsetLeft:"+idHandle.offsetLeft+";idHandle top:"+idHandle.offsetTop);
		
		var apimg = document.getElementsByTagName("img");
		//alert("apimg.length:"+apimg.length);
		for(var i =0;i<apimg.length;i++){
		var apimgid = apimg[i].id;
			if(apimgid.indexOf("ap_")!=-1){
				var num =  apimgid.split("_")[1];
				//alert("num :"+num);
				var objimg = document.getElementById("apdiv_"+num);
				var currentX =objimg.offsetLeft*((oWidth + size)/oWidth);
				var currentY = objimg.offsetTop*((oHeight + size/oWidth*oHeight)/oHeight);
				var left = currentX;
				var top = currentY;
				objimg.style.left=left+"px";
				objimg.style.top=top+"px";
				
			}
				
		}

		
    }

function changeImgBig(){
		var img = document.getElementById("bgimg");
		var oWidth=img.width; //ȡ��ͼƬ��ʵ�ʿ��

		if(oWidth < 1200){
		imgToSize(50);	
		var scrollButton = document.getElementById("scrollBt");
		toptop -=10;
		scrollButton.style.top = toptop + "px";
		if(toptop<20){
		toptop = 20;
		scrollButton.style.top = toptop +"px";
		return false;
		}
		}	
}
function changeImgSmall(){
		var img = document.getElementById("bgimg");
		var oWidth=img.width; //ȡ��ͼƬ��ʵ�ʿ��

		if(oWidth>700){
		imgToSize(-50);	
		var scrollButton = document.getElementById("scrollBt");
		toptop +=10;
		scrollButton.style.top = toptop +"px";
		if(toptop>121){
		toptop = 121;
		scrollButton.style.top = toptop +"px";
		return false;
		}
   }	
}
function changeShowModel(tagObj,fullScreenTag,normalTag)
{
	this.curMostOuter="208px,0,*";//parent.document.getElementById('mostOuterFrameSet').getAttribute("cols");
	this.curMain="46px,40,*";//parent.document.getElementById('mainFrameSet').getAttribute("rows");
//	this.curMiddle="205,*";//parent.document.getElementById('middleFrameSet').getAttribute("cols");
	this.tagObj=(typeof(tagObj) == 'string') ? document.getElementById(tagObj) : tagObj;
	//�����tagObj���뺬��value���ԡ���������дchange��Ա����
	this.fullScreenTag=fullScreenTag || "ȫ��";
	this.normalTag=normalTag || "�˳�ȫ��";
	if(window.parent!=window.self) 
	{
		if(parent.document.getElementById('frameset1').getAttribute("cols") == this.curMostOuter)
		{
			this.tagObj.value=this.fullScreenTag;
		}
		else
		{
			this.tagObj.value=this.normalTag;
		}
	}//��ť����ʾȫ����ʾ�����˳�ȫ��
	/*******************
	ȫ����ʾ
	*******************/
	this.showInFullScreen=function()
	{
		if(window.parent==window.self) return;
		parent.document.getElementById('frameset1').setAttribute("cols","0,0,*");
		parent.document.getElementById('mainFrameSet').setAttribute("rows","0,0,*");
//		parent.document.getElementById('middleFrameSet').setAttribute("cols","0,*");
	}
	/**************************
	�˳�ȫ��
	***************************/
	this.showInNormal=function()
	{
		if(window.parent==window.self) return;
		parent.document.getElementById('frameset1').setAttribute("cols",this.curMostOuter);
		parent.document.getElementById('mainFrameSet').setAttribute("rows",this.curMain);
//		parent.document.getElementById('middleFrameSet').setAttribute("cols",this.curMiddle);
	}
	/*********
	�ı���ʾģʽ
	**********/
	this.change=function()
	{
		if(window.parent==window.self) return;
		if(this.tagObj.value == this.fullScreenTag)
		{
			this.showInFullScreen();
			this.tagObj.value=this.normalTag;
		}
		else
		{
			this.showInNormal();
			this.tagObj.value=this.fullScreenTag;
			
		}
	}
	
}
var Class = {
	create: function() {
		return function() { this.initialize.apply(this, arguments); }
	}
}

var Extend = function(destination, source) {
	for (var property in source) {
		destination[property] = source[property];
	}
}

var Bind = function(object, fun) {
	return function() {
		return fun.apply(object, arguments);
	}
}

var BindAsEventListener = function(object, fun) {
	return function(event) {
		return fun.call(object, (event || window.event));
	}
}

var CurrentStyle = function(element){
	return element.currentStyle || document.defaultView.getComputedStyle(element, null);
}

function addEventHandler(oTarget, sEventType, fnHandler) {
	if (oTarget.addEventListener) {
		oTarget.addEventListener(sEventType, fnHandler, false);
	} else if (oTarget.attachEvent) {
		oTarget.attachEvent("on" + sEventType, fnHandler);
	} else {
		oTarget["on" + sEventType] = fnHandler;
	}
};

function removeEventHandler(oTarget, sEventType, fnHandler) {
    if (oTarget.removeEventListener) {
        oTarget.removeEventListener(sEventType, fnHandler, false);
    } else if (oTarget.detachEvent) {
        oTarget.detachEvent("on" + sEventType, fnHandler);
    } else { 
        oTarget["on" + sEventType] = null;
    }
};

//�Ϸų���
var Drag = Class.create();
Drag.prototype = {
  //�ϷŶ���
  initialize: function(drag, options) {
	this.Drag = $(drag);//�ϷŶ���
	//alert("drag:"+drag+";this.Drag:"+this.Drag.offsetLeft+";;top:"+this.Drag.offsetTop); 
	this._x = x;
	this._y =  y;//��¼�������ϷŶ����λ��
	this._marginLeft = this._marginTop = 0;//��¼margin
	//�¼�����(���ڰ��Ƴ��¼�)
	this._fM = BindAsEventListener(this, this.Move);
	this._fS = Bind(this, this.Stop);
	
	this.SetOptions(options);
	
	this.Limit = !!this.options.Limit;
	this.mxLeft = parseInt(this.options.mxLeft);
	this.mxRight = parseInt(this.options.mxRight);
	this.mxTop = parseInt(this.options.mxTop);
	this.mxBottom = parseInt(this.options.mxBottom);
	
	this.LockX = !!this.options.LockX;
	this.LockY = !!this.options.LockY;
	this.Lock = !!this.options.Lock;
	
	this.onStart = this.options.onStart;
	this.onMove = this.options.onMove;
	this.onStop = this.options.onStop;
	
	this._Handle = $(this.options.Handle) || this.Drag;
	this._mxContainer = $(this.options.mxContainer) || null;
	
	this.Drag.style.position = "absolute";
	//͸��
	if(isIE && !!this.options.Transparent){
		//����ϷŶ���
		with(this._Handle.appendChild(document.createElement("div")).style){
			width = height = "100%"; backgroundColor = "#fff"; filter = "alpha(opacity:0)"; fontSize = 0;
		}
	}
	//������Χ
	this.Repair();
	addEventHandler(this._Handle, "mousedown", BindAsEventListener(this, this.Start));
  },
  //����Ĭ������
  SetOptions: function(options) {
	this.options = {//Ĭ��ֵ
		Handle:			"",//���ô������󣨲�������ʹ���ϷŶ���
		Limit:			false,//�Ƿ����÷�Χ����(Ϊtrueʱ�����������,�����Ǹ���)
		mxLeft:			0,//�������
		mxRight:		9999,//�ұ�����
		mxTop:			0,//�ϱ�����
		mxBottom:		9999,//�±�����
		mxContainer:	"",//ָ��������������
		LockX:			false,//�Ƿ�����ˮƽ�����Ϸ�
		LockY:			false,//�Ƿ�������ֱ�����Ϸ�
		Lock:			false,//�Ƿ�����
		Transparent:	false,//�Ƿ�͸��
		onStart:		function(){},//��ʼ�ƶ�ʱִ��
		onMove:			function(){},//�ƶ�ʱִ��
		onStop:			function(){},//�����ƶ�ʱִ��
		onDbClick:		function(){}//�����ƶ�ʱִ��
	};
	Extend(this.options, options || {});
  },
  //׼���϶�
  Start: function(oEvent) {
	if(this.Lock){ return; }
	this.Repair();
	//��¼�������ϷŶ����λ��
	this._x = oEvent.clientX - this.Drag.offsetLeft;
	this._y = oEvent.clientY - this.Drag.offsetTop;
	//alert("this._x:"+this._x+";;;this._y:"+this._y);
	//��¼margin
	this._marginLeft = parseInt(CurrentStyle(this.Drag).marginLeft) || 0;
	this._marginTop = parseInt(CurrentStyle(this.Drag).marginTop) || 0;
	//mousemoveʱ�ƶ� mouseupʱֹͣ
	addEventHandler(document, "mousemove", this._fM);
	addEventHandler(document, "mouseup", this._fS);
	if(isIE){
		//���㶪ʧ
		addEventHandler(this._Handle, "losecapture", this._fS);
		//������겶��
		this._Handle.setCapture();
	}else{
		//���㶪ʧ
		addEventHandler(window, "blur", this._fS);
		//��ֹĬ�϶���
		oEvent.preventDefault();
	};
	//���ӳ���
	this.onStart();
  },
  //������Χ
  Repair: function() {
	if(this.Limit){
		//��������Χ����
		this.mxRight = Math.max(this.mxRight, this.mxLeft + this.Drag.offsetWidth);
		this.mxBottom = Math.max(this.mxBottom, this.mxTop + this.Drag.offsetHeight);
		//�����������������positionΪrelative��absolute����Ի���Զ�λ�����ڻ�ȡoffset֮ǰ����
		!this._mxContainer || CurrentStyle(this._mxContainer).position == "relative" || CurrentStyle(this._mxContainer).position == "absolute" || (this._mxContainer.style.position = "relative");
	}
  },
  //�϶�
  Move: function(oEvent) {
	//�ж��Ƿ�����
	if(this.Lock){ this.Stop(); return; };
	//���ѡ��
	window.getSelection ? window.getSelection().removeAllRanges() : document.selection.empty();
	//�����ƶ�����
	var iLeft = oEvent.clientX - this._x, iTop = oEvent.clientY - this._y;
	//���÷�Χ����
	if(this.Limit){
		//���÷�Χ����
		var mxLeft = this.mxLeft, mxRight = this.mxRight, mxTop = this.mxTop, mxBottom = this.mxBottom;
		//�����������������������Χ����
		if(!!this._mxContainer){
			mxLeft = Math.max(mxLeft, 0);
			mxTop = Math.max(mxTop, 0);
			mxRight = Math.min(mxRight, this._mxContainer.clientWidth);
			mxBottom = Math.min(mxBottom, this._mxContainer.clientHeight);
		};
		//�����ƶ�����
		iLeft = Math.max(Math.min(iLeft, mxRight - this.Drag.offsetWidth), mxLeft);
		iTop = Math.max(Math.min(iTop, mxBottom - this.Drag.offsetHeight), mxTop);
	}
	//����λ�ã�������margin
	if(!this.LockX){ this.Drag.style.left = iLeft - this._marginLeft + "px"; }
	if(!this.LockY){ this.Drag.style.top = iTop - this._marginTop + "px"; }
	//���ӳ���
	this.onMove();
  },
  //ֹͣ�϶�
  Stop: function() {
	//�Ƴ��¼�
	removeEventHandler(document, "mousemove", this._fM);
	removeEventHandler(document, "mouseup", this._fS);
	if(isIE){
		removeEventHandler(this._Handle, "losecapture", this._fS);
		this._Handle.releaseCapture();
	}else{
		removeEventHandler(window, "blur", this._fS);
	};
	//���ӳ���
	this.onStop();
  }
};

function manageAPOk(){
	var f = APCONFIG;
	var num = document.getElementById("img_id").value;
	if(!checkPassword2(document.getElementById("password"),11,"����",1,1)) return;
	//document.getElementById("passStr").value = document.getElementById("password").value;
	document.getElementById("passStr").value = "admin";
	//alert("document.getElementById(passStr).value:"+document.getElementById("passStr").value);
	document.getElementById("ipStr").value= document.getElementById("apip_"+num).value;
	//alert("document.getElementById(ipStr).value:"+document.getElementById("ipStr").value);
	document.getElementById("macStr").value= document.getElementById("APMAC_"+num).value;
	//alert("document.getElementById(macStr).value:"+document.getElementById("macStr").value);
	document.getElementById("managedEn").value= document.getElementById("managed_enable_"+num).value;	
	//alert("document.getElementById(managedEn).value:"+document.getElementById("managedEn").value);
	
	openForm(APCONFIG,"ap_remote_manage.asp",750,470);
	close();
	
}


function showAPs(){
				
		if(jQuery("#leftApHanddle").position().left>100){
			jQuery("#leftAp").animate({width:'0px'});
			jQuery("#leftApHanddle").animate({left: '0px'});
			jQuery("#pushimg").attr("src","images/DPpush.png");
		}else{
			jQuery("#leftAp").animate({width:'220px'});
			jQuery("#leftApHanddle").animate({left:'220px'});
			jQuery("#pushimg").attr("src","images/DPpush2.png");
		}
	
}

function selectDW(val){
	var f=APCONFIG;
	//f.DWname.value = val;//��ѡ�е�ͼֽ���Ƹ�ֵ������Ԫ�� DWname Ȼ�󴫵���̨
	//alert("f.oldName.value"+f.oldName.value);
	//f.action="";//DrawingPreview.asp?oldName=val
	//f.submit();
	//alert(val);
	self.location='DrawingPreview.asp?oldName='+f.oldName.value;
	
}

function showSummary(val){
	var img = $("idContainer").getElementsByTagName("img");
	if(val.checked){
		for(var i=0;i<img.length;i++){
			var imgid = img[i].id;
			if(imgid.indexOf("ap_")!=-1){//�Ǳ���ͼƬ����ͼƬ����
				var a =  imgid.split("_")[1];
			    var summ = document.getElementById("apsummary_"+a);
			    summ.style.display="";
			}/*if*/
	    }/*for*/
		var other = document.getElementById("detail");
			other.checked=0;
			for(var i=0;i<img.length;i++){
				var imgid = img[i].id;
				if(imgid.indexOf("ap_")!=-1){//�Ǳ���ͼƬ����ͼƬ����
					var a =  imgid.split("_")[1];
			    	var deta = document.getElementById("apdetail_"+a);
					deta.style.display="none";
				}/*if*/
	    	}/*for*/
			
  }else{
		  for(var i=0;i<img.length;i++){
		 	 var imgid = img[i].id;
		  	 if(imgid.indexOf("ap_")!=-1){//�Ǳ���ͼƬ����ͼƬ����
			  	var a =  imgid.split("_")[1];
			 	 var summ = document.getElementById("apsummary_"+a);
			 	 summ.style.display="none";
		 	 }/*if*/
	 	}/*for*/
	}/*else*/
	
}

function showDetail(val){
	var img = $("idContainer").getElementsByTagName("img");
	
	if(val.checked){
		for(var i=0;i<img.length;i++){
			var imgid = img[i].id;
			if(imgid.indexOf("ap_")!=-1){//�Ǳ���ͼƬ����ͼƬ����
				var a =  imgid.split("_")[1];
			    var deta = document.getElementById("apdetail_"+a);
			    deta.style.display="";
			}/*if*/
	    }/*for*/
		
		var other = document.getElementById("summary");
			other.checked=0;
			for(var i=0;i<img.length;i++){
				var imgid = img[i].id;
				if(imgid.indexOf("ap_")!=-1){//�Ǳ���ͼƬ����ͼƬ����
					var a =  imgid.split("_")[1];
			    	var summ = document.getElementById("apsummary_"+a);
				    summ.style.display="none";
				}/*if*/
	    	}/*for*/
		}
	else{
			 for(var i=0;i<img.length;i++){
		 	 	var imgid = img[i].id;
		  	    if(imgid.indexOf("ap_")!=-1){//�Ǳ���ͼƬ����ͼƬ����
			  		var a =  imgid.split("_")[1];
			 		var deta = document.getElementById("apdetail_"+a);
					deta.style.display="none";
		 		 }/*if*/
	 		}/*for*/
			
		}/*else*/
	
}

function del(){
var oSelect= document.getElementById("oldName");
//      alert("123"+oSelect.value);        
    if(confirm("ȷ��ɾ����")){
		APCONFIG.delstr.value=oSelect.value;
		APCONFIG.action="/goform/formApDrawingDel";
	        APCONFIG.submit();
	}
}
function delAll(){
    if(confirm("ȷ��ɾ��ȫ����")){
		APCONFIG.action="/goform/formApDrawingDelAll";
		APCONFIG.submit();
	}
}




	
function LTC(){	
  
	for(var i =0;i<userialnum.length;i++){
		
		(function(i){jQuery("#uap"+i).on('dblclick',function(event,ui){
			//alert("LTCɾǰuserialnum.length��"+userialnum.length);
			var uap=document.getElementById("uap"+i);
			//alert("LTCapList"+document.getElementById("apList"));
			//alert("LTCuap"+i+":"+uap.id);
			document.getElementById("apList").removeChild(uap);
			  m++;
			//alert("LTCmove m:"+m)
		var divobj =document.createElement("div");
		var divid ="apdiv_"+m;
			divobj.id=divid;
			divobj.style.position="absolute";
			divobj.style.left=240+"px";//�����ȥ�ĳ�ʼλ��
			divobj.style.top=10+"px";
			//jQuery(divobj).draggable();
		//	$("idContainer").appendChild(divobj);//��idcontainer�̶�λ�����ap
		//var drag = new Drag(divid, { mxContainer: "idContainer", Handle: id, Limit: true})	;
		var imgobj =document.createElement("img");
		   	imgobj.src="images/normal.png";
			var id = "ap_"+m;
			imgobj.id=id;	
			imgobj.className="contextMenu";
			imgobj.style.position="absolute";	
			imgobj.style.left=25+"px";
			imgobj.style.top=21+"px";
			imgobj.style.cursor = "pointer";
			divobj.appendChild(imgobj);
			
	   var delobj=document.createElement("div");
	   		delobj.innerHTML="<img src='images/DPdelete.png' width='20' height='20' />";
			delobj.id="apdel_"+m;
			delobj.style.width=20+"px";
			delobj.style.position="absolute";
			delobj.style.top=20+"px";
			delobj.style.left=55+"px";
			delobj.style.display="none";
			delobj.style.cursor = "pointer";
	  		divobj.appendChild(delobj);
					 
	   var statusobj=document.createElement("div");
	   		statusobj.innerHTML="<img src='images/DPstatus.png' width='20' height='20' />";
			statusobj.id="apstatus_"+m;
			statusobj.style.width=20+"px";
			statusobj.style.position="absolute";
			statusobj.style.left=28+"px";
			statusobj.style.top=0+"px";
			statusobj.style.display="none";
			statusobj.style.cursor = "pointer";
			divobj.appendChild(statusobj);
	 	   
	   var manageobj=document.createElement("div");
	   
	 	    manageobj.innerHTML="<img src='images/DPmanage.png' width='20' height='20' />";
			manageobj.id="apmanage_"+m;
			manageobj.style.width=20+"px";
			manageobj.style.position="absolute";
			manageobj.style.left=0+"px";
			manageobj.style.top=20+"px";
			manageobj.style.display="none";
			manageobj.style.cursor = "pointer";
			divobj.appendChild(manageobj);
	
	 var summaryobj=document.createElement("div");
	 	  summaryobj.id="apsummary_"+m;
		  summaryobj.style.position="absolute";
		  summaryobj.style.left=40+"px";
		  summaryobj.style.top=40+"px";
		  summaryobj.style.border="black 1px solid";
		  summaryobj.align="left";
	  var titlemsg="";
	  var titlesum="";
	  var imgtitle = "";
	  
	  var apname_h = document.createElement("input");
		  apname_h.type="hidden";
		  apname_h.id="apname_"+m;
		  document.body.appendChild(apname_h);
		  imgtitle +="��  �ƣ�"+uapname[i]+'\n';
		 
	  if(uapname[i]!=""){
		  titlemsg +="��  �ƣ�"+uapname[i]+'<br />';
		  apname_h.value=uapname[i];
		 }else{apname_h.value="";}
		 
		
	var apmodel_h = document.createElement("input");
		 apmodel_h.type="hidden";
		 apmodel_h.id="apmodel_"+m	; 
		 document.body.appendChild(apmodel_h);	
	  if(uapmodel[i]!=""){
		  titlemsg +="��  �ţ�"+uapmodel[i]+'<br />';
		  apmodel_h.value=uapmodel[i];
		  }else{apmodel_h.value="";}
		  
		 
	var apip_h = document.createElement("input");
		  apip_h.type="hidden";
		  apip_h.id="apip_"+m;
		  //alert(apip_h.id); 
		  document.body.appendChild(apip_h);	
	  if(uapip[i]!=""){
		  titlemsg +="IP ��ַ��"+uapip[i]+'<br />';
		  titlesum +="IP ��ַ��"+uapip[i]+'<br />';
		 // imgtitle +="IP ��ַ��"+uapip[i]+'\n';
		  apip_h.value=uapip[i];
		 }else{apip_h.value="";}
		 
		 
	var apmac_h = document.createElement("input");
		  apmac_h.type="hidden";
		  apmac_h.id="apmac_"+m;	
		  document.body.appendChild(apmac_h); 
	  if(apmac[i]!=""){
		  titlemsg +="MAC��ַ��"+uapmac[i]+'<br />';
		  titlesum +="MAC��ַ��"+uapmac[i]+'<br />';
		  imgtitle +="MAC��ַ��"+uapmac[i]+'\n';
		  apmac_h.value=uapmac[i];		
		  }else{apmac_h.value="";}
		  
		  var APMAC_H = document.createElement("input");
		  APMAC_H.type = "hidden";
		  APMAC_H.id = "APMAC_"+m;
		  APMAC_H.value = UNMAC[i];
		  document.body.appendChild(APMAC_H);
                  document.getElementById('APMAC_'+m).value=UNMAC[i];
		  
		  
	  if(umanaged_enable[i]!=""){
		  var managed_enable_h = document.createElement("input");
		  managed_enable_h.type="hidden";
		  managed_enable_h.id="managed_enable_"+m;
		  managed_enable_h.value=umanaged_enable[i];		
		  document.body.appendChild(managed_enable_h);
	  }
  
	var hidden = document.createElement("input");
		  hidden.type="hidden";
		  hidden.id="apserial_"+m;	
		 document.body.appendChild(hidden);  
	 if(userialnum[i]!="0"){//0
		  titlemsg +="���к�:"+userialnum[i]+'<br />';	
		  titlesum +="���к�:"+userialnum[i]+'<br />';	
		 // imgtitle +="���к�:"+userialnum[i]+'\n';	
		  hidden.value=userialnum[i];
		  }else{hidden.value="0";}
		  
		  
	 var clientCt_h = document.createElement("input");
		  clientCt_h.type = "hidden";
		  clientCt_h.id = "clientCt_"+ m;
		  document.body.appendChild(clientCt_h);
	  if(uclienCt[i]!=""){
		  titlemsg +="�ͻ�������"+uclienCt[i]+'<br />';	
		  imgtitle +="�ͻ�������"+uclienCt[i]+'\n';
		  clientCt_h.value = uclienCt[i];
		 }else{clientCt_h.value = "";}
		 
	 var channel_h = document.createElement("input");
		  channel_h.type = "hidden";
		  channel_h.id = "channel_"+ m;
		  document.body.appendChild(channel_h);	 
	  if(uchannel[i]!=""){
		  titlemsg +="�ŵ���"+uchannel[i]+'<br />';	
		  imgtitle +="�ŵ���"+uchannel[i]+'\n';
		  channel_h.value = uchannel[i];
		  }else{channel_h.value ="";}
		  
	 var ssid_h = document.createElement("input");
		 ssid_h.type = "hidden";
		 ssid_h.id = "ssid_" + m;  
		 document.body.appendChild(ssid_h);
	  if(ussid[i]!=""){
		  titlemsg +="SSID��"+ussid[i];	
		  imgtitle +="SSID��"+ussid[i]+'\n';
		  ssid_h.value = ussid[i];
		 }else{ssid_h.value = "";}
	  //alert(titlemsg);
	  
	  summaryobj.style.width=150+"px";
	  summaryobj.style.backgroundColor="white";
	  summaryobj.innerHTML=titlesum;
	  summaryobj.style.display="none";
	  divobj.appendChild(summaryobj);
	
 var detailobj=document.createElement("div");
	  detailobj.id="apdetail_"+m;
	  detailobj.style.position="absolute";
	  detailobj.style.left=40+"px";
	  detailobj.style.top=40+"px";
	  detailobj.style.width=150+"px";
	  detailobj.style.backgroundColor="white";
	  detailobj.innerHTML=titlemsg;
	  detailobj.style.display="none";
	  detailobj.style.border="black 1px solid";
	  detailobj.align="left";
	  divobj.appendChild(detailobj);
	  
	 imgobj.title = imgtitle;
	  
	//alert("aftermove"+m);
	
  //imgobj.onclick=function(){
//	  if(delobj.style.display=="none"){
//		delobj.style.display="";
//		statusobj.style.display="";
//		manageobj.style.display="";
//		}else{
//		delobj.style.display="none";
//		statusobj.style.display="none";
//		manageobj.style.display="none";
//			
//			}
//	  }; /*imgobj.ondblclick*/
//	  
	 
	  imgobj.onmousedown=function(ev){
		  var oEvent = ev||event;
		  downX = this.parentNode.offsetLeft;
		  downY = this.parentNode.offsetTop;
		  
	  }
	  imgobj.onmouseup =function(up){
		   var oEventUp = up||event;
		   upX = this.parentNode.offsetLeft;
		   upY = this.parentNode.offsetTop;
		   
		   
		   if((downX==upX)&&(downY==upY)){
			   var b = this.id.split("_")[1];
			   var delobj = document.getElementById("apdel_"+b);
			   var statusobj = document.getElementById("apstatus_"+b);
			   var manageobj = document.getElementById("apmanage_"+b);
				if(delobj.style.display=="none"){
				  delobj.style.display="";
				  statusobj.style.display="";
				  manageobj.style.display="";
				  }else{
				  delobj.style.display="none";
				  statusobj.style.display="none";
				  manageobj.style.display="none";
				}
			}
			
			//alert("downX"+downX)
		   //alert(" upX"+ upX);
	  }
			
	
		  
  delobj.onclick=function(){
	  if(confirm("ȷ��ɾ����")){
		   $("idContainer").removeChild(divobj);
		  var li = document.createElement("li");
		  li.id = "uap" + u;
		  var b = this.id.split("_")[1];
		  userialnum.push(document.getElementById("apserial_"+b).value);
		  uapname.push(document.getElementById("apname_"+b).value);
		  uapmodel.push(document.getElementById("apmodel_"+b).value);
		  uapip.push(document.getElementById("apip_"+b).value);
		  uapmac.push(document.getElementById("apmac_"+b).value);
		  uclienCt.push(document.getElementById("clientCt_"+b).value);
		  umanaged_enable.push(document.getElementById("managed_enable_"+b).value);
		  uchannel.push(document.getElementById("channel_"+b).value);
		  ussid.push(document.getElementById("ssid_"+b).value);
		  UNMAC.push(document.getElementById("APMAC_"+b).value);
		  //alert(userialnum);
		  li.innerHTML="<img src='images/normal.png' /> "+ uapmac[u] + "("+ userialnum[u]+")";
		  document.getElementById("apList").appendChild(li);
		  //alert(li.id);
		  u++;
		  LTC();
		  }
	   }
		   
   statusobj.onclick = function(){
		var j= this.id.split("_")[1];
		    var mac = document.getElementById("APMAC_"+j).value;
			APCONFIG.macStr.value= mac;
		   	openForm(APCONFIG,"system_summary.asp",750,470);
		}	 
	
  manageobj.onclick = function(){
	  var j= this.id.split("_")[1];
	  document.getElementById("errorS").innerHTML="";
		  var left = this.offsetLeft;
		  var top = this.offsetTop;	
		  left = document.getElementById("idContainer").offsetLeft+this.parentNode.offsetLeft+left+40;//���ڵ������к���������������
		  top = document.getElementById("idContainer").offsetTop+this.parentNode.offsetTop+top+15;	
		  var tab = document.getElementById("tbPwd");
		  tab.style.display="none";
		  var newDiv;
		  var s = document.getElementById("DivS");
		  if(s){
			  s.style.display="";
			  newDiv = document.getElementById("DivS");
		  }else{
			  newDiv = document.createElement("div");
		  }
		  newDiv.id = "DivS";
		  newDiv.style.position = "absolute";
		  newDiv.style.zIndex = "9999";
		  newDivWidth = 250;
		  newDivHeight = 100;
		  newDiv.style.width = newDivWidth + "px";
		  newDiv.style.height = newDivHeight + "px";
		  newDiv.style.top = top + "px";
		  newDiv.style.left = left + "px";
		  newDiv.style.background = "#ffffff";
		  tab.style.border = "#888 1px solid";
		  newDiv.style.padding = "5px"; 
		  newDiv.appendChild(tab);		
		  document.body.appendChild(newDiv);
		  document.getElementById("img_id").value=j;
		  //alert("mmm"+m);
		  //alert("document.getElementById(img_id).value"+document.getElementById("img_id").value);
		  document.getElementById("password").value="";//Ϊ0ʱ���ap��ֱ���������к� �ı������� 
			manageAPOk();
	  }
	  
		  $("idContainer").appendChild(divobj);
		  //jQuery(divobj).draggable();
		  var drag = new Drag(divid, { mxContainer: "idContainer", Handle: id, Limit: true})	;
		  document.getElementById("tbPwd").style.display="none";
		  
	  })//on
 })(i);
}//for
}//LTC	


jQuery(document).ready(function(){
	init();
	jQuery("#leftApHanddle").click(function(){
		if(jQuery("#leftApHanddle").position().left>100){
			jQuery("#leftAp").css("overflow-x","hidden");
			jQuery("#leftAp").animate({width:'0px'});
			jQuery("#leftApHanddle").animate({left: '0px'});
			jQuery("#pushimg").attr("src","images/DPpush.png");
		}else{
			jQuery("#leftAp").animate({width:'220px'});
			jQuery("#leftApHanddle").animate({left:'220px'});
			jQuery("#pushimg").attr("src","images/DPpush2.png");
			
		}
	});//jQuery("#leftApHanddle").click(function()
	
	LTC();
  
})//ready




</script>
<style type="text/css">
#idContainer{ position:relative;}
#idDrag{ }
#idHandle{ height:25px; overflow:hidden; background:#fff;}
#left{
	
	position:absolute;
	overflow:visible;
	top:0px;
	left:0px;
	z-index:999999;
	
	}
#leftAp{
	top:58px;
	width:0px;
	height:498px;
	position:absolute;
	z-index:999999;
	border:black 1px solid;
	background-color:#EAEAEA;
	opacity:0.95;
	filter:alpha(opacity=95);
	overflow-x:visible;
	overflow-y:auto;
	}
#leftAp ul{
	overflow:visible;
	list-style:none;
	text-align:left;	
	}
#leftAp ul li{
	/*display:block;*/
	/*position:relative;*/
	/*border:1px pink solid;*/
	height:22px;
	line-height:22px;
	vertical-align:middle;
	
	}
#leftApHanddle{
	top:58px;
	left:0px;
	width:10px;
	height:498px;
	position:absolute;
	z-index:999999;
	
	}	
</style>
<body background="images/body_main.png" style="background-repeat:repeat-x;" onload="">
<table width="100%" border="0" bordercolor="red" cellspacing="0" cellpadding="0">
   <!--*****���ϼ��****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****�м�����****-->
  <tr>
   <td width="15"></td>
  <!--////����-->
  <td ><script language="javascript">showSecondTitle("AP�ֲ�ͼ");</script>
  <script language="javascript">
		var arrayThreeMenu=new Array(
						"90%",
						2,
						new Array("�ֲ�ͼ",'DrawingList.asp',1,1),
						new Array("����ͼֽ",'ImportDrawing.asp',1,0)
						


		);
		showThreeMenu();
	</script>
 <!-- <table width="100%" border="1" bordercolor="green" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">//showerror(errorstr,80);//��ʾ���淵�صĴ�����Ϣ</script>
  <tr valign="top">
    <td>-->
  <!--<table width="100%" border="1" bordercolor="blue" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
    <td align="center"  class="style7">--><table width="90%" border="0" bordercolor="yellow" cellspacing="0" cellpadding="0" align="center" class="three_td">
  <tr>
    <td>
	<table border="0" bordercolor="pink" cellspacing="0" cellpadding="0" width="100%" align="center" style="position:relative;"><tr><!--<td class="gapTd"></td>--><td align="center">
    <!--//ֱ�ӽ�ҳ���form���ڴ˴������磺<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
 <form name="APCONFIG" id="APCONFIG"   method="post" action="/goform/formApDrawingEdit">
       <input type="hidden" value="DrawingPreview" name="parentPage" id="parentPage"> 
       <input type="hidden"  name="path_obj" id="path_obj" value="" />
       <input type="hidden"  name="bgimg_h" id="bgimg_h" value="" />
       <input type="hidden"  name="bgimg_w" id="bgimg_w" value="" />
       <input type="hidden" name="passStr" id="passStr" value="" />
       <input type="hidden" name="ipStr" id="ipStr" value="" />
       <input type="hidden" name="macStr" id="macStr" value="" />
       <input type="hidden" name="vidStr" id="vidStr" value="" />
       <input type="hidden" value="" name="dhcpEn">
       <input type="hidden" value="" name="devType">
       <input type="hidden" value="" name="szName">
       <input type="hidden" value="" name="managedEn">
       <input type="hidden" name="managedEn" id="managedEn" value="" />
       <input type="hidden" name="delstr" id="delstr" value="" />
       
       <input type="text" style="display:none"><!--�˿ؼ�����ɾ�������ڽ��ie����input��[ENTER]�Զ��ύ������-->
	<table border="0" bordercolor="green" cellspacing="0"  cellpadding="0" height="100%" align="center" width="100%" style="position:relative;">
    <tr><td height="10" colspan="2"></td></tr>    
    <tr><td align="left" valign="center">
   &nbsp;&nbsp; ��ʾͼֽ:
    <select name="oldName" id="oldName" class="input" style="width:84px;" onChange="selectDW(this.value)">
    	<script type="text/javascript">
		document.write('<option selected="selected" value="">��ѡ��</option>');
		
       for(var i=0;i<drawings.length;i++){
			if(drawings[i]==path_obj)document.write('<option selected="selected" value="'+drawings[i]+'">'+drawings[i]+'</option>');
			else document.write('<option value="'+drawings[i]+'">'+drawings[i]+'</option>');
				
			}
        </script>
    </select>
    &nbsp;&nbsp;&nbsp;��ʾ��Ϣ��<input type="checkbox" name="summary" id="summary" onClick="showSummary(this)"/>��Ҫ<input type="checkbox" name="detail" id="detail" onClick="showDetail(this)"/>��ϸ</td><td align="right">
    <input type="button" class="input-button" value="AP�б�"  onClick="showAPs();" />
    <input type="button" class="input-button" value="����"  onClick="save();" />
    <input type="button" class="input-button" value="ɾ��" onClick="del();"/>
    <input type="button" class="input-button" value="ɾ��ȫ��" onClick="delAll();"/>
    <input type="button" class="input-button" value="ˢ��"  onClick="fresh()" />
    <input type="button" class="input-button" value="ȫ��"  id="changeShow" onClick="csm.change();" />
    &nbsp;&nbsp;&nbsp;&nbsp;
    <div style="width:15px; height:15px; top:124px; right:100px; position:fixed;  z-index:9999999; cursor:pointer;" onselectstart="return false"  onClick="changeImgBig()"></div>
    
    <div style="width:15px; height:150px; position:fixed; right:100px; top:120px; z-index:999999; background-image:url(images/DPscroll.png); background-repeat:no-repeat; background-position:-6px 0;" onselectstart="return false">   
    
    <img id="scrollBt" src="images/DPscrollbt.png" width="8" height="8" style="top:122px; left:-3px; position:relative; z-index:9999999;" onselectstart="return false"/>
    </div>
     <div style="width:15px; height:15px; top:250px; right:100px; position:fixed; z-index:9999999; cursor:pointer;" onselectstart="return false" onClick="changeImgSmall()"></div>
   <!-- <input type="button" class="input-button" value="����" name="input2" onClick="window.open('../rhelpinfo.htm#topoy')">--></td>
    </tr>
   <tr><td height="10" colspan="2"></td></tr>
   
    <tr height="20">
     <div id="left">
    	<div id="leftAp">
         	<ul id="apList">
            </ul>
         </div>
         <div id="leftApHanddle">
         <img id="pushimg" src="images/DPpush.png" width="10" height="498" />
         </div>
    </div> 
    
         <td colspan="2" id="idContainer" style="height:500px;" align="center"><!--border:red 1px solid-->
    	 <div id="idDrag" style="border:blue 1px solid; display:none;">
         	<div id="idHandle" style="border:green 1px solid; display:none;">
         	</div>
         </div>
         
         
        
         <!--<div id="leftAp">
         	<ul id="apList"></ul>
         </div>-->
         <br />
    	<!--<img src="" width="700px" height="500px" id="bgimg"/>  -->      
    </td></tr>  
     <tr height="20"><td colspan="2" id="idShow">
    	
    </td></tr>  
    </table> 
    <input type="hidden" name="apsinfo" id="apsinfo" value="" />
    <style type="text/css">
	div{ margin:0; padding:0;}
    </style>
    <div id="tbPs"  style="display:none; position:absolute; z-index:99999; " border="0" cellpadding="0" cellspacing="0" style="margin:0; padding:0;">
	<div class="style21" style="background-color:#008893; width:250px; line-height:25px; font-size:12px; color:#fff;"><div style="width:130px; text-align:left; float:left;color:#fff;" >&nbsp;�������к�</div>&nbsp;<div style="width:100px; text-align:right; float:right;color:#fff;"><a style=" color:#fff; text-decoration:none;" href="javascript:close();">x</a>&nbsp;&nbsp;</div></div>
    <div style="clear:both"></div>
    <div style="line-height:45px; font-size:12px"><label height="31" colspan="2"  align="center">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;������:
      <input type="text" size="13" maxlength="11" value=""  class="input" name="serialnum" id="serialnum"></span>&nbsp;&nbsp;<label id="errorS" style="color:red;"></label>
      <input type="hidden" id="img_id" name="img_id" value="" />
      </div>
    <div  style="line-height:30px; font-size:12px; text-align:center;"><span colspan="2" align="center"><input name="input" type="button" class="input-button" onClick="manageOk()" value="ȷ��" />
      <input name="" type="button" class="input-button"  value="ȡ��" onClick="cancel()" /></span></div>
</div>

 <div id="bmenu" style="position:absolute;display:none; height:20px;margin:0px;padding:2px;border:1px solid #cccccc;background-color:#fff;">
 
  <li id="del" style="text-align:left; padding-left:10px; padding-right:10px; line-height:20px; cursor:pointer; list-style:none;">ɾ��</li>
 </div>
 
 
 <div id="tbPwd"  style="display:none; position:absolute; z-index:99999; " border="0" cellpadding="0" cellspacing="0" style="margin:0; padding:0;">
	<div class="style21" style="background-color:#008893; width:250px; line-height:25px; font-size:12px; color:#fff;"><div style="width:130px; text-align:left; float:left;color:#fff;" >&nbsp;��������</div>&nbsp;<div style="width:100px; text-align:right; float:right;color:#fff;"><a style=" color:#fff; text-decoration:none;" href="javascript:close();">x</a>&nbsp;&nbsp;</div></div>
    <div style="clear:both"></div>
    <div style="line-height:45px; font-size:12px"><label height="31" colspan="2"  align="center">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;������:
      <input type="password" size="13" value=""  class="input" name="password"id="password"></span>&nbsp;&nbsp;<label id="errorS" style="color:red;"></label>
      <input type="hidden" id="img_id" name="img_id" value="" />
      </div>
    <div  style="line-height:30px; font-size:12px; text-align:center;"><span colspan="2" align="center"><input name="input" type="button" class="input-button" onClick="manageAPOk()" value="ȷ��" />
      <input name="" type="button" class="input-button"  value="ȡ��" onClick="cancel()" /></span></div>
</div>

 <div id="bmenu" style="position:absolute;display:none; height:20px;margin:0px;padding:2px;border:1px solid #cccccc;background-color:#fff;">
 
  <li id="del" style="text-align:left; padding-left:10px; padding-right:10px; line-height:20px; cursor:pointer; list-style:none;">ɾ��</li>
 </div>
    </form>
 </td><!-- <td class="gapTd"></td>--></tr></table>
 </td>
  </tr>
</table> 
  <!--</td></tr>
</table>--><!--</td></tr></table>--></td>
<!--////end-->
<td width="15"></td>
  </tr>
</table>


<script type="text/javascript">
//�Զ����Ҽ�������
var cmenu = new contextMenu(
    {
   /*menuID : "bmenu",*/
   targetEle : "contextMenu"
  }
 // {
//  // bindings:{
////    //'del' : function(o){//alert("ɾ�� "+o.id);
//////				//alert("ooid:"+o.id);
//////				var id = o.id
//////				var num = id.split("ap_")[1];
//////				var obj = document.getElementById("apdiv_"+num);
//////				document.getElementById("idHandle").removeChild(obj);
//////			}
////   
////    }
//   }
  );
 
// cmenu.buildContextMenu();

//var myimage = document.getElementById("bgimg"); 
//if (myimage.addEventListener) { 
//// IE9, Chrome, Safari, Opera 
//myimage.addEventListener("mousewheel", MouseWheelHandler, false); 
//// Firefox 
//myimage.addEventListener("DOMMouseScroll", MouseWheelHandler, false); 
//} 
//// IE 6/7/8 
//else myimage.attachEvent("onmousewheel", MouseWheelHandler); 



//function MouseWheelHandler(e) {   
//       var e = window.event || e; // old IE support   
//    /*var delta = Math.max(-1, Math.min(1, (e.wheelDelta || -e.detail))); 
//myimage.style.width = Math.max(50, Math.min(800, myimage.width + (30 * delta))) + "px";   */
//       var img = document.getElementById("bgimg");
//	   var oWidth=img.width; //ȡ��ͼƬ��ʵ�ʿ��
//	   var step = (e.wheelDelta || -e.detail)/(Math.abs(e.wheelDelta || -e.detail));
//      // alert("MouseWheelHandler:oWidth = "+ oWidth);
//		if(step>0){
//     		 imgToSize(50*(e.wheelDelta || -e.detail)/(Math.abs(e.wheelDelta || -e.detail)));
//		}else if((step<0)&&(oWidth>700)){
//			 imgToSize(50*(e.wheelDelta || -e.detail)/(Math.abs(e.wheelDelta || -e.detail)));
//		}else { //alert("������С��")
//		return false;
//		}
//		
//    return false;   
//}  	
</script>
</body>
</html>
