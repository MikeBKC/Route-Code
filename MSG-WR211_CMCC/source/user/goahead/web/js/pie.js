var defaultColors = new Array("#49bef5","#e3563a", "#13c1d1", "#17aa5f", "#a8c656","#514e95","#e15592","#e2b163","#677bde");
var defaultColor = "#5bca87";//无数据时饼图默认颜色
//获得数据
var pieCls;
function pieInit(VALUES, COLORS ,DCOLOR){
	this.CENTERX = 80;
	this.CENTERY = 100;
	this.RX = 75;//长半径
	this.RY = 75;//短半径
	this.STARTANGLE = 0;
	this.COLORS = COLORS || defaultColors;
	this.DCOLOR = DCOLOR || defaultColor; //数据都为零时的默认颜色
	this.VALUES = VALUES;
	//this.title= title || "饼图";
	this.drawPieFun;
	this.svgDocument;


};
//获得当前上网的人数
function  getOnlinePople(){
	return 12;
};
//对Date的扩展，将 Date 转化为指定格式的String  
Date.prototype.pattern=function(fmt) {        
    var o = {        
    "M+" : this.getMonth()+1, //月份         
    "d+" : this.getDate(), //日         
    "h+" : this.getHours()%12 == 0 ? 12 : this.getHours()%12, //小时         
   "H+" : this.getHours(), //小时         
    "m+" : this.getMinutes(), //分         
    "s+" : this.getSeconds(), //秒         
    "q+" : Math.floor((this.getMonth()+3)/3), //季度         
    "S" : this.getMilliseconds() //毫秒         
    };        
    var week = {        
    "0" : "/u65e5",        
    "1" : "/u4e00",        
    "2" : "/u4e8c",        
    "3" : "/u4e09",        
    "4" : "/u56db",        
    "5" : "/u4e94",        
    "6" : "/u516d"       
    };        
    if(/(y+)/.test(fmt)){        
       fmt=fmt.replace(RegExp.$1, (this.getFullYear()+"").substr(4 - RegExp.$1.length));        
    }        
    if(/(E+)/.test(fmt)){        
        fmt=fmt.replace(RegExp.$1, ((RegExp.$1.length>1) ? (RegExp.$1.length>2 ? "/u661f/u671f" : "/u5468") : "")+week[this.getDay()+""]);        
    }        
    for(var k in o){        
        if(new RegExp("("+ k +")").test(fmt)){        
            fmt = fmt.replace(RegExp.$1, (RegExp.$1.length==1) ? (o[k]) : (("00"+ o[k]).substr((""+ o[k]).length)));        
        }        
    }        
    return fmt;        
}   
//获得系统当前时间
function getCurrentDate(){
	return new Date().pattern("yyyy-MM-dd hh:mm:ss");  
};



var svgns = "http://www.w3.org/2000/svg";


