/*
 * plot.h
 *
 *  Created on: Oct 22, 2010
 *      Author: Arnaud BLanchard
 */

#ifndef PLOT_H
#define PLOT_H

#include <gtk/gtk.h>

#define MAXIMUM_NUMBER_OF_CURVES 32

typedef struct dash
{
  const double *dashes;
  int number_of_dashes;
}type_dash;

typedef struct curve
{
  cairo_t *cr;
  const GdkColor *color;
  GtkTreeModel *model;
  GtkTreeIter iter;
  int column_of_abscissa, column_of_ordonate;
  const type_dash *dash;

}type_curve;

typedef struct plot
{
  GtkWidget *window, *graph;
  type_curve *curves[MAXIMUM_NUMBER_OF_CURVES];
  int number_of_curves;
  float minimum_abscissa, maximum_abscissa, minimum_ordonate, maximum_ordonate;
  guint timout;
}type_plot;

enum
{
  LINE_TYPE_PLAIN=0,
  LINE_TYPE_SYMETRIC_DASH,
  NUMBER_OF_LINE_TYPES
};

enum
{
  COLOR_BLACK=0,
  COLOR_RED,
  NUMBER_OF_COLORS
};

static const double dash_symetric[1] = {5.0};

static const GdkColor colors[NUMBER_OF_COLORS] = {
    {0, 0x0000, 0x0000, 0x0000}, /* Black */
    {0, 0xFFFF, 0x0000, 0x0000} /*Red*/
};

static const type_dash line_types[NUMBER_OF_LINE_TYPES] = {
    { NULL , 0}, /* LINE_TYPE_PLAIN */
    {dash_symetric, sizeof(dash_symetric)/sizeof(double)} /* LINE_TYPE_SYMETRIC_DASH */
};

type_plot *new_plot(float minimum_abscissa, float minimum_ordonate, float maximum_abscissa, float maximum_ordonate);
void plot_add_curve(type_plot *plot, type_curve *curve);
gboolean plot_refresh_graph(gpointer data);
void destroy_plot(type_plot *my_data);

type_curve *new_curve(GtkTreeModel *model, int column_of_abscissa, int column_of_ordonate, int color_index, int line_type_index);

#endif /* PLOT_H */
