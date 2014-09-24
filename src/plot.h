/*
Copyright  ETIS — ENSEA, Université de Cergy-Pontoise, CNRS (1991-2014)
promethe@ensea.fr

Authors: P. Andry, J.C. Baccon, D. Bailly, A. Blanchard, S. Boucena, A. Chatty, N. Cuperlier, P. Delarboulas, P. Gaussier, 
C. Giovannangeli, C. Grand, L. Hafemeister, C. Hasson, S.K. Hasnain, S. Hanoune, J. Hirel, A. Jauffret, C. Joulain, A. Karaouzène,  
M. Lagarde, S. Leprêtre, M. Maillard, B. Miramond, S. Moga, G. Mostafaoui, A. Pitti, K. Prepin, M. Quoy, A. de Rengervé, A. Revel ...

See more details and updates in the file AUTHORS 

This software is a computer program whose purpose is to simulate neural networks and control robots or simulations.
This software is governed by the CeCILL v2.1 license under French law and abiding by the rules of distribution of free software. 
You can use, modify and/ or redistribute the software under the terms of the CeCILL v2.1 license as circulated by CEA, CNRS and INRIA at the following URL "http://www.cecill.info".
As a counterpart to the access to the source code and  rights to copy, modify and redistribute granted by the license, 
users are provided only with a limited warranty and the software's author, the holder of the economic rights,  and the successive licensors have only limited liability. 
In this respect, the user's attention is drawn to the risks associated with loading, using, modifying and/or developing or reproducing the software by the user in light of its specific status of free software, 
that may mean  that it is complicated to manipulate, and that also therefore means that it is reserved for developers and experienced professionals having in-depth computer knowledge. 
Users are therefore encouraged to load and test the software's suitability as regards their requirements in conditions enabling the security of their systems and/or data to be ensured 
and, more generally, to use and operate it in the same conditions as regards security. 
The fact that you are presently reading this means that you have had knowledge of the CeCILL v2.1 license and that you accept its terms.
*/
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
