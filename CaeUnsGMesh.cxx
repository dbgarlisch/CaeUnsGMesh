/****************************************************************************
 *
 * class CaeUnsGMesh
 *
 * Proprietary software product of Pointwise, Inc.
 * Copyright (c) 1995-2011 Pointwise, Inc.
 * All rights reserved.
 *
 ***************************************************************************/

#include <cmath>
#include <sstream>
#include <vector>

#include "apiCAEP.h"
#include "apiCAEPUtils.h"
#include "apiGridModel.h"
#include "apiPWP.h"
#include "runtimeWrite.h"
#include "pwpPlatform.h"

#include "CaePlugin.h"
#include "CaeUnsGridModel.h"
#include "CaeUnsGMesh.h"


//***************************************************************************
//***************************************************************************
//***************************************************************************

CaeUnsGMesh::CaeUnsGMesh(CAEP_RTITEM *pRti, PWGM_HGRIDMODEL
        model, const CAEP_WRITEINFO *pWriteInfo) :
    CaeUnsPlugin(pRti, model, pWriteInfo),
    curElemParentId_(PWP_UINT32_UNDEF),
    writingBlocks_(true),
    curCondName_(0),
    physNameId_(0),
    physNameCache_(),
    elemId_(1),
    elemIdWd_(1),
    indexWd_(1),
    prec_(7)
{
}


CaeUnsGMesh::~CaeUnsGMesh()
{
}


static int
elemTypeToGmshElemType(PWGM_ENUM_ELEMTYPE etype)
{
    //GMSH linear element labels
    //For complete definitions refer to:
    //http://geuz.org/gmsh/doc/texinfo/gmsh.html#MSH-ASCII-file-format
    int ret = 0;
    switch(etype) {
    case PWGM_ELEMTYPE_BAR:     ret = 1; break;
    case PWGM_ELEMTYPE_HEX:     ret = 5; break;
    case PWGM_ELEMTYPE_QUAD:    ret = 3; break;
    case PWGM_ELEMTYPE_TRI:     ret = 2; break;
    case PWGM_ELEMTYPE_TET:     ret = 4; break;
    case PWGM_ELEMTYPE_WEDGE:   ret = 6; break;
    case PWGM_ELEMTYPE_PYRAMID: ret = 7; break;
    }
    return ret;
}


bool
CaeUnsGMesh::beginExport()
{
    //PWP_BOOL doDump;
    //model_.getAttribute("debugDump", doDump);
    //model_.getAttribute("quality", quality_);
    setProgressMajorSteps(3);
    model_.appendEnumElementOrder(PWGM_ELEMORDER_VC);
    return true;
}


PWP_BOOL
CaeUnsGMesh::write()
{
    writeHeader();
    writeVertices();
    writeGrid();
    writePhysicalNames();
    return !aborted();
}


bool
CaeUnsGMesh::endExport()
{
    return true;
}


void
CaeUnsGMesh::writeHeader()
{
    writeComment("File exported by Pointwise (www.pointwise.com)");
    fprintf(fp(),
        "$MeshFormat\n"
        "2.2 0 %d\n"
        "$EndMeshFormat\n", int(sizeof(double)));
}


void
CaeUnsGMesh::writeVertices()
{
    if (!aborted()) {
        PWP_UINT32 cnt = model_.vertexCount();
        // step 1 of 3
        if (progressBeginStep(cnt)) {
            indexWd_ = int(log10(float(cnt))) + 1;
            prec_ = isSinglePrecision() ? 8 : 16;
            writeComment("NumNodes\nId x y z");
            rtFile_.write("$Nodes\n");
            rtFile_.write(cnt, "\n");
            CaeUnsVertex v(model_);
            while (writeVertex(v++)) {
                progressIncrement();
            }
            rtFile_.write("$EndNodes\n");
        }
        progressEndStep();
    }
}


void
CaeUnsGMesh::writeGrid()
{
    if (!aborted()) {
        // now stream boundary elements
        model_.streamFaces(PWGM_FACEORDER_BCGROUPSONLY, *this);
    }
}


