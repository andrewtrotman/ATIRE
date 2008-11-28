/**************************************************************************
 *
 * stem.h -- Lovins' stemmer header
 * Copyright (C) 1994  Linh Huynh, 
 *                     (glued into library by tes@kbs.citri.edu.au)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * $Id: stem.h,v 1.2 1994/09/20 04:20:10 tes Exp $
 *
 **************************************************************************/

/*
   $Log: stem.h,v $
   * Revision 1.2  1994/09/20  04:20:10  tes
   * Changes made for 1.1
   *
   *
 */
#ifndef STEM_H
#define STEM_H

void stem (unsigned char *word);

#endif
