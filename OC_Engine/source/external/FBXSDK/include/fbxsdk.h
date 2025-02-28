/****************************************************************************************
 
   Copyright (C) 2016 Autodesk, Inc.
   All rights reserved.
 
   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.
 
****************************************************************************************/

//! \file fbxsdk.h
#ifndef _FBXSDK_H_
#define _FBXSDK_H_

/**
  * \mainpage FBX SDK Reference
  * <p>
  * \section welcome Welcome to the FBX SDK Reference
  * The FBX SDK Reference contains reference information on every header file, 
  * namespace, class, method, enum, typedef, variable, and other C++ elements 
  * that comprise the FBX software development kit (SDK).
  * <p>
  * The FBX SDK Reference is organized into the following sections:
  * <ul><li>Class List: an alphabetical list of FBX SDK classes
  *     <li>Class Hierarchy: a textual representation of the FBX SDK class structure
  *     <li>Graphical Class Hierarchy: a graphical representation of the FBX SDK class structure
  *     <li>File List: an alphabetical list of all documented header files</ul>
  * <p>
  * \section otherdocumentation Other Documentation
  * Apart from this reference guide, an FBX SDK Programming Guide and many FBX 
  * SDK examples are also provided.
  * <p>
  * \section aboutFBXSDK About the FBX SDK
  * The FBX SDK is a C++ software development kit (SDK) that lets you import 
  * and export 3D scenes using the Autodesk FBX file format. The FBX SDK 
  * reads FBX files created with FiLMBOX version 2.5 and later and writes FBX 
  * files compatible with MotionBuilder version 6.0 and up. 
  */

#include <../FBXSDK/include/fbxsdk/fbxsdk_def.h>

#ifndef FBXSDK_NAMESPACE_USING
	#define FBXSDK_NAMESPACE_USING 1
#endif

//---------------------------------------------------------------------------------------
//Core Base Includes
#include <../FBXSDK/include/fbxsdk/core/base/fbxarray.h>
#include <../FBXSDK/include/fbxsdk/core/base/fbxbitset.h>
#include <../FBXSDK/include/fbxsdk/core/base/fbxcharptrset.h>
#include <../FBXSDK/include/fbxsdk/core/base/fbxcontainerallocators.h>
#include <../FBXSDK/include/fbxsdk/core/base/fbxdynamicarray.h>
#include <../FBXSDK/include/fbxsdk/core/base/fbxstatus.h>
#include <../FBXSDK/include/fbxsdk/core/base/fbxfile.h>
#ifndef FBXSDK_ENV_WINSTORE
	#include <../FBXSDK/include/fbxsdk/core/base/fbxfolder.h>
#endif
#include <../FBXSDK/include/fbxsdk/core/base/fbxhashmap.h>
#include <../FBXSDK/include/fbxsdk/core/base/fbxintrusivelist.h>
#include <../FBXSDK/include/fbxsdk/core/base/fbxmap.h>
#include <../FBXSDK/include/fbxsdk/core/base/fbxmemorypool.h>
#include <../FBXSDK/include/fbxsdk/core/base/fbxpair.h>
#include <../FBXSDK/include/fbxsdk/core/base/fbxset.h>
#include <../FBXSDK/include/fbxsdk/core/base/fbxstring.h>
#include <../FBXSDK/include/fbxsdk/core/base/fbxstringlist.h>
#include <../FBXSDK/include/fbxsdk/core/base/fbxtime.h>
#include <../FBXSDK/include/fbxsdk/core/base/fbxtimecode.h>
#include <../FBXSDK/include/fbxsdk/core/base/fbxutils.h>

//---------------------------------------------------------------------------------------
//Core Math Includes
#include <../FBXSDK/include/fbxsdk/core/math/fbxmath.h>
#include <../FBXSDK/include/fbxsdk/core/math/fbxdualquaternion.h>
#include <../FBXSDK/include/fbxsdk/core/math/fbxmatrix.h>
#include <../FBXSDK/include/fbxsdk/core/math/fbxquaternion.h>
#include <../FBXSDK/include/fbxsdk/core/math/fbxvector2.h>
#include <../FBXSDK/include/fbxsdk/core/math/fbxvector4.h>

