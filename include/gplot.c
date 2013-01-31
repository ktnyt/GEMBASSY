/******************************************************************************
** @source GEMBASSY plot routines
**
** @version 1.0
** @modified December 27 2012 Hidetoshi Itaya Created this file
** @@
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License as published by the Free Software Foundation; either
** version 2 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
**
** You should have received a copy of the GNU Library General Public
** License along with this library; if not, write to the
** Free Software Foundation, Inc., 59 Temple Place - Suite 330,
** Boston, MA  02111-1307, USA.
******************************************************************************/


#include "gplot.h"




/* @funclist gPlotFilebuff ****************************************************
**
** Retrieves data from file buffer and plots the data using gPlotData
**
******************************************************************************/

AjBool gPlotFilebuff(AjPFilebuff buff, AjPGraph graphs, gPlotParams *gpp)
{
  AjPStr  line = NULL;
  AjPPStr temp = NULL;
  AjPPStr name = NULL;
  ajint   i    = 0;
  ajint   j    = 0;
  ajint   col  = 0;
  ajint   flag = 0;
  float **data = NULL;

  while(ajBuffreadLine(buff, &line))
    {

    /*
    ** Allocate first time only
    */

    if(!col)
      {
      col = ajStrCalcCountC(line, ",") + 1;

      if((temp = (AjPPStr)malloc(sizeof(AjPStr) * col)) == NULL)
	{
	return ajFalse;
      }

      if((name = (AjPPStr)malloc(sizeof(AjPStr) * col)) == NULL)
	{
	  AJFREE(temp);
	  return ajFalse;
	}

      if((data = (float**)malloc(sizeof(float*) * col)) == NULL)
	{
	  AJFREE(temp);
	  AJFREE(name);
	  return ajFalse;
	}
      for(i = 0; i < col; ++i)
	{
	  if((data[i] = (float*)malloc(sizeof(float))) == NULL){
	    {
	      AJFREE(temp);
	      AJFREE(name);
	      for(j = 0; j < i; ++j)
		{
		  AJFREE(data[j]);
		}
	      AJFREE(data);
	      return ajFalse;
	    }
	  }
	}
      }
    
    ajStrExchangeCC(&line, ",", "\n");
    ajStrParseSplit(line, &temp);

    for(i = 0; i < col; ++i)
      {
      if((data[i] = (float*)realloc(data[i], sizeof(float) * (j + 1))) == NULL)
	{
	  AJFREE(temp);
	  AJFREE(name);
	  for(j = 0; j < i; ++j)
	    {
	      AJFREE(data[j]);
	    }
	  AJFREE(data);
	  return ajFalse;
	}

      ajStrRemoveLastNewline(&(temp[i]));
      if(ajStrIsFloat(temp[i]))
	{
	ajStrToFloat(temp[i], &(data[i][j]));
	++flag;
      }
      else
	{
	name[i] = ajStrNewS(temp[i]);
      }
    }
    j = flag ? j + 1 : j;
    flag = 0;
  }

  (*gpp).data    = data;
  (*gpp).setNum  = 0;
  (*gpp).dataNum = j;
  (*gpp).typeNum = col;
  if(!(*gpp).names)
    (*gpp).names = name;

  if(j < 2)
    gPlotFlip(gpp);

  gPlotData(graphs, gpp);

  for(i = 0; i < (*gpp).typeNum; ++i)
    AJFREE((*gpp).data[i]);
  AJFREE((*gpp).data);

  data = NULL;

  ajFilebuffDel(&buff);
  ajStrDel(&line);

  return ajTrue;
}




/* @funclist gPlotData ********************************************************
**
** Function to plot from given data
**
******************************************************************************/

