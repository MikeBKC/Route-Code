var svgns = "http://www.w3.org/2000/svg";	
var barCls;
 var unit="";
function BarInit(VALUES,COLORS){
	this.svgDocument;	
	this.centerX = 70;//x轴的坐标
	this.centerY = 240;//
	this.Xlength = 400;//x轴横坐标的长度
	this.Ylength = 200;//y抽纵坐标的长度
	this.COLORS = COLORS 
	this.VALUES = VALUES;
	this.drawBarFun;

}
//获得原始数据中最小的数据
function getMinVal(VALUES){
	
	  var pData = new Array(); 
		
	   for (var i in VALUES) {
		  pData.push(VALUES[i]);
	   }
		 var i=0;
		 var min = pData[0];
		  for (i = 0;i<pData.length;i++) {
			
			   if (pData[i] < min){
				 min = pData[i];
				}
		 }	 
	  
	  
	return min;
}
//根据最小值获得单位
function getUnits(min){
	
	var minleng;
	 if(String(min).indexOf(".")!=-1){
		var data = String(min).split(".")[0];
		minleng = String(data).length;
	}else{
		minleng = String(min).length;
	}
	
	var u ="";
	if(minleng<=3){
		u ="B";
	}
	if(minleng==4||minleng==5||minleng==6){
		u ="KB";
		
	}
	if(minleng==7||minleng==8||minleng==9){
		u ="MB";
		
	}
	if(minleng==10||minleng==11||minleng==12){
		u ="GB";
		
	}
          
	return u;
}
//根据单位和字节进行转换
function convertFsize(fsize,unit){

    var data = 0;

	if(unit=="B"){		
		 data = new Number(parseFloat(fsize));
	}
	if(unit=="KB"){		
		 data = new Number(parseFloat(fsize/1024));
	}
	if(unit=="MB"){	
		data = new Number(parseFloat(fsize/(1024*1024)));
	}
	if(unit=="GB"){	
		data = new Number(parseFloat(fsize/(1024*1024*1024)));
	}	
    return data;
}
 


//获得数组最大值
function getValMax(pData){

 var max = pData[0];
 var min = pData[0];
 var i=0;
  for (i = 0;i<pData.length;i++) {
	   if (pData[i] > max){
		max = pData[i];
	    }
	   if (pData[i] < min){
		 min = pData[i];
	    }
 }	 
	  if (min >= 0 && max >= 0) {
	   if (max == 0.0) {
	    max = min = 0.0;
	   } else {
	    if (max <= 1)
	     max = 1;
	    else {
	     max = max < 100 ? Math.ceil(max / 10) * 10 : Math
	       .ceil(max / 50) * 50;
	    }
	   }
	  }
	  var absMax = Math.abs(max) > Math.abs(min) ? Math.abs(max) : Math
	    .abs(min);
	  
	  return absMax;
	 
}



function normalizeValues1(VALUES) {
	   var total = 0;
	   var min = getValueMax(VALUES);//getMinVal(VALUES);
	    unit = getUnits(min);
	   var ret = new Object();
	   var data;
	   for (var i in VALUES) {
		   
	     total += convertFsize(VALUES[i],unit);
		 
	   }
	 
	   for (var i in VALUES) {
		   data = convertFsize(VALUES[i],unit);
		   if(total!=0){
				ret[i] = data;
		   }else{
				ret[i] = 0;
		   }		
	   }	   
	   return ret;
	}