//---------------------------------------------------------------------------------------
//Core Sync Includes
#ifndef FBXSDK_ENV_WINSTORE
	#include <../FBXSDK/include/fbxsdk/core/sync/fbxatomic.h>
	#include <../FBXSDK/include/fbxsdk/core/sync/fbxclock.h>
	#include <../FBXSDK/include/fbxsdk/core/sync/fbxsync.h>
	#include <../FBXSDK/include/fbxsdk/core/sync/fbxthread.h>
#endif /* !FBXSDK_ENV_WINSTORE */

//---------------------------------------------------------------------------------------
//Core Includes
#include <../FBXSDK/include/fbxsdk/core/fbxclassid.h>
#include <../FBXSDK/include/fbxsdk/core/fbxconnectionpoint.h>
#include <../FBXSDK/include/fbxsdk/core/fbxdatatypes.h>
#ifndef FBXSDK_ENV_WINSTORE
	#include <../FBXSDK/include/fbxsdk/core/fbxmodule.h>
	#include <../FBXSDK/include/fbxsdk/core/fbxloadingstrategy.h>
#endif /* !FBXSDK_ENV_WINSTORE */
#include <../FBXSDK/include/fbxsdk/core/fbxmanager.h>
#include <../FBXSDK/include/fbxsdk/core/fbxobject.h>
#include <../FBXSDK/include/fbxsdk/core/fbxperipheral.h>
#ifndef FBXSDK_ENV_WINSTORE
	#include <../FBXSDK/include/fbxsdk/core/fbxplugin.h>
	#include <../FBXSDK/include/fbxsdk/core/fbxplugincontainer.h>
#endif /* !FBXSDK_ENV_WINSTORE */
#include <../FBXSDK/include/fbxsdk/core/fbxproperty.h>
#include <../FBXSDK/include/fbxsdk/core/fbxpropertydef.h>
#include <../FBXSDK/include/fbxsdk/core/fbxpropertyhandle.h>
#include <../FBXSDK/include/fbxsdk/core/fbxpropertypage.h>
#include <../FBXSDK/include/fbxsdk/core/fbxpropertytypes.h>
#include <../FBXSDK/include/fbxsdk/core/fbxquery.h>
#include <../FBXSDK/include/fbxsdk/core/fbxqueryevent.h>
#ifndef FBXSDK_ENV_WINSTORE
	#include <../FBXSDK/include/fbxsdk/core/fbxscopedloadingdirectory.h>
	#include <../FBXSDK/include/fbxsdk/core/fbxscopedloadingfilename.h>
#endif /* !FBXSDK_ENV_WINSTORE */
#include <../FBXSDK/include/fbxsdk/core/fbxxref.h>

//---------------------------------------------------------------------------------------
//File I/O Includes
#include <../FBXSDK/include/fbxsdk/fileio/fbxexporter.h>
#include <../FBXSDK/include/fbxsdk/fileio/fbxexternaldocreflistener.h>
#include <../FBXSDK/include/fbxsdk/fileio/fbxfiletokens.h>
#include <../FBXSDK/include/fbxsdk/fileio/fbxglobalcamerasettings.h>
#include <../FBXSDK/include/fbxsdk/fileio/fbxgloballightsettings.h>
#include <../FBXSDK/include/fbxsdk/fileio/fbxgobo.h>
#include <../FBXSDK/include/fbxsdk/fileio/fbximporter.h>
#include <../FBXSDK/include/fbxsdk/fileio/fbxiobase.h>
#include <../FBXSDK/include/fbxsdk/fileio/fbxiopluginregistry.h>
#include <../FBXSDK/include/fbxsdk/fileio/fbxiosettings.h>
#include <../FBXSDK/include/fbxsdk/fileio/fbxstatisticsfbx.h>
#include <../FBXSDK/include/fbxsdk/fileio/fbxstatistics.h>
#include <../FBXSDK/include/fbxsdk/fileio/fbxcallbacks.h>