function drawPieChart(pieCls){
		
			var elem = pieCls.svgDocument.getElementById("canvas");
		try{
			elem.removeChild(pieCls.svgDocument.getElementById("chartpie"));
			elem.removeChild(pieCls.svgDocument.getElementById("legendlable"));
	
	
		}
		catch(e){}
	   var canvas = pieCls.svgDocument.getElementById("canvas");
	   var normValues = normalizeValues( pieCls.VALUES );

	   var lastAngle = pieCls.STARTANGLE;
	
	   var count = 0;
	   var chart = pieCls.svgDocument.createElementNS(svgns,"g");
		
	   chart.setAttribute( "id", "chartpie");
	 
	   canvas.appendChild( chart );
	
	   var legend = pieCls.svgDocument.createElementNS(svgns,"g");
	   legend.setAttribute( "id", "legendlable");
	  
	   canvas.appendChild(legend);

		
	   for(var i in pieCls.VALUES){
	     
		   if(normValues[i]>0){
			
			   addWedge(pieCls.svgDocument, chart, 0, pieCls.CENTERX, pieCls.CENTERY, pieCls.RX, pieCls.RY,
			       lastAngle, lastAngle + 360*normValues[i], pieCls.COLORS[count]);
			
		   }
	
		   var label = createLabel(pieCls.svgDocument,pieCls.CENTERX+pieCls.RX+10,(20+20*count),pieCls.COLORS[count], i, normValues[i]);
		   legend.appendChild(label);
		   lastAngle += 360*normValues[i];
		   count++;
	   }	  	
	   	if(totalValues(pieCls.VALUES)==0){
			drawCircel(pieCls.svgDocument,chart,pieCls.CENTERX,pieCls.CENTERY,pieCls.RX,pieCls.DCOLOR);
		}
	};
	function totalValues(VALUES) {
	   var total = 0;	  
	   for (var i in VALUES) {
	     total += VALUES[i];
	   }     
	   return total;
	}
	function normalizeValues(VALUES) {
	   var total = 0;
	   var ret = new Object();
	   for (var i in VALUES) {
		   
	     total += VALUES[i];
		 
	   }
	 
	   for (var i in VALUES) {
		   if(total!=0){
				ret[i] = VALUES[i]/total;
		   }else{
				ret[i] = 0;
		   }		
	   }	   
	   return ret;
	}
	function drawCircel(svgDocument,chart,centerx,centery,rx,color){
		   var circle = svgDocument.createElementNS(svgns, "circle");
        
       circle.setAttribute("shape-rendering", false);
        circle.setAttribute("stroke-width", "2px");
        circle.setAttribute("stroke-color", color);
        circle.setAttribute("r", rx);
        
        circle.setAttribute("cx", centerx);
        circle.setAttribute("cy", centery);
        circle.setAttribute("stroke", color);
        circle.setAttribute("fill", color);
        circle.style.cursor = "pointer";
        chart.appendChild(circle);

	}
	function addWedge(svgDocument, chart, phi, x, y, rx, ry, start, stop, color) {
	   var degPerRad = Math.PI/180.0;
	   var e1x = rx*Math.cos((phi+start)*degPerRad);
	   var e1y = rx*Math.sin((phi+start)*degPerRad);
	   var e2x;
	   var e2y;
	   var e3x;
	   var e3y;
	   if ( stop - start < 180 ) {
	     e2x = ry*Math.cos((phi+stop)*degPerRad);
	     e2y = ry*Math.sin((phi+stop)*degPerRad);
	     e3x = e2x;
	     e3y = e2y;
	   } else {
	     e2x = ry*Math.cos((phi+180)*degPerRad);
	     e2y = ry*Math.sin((phi+180)*degPerRad);
	     e3x = ry*Math.cos((phi+stop)*degPerRad);
	     e3y = ry*Math.sin((phi+stop)*degPerRad);
	   }
	 
	   var wedge = svgDocument.createElementNS(svgns,"path");
	   var dt = "";
	   wedge.setAttribute("class", "wedge");
	   wedge.setAttribute("stroke", color);
	   wedge.setAttribute("fill", color);
	   wedge.setAttribute("fill-opacity", 1);
	   dt = "M" + x+","+y+" "+ (x+e1x) + "," + (y+e1y) +
			      "A" + rx + "," + ry + " " + phi + " 0,1 " + (x+e2x) + "," + (y+e2y);
	   if((e2x != e3x) || (e2y!=e3y))   
	   dt += "A" + rx + "," + ry + " " + phi + " 0,1 " + (x+e3x) + "," + (y+e3y) ;
	   dt+="z";
	   
	   wedge.setAttribute("d", dt);	 
	   chart.appendChild( wedge );
	};
	
	function createLabel(svgDocument, x, y, color, name, value) {
			
		 var label = svgDocument.createElementNS(svgns,"g");
		 var text = svgDocument.createElementNS(svgns,"text");
		 var textvalue = svgDocument.createElementNS(svgns,"text");
		 var box = svgDocument.createElementNS(svgns,"rect");
	
		  var val=value*100;
		   box.setAttribute( "x", x );
		   box.setAttribute( "y", y-2);
		   box.setAttribute( "height", 10 );
		   box.setAttribute( "width", 10 );
		   box.setAttribute( "stroke", color );
		   box.setAttribute( "fill", color );
		   box.setAttribute( "fill-opacity", 1 );
		   text.setAttribute( "x", x+30 );
		   text.setAttribute( "y", y + 5 );
		   text.appendChild(svgDocument.createTextNode(name));
		   textvalue.setAttribute( "x", x +120 );
		   textvalue.setAttribute( "y", y + 5 );
		   textvalue.appendChild(svgDocument.createTextNode(val.toFixed(2)+"%"));
		 
		   label.setAttribute( "id", "pielabel");
		   label.appendChild( box );
		   label.appendChild( text );
		   label.appendChild( textvalue );	
		   return label;
		
	}