void
CaeUnsGMesh::writePhysicalNames()
{
    if (!aborted()) {
        writeComment("NumNames\nDim Id Name");
        rtFile_.write("$PhysicalNames\n");
        rtFile_.write(physNameCache_.size(), "\n");
        PhysNameCache::const_iterator it;
        for (it = physNameCache_.begin(); it != physNameCache_.end(); ++it) {
            rtFile_.write(it->c_str());
            rtFile_.write("\n");
        }
        rtFile_.write("$EndPhyscialNames\n");
    }
}


bool
CaeUnsGMesh::writeVertex(const CaeUnsVertex &v)
{
    bool ret = v.isValid() && !aborted();
    if (ret) {
        PWGM_VERTDATA vdata;
        v.dataMod(vdata);
        fprintf(fp(), "%*lu %.*g %.*g %.*g\n", indexWd_,
            (unsigned long)vdata.i + 1, prec_, vdata.x, prec_, vdata.y, prec_,
            vdata.z);
    }
    return ret;
}


void
CaeUnsGMesh::writeElement(PWGM_HELEMENT hElem, const PWGM_ELEMDATA &edata)
{
    int gmshType = elemTypeToGmshElemType(edata.type);
    int gmshPhysGrpId = elemToGmshPhysId(hElem);
    int gmshElemGrpId = gmshPhysGrpId;
    fprintf(fp(), "%*lu %2d 2 %d %d", elemIdWd_, (unsigned long)elemId_++,
        gmshType, gmshPhysGrpId, gmshElemGrpId);
    for (PWP_UINT32 i = 0; i < edata.vertCnt; ++i) {
        fprintf(fp(), " %*lu", indexWd_, (unsigned long)(edata.index[i] + 1));
    }
    rtFile_.write("\n");
}


int
CaeUnsGMesh::elemToGmshPhysId(const PWGM_HELEMENT &hElem)
{
    // Since we specified PWGM_ELEMORDER_VC in beginExport() and
    // PWGM_FACEORDER_BCGROUPSONLY in streamFaces(), elements will be
    // enumerated in VC/BC order. Only do the work of looking up the VC/BC when
    // the element parent changes.
    PWP_UINT32 elemParentId = PWGM_HELEMENT_PID(hElem);
    if (curElemParentId_ != elemParentId) {
        // First elem of a new parent block/patch. Check if VC/BC changed.
        curElemParentId_ = elemParentId;
        PWGM_CONDDATA cond;
        bool haveCond;
        if (writingBlocks_) {
            CaeUnsBlock block(model_, elemParentId);
            haveCond = block.condition(cond);
        }
        else {
            CaeUnsPatch patch(model_, elemParentId);
            haveCond = patch.condition(cond);
        }
        // Since condition names are const pointers that do not change during
        // an export, just comparing pointer addresses is sufficient for
        // uniqueness.
        if (haveCond && (curCondName_ != cond.name)) {
            // parent has a different condition.
            curCondName_ = cond.name;
            // start a new Gmsh condition id
            ++physNameId_;
            cachePhysicalName();
        }
    }
    return physNameId_;
}


void
CaeUnsGMesh::cachePhysicalName()
{
    // cache physical name item: "elemDim physId physName"
    const char *dimStr;
    if (writingBlocks_) {
        dimStr = (isDimension3D() ? "3 " : "2 ");
    }
    else {
        dimStr = (isDimension3D() ? "2 " : "1 ");
    }
    std::ostringstream physName;
    physName << dimStr;
    physName << physNameId_;
    physName << " \"";
    physName << curCondName_;
    physName << "\"";
    physNameCache_.push_back(physName.str());
}


void
CaeUnsGMesh::writeComment(const char *txt)
{
    rtFile_.write("$Comments\n");
    rtFile_.write("-------------------------------------------------------\n");
    rtFile_.write(txt);
    rtFile_.write("\n");
    rtFile_.write("-------------------------------------------------------\n");
    rtFile_.write("$EndComments\n");
}


//===========================================================================
// face streaming handlers
//===========================================================================

