/****************************************************************************
 *
 * class CaeUnsGMesh
 *
 * Proprietary software product of Pointwise, Inc.
 * Copyright (c) 1995-2011 Pointwise, Inc.
 * All rights reserved.
 *
 ***************************************************************************/

#ifndef _CAEUNSGMESH_H_
#define _CAEUNSGMESH_H_

#include <string>
#include <vector>

#include "apiGridModel.h"
#include "apiPWP.h"

#include "CaePlugin.h"
#include "CaeUnsGridModel.h"


//***************************************************************************
//***************************************************************************
//***************************************************************************

class CaeUnsGMesh :
    public CaeUnsPlugin,
    public CaeFaceStreamHandler {

public:
    CaeUnsGMesh(CAEP_RTITEM *pRti, PWGM_HGRIDMODEL model,
        const CAEP_WRITEINFO *pWriteInfo);
    ~CaeUnsGMesh();
    static bool create(CAEP_RTITEM &rti);
    static void destroy(CAEP_RTITEM &rti);

private:

    typedef std::vector<std::string> PhysNameCache;

    virtual bool        beginExport();
    virtual PWP_BOOL    write();
    virtual bool        endExport();

    void    writeHeader();
    void    writeVertices();
    void    writeGrid();
    void    writePhysicalNames();

    bool    writeVertex(const CaeUnsVertex &v);
    void    writeElement(PWGM_HELEMENT hElem, const PWGM_ELEMDATA &edata);
    int     elemToGmshPhysId(const PWGM_HELEMENT &hElem);
    void    cachePhysicalName();
    void    writeComment(const char *txt);

    // face streaming handlers
    virtual PWP_UINT32 streamBegin(const PWGM_BEGINSTREAM_DATA &data);
    virtual PWP_UINT32 streamFace(const PWGM_FACESTREAM_DATA &data);
    virtual PWP_UINT32 streamEnd(const PWGM_ENDSTREAM_DATA &data);

private:

    //! Id of the block/domain whose elements are currently being written.
    PWP_UINT32      curElemParentId_;

    //! Set true/false while wrtiting block/boundary elements.
    bool            writingBlocks_;

    //! The VC/BC of the elements currently being written.
    const char *    curCondName_;

    //! The current Gmsh physical id.
    PWP_UINT32      physNameId_;

    //! Cached Gmsh physical names.
    PhysNameCache   physNameCache_;

    //! The current Gmsh element id.
    PWP_UINT32      elemId_;

    //! The max number of digits in an element id.
    int             elemIdWd_;

    //! The max number of digits in a vertex index.
    int             indexWd_;

    //! The floating point decimal precision.
    int             prec_;
};

#endif // _CAEUNSGMESH_H_
