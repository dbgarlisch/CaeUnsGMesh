/****************************************************************************
 *
 * Pointwise Plugin utility functions
 *
 * Proprietary software product of Pointwise, Inc.
 * Copyright (c) 1995-2012 Pointwise, Inc.
 * All rights reserved.
 *
 ***************************************************************************/

#ifndef _RTCAEPSUPPORTDATA_H_
#define _RTCAEPSUPPORTDATA_H_

/*------------------------------------*/
/* CaeUnsGMesh format item setup data */
/*------------------------------------*/
CAEP_BCINFO CaeUnsGMeshBCInfo[] = {
    { "inflow", 100 },
    { "outflow", 101 },
    { "wall", 102 },
};
/*------------------------------------*/
CAEP_VCINFO CaeUnsGMeshVCInfo[] = {
    { "fluid", 200 },
    { "solid", 201 },
};
/*------------------------------------*/
const char *CaeUnsGMeshFileExt[] = {
    "msh"
};

#endif /* _RTCAEPSUPPORTDATA_H_ */
