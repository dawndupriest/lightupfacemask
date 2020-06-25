ColorPicker cp;
Matrix8by8 mx;

// original color picker code here:
// https://forum.processing.org/one/topic/processing-color-picker.html

// modifications by me (storing info in color matrix, outputting neopixel code)

void setup() 
{
  size( 1000, 500 );
  frameRate( 100 );
  mx = new Matrix8by8();
  cp = new ColorPicker(450, 10, 400, 400, 255, mx );
  
}

void draw ()
{
  background( 80 );
  
  
  cp.render();
  mx.render();
 
}

void mouseClicked() {
  if(mouseX < 450 && mouseY < 500)
  {
    mx.selectCell(mouseX,mouseY);
  }
  if (cp.checkChosen(mouseX,mouseY)) {
    //mx.setColor(cp.mycolor);
    mx.clear();
  }
  if(cp.checkClear(mouseX,mouseY))
  {
    cp.name = "";
  }
  if(cp.checkExport(mouseX,mouseY))
  {
    println("Exported!");
  }
  
}

void keyPressed()
{
  cp.name = cp.name + key;
}

public class ColorPicker 
{
  int x, y, w, h, c;
  PImage cpImage;
  color mycolor;
  Matrix8by8 m;
  public String name;
  color palette[];
  int palettex[];
  int palselected;
  
  public ColorPicker ( int x, int y, int w, int h, int c, Matrix8by8 m )
  {
    this.x = x;
    this.y = y;
    this.w = w;
    this.h = h;
    this.c = c;
    this.m = m;
    name = "";
    
    cpImage = new PImage( w, h );
    palette = new color[8];
    palettex = new int[8];
    palselected = 0;
    for(int q = 0; q < 8; q++)
    {
      palette[q] = color(0,0,0);
      palettex[q] = x + 30*q;
    }
    init();
    
  }
  
  private void init ()
  {
    // draw color.
    int cw = w - 60;
    for( int i=0; i<cw; i++ ) 
    {
      float nColorPercent = i / (float)cw;
      float rad = (-360 * nColorPercent) * (PI / 180);
      int nR = (int)(cos(rad) * 127 + 128) << 16;
      int nG = (int)(cos(rad + 2 * PI / 3) * 127 + 128) << 8;
      int nB = (int)(Math.cos(rad + 4 * PI / 3) * 127 + 128);
      int nColor = nR | nG | nB;
      
      setGradient( i, 0, 1, h/2, 0xFFFFFF, nColor );
      setGradient( i, (h/2), 1, h/2, nColor, 0x000000 );
    }
    
    // draw black/white.
    drawRect( cw, 0,   30, h/2, 0xFFFFFF );
    drawRect( cw, h/2, 30, h/2, 0 );
    
    // draw grey scale.
    for( int j=0; j<h; j++ )
    {
      int g = 255 - (int)(j/(float)(h-1) * 255 );
      drawRect( w-30, j, 30, 1, color( g, g, g ) );
    }
  }

  private void setGradient(int x, int y, float w, float h, int c1, int c2 )
  {
    float deltaR = red(c2) - red(c1);
    float deltaG = green(c2) - green(c1);
    float deltaB = blue(c2) - blue(c1);

    for (int j = y; j<(y+h); j++)
    {
      int c = color( red(c1)+(j-y)*(deltaR/h), green(c1)+(j-y)*(deltaG/h), blue(c1)+(j-y)*(deltaB/h) );
      cpImage.set( x, j, c );
    }
  }
  
  private void drawRect( int rx, int ry, int rw, int rh, int rc )
  {
    for(int i=rx; i<rx+rw; i++) 
    {
      for(int j=ry; j<ry+rh; j++) 
      {
        cpImage.set( i, j, rc );
      }
    }
  }
  
  public void render ()
  {
    image( cpImage, x, y );
    if( mousePressed &&
  mouseX >= x && 
  mouseX < x + w &&
  mouseY >= y &&
  mouseY < y + h )
    {
      c = get( mouseX, mouseY );
      m.setColor(c);
      palette[palselected] = c;
      
    }
    for(int z = 0; z<8; z++)
    {
      if(mousePressed && (mouseX >= palettex[z]) &&
         (mouseX <= palettex[z]+20) &&
         (mouseY >= y+h+10) && 
         (mouseY <= y+h+30))
        {
          //c=get(mouseX,mouseY);
          
          palselected = z;
          c=palette[palselected];
          m.setColor(c);
          
        }
    }
    
    
    // draw the color palette
    for(int q = 0; q < 8; q++)
    {
    fill( palette[q] );
    rect( palettex[q], y+h+10, 20, 20 );
    }
    fill(255,255,255);
    rect( x + 250, y + h + 10, 50, 20);
    fill(0,0,0);
    text("Clear",x+255, y+h+25);
    fill(255,255,255);
    rect(x, y + h + 50, 70, 30);
    fill(0,0,0);
    text(name, x+5,y+h+65);
    fill(255,255,255);
    rect(x+80, y+h+50, 80, 30);
    fill(0,0,0);
    text("Clear Name",x+85, y+h+65);
    
    fill(255,255,255);
    rect(x+180, y+h+50, 80,30);
    fill(0,0,0);
    text("Export",x+185,y+h+65);
    
    
  }
  