PWP_UINT32
CaeUnsGMesh::streamBegin(const PWGM_BEGINSTREAM_DATA &data)
{
    writeComment("Types: bar(1) tri(2) quad(3) tet(4) hex(5) prism(6) "
        "pyramid(7)\nNumElements\nId Type NumTags PhysGrp ElemGrp IndexList");
    // Total elements = block element count + boundary element count
    PWP_UINT32 cnt = model_.elementCount() + data.totalNumFaces;
    rtFile_.write("$Elements\n");
    rtFile_.write(cnt, "\n");

    // step 2 of 3
    bool ret = progressBeginStep(model_.elementCount());

    // write block elements before streaming boundary elements
    if (ret) {
        elemIdWd_ = int(log10(float(cnt))) + 1;
        writingBlocks_ = true;
        CaeUnsElement e(model_);
        PWGM_ENUMELEMDATA eed;
        while (e.data(eed)) {
            writeElement(eed.hBlkElement, eed.elemData);
            if (!progressIncrement()) {
                ret = false;
                break;
            }
            ++e;
        }
        progressEndStep();
    }

    // step 3 of 3
    ret = ret && progressBeginStep(data.totalNumFaces);
    // The boundary elements are now written if streaming continues
    if (ret) {
        // reset condition data tracking vars for the boundary elements
        writingBlocks_ = false;
        curElemParentId_ = PWP_UINT32_UNDEF;
        curCondName_ = 0;
    }
    return (ret ? 1 : 0);
}


PWP_UINT32
CaeUnsGMesh::streamFace(const PWGM_FACESTREAM_DATA &data)
{
    bool ret = false;
    if (PWGM_FACETYPE_BOUNDARY == data.type) {
        writeElement(data.owner.domainElem, data.elemData);
        ret = progressIncrement();
    }
    return (ret ? 1 : 0);
}


PWP_UINT32
CaeUnsGMesh::streamEnd(const PWGM_ENDSTREAM_DATA &data)
{
    rtFile_.write("$EndElements\n");
    progressEndStep();
    return data.ok;
}


//===========================================================================
// called ONCE when plugin first loaded into memeory
//===========================================================================

bool
CaeUnsGMesh::create(CAEP_RTITEM &rti)
{
    (void)rti.BCCnt; // silence unused arg warning
    bool ret = true;

    // Uncomment this INFO attribute if your solver supports both big and
    // little endian byte orderings.
    //ret = ret && caeuAssignInfoValue("AllowedFileByteOrders",
    //                "BigEndian|LittleEndian", true);

    // Uncomment one of these INFO attributes if your solver requires a
    // particular byte ordering.
    //ret = ret && caeuAssignInfoValue("AllowedFileByteOrders", "BigEndian",
    //                true);
    //ret = ret && caeuAssignInfoValue("AllowedFileByteOrders", "LittleEndian",
    //                true);

    // These attributes are for example only. You can publish any attribute
    // needed for your solver.
    // ret = ret &&
    //      caeuPublishValueDefinition("iterations", PWP_VALTYPE_UINT, "5",
    //          "RW", "Number of iterations", "0 2000") &&
    //      caeuPublishValueDefinition("magnitude", PWP_VALTYPE_INT, "-5",
    //          "RW", "Signed int magnitude", "-100 100") &&
    //      caeuPublishValueDefinition("mach", PWP_VALTYPE_REAL, "0.3", "RW",
    //          "Incoming flow velocity", "-Inf +Inf 0.0 50.0") &&
    //      caeuPublishValueDefinition("temperature", PWP_VALTYPE_REAL, "77.5",
    //          "RW", "Ambient temperature", "-Inf +Inf -100.0 3000.0") &&
    //      caeuPublishValueDefinition("temperature.units", PWP_VALTYPE_ENUM,
    //          "Fahrenheit", "RW", "Grid temperature units", TempUnitEnum) &&
    //      caeuPublishValueDefinition("units", PWP_VALTYPE_ENUM, "Inches",
    //          "RW", "Grid dimensional units", DimUnitEnum) &&
    //      caeuPublishValueDefinition("description", PWP_VALTYPE_STRING, "",
    //          "RW", "Grid description", "") &&
    //      caeuPublishValueDefinition("linear", PWP_VALTYPE_BOOL, "reject",
    //          "RW", "Grid is linear", "reject|accept");

    return ret;
}


//===========================================================================
// called ONCE just before plugin unloaded from memeory
//===========================================================================

void
CaeUnsGMesh::destroy(CAEP_RTITEM &rti)
{
    (void)rti.BCCnt; // silence unused arg warning
}