//---------------------------------------------------------------------------------------
//Scene Includes
#include <../FBXSDK/include/fbxsdk/scene/fbxaudio.h>
#include <../FBXSDK/include/fbxsdk/scene/fbxaudiolayer.h>
#include <../FBXSDK/include/fbxsdk/scene/fbxcollection.h>
#include <../FBXSDK/include/fbxsdk/scene/fbxcollectionexclusive.h>
#include <../FBXSDK/include/fbxsdk/scene/fbxcontainer.h>
#include <../FBXSDK/include/fbxsdk/scene/fbxcontainertemplate.h>
#include <../FBXSDK/include/fbxsdk/scene/fbxdisplaylayer.h>
#include <../FBXSDK/include/fbxsdk/scene/fbxdocument.h>
#include <../FBXSDK/include/fbxsdk/scene/fbxdocumentinfo.h>
#include <../FBXSDK/include/fbxsdk/scene/fbxenvironment.h>
#include <../FBXSDK/include/fbxsdk/scene/fbxgroupname.h>
#include <../FBXSDK/include/fbxsdk/scene/fbxlibrary.h>
#include <../FBXSDK/include/fbxsdk/scene/fbxmediaclip.h>
#include <../FBXSDK/include/fbxsdk/scene/fbxobjectmetadata.h>
#include <../FBXSDK/include/fbxsdk/scene/fbxpose.h>
#include <../FBXSDK/include/fbxsdk/scene/fbxreference.h>
#include <../FBXSDK/include/fbxsdk/scene/fbxscene.h>
#include <../FBXSDK/include/fbxsdk/scene/fbxselectionset.h>
#include <../FBXSDK/include/fbxsdk/scene/fbxselectionnode.h>
#include <../FBXSDK/include/fbxsdk/scene/fbxtakeinfo.h>
#include <../FBXSDK/include/fbxsdk/scene/fbxthumbnail.h>
#include <../FBXSDK/include/fbxsdk/scene/fbxvideo.h>

//---------------------------------------------------------------------------------------
//Scene Animation Includes
#include <../FBXSDK/include/fbxsdk/scene/animation/fbxanimcurve.h>
#include <../FBXSDK/include/fbxsdk/scene/animation/fbxanimcurvebase.h>
#include <../FBXSDK/include/fbxsdk/scene/animation/fbxanimcurvefilters.h>
#include <../FBXSDK/include/fbxsdk/scene/animation/fbxanimcurvenode.h>
#include <../FBXSDK/include/fbxsdk/scene/animation/fbxanimevalclassic.h>
#include <../FBXSDK/include/fbxsdk/scene/animation/fbxanimevalstate.h>
#include <../FBXSDK/include/fbxsdk/scene/animation/fbxanimevaluator.h>
#include <../FBXSDK/include/fbxsdk/scene/animation/fbxanimlayer.h>
#include <../FBXSDK/include/fbxsdk/scene/animation/fbxanimstack.h>
#include <../FBXSDK/include/fbxsdk/scene/animation/fbxanimutilities.h>

//---------------------------------------------------------------------------------------
//Scene Constraint Includes
#include <../FBXSDK/include/fbxsdk/scene/constraint/fbxcharacternodename.h>
#include <../FBXSDK/include/fbxsdk/scene/constraint/fbxcharacter.h>
#include <../FBXSDK/include/fbxsdk/scene/constraint/fbxcharacterpose.h>
#include <../FBXSDK/include/fbxsdk/scene/constraint/fbxconstraint.h>
#include <../FBXSDK/include/fbxsdk/scene/constraint/fbxconstraintaim.h>
#include <../FBXSDK/include/fbxsdk/scene/constraint/fbxconstraintcustom.h>
#include <../FBXSDK/include/fbxsdk/scene/constraint/fbxconstraintparent.h>
#include <../FBXSDK/include/fbxsdk/scene/constraint/fbxconstraintposition.h>
#include <../FBXSDK/include/fbxsdk/scene/constraint/fbxconstraintrotation.h>
#include <../FBXSDK/include/fbxsdk/scene/constraint/fbxconstraintscale.h>
#include <../FBXSDK/include/fbxsdk/scene/constraint/fbxconstraintsinglechainik.h>
#include <../FBXSDK/include/fbxsdk/scene/constraint/fbxconstraintutils.h>
#include <../FBXSDK/include/fbxsdk/scene/constraint/fbxcontrolset.h>
#include <../FBXSDK/include/fbxsdk/scene/constraint/fbxhik2fbxcharacter.h>