//画图
function drawBarChart(barCls){

	
	var normValues = normalizeValues1(barCls.VALUES);
	var obj = normalizeValues(barCls.VALUES);
	var elem = barCls.svgDocument.getElementById("canvas");
	try{
		elem.removeChild(barCls.svgDocument.getElementById("chartbar"));
		elem.removeChild(barCls.svgDocument.getElementById("legendlable"));	
	}
	catch(e){}
	var canvas = barCls.svgDocument.getElementById("canvas");
	var chart = barCls.svgDocument.createElementNS(svgns,"g");
	chart.setAttribute( "id", "chartbar");


	canvas.appendChild( chart );	

	var legend = barCls.svgDocument.createElementNS(svgns,"g");
	legend.setAttribute( "id", "legendlable");
	canvas.appendChild(legend);		
	initBar(barCls.svgDocument,chart,barCls.centerX,barCls.centerY,barCls.Xlength,barCls.Ylength,"(单位："+unit+")")
	var c=0;
	var pNames = new Array();
	var pDatas = new Array();
	var total = totalValues(barCls.VALUES);
	//

	//飬??
	for(var i in barCls.VALUES){
		if(normValues[i]>=0){
			pNames.push(i);
			pDatas.push(normValues[i]);			
		}
	}

	var valueMM = getValMax(pDatas);//?	
	var yStep = barCls.Ylength / valueMM; //y?
	var DataLength = pNames.length ;
	var xStep = barCls.Xlength / (pNames.length*2+1);//x?
	var colWidth = xStep; 
	var colWidthPre = xStep;
	if(DataLength <4){//?С4ù??
		colWidth = 50;
		colWidthPre = (barCls.Xlength-DataLength*colWidth)/(DataLength+1);
	}
		

	
	var count = 0;	
	var val;
	//λ0?y??y???
	//?0?y???
	if(total==0){
		val=barCls.Ylength/10;
		
		for(n=0;n<11;n++){
			drawY(chart,barCls.svgDocument,(barCls.centerY-n*val),barCls.centerX,barCls.Xlength);
			drawtextY(chart,barCls.svgDocument,(barCls.centerX-20),(barCls.centerY-n*val)+5,(n*val));
		}
	}else{
		val=valueMM/10;

		for(var n=0;n<11;n++){
		drawY(chart,barCls.svgDocument,(barCls.centerY-n*val*yStep),barCls.centerX,barCls.Xlength);
		drawtextY(chart,barCls.svgDocument,(barCls.centerX-20),(barCls.centerY-n*val*yStep)+5,(n*val));
		
	}
	
	}
	
	for(var i in barCls.VALUES){
		c=c+1;
		//var converData = convertFsize(normValues[i],unit);
		if(normValues[i]!=0){
			drawBar(chart,barCls.svgDocument,colWidthPre,colWidth,normValues[i] *yStep,barCls.centerX+c*colWidthPre+(c-1)*colWidth,barCls.centerY,barCls.COLORS[count]);
		if(c!=1){//????
			drawTextBar(chart,barCls.svgDocument,barCls.centerX+c*colWidthPre+(c-1)*colWidth,barCls.centerY-normValues[i]*yStep-5,obj[i]);
		}
		}
	
		createBarLabel(legend,barCls.svgDocument,barCls.centerX+c*colWidthPre+(c-1)*colWidth,i,barCls.centerY);		
		count ++;
	}

}
function initBar(svgDocument,chart,centerX,centerY,Xlength,Ylength,units){
	var pathX = svgDocument.createElementNS(svgns,"path");//X
	pathX.setAttribute("d","M "+centerX+" "+centerY+" L "+(centerX+Xlength)+" "+centerY+" z");
	pathX.setAttribute("stroke-width",2);
	pathX.setAttribute("stroke-opacity",".4");
	pathX.setAttribute("stroke","#333300");

	var text = svgDocument.createElementNS(svgns,"text");//λ
	text.setAttribute("x",centerX-60);
	text.setAttribute("y",centerY+20);
	text.setAttribute("font-family","SimSun");
	text.setAttribute("font-size","16px");
	text.appendChild(svgDocument.createTextNode(units));
	


	var pathX1 = svgDocument.createElementNS(svgns,"path");//X
	pathX1.setAttribute("d","M "+centerX+" "+(centerY-Ylength)+" L "+(centerX+Xlength)+" "+(centerY-Ylength)+" z");
	pathX1.setAttribute("stroke-width",1);
	pathX1.setAttribute("stroke-opacity",".4");
	pathX1.setAttribute("stroke","#333300");

	var pathY= svgDocument.createElementNS(svgns,"path");//y
	pathY.setAttribute("d","M "+centerX+" "+centerY+" L "+centerX+" "+(centerY-Ylength)+" z");
	pathY.setAttribute("stroke-width",2);
	pathY.setAttribute("stroke-opacity",".4");
	pathY.setAttribute("stroke","#333300");
	
	var pathY1= svgDocument.createElementNS(svgns,"path");//y
	pathY1.setAttribute("d","M "+(centerX+Xlength)+" "+centerY+" L "+(centerX+Xlength)+" "+(centerY-Ylength)+" z");
	pathY1.setAttribute("stroke-width",2);
	pathY1.setAttribute("stroke-opacity",".4");
	pathY1.setAttribute("stroke","#333300");

	//var zerotext = svgDocument.createElementNS(svgns,"text");
	chart.appendChild( pathX1 );
	chart.appendChild( text );
	chart.appendChild( pathX );
	chart.appendChild( pathY );
	chart.appendChild( pathY1 );
}
function drawY(chart,svgDocument,serialY,centerX,Xlength){
	var serialY=serialY;
	var g=svgDocument.createElementNS(svgns,"g");
	var path = svgDocument.createElementNS(svgns,"path");
	path.setAttribute("d","M "+(centerX-5)+" "+serialY+" L "+(centerX+Xlength)+" "+serialY+" z");
	path.setAttribute("stroke-width",0.1);
	path.setAttribute("stroke-opacity",".4");
	path.setAttribute("stroke-dasharray","5.2");
	path.setAttribute("stroke","#333300");
	
	var path1 = svgDocument.createElementNS(svgns,"path");
	path1.setAttribute("d","M "+(centerX-5)+" "+serialY+" L "+centerX+" "+serialY+" z");
	path1.setAttribute("stroke-width",2);
	path1.setAttribute("stroke-opacity",".4");
	path1.setAttribute("stroke-dasharray","5.2");
	path1.setAttribute("stroke","#333300");

	g.appendChild(path);
	g.appendChild(path1);
	chart.appendChild(g);
}

