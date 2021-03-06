/*
 Copyright (C) 2006-2007 M.A.L. Marques

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.
  
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.
  
 You should have received a copy of the GNU Lesser General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "util.h"

#define XC_GGA_X_RPBE  117 /* Hammer, Hansen & Norskov (PBE-like) */


typedef struct{
  FLOAT kappa, mu;
} gga_x_rpbe_params;


static void 
gga_x_rpbe_init(XC(func_type) *p)
{
  assert(p!=NULL && p->params == NULL);
  p->params = malloc(sizeof(gga_x_rpbe_params));

  /* same parameters as standard PBE */
  XC(gga_x_rpbe_set_params)(p, 0.8040, 0.2195149727645171);
}


void 
XC(gga_x_rpbe_set_params)(XC(func_type) *p, FLOAT kappa, FLOAT mu)
{
  gga_x_rpbe_params *params;

  assert(p != NULL && p->params != NULL);
  params = (gga_x_rpbe_params *) (p->params);

  params->kappa = kappa;
  params->mu    = mu;
}


/* RPBE: see PBE for more details */
void XC(gga_x_rpbe_enhance) 
  (const XC(func_type) *p, int order, FLOAT x, 
   FLOAT *f, FLOAT *dfdx, FLOAT *d2fdx2, FLOAT *d3fdx3)
{
  FLOAT kappa, mu, f0, df0, d2f0, d3f0;

  assert(p->params != NULL);
  kappa = ((gga_x_rpbe_params *) (p->params))->kappa;
  mu    = ((gga_x_rpbe_params *) (p->params))->mu*X2S*X2S;

  f0 = EXP(-mu*x*x/kappa);
  *f = 1.0 + kappa*(1.0 - f0);

  if(order < 1) return;

  df0 = -2.0*x*mu/kappa*f0;
  
  *dfdx  = -kappa*df0;

  if(order < 2) return;

  d2f0    = -2.0*mu*f0*(kappa - 2.0*x*x*mu)/(kappa*kappa);
  *d2fdx2 = -kappa*d2f0;

  if(order < 3) return;

  d3f0    = 4.0*mu*mu*f0*x*(3.0*kappa - 2.0*mu*x*x)/(kappa*kappa*kappa);
  *d3fdx3 = -kappa*d3f0;
}


#define func XC(gga_x_rpbe_enhance)
#include "work_gga_x.c"


const XC(func_info_type) XC(func_info_gga_x_rpbe) = {
  XC_GGA_X_RPBE,
  XC_EXCHANGE,
  "Hammer, Hansen, and Norskov",
  XC_FAMILY_GGA,
  {&xc_ref_Hammer1999_7413, NULL, NULL, NULL, NULL},
  XC_FLAGS_3D | XC_FLAGS_HAVE_EXC | XC_FLAGS_HAVE_VXC | XC_FLAGS_HAVE_FXC | XC_FLAGS_HAVE_KXC,
  1e-32, 1e-32, 0.0, 1e-32,
  gga_x_rpbe_init, 
  NULL, NULL,
  work_gga_x,
  NULL
};