//---------------------------------------------------------------------------------------
//Scene Geometry Includes
#include <../FBXSDK/include/fbxsdk/scene/geometry/fbxblendshape.h>
#include <../FBXSDK/include/fbxsdk/scene/geometry/fbxblendshapechannel.h>
#include <../FBXSDK/include/fbxsdk/scene/geometry/fbxcache.h>
#include <../FBXSDK/include/fbxsdk/scene/geometry/fbxcachedeffect.h>
#include <../FBXSDK/include/fbxsdk/scene/geometry/fbxcamera.h>
#include <../FBXSDK/include/fbxsdk/scene/geometry/fbxcamerastereo.h>
#include <../FBXSDK/include/fbxsdk/scene/geometry/fbxcameraswitcher.h>
#include <../FBXSDK/include/fbxsdk/scene/geometry/fbxcluster.h>
#include <../FBXSDK/include/fbxsdk/scene/geometry/fbxdeformer.h>
#include <../FBXSDK/include/fbxsdk/scene/geometry/fbxgenericnode.h>
#include <../FBXSDK/include/fbxsdk/scene/geometry/fbxgeometry.h>
#include <../FBXSDK/include/fbxsdk/scene/geometry/fbxgeometrybase.h>
#include <../FBXSDK/include/fbxsdk/scene/geometry/fbxgeometryweightedmap.h>
#include <../FBXSDK/include/fbxsdk/scene/geometry/fbxlight.h>
#include <../FBXSDK/include/fbxsdk/scene/geometry/fbxlimitsutilities.h>
#include <../FBXSDK/include/fbxsdk/scene/geometry/fbxline.h>
#include <../FBXSDK/include/fbxsdk/scene/geometry/fbxlodgroup.h>
#include <../FBXSDK/include/fbxsdk/scene/geometry/fbxmarker.h>
#include <../FBXSDK/include/fbxsdk/scene/geometry/fbxmesh.h>
#include <../FBXSDK/include/fbxsdk/scene/geometry/fbxnode.h>
#include <../FBXSDK/include/fbxsdk/scene/geometry/fbxnodeattribute.h>
#include <../FBXSDK/include/fbxsdk/scene/geometry/fbxnull.h>
#include <../FBXSDK/include/fbxsdk/scene/geometry/fbxnurbs.h>
#include <../FBXSDK/include/fbxsdk/scene/geometry/fbxnurbscurve.h>
#include <../FBXSDK/include/fbxsdk/scene/geometry/fbxnurbssurface.h>
#include <../FBXSDK/include/fbxsdk/scene/geometry/fbxopticalreference.h>
#include <../FBXSDK/include/fbxsdk/scene/geometry/fbxpatch.h>
#include <../FBXSDK/include/fbxsdk/scene/geometry/fbxproceduralgeometry.h>
#include <../FBXSDK/include/fbxsdk/scene/geometry/fbxshape.h>
#include <../FBXSDK/include/fbxsdk/scene/geometry/fbxskeleton.h>
#include <../FBXSDK/include/fbxsdk/scene/geometry/fbxskin.h>
#include <../FBXSDK/include/fbxsdk/scene/geometry/fbxsubdeformer.h>
#include <../FBXSDK/include/fbxsdk/scene/geometry/fbxsubdiv.h>
#include <../FBXSDK/include/fbxsdk/scene/geometry/fbxtrimnurbssurface.h>
#include <../FBXSDK/include/fbxsdk/scene/geometry/fbxvertexcachedeformer.h>
#include <../FBXSDK/include/fbxsdk/scene/geometry/fbxweightedmapping.h>

