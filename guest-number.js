const canvas = document.querySelector("#canvas");
const ctx = canvas.getContext("2d");
const minicanvas = document.querySelector("#minicanvas");
const ctx1 = minicanvas.getContext("2d");
const clear = document.querySelector("#Clear");
const guest = document.querySelector("#Guest");
const yournumber= document.querySelector("#yournumber");

paint=false;

//paint
function mousedown (e)
{   
    ctx.beginPath();
    ctx.moveTo(e.offsetX,e.offsetY);
    paint = true;
}

function mouseup (e)
{   
    paint = false;
}

//draw
function draws(e)
{   
    if(!paint)
        return;
        
    ctx.lineTo(e.offsetX,e.offsetY);
    ctx.stroke();
}

//clear
function clearcanvas()
{   
    ctx.fillStyle = "black";
    ctx.fillRect(0,0,canvas.width,canvas.height);
}

//guest
var myweightdata=[];

function grayscale(image){
    var newimage=image;

    for(var x=0;x<newimage.height;++x)
    for(var y=0;y<newimage.width;++y)
    {
        var i=(x*4)*newimage.width+ y*4;
        var avg=image.data[i]+image.data[i+1]+image.data[i+2];
        avg/=3;

        newimage.data[i]=newimage.data[i+1]=newimage.data[i+2]=avg;
    }

    return newimage;
}

function Readf()
{   
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "trainingweight.dat", false);

    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            var weightdata=xhttp.responseText;
            myweightdata=weightdata.split(' ');
    }
    };
    
    xhttp.send();
};

function guestnumber()
{
    ctx1.drawImage(canvas,0,0,28,28);
    var imgData=ctx1.getImageData(0,0,minicanvas.width,minicanvas.height);
    imgData = grayscale(imgData);
    
    Readf();

    var topo=[784,15,12,10];
    var vlue=new Array(4);
    var ok=0;
    
    for(let i=0;i<topo.length;++i)
    {   
        vlue[i]=new Array(topo[i]+1);
        vlue[i][topo[i]]=1;

        for(let j=0;j<topo[i];++j)
        vlue[i][j]=0;
    }
    
    var l=28;
    var r=28;

    for(var x=0;x<imgData.height;++x)
    for(var y=0;y<imgData.width;++y)
    {
        var i=(x*imgData.width+ y)*4;
        
        if(imgData.data[i])
        l=Math.min(l,x),r=Math.min(r,y);
    }

    for(var x=0;x<imgData.height;++x)
    for(var y=0;y<imgData.width;++y)
    {   
        var x1=x+l;
        var y1=y+r;

        var i=(x1*imgData.width+ y1)*4;
        var j=x*imgData.width+y;
        
        if(x1<28&&y1<28)
        vlue[0][j]=10*imgData.data[i]/255;
    }
    
    for(let i=0;i<topo.length-1;++i)
    {   
        for(let j=0;j<=topo[i];++j)
            vlue[i][j]=Math.max(0,vlue[i][j])/10;

        for(let j=0;j<=topo[i];++j)
        for(let z=0;z<topo[i+1];++z)
            vlue[i+1][z]+=vlue[i][j]*myweightdata[ok++];
    }
    
    let numberss=0;

    for(let j=1;j<10;++j)
    if(vlue[3][j]>vlue[3][numberss])
    numberss=j;

    yournumber.innerHTML="Your number is : " + numberss;
}

//addevent
canvas.addEventListener("pointerdown", mousedown);
canvas.addEventListener("pointerup", mouseup);
canvas.addEventListener("pointermove",draws);
clear.addEventListener("click",clearcanvas);
guest.addEventListener("click",guestnumber);
window.addEventListener("load",() =>{
    canvas.width=canvas.height=300;
    ctx.lineWidth=25;
    ctx.strokeStyle="white";
    ctx.fillStyle = "black";
    ctx.fillRect(0,0,canvas.width,canvas.height);

    minicanvas.width=minicanvas.height=28;
    ctx1.fillStyle = "black";
    ctx1.fillRect(0,0,minicanvas.width,minicanvas.height);
})
