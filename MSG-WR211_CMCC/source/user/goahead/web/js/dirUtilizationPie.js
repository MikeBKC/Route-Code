var defaultColors = new Array("#e55636", "#19aa5f", "#a8c656");
var defaultColor = "#5bca87";
//var pieDefaultValues = {"":0,"已使用":0,"未使用":0};


function pieInit(VALUES, COLORS ,DCOLOR){
	this.CENTERX = 80;
	this.CENTERY = 100;
	this.RX = 60;
	this.RY = 60;
	this.STARTANGLE = 0;
	this.COLORS = COLORS || defaultColors;
	this.DCOLOR = DCOLOR || defaultColor; 
	this.VALUES = VALUES ;
	this.drawPieFun;
	this.svgDocument;


}
var svgns = "http://www.w3.org/2000/svg";
var pDatas=new Array();

function convertData(value){
	var datalength ;
	if(String(value).indexOf(".")!=-1){
		var tempdata = String(value).split(".")[0];
		datalength = String(tempdata).length;   
	}else{
		datalength = String(value).length;
	}
	var data = 0;
	var u="";
	if(datalength<=3){
		data = value;
		u="B";
	}
	if(datalength==4||datalength==5||datalength==6){
		data = new Number(parseFloat(value/1024));
		u="KB";
	}
	if(datalength==7||datalength==8||datalength==9){
		data = new Number(parseFloat(value/(1024*1024)));
		u="MB";
	}
	if(datalength==10||datalength==11||datalength==12){
		data = new Number(parseFloat(value/(1024*1024*1024)));
		u="GB";
	}
									
	return data.toFixed(2)+u;
}

function drawPieChart(pieCls){
		
			var elem = pieCls.svgDocument.getElementById("canvas");
		try{
			elem.removeChild(pieCls.svgDocument.getElementById("chartpie"));
			elem.removeChild(pieCls.svgDocument.getElementById("legendlable"));
	
	
		}
		catch(e){}
	   var canvas = pieCls.svgDocument.getElementById("canvas");
	   var normValues = normalizeValues( pieCls.VALUES );
	   var values = getValues(pieCls.VALUES);
	   var lastAngle = pieCls.STARTANGLE;
	
	   var count = 0;
	   var chart = pieCls.svgDocument.createElementNS(svgns,"g");
		
	   chart.setAttribute( "id", "chartpie");
	 
	   canvas.appendChild( chart );
	
	   var legend = pieCls.svgDocument.createElementNS(svgns,"g");
	   legend.setAttribute( "id", "legendlable");
	  
	   canvas.appendChild(legend);
	   var minOriginalData = getMinVal(pieCls.VALUES);
	   var units = getUnits(minOriginalData);
	
	   for(var i in pieCls.VALUES){
	      
		  	
		   if(normValues[i]>0){
			
			   addWedge(pieCls.svgDocument, chart, 0, pieCls.CENTERX, pieCls.CENTERY, pieCls.RX, pieCls.RY,
			       lastAngle, lastAngle + 360*normValues[i], pieCls.COLORS[count]);
			
		   }
			var covertVal = convertData(values[i]);	
		   var label = createLabel(pieCls.svgDocument,pieCls.CENTERX+pieCls.RX+10,(20+20*count),pieCls.COLORS[count], i, normValues[i],covertVal);
		
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

function getValueMax(VALUES){

		 var pData = new Array(); 
		
		 for (var i in VALUES) {
			pData.push(VALUES[i]);
		 }
		 var i=0;
		 var max = pData[0];
		  for (i = 0;i<pData.length;i++) {
			
			   if (pData[i] > max){
				 max = pData[i];
				}
		 }	 
	
	  
	  return max;
	 
}

	function getValues(VALUES) {
	   var minOriginalData = getMinVal(VALUES);
	   var units = getUnits(minOriginalData);
		
	   var total = 0;
	   var ret = new Object();
	
	   for (var i in VALUES) {
		ret[i] = VALUES[i];					  
	   }
	    
	   return ret;
	}

	function normalizeValues(VALUES) {
		var minOriginalData = getMinVal(VALUES);
	   var units = getUnits(minOriginalData);
		
	   var total = 0;
	   var ret = new Object();
	
		var c=0;
		var n=0;
	   for (var i in VALUES) {
		   c=c+1;
		    if(c!=1){
					total += convertFsize(VALUES[i],units);

			}
			
		 
	   }
		
	   for (var i in VALUES) {
			n=n+1
			   if(total!=0){
				   if(n!=1){
					   ret[i] = convertFsize(VALUES[i],units)/total;
					}else{
						ret[i] = 1;
					}
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
	
	function createLabel(svgDocument, x, y, color, name, value,total) {
			
		 var label = svgDocument.createElementNS(svgns,"g");
		 var text = svgDocument.createElementNS(svgns,"text");
		 var textvalue = svgDocument.createElementNS(svgns,"text");
		 var box = svgDocument.createElementNS(svgns,"rect");
		 var textvalue1 = svgDocument.createElementNS(svgns,"text");
		  var val=value*100;

		   box.setAttribute( "x", x+15 );
		   box.setAttribute( "y", y+50);
		   box.setAttribute( "height", 10 );
		   box.setAttribute( "width", 10 );
		   box.setAttribute( "stroke", color );
		   box.setAttribute( "fill", color );
		   box.setAttribute( "fill-opacity", 1 );
		   text.setAttribute( "x", x+30 );
		   text.setAttribute( "y", y + 58 );
		   text.appendChild(svgDocument.createTextNode(name));
		   textvalue.setAttribute( "x", x +80 );
		   textvalue.setAttribute( "y", y + 58 );
		   textvalue.appendChild(svgDocument.createTextNode(val.toFixed(2)+"%"));

			textvalue1.setAttribute( "x", x +130 );
		   textvalue1.setAttribute( "y", y + 58 );
		   textvalue1.appendChild(svgDocument.createTextNode(total));

		   label.setAttribute( "id", "pielabel");
		   label.appendChild( box );
		   label.appendChild( text );
		   label.appendChild( textvalue );	
			label.appendChild( textvalue1 );

		   return label;
		
	}
	

