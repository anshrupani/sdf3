/*
 *  Eindhoven University of Technology
 *  Eindhoven, The Netherlands
 *
 *  Name            :   sadf2sdf.h
 *
 *  Author          :   Bart Theelen (B.D.Theelen@tue.nl)
 *
 *  Date            :   2 December 2009
 *
 *  Function        :   Abstract SADF Graph in FSM-based SADF Graph
 *
 *  History         :
 *      13-09-06    :   Initial version.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * In other words, you are welcome to use, share and improve this program.
 * You are forbidden to forbid anyone else to use, share and improve
 * what you give them.   Happy coding!
 */

#ifndef SADF_SADF2FSMSADF_H_INCLUDED
#define SADF_SADF2FSMSADF_H_INCLUDED

#include "../../base/sadf/sadf_graph.h"
#include "../scenario/sadf_collection.h"
#include "../../../fsmsadf/fsmsadf.h"

FSMSADF::Graph *SADF2FSMSADF(SADF_Graph *SADF);

#endif