function drawtextY(chart,svgDocument,x,serialY,value){
	var textLabel=svgDocument.createElementNS(svgns,"g");
	var text = svgDocument.createElementNS(svgns,"text");
	text.setAttribute("x",x-30);
	text.setAttribute("y",serialY);
	text.appendChild(svgDocument.createTextNode(value.toFixed(1)));
	
	textLabel.appendChild(text);
	chart.appendChild(textLabel);
}
function drawBar(chart,svgDocument,colWidthPre,colWidth,colHeight,x,centerY,color){

	

	
	var bar=svgDocument.createElementNS(svgns,"g");
	var rect = svgDocument.createElementNS(svgns,"rect");
	rect.setAttribute("x",x);
	rect.setAttribute("y",centerY-colHeight);
	rect.setAttribute("width",colWidth);
	rect.setAttribute("height",colHeight);
	rect.setAttribute("fill",color);
	bar.appendChild(rect);
	chart.appendChild(bar);

}
function createBarLabel(legend,svgDocument,x,name,centerY){
	
	var label1 = svgDocument.createElementNS(svgns,"g");
	
	
	var text = svgDocument.createElementNS(svgns,"text");
	text.setAttribute("x",x);
	
	text.setAttribute("y",centerY+20);
	var node = svgDocument.createTextNode(name);
	text.setAttribute("font-size","15px");
	//node.setAttribute("dy","-5,-5,-5,-5,-5");
	text.appendChild(node);
	label1.appendChild(text);
	legend.appendChild(label1);

}

function drawTextBar(chart,svgDocument,x,y,value){

	var g = svgDocument.createElementNS(svgns,"g");
	var text = svgDocument.createElementNS(svgns,"text");
	text.setAttribute("x",x);	
	text.setAttribute("y",y);
	
	var node = svgDocument.createTextNode((value*100).toFixed(2)+" %");
	//node.setAttribute("dy","-5,-5,-5,-5,-5");
	text.appendChild(node);
	g.appendChild(text);
	chart.appendChild(g);
}	