  // check to see if the 'Clear' button was clicked, if so then clear the matrix
  public boolean checkChosen(int xcoord, int ycoord)
  {
    if(xcoord >= x+255 && xcoord <= x+305 && ycoord >= y+h+10 && ycoord <= y+h+30)
    {
      //mycolor = c;
      return true;
    }
    else
    {
      return false;
    }
  }
  
  public boolean checkClear(int xcoord, int ycoord)
  {
   
    
    if( xcoord >= x+80 && xcoord <= x+80+80 && ycoord >= y+h+50 && ycoord <= y+h+80)
    {
        
      return true;
    }
    else
    {
      return false;
    }
  }
  
  public boolean checkExport(int xcoord, int ycoord)
  {
    color cellcolor;
    
    int codeline = 0;
    if( xcoord >= x+180 && xcoord <= x+180+80 && ycoord >= y+h+50 && ycoord <= y+h+80)
    {
      PrintWriter pw;
      pw = createWriter(this.name + ".txt");
 
      pw.println("void " + name + "(){");
      pw.println("matrix.fillScreen(0);");
     
      for(int i = 0; i<8; i++)
        for(int j = 0; j<8;j++)
        {
         cellcolor=  m.cmatrix[j][i];
         if(int(red(cellcolor)) != 0 || int(green(cellcolor)) != 0 ||int(blue(cellcolor)) != 0)
         {
          pw.println("matrix.drawPixel("+j+","+i+", matrix.Color("+int(red(cellcolor))+","+int(green(cellcolor))+","+int(blue(cellcolor))+"));");
         }
        
        }
      
      pw.println("matrix.show();");
      
      pw.println("}");
      pw.flush();
      pw.close();
      return true;
    }
    else
    {
      return false;
    }
  }
  
  
  
}
// end class colorpicker

public class xycoord
{
  public int x;
  public int y;
  
  public xycoord()
  {
    x = 0; y = 0;
  }
}



public class Matrix8by8
{
  public color cmatrix[][];
  xycoord posmatrix[][];
  public xycoord cellselected;
  int i;
  int j;
  int w;
  int spacing;
  int startpos;
  
  public Matrix8by8()
  {
    init();
  }
  
  private void init()
  {
    i = 0; 
    j = 0;
    w = 25;
    spacing = 10;
    startpos = 10;
    
    cmatrix = new color[8][8];
    posmatrix = new xycoord[8][8];
    cellselected = new xycoord();
    
    for(i = 0; i<8;i++)
    {
      for(j=0; j<8; j++)
      {
        cmatrix[j][i] = color(0,0,0);
        posmatrix[j][i] = new xycoord();
      }
    }
  }
  
  public void clear()
  {
    int i = 0;
    int j = 0;
    for(i = 0; i<8;i++)
    {
      for(j=0; j<8; j++)
      {
        cmatrix[j][i] = color(0,0,0);
      }
    }
  }
  
  public void selectCell(int xpos, int ypos)
  {
    int xindex = ceil((xpos - startpos) / (w + spacing));
    int yindex = ceil((ypos - startpos) / (w + spacing));
    if(xindex < 8 && yindex < 8)
    {
      cellselected.x = xindex; 
      cellselected.y = yindex;
      noFill();
      stroke(0, 255, 255);
      rect(startpos + (xindex*(w + spacing) - 4), startpos + (yindex*(w + spacing) - 4),w + 8,w+8 ); 
    }
  }
  
  public void setColor(color col)
  {
    
    cmatrix[cellselected.x][cellselected.y] = col;
    
    
  }
  
  
  public void render()
  {
    int curx = startpos; 
    int cury = startpos;
    int i = 0; 
    int j = 0;
    
    for(i = 0; i<8; i++)
    {
      cury = startpos + (i*(w + spacing));
      for(j=0; j<8; j++)
      {
         curx = startpos + (j*(w + spacing));
         if(j == cellselected.x && i == cellselected.y)
         {
           stroke(0,255,255);
         }
         else
         {
           noStroke();
         }
         fill(cmatrix[j][i]);
         rect(curx, cury, w, w);
         posmatrix[j][i].x = curx;
         posmatrix[j][i].y = cury;
      }
      
    }
    
  }
  
}
