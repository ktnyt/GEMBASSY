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

/* @funclist gPlotFile ********************************************************
**
** Retrieves data from CSV file and plots the data using gPlotData
**
******************************************************************************/

int gPlotFile(AjPStr filename, AjPGraph graphs, gPlotParams *gpp)
{
  AjPFile file = ajFileNewInNameS(filename);
  AjPStr  line = NULL;
  AjPPStr temp = NULL;
  AjPPStr name = NULL;
  ajint   i=0, j=0, col=0, flag=0;
  float **data = NULL;

  while(ajReadline(file, &line))
    {

    /*
    ** Allocate first time only
    */

    if(!col)
      {
      col = ajStrCalcCountC(line, ",") + 1;

      if((temp = (AjPPStr)malloc(sizeof(AjPStr) * col)) == NULL)
	{
	return 1;
      }

      if((name = (AjPPStr)malloc(sizeof(AjPStr) * col)) == NULL)
	{
	free(temp);
	return 1;
      }

      if((data = (float**)malloc(sizeof(float*) * col)) == NULL)
	{
	free(temp);
	free(name);
	return 1;
      }
      for(i = 0; i < col; ++i)
	{
	  if((data[i] = (float*)malloc(sizeof(float))) == NULL){
	    {
	      free(temp);
	      free(name);
	      for(j = 0; j < i; ++j)
		{
		  free(data[j]);
		}
	      free(data);
	      return 1;
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
	free(temp);
	free(name);
	for(j = 0; j < i; ++j)
	  {
	  free(data[j]);
	}
	free(data);
	return 1;
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

  ajSysFileUnlinkS(filename);

  if(j < 2)
    gPlotFlip(gpp);

  gPlotData(graphs, gpp);

  for(i = 0; i < (*gpp).typeNum; ++i)
    free((*gpp).data[i]);
  free((*gpp).data);
  data = NULL;

  ajFileClose(&file);
  ajStrDel(&line);

  return 0;
}




/* @funclist gPlotData ********************************************************
**
** Function to plot from given data
**
******************************************************************************/

int gPlotData(AjPGraph graphs, gPlotParams *gpp)
{
  AjPGraphdata gd = NULL;

  float min = 0.0;
  float max = 0.0;
  float dif = 0.0;
  ajint i, j;

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

  return 0;
}




/* @funclist gPlotFlip ********************************************************
**
** Function to flip x and y data
**
******************************************************************************/

int gPlotFlip(gPlotParams *gpp)
{
  ajint   setNum  = (*gpp).setNum;
  ajint   dataNum = (*gpp).typeNum;
  ajint   typeNum = (*gpp).dataNum;
  float **data    = (*gpp).data;

  float **tmp;
  ajint i;

  if((tmp = (float**)malloc(sizeof(float*) * typeNum)) == NULL)
    return 1;
  else
    for(i = 0; i < typeNum; ++i)
      if((tmp[i] = (float*)malloc(sizeof(float))) == NULL)
	return 1;

  for(i = 0; i < dataNum; ++i){
    if((tmp[0] = (float*)realloc(tmp[0], sizeof(float) * (i + 1))) == NULL)
      return 1;
    if((tmp[1] = (float*)realloc(tmp[1], sizeof(float) * (i + 1))) == NULL)
      return 1;
    tmp[0][i] = i;
    tmp[1][i] = data[i][0];
  }

  for(i = 0; i < dataNum; ++i)
    {
      free((*gpp).data[i]);
  }
  free((*gpp).data);

  (*gpp).dataNum = dataNum;
  (*gpp).typeNum = 2;
  (*gpp).data    = tmp;

  return 0;
}