//---------------------------------------------------------------------------------------
//Scene Shading Includes
#include <../FBXSDK/include/fbxsdk/scene/shading/fbxshadingconventions.h>
#include <../FBXSDK/include/fbxsdk/scene/shading/fbxbindingsentryview.h>
#include <../FBXSDK/include/fbxsdk/scene/shading/fbxbindingtable.h>
#include <../FBXSDK/include/fbxsdk/scene/shading/fbxbindingtableentry.h>
#include <../FBXSDK/include/fbxsdk/scene/shading/fbxbindingoperator.h>
#include <../FBXSDK/include/fbxsdk/scene/shading/fbxconstantentryview.h>
#include <../FBXSDK/include/fbxsdk/scene/shading/fbxentryview.h>
#include <../FBXSDK/include/fbxsdk/scene/shading/fbxfiletexture.h>
#include <../FBXSDK/include/fbxsdk/scene/shading/fbximplementation.h>
#include <../FBXSDK/include/fbxsdk/scene/shading/fbximplementationfilter.h>
#include <../FBXSDK/include/fbxsdk/scene/shading/fbximplementationutils.h>
#include <../FBXSDK/include/fbxsdk/scene/shading/fbxlayeredtexture.h>
#include <../FBXSDK/include/fbxsdk/scene/shading/fbxoperatorentryview.h>
#include <../FBXSDK/include/fbxsdk/scene/shading/fbxproceduraltexture.h>
#include <../FBXSDK/include/fbxsdk/scene/shading/fbxpropertyentryview.h>
#include <../FBXSDK/include/fbxsdk/scene/shading/fbxsemanticentryview.h>
#include <../FBXSDK/include/fbxsdk/scene/shading/fbxsurfacelambert.h>
#include <../FBXSDK/include/fbxsdk/scene/shading/fbxsurfacematerial.h>
#include <../FBXSDK/include/fbxsdk/scene/shading/fbxsurfacephong.h>
#include <../FBXSDK/include/fbxsdk/scene/shading/fbxtexture.h>

//---------------------------------------------------------------------------------------
//Utilities Includes
#include <../FBXSDK/include/fbxsdk/utils/fbxdeformationsevaluator.h>
#include <../FBXSDK/include/fbxsdk/utils/fbxprocessor.h>
#include <../FBXSDK/include/fbxsdk/utils/fbxprocessorxref.h>
#include <../FBXSDK/include/fbxsdk/utils/fbxprocessorxrefuserlib.h>
#include <../FBXSDK/include/fbxsdk/utils/fbxprocessorshaderdependency.h>
#include <../FBXSDK/include/fbxsdk/utils/fbxclonemanager.h>
#include <../FBXSDK/include/fbxsdk/utils/fbxgeometryconverter.h>
#include <../FBXSDK/include/fbxsdk/utils/fbxmanipulators.h>
#include <../FBXSDK/include/fbxsdk/utils/fbxmaterialconverter.h>
#include <../FBXSDK/include/fbxsdk/utils/fbxrenamingstrategyfbx5.h>
#include <../FBXSDK/include/fbxsdk/utils/fbxrenamingstrategyfbx6.h>
#include <../FBXSDK/include/fbxsdk/utils/fbxrenamingstrategyutilities.h>
#include <../FBXSDK/include/fbxsdk/utils/fbxrootnodeutility.h>
#include <../FBXSDK/include/fbxsdk/utils/fbxusernotification.h>
#include <../FBXSDK/include/fbxsdk/utils/fbxscenecheckutility.h>

//---------------------------------------------------------------------------------------
#if defined(FBXSDK_NAMESPACE) && (FBXSDK_NAMESPACE_USING == 1)
	using namespace FBXSDK_NAMESPACE;
#endif

#endif /* _FBXSDK_H_ */
