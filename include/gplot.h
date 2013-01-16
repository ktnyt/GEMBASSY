#ifndef GPLOT_H
#define GPLOT_H

#include "emboss.h"

/* @datastatic gPlotParams*****************************************************
**
** gPlot plotting parameters
**
** @attr data [float**] Data used to plot
** @attr setNum [size_t] Number of data sets
** @attr typeNum [size_t] Number of data types
** @attr dataNum [size_t] Number of data in set
** @attr height [ajint] Height of graph - To be supported
** @attr width [ajint] Width of graph - To be supported
** @attr title [AjPStr] Graph title
** @attr xlab [AjPStr] Label for X axis
** @attr ylab [AjPStr] Label for Y axis
** @attr names [AjPPStr] name of each data type
** @@
******************************************************************************/

typedef struct gPlotStruct
{
  float **data;
  size_t  setNum;
  size_t  typeNum;
  size_t  dataNum;
  /*ajint   width  Not supported yet! ;*/
  /*ajint   height Not supported yet! ;*/
  AjPStr  title;
  AjPStr  xlab;
  AjPStr  ylab;
  AjPPStr names;
} gPlotParams;

/*
** Prototype Definitions
*/

int gPlotFile(AjPStr filename, AjPGraph graphs, gPlotParams *gpp);
int gPlotData(AjPGraph graphs, gPlotParams *gpp);
int gPlotFlip(gPlotParams *gpp);

#endif