AjBool gPlotData(AjPGraph graphs, gPlotParams *gpp)
{
  AjPGraphdata gd = NULL;

  float min = 0.0;
  float max = 0.0;
  float dif = 0.0;
  ajint i;
  ajint j;

  ajint   setNum  = (*gpp).setNum;
  ajint   dataNum = (*gpp).dataNum;
  ajint   typeNum = (*gpp).typeNum;
  AjPStr  title   = (*gpp).title;
  AjPStr  xlab    = (*gpp).xlab;
  AjPStr  ylab    = (*gpp).ylab;
  AjPPStr names   = (*gpp).names;
  float **data    = (*gpp).data;

  float x[dataNum];
  float y[dataNum];
  float begin = data[0][0];
  float end   = data[0][dataNum-1];
  float range = end - begin;

  int c[] = {1,3,9,13};

  for(i = 1; i < typeNum; ++i)
    {
    for(j = 0; j < dataNum; ++j)
      {
      min = (min < data[i][j]) ? min : data[i][j];
      max = (max > data[i][j]) ? max : data[i][j];
    }
  }

  dif = (min == max) ? 20 : max - min;
  max += dif / 20;
  min -= dif / 20;

  for(i = 1; i < typeNum; ++i)
    {
    gd = ajGraphdataNewI(dataNum);

    ajGraphdataSetColour(gd, c[i-1]);
    ajGraphdataSetMinmax(gd, begin, end, min, max);
    ajGraphdataSetTruescale(gd, begin, end, min, max);
    ajGraphdataSetTypeC(gd, "Multi 2D Plot");
      
    for(j = 0; j <  dataNum; ++j)
      {
      x[j] = data[0][j];
      y[j] = data[i][j];
    }
      
    ajGraphdataAddXY(gd, x, y);
    ajGraphDataAdd(graphs, gd);
      
    if(typeNum > 2)
      {
      float len = 0.0;

      for(j = 1; j < typeNum; ++j)
	len = (len < (float)ajStrGetLen(names[j])) ?
	  (float)ajStrGetLen(names[j]) : len;

      ajGraphAddLine(graphs,
		     range * 7.4/8 + begin, dif * (8.6-i*0.3)/8 + min,
		     range * 7.8/8 + begin, dif * (8.6-i*0.3)/8 + min,
		     c[i-1]);
      ajGraphAddTextScaleS(graphs,
			   range * (7.3/8 - len*1/144) + begin,
			   dif * (8.6-i*0.3)/8 + min,
			   0, 0.4,
			   names[i]);
    }
      
    gd = NULL;
  }

  ajGraphxySetXstartF(graphs, begin);
  ajGraphxySetXendF(graphs, end);
  ajGraphxySetYstartF(graphs, min - ((max - min) / 10));
  ajGraphxySetYendF(graphs, max + ((max - min) / 10));

  //ajGraphSetTitleS(graphs, title);
  ajGraphSetXlabelS(graphs, xlab);
  ajGraphSetYlabelS(graphs, ylab);
  ajGraphxySetflagOverlay(graphs, ajTrue);

  ajGraphxyDisplay(graphs, AJFALSE);
  ajGraphicsClose();

  return ajTrue;
}




/* @funclist gPlotFlip ********************************************************
**
** Function to flip x and y data
**
******************************************************************************/

AjBool gPlotFlip(gPlotParams *gpp)
{
  ajint   setNum  = (*gpp).setNum;
  ajint   dataNum = (*gpp).typeNum;
  ajint   typeNum = (*gpp).dataNum;
  float **data    = (*gpp).data;

  float **newdata;
  ajint i;
  ajint j;

  if((newdata = (float**)malloc(sizeof(float*) * typeNum)) == NULL)
    return ajFalse;
  else
    for(i = 0; i < typeNum + 1; ++i)
      if((newdata[i] = (float*)malloc(sizeof(float))) == NULL)
	{
	  AJFREE(newdata);
	  return ajFalse;
	}

  for(i = 0; i < dataNum; ++i){
    if((newdata[0] = (float*)realloc(newdata[0],
				     sizeof(float) * (i + 1))) == NULL)
      {
	for(j = 0; j < i; ++j)
	    AJFREE(newdata[j]);
	AJFREE(newdata);
	return ajFalse;
      }
    if((newdata[1] = (float*)realloc(newdata[1],
				     sizeof(float) * (i + 1))) == NULL)
      {
	for(j = 0; j < i; ++j)
	    AJFREE(newdata[j]);
	AJFREE(newdata);
	return ajFalse;
      }
    newdata[0][i] = i;
    newdata[1][i] = data[i][0];
  }

  for(i = 0; i < dataNum; ++i)
    {
      AJFREE((*gpp).data[i]);
    }
  AJFREE((*gpp).data);

  (*gpp).dataNum = dataNum;
  (*gpp).typeNum = 2;
  (*gpp).data    = newdata;

  return ajTrue;
}
