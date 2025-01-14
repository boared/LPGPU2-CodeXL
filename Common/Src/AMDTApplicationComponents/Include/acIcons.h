//==================================================================================
// Copyright (c) 2016 , Advanced Micro Devices, Inc.  All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file acIcons.h
///
//==================================================================================

//------------------------------ acIcons.h ------------------------------

#ifndef __ACICONS
#define __ACICONS

// Qt:
#include <QtWidgets>

// Local:
#include <AMDTApplicationComponents/Include/acApplicationComponentsDLLBuild.h>

// ----------------------------------------------------------------------------------
// Class Name:          acIconSize
// General Description: Represents an icon size
// Author:              Uri Shomroni
// Creation Date:       Represents an icon identifier
// ----------------------------------------------------------------------------------
enum AC_API acIconSize
{
    AC_16x16_ICON,
    AC_24x24_ICON,
    AC_32x32_ICON,
    AC_48x48_ICON,
    AC_64x64_ICON,
    // Future sizes, if resolutions grow even higher - to keep the scaling consistent
    // AC_96x96_ICON,
    // AC_128x128_ICON,
    // AC_192x192_ICON,
    // AC_256x256_ICON,
    AC_NUMBER_OF_ICON_SIZES
};

// ----------------------------------------------------------------------------------
// Class Name:          acIconId
// General Description: Identifies an icon image
// Author:              Uri Shomroni
// Creation Date:       Represents an icon identifier
// ----------------------------------------------------------------------------------
enum AC_API acIconId
{
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    // General application
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    AC_ICON_EMPTY,
    AC_ICON_CODEXL_LOGO,
    AC_ICON_CODEXL_LOGO_INVERTED,

    AC_ICON_AMDCOMPRESS_LOGO,
    AC_ICON_AMDCOMPRESS_LOGO_INVERTED,

    // General views:
    AC_ICON_VIEW_PROPERTIES,
    AC_ICON_VIEW_INFORMATION,

    // Startup dialog:
    AC_ICON_STARTUP_NEW,
    AC_ICON_STARTUP_NEW_DEBUG,
    AC_ICON_STARTUP_NEW_PROFILE,
    AC_ICON_STARTUP_NEW_PROFILE_ATTACH,
    AC_ICON_STARTUP_NEW_PROFILE_SYSTEM,
    AC_ICON_STARTUP_NEW_FRAME_ANALYZE,
    AC_ICON_STARTUP_NEW_ANALYZE,
    AC_ICON_STARTUP_NEW_ANALYZE_FILE,

    // Application Tree view, generic items:
    AC_ICON_APPTREE_ROOT,
    AC_ICON_APPTREE_BACK,
    AC_ICON_APPTREE_FWD,

    // Ribbon control
    AC_ICON_RIBBON_OPEN,
    AC_ICON_RIBBON_OPEN_DISABLED,
    AC_ICON_RIBBON_CLOSE,
    AC_ICON_RIBBON_CLOSE_DISABLED,
    AC_ICON_RIBBON_FILTER,

    // Execution toolbar:
    AC_ICON_EXECUTION_PLAY,
    AC_ICON_EXECUTION_PAUSE,
    AC_ICON_EXECUTION_STOP,
    AC_ICON_EXECUTION_API_STEP,
    AC_ICON_EXECUTION_DRAW_STEP,
    AC_ICON_EXECUTION_FRAME_STEP,
    AC_ICON_EXECUTION_STEP_IN,
    AC_ICON_EXECUTION_STEP_OVER,
    AC_ICON_EXECUTION_STEP_OUT,
    AC_ICON_EXECUTION_BUILD,
    AC_ICON_EXECUTION_CANCEL_BUILD,
    AC_ICON_EXECUTION_CAPTURE,
    AC_ICON_EXECUTION_CAPTURE_ONLY_CPU,
    AC_ICON_EXECUTION_CAPTURE_ONLY_GPU,

    // Source view:
    AC_ICON_SOURCE_C,
    AC_ICON_SOURCE_CL,
    AC_ICON_SOURCE_CPP,
    AC_ICON_SOURCE_GENERIC,
    AC_ICON_SOURCE_GLSL,
    AC_ICON_SOURCE_H,
    AC_ICON_SOURCE_READONLY,
    AC_ICON_SOURCE_DISABLED_BREAKPOINT,
    AC_ICON_SOURCE_ENABLED_BREAKPOINT,
    AC_ICON_SOURCE_PROGRAM_COUNTER,
    AC_ICON_SOURCE_TOP_PROGRAM_COUNTER,

    // Find Toolbar:
    AC_ICON_FIND_UP,
    AC_ICON_FIND_DOWN,
    AC_ICON_FIND_CLOSE_CLEAR,
    AC_ICON_FIND_FIND,

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    // Debugger
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    AC_ICON_DEBUG_MODE,
    AC_ICON_WARNING_INFO,
    AC_ICON_WARNING_YELLOW,
    AC_ICON_WARNING_ORANGE,
    AC_ICON_WARNING_RED,

    // Debugger views:
    AC_ICON_DEBUG_VIEW_BREAKPOINTS,
    AC_ICON_DEBUG_VIEW_CALLSTACK,
    AC_ICON_DEBUG_VIEW_CALLHISTORY,
    AC_ICON_DEBUG_VIEW_LOCALS,
    AC_ICON_DEBUG_VIEW_MEMORY,
    AC_ICON_DEBUG_VIEW_EVENTS,
    AC_ICON_DEBUG_VIEW_STATISTICS,
    AC_ICON_DEBUG_VIEW_STATEVARS,
    AC_ICON_DEBUG_VIEW_WATCH,
    AC_ICON_DEBUG_VIEW_MULTIWATCH,

    // Debugger tree items:
    AC_ICON_DEBUG_APPTREE_GL_CONTEXT,
    AC_ICON_DEBUG_APPTREE_GL_CONTEXTDELETED,
    AC_ICON_DEBUG_APPTREE_GL_CONTEXTSHARED,
    AC_ICON_DEBUG_APPTREE_GL_CONTEXTDELETEDSHARED,
    AC_ICON_DEBUG_APPTREE_GL_TEXGENERIC,
    AC_ICON_DEBUG_APPTREE_GL_TEX1D,
    AC_ICON_DEBUG_APPTREE_GL_TEX2D,
    AC_ICON_DEBUG_APPTREE_GL_TEX2DMS,
    AC_ICON_DEBUG_APPTREE_GL_TEXCUBE,
    AC_ICON_DEBUG_APPTREE_GL_TEX3D,
    AC_ICON_DEBUG_APPTREE_GL_TEXRECT,
    AC_ICON_DEBUG_APPTREE_GL_TEX1DA,
    AC_ICON_DEBUG_APPTREE_GL_TEX2DA,
    AC_ICON_DEBUG_APPTREE_GL_TEX2DMSA,
    AC_ICON_DEBUG_APPTREE_GL_TEXCUBEA,
    AC_ICON_DEBUG_APPTREE_GL_TEXBUFFER,
    AC_ICON_DEBUG_APPTREE_GL_TEXUNKNOWN,
    AC_ICON_DEBUG_APPTREE_GL_BUFFER_GENERIC,
    AC_ICON_DEBUG_APPTREE_GL_BUFFER_ARRAY,
    AC_ICON_DEBUG_APPTREE_GL_BUFFER_DRAW_INDIRECT,
    AC_ICON_DEBUG_APPTREE_GL_BUFFER_DISPATCH_INDIRECT,
    AC_ICON_DEBUG_APPTREE_GL_BUFFER_ELEMENT_ARRAY,
    AC_ICON_DEBUG_APPTREE_GL_BUFFER_PIXEL_PACK,
    AC_ICON_DEBUG_APPTREE_GL_BUFFER_PIXEL_UNPACK,
    AC_ICON_DEBUG_APPTREE_GL_BUFFER_COPY_READ,
    AC_ICON_DEBUG_APPTREE_GL_BUFFER_COPY_WRITE,
    AC_ICON_DEBUG_APPTREE_GL_BUFFER_TRANSFORM_FEEDBACK,
    AC_ICON_DEBUG_APPTREE_GL_BUFFER_UNIFORM,
    AC_ICON_DEBUG_APPTREE_GL_BUFFER_ATOMIC_COUNTER,
    AC_ICON_DEBUG_APPTREE_GL_BUFFER_SHADER_STORAGE,
    AC_ICON_DEBUG_APPTREE_GL_BUFFER_QUERY,
    AC_ICON_DEBUG_APPTREE_GL_BUFFER_TEXTURE,
    AC_ICON_DEBUG_APPTREE_GL_BUFFER_UNKNOWN,
    AC_ICON_DEBUG_APPTREE_GL_SAMPLER,
    AC_ICON_DEBUG_APPTREE_GL_STATICBUFFER,
    AC_ICON_DEBUG_APPTREE_GL_STATICBUFFERLINK,
    AC_ICON_DEBUG_APPTREE_GL_RENDERBUFFER,
    AC_ICON_DEBUG_APPTREE_GL_RENDERBUFFERLINK,
    AC_ICON_DEBUG_APPTREE_GL_FBO,
    AC_ICON_DEBUG_APPTREE_GL_PROGRAM,
    AC_ICON_DEBUG_APPTREE_GL_PROGRAMDELETED,
    AC_ICON_DEBUG_APPTREE_GL_SHADER,
    AC_ICON_DEBUG_APPTREE_GL_SHADERDELETED,
    AC_ICON_DEBUG_APPTREE_GL_PIPELINE,
    AC_ICON_DEBUG_APPTREE_GL_DISPLAYLIST,
    AC_ICON_DEBUG_APPTREE_GL_PBUFFER,
    AC_ICON_DEBUG_APPTREE_GL_SYNC,
    AC_ICON_DEBUG_APPTREE_CL_CONTEXT,
    AC_ICON_DEBUG_APPTREE_CL_CONTEXTDELETED,
    AC_ICON_DEBUG_APPTREE_CL_BUFFER,
    AC_ICON_DEBUG_APPTREE_CL_IMAGEGENERIC,
    AC_ICON_DEBUG_APPTREE_CL_IMAGE2D,
    AC_ICON_DEBUG_APPTREE_CL_IMAGE3D,
    AC_ICON_DEBUG_APPTREE_CL_PIPE,
    AC_ICON_DEBUG_APPTREE_CL_SAMPLER,
    AC_ICON_DEBUG_APPTREE_CL_QUEUE,
    AC_ICON_DEBUG_APPTREE_CL_EVENT,
    AC_ICON_DEBUG_APPTREE_CL_PROGRAM,
    AC_ICON_DEBUG_APPTREE_CL_KERNEL,

    // Calls History view:
    AC_ICON_DEBUG_CALLSHISTORY_GL,
    AC_ICON_DEBUG_CALLSHISTORY_GLEXT,
    AC_ICON_DEBUG_CALLSHISTORY_WGL,
    AC_ICON_DEBUG_CALLSHISTORY_GLX,
    AC_ICON_DEBUG_CALLSHISTORY_CGL,
    AC_ICON_DEBUG_CALLSHISTORY_CL,
    AC_ICON_DEBUG_CALLSHISTORY_NEXTMARKER,
    AC_ICON_DEBUG_CALLSHISTORY_PREVIOUSMARKER,
    AC_ICON_DEBUG_CALLSHISTORY_OPENLOG,
    AC_ICON_DEBUG_CALLSHISTORY_RECORD,

    // Process Events view:
    AC_ICON_DEBUG_EVENTSVIEW_PROCESS_PLUS,
    AC_ICON_DEBUG_EVENTSVIEW_PROCESS_RUN,
    AC_ICON_DEBUG_EVENTSVIEW_PROCESS_MINUS,
    AC_ICON_DEBUG_EVENTSVIEW_MODULE_PLUS,
    AC_ICON_DEBUG_EVENTSVIEW_MODULE_MINUS,
    AC_ICON_DEBUG_EVENTSVIEW_THREAD_PLUS,
    AC_ICON_DEBUG_EVENTSVIEW_THREAD_MINUS,
    AC_ICON_DEBUG_EVENTSVIEW_CONNECTION_PLUS,
    AC_ICON_DEBUG_EVENTSVIEW_CONNECTION_MINUS,
    AC_ICON_DEBUG_EVENTSVIEW_GLCONTEXT_PLUS,
    AC_ICON_DEBUG_EVENTSVIEW_GLCONTEXT_MINUS,
    AC_ICON_DEBUG_EVENTSVIEW_CLCONTEXT_PLUS,
    AC_ICON_DEBUG_EVENTSVIEW_CLCONTEXT_MINUS,
    AC_ICON_DEBUG_EVENTSVIEW_CLQUEUE_PLUS,
    AC_ICON_DEBUG_EVENTSVIEW_CLQUEUE_MINUS,
    AC_ICON_DEBUG_EVENTSVIEW_CLPROGRAM_PLUS,
    AC_ICON_DEBUG_EVENTSVIEW_CLPROGRAM_MINUS,
    AC_ICON_DEBUG_EVENTSVIEW_CLPROGRAM_BUILD,
    AC_ICON_DEBUG_EVENTSVIEW_BREAK,
    AC_ICON_DEBUG_EVENTSVIEW_BREAK_POINT,
    AC_ICON_DEBUG_EVENTSVIEW_BREAK_APISTEP,
    AC_ICON_DEBUG_EVENTSVIEW_BREAK_DRAWSTEP,
    AC_ICON_DEBUG_EVENTSVIEW_BREAK_FRAMESTEP,
    AC_ICON_DEBUG_EVENTSVIEW_BREAK_GLERROR,
    AC_ICON_DEBUG_EVENTSVIEW_BREAK_CLERROR,
    AC_ICON_DEBUG_EVENTSVIEW_CLERROR,
    AC_ICON_DEBUG_EVENTSVIEW_BREAK_DETECTED,
    AC_ICON_DEBUG_EVENTSVIEW_DETECTED,
    AC_ICON_DEBUG_EVENTSVIEW_BREAK_REDUNDANT,
    AC_ICON_DEBUG_EVENTSVIEW_BREAK_DEPRECATED,
    AC_ICON_DEBUG_EVENTSVIEW_BREAK_MEMORY,
    AC_ICON_DEBUG_EVENTSVIEW_BREAK_SWFALLBACK,
    AC_ICON_DEBUG_EVENTSVIEW_BREAK_UNKNOWN,
    AC_ICON_DEBUG_EVENTSVIEW_EXCEPTION,
    AC_ICON_DEBUG_EVENTSVIEW_OUTPUT,
    AC_ICON_DEBUG_EVENTSVIEW_GDB,

    // Texture view:
    AC_ICON_DEBUG_TEXVW_TOOLBAR_ZOOMIN,
    AC_ICON_DEBUG_TEXVW_TOOLBAR_ZOOMOUT,
    AC_ICON_DEBUG_TEXVW_TOOLBAR_PAN,
    AC_ICON_DEBUG_TEXVW_TOOLBAR_MOUSE,
    AC_ICON_DEBUG_TEXVW_TOOLBAR_RED,
    AC_ICON_DEBUG_TEXVW_TOOLBAR_GREEN,
    AC_ICON_DEBUG_TEXVW_TOOLBAR_BLUE,
    AC_ICON_DEBUG_TEXVW_TOOLBAR_ALPHA,
    AC_ICON_DEBUG_TEXVW_TOOLBAR_GRAY,
    AC_ICON_DEBUG_TEXVW_TOOLBAR_INVERT,
    AC_ICON_DEBUG_TEXVW_TOOLBAR_ROTATEL,
    AC_ICON_DEBUG_TEXVW_TOOLBAR_ROTATER,
    AC_ICON_DEBUG_TEXVW_TOOLBAR_ORIGINAL,
    AC_ICON_DEBUG_TEXVW_TOOLBAR_BEST,

    // Statistics view:
    AC_ICON_DEBUG_STATISTICS_CLEAR,
    AC_ICON_DEBUG_STATISTICS_BATCH,

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    // Profiler
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    AC_ICON_PROFILE_MODE,

    // General profiling tree items:
    AC_ICON_PROFILE_APPTREE_SESSION_SINGLE,
    AC_ICON_PROFILE_APPTREE_SESSION_MULTI,
    AC_ICON_PROFILE_APPTREE_KERNEL_SINGLE,
    AC_ICON_PROFILE_APPTREE_KERNEL_MULTI,

    //////////////////////////////////////////////////////////////////////////
    // GPU profiling
    //////////////////////////////////////////////////////////////////////////
    // GPU profiling tree items:
    AC_ICON_PROFILE_GPU_APPTREE_COUNTERS_SINGLE,
    AC_ICON_PROFILE_GPU_APPTREE_COUNTERS_MULTI,
    AC_ICON_PROFILE_GPU_APPTREE_APPTRACE_SINGLE,
    AC_ICON_PROFILE_GPU_APPTREE_APPTRACE_MULTI,
    AC_ICON_PROFILE_GPU_APPTREE_SUMMARY,
    AC_ICON_PROFILE_GPU_APPTREE_CL_API,
    AC_ICON_PROFILE_GPU_APPTREE_HSA_API,
    AC_ICON_PROFILE_GPU_APPTREE_CONTEXT,
    AC_ICON_PROFILE_GPU_APPTREE_KERNEL,
    AC_ICON_PROFILE_GPU_APPTREE_TOP10_TRANSFER,
    AC_ICON_PROFILE_GPU_APPTREE_TOP10_KERNEL,
    AC_ICON_PROFILE_GPU_APPTREE_BESTPRACTICE,

    //////////////////////////////////////////////////////////////////////////
    // CPU profiling
    //////////////////////////////////////////////////////////////////////////
    // CPU profiling tree items:
    AC_ICON_PROFILE_CPU_APPTREE_TBS_SINGLE,
    AC_ICON_PROFILE_CPU_APPTREE_TBS_MULTI,
    AC_ICON_PROFILE_CPU_APPTREE_ASSESS_SINGLE,
    AC_ICON_PROFILE_CPU_APPTREE_ASSESS_MULTI,
    AC_ICON_PROFILE_CPU_APPTREE_IBS_SINGLE,
    AC_ICON_PROFILE_CPU_APPTREE_IBS_MULTI,
    AC_ICON_PROFILE_CPU_APPTREE_BRANCHING_SINGLE,
    AC_ICON_PROFILE_CPU_APPTREE_BRANCHING_MULTI,
    AC_ICON_PROFILE_CPU_APPTREE_DATAACCESS_SINGLE,
    AC_ICON_PROFILE_CPU_APPTREE_DATAACCESS_MULTI,
    AC_ICON_PROFILE_CPU_APPTREE_INSTACCESS_SINGLE,
    AC_ICON_PROFILE_CPU_APPTREE_INSTACCESS_MULTI,
    AC_ICON_PROFILE_CPU_APPTREE_L2CACHE_SINGLE,
    AC_ICON_PROFILE_CPU_APPTREE_L2CACHE_MULTI,
    AC_ICON_PROFILE_CPU_APPTREE_CUSTOM_SINGLE,
    AC_ICON_PROFILE_CPU_APPTREE_CUSTOM_MULTI,
    AC_ICON_PROFILE_CPU_APPTREE_CLU_SINGLE,
    AC_ICON_PROFILE_CPU_APPTREE_CLU_MULTI,
    AC_ICON_PROFILE_CPU_APPTREE_OVERVIEW,
    AC_ICON_PROFILE_CPU_APPTREE_MODULES,
    AC_ICON_PROFILE_CPU_APPTREE_CALLGRAPH,
    AC_ICON_PROFILE_CPU_APPTREE_FUNCTIONS,
    AC_ICON_PROFILE_CPU_APPTREE_SOURCE,
    AC_ICON_PROFILE_CPU_APPTREE_SOURCES,

    // CPU profiler modules view:
    AC_ICON_PROFILE_CPU_MOUDLE_USER_32,
    AC_ICON_PROFILE_CPU_MOUDLE_USER_64,
    AC_ICON_PROFILE_CPU_MOUDLE_SYSTEM_32,
    AC_ICON_PROFILE_CPU_MOUDLE_SYSTEM_64,

    //////////////////////////////////////////////////////////////////////////
    // Power profiling
    //////////////////////////////////////////////////////////////////////////
    // Power profiling tree items:
    AC_ICON_PROFILE_PWR_APPTREE_SESSION_SINGLE,
    AC_ICON_PROFILE_PWR_APPTREE_SESSION_MULTI,
    AC_ICON_PROFILE_PWR_APPTREE_TIMELINE,
    AC_ICON_PROFILE_PWR_APPTREE_SUMMARY,
    AC_ICON_PROFILE_PWR_APPTREE_NEW,

    // Power profiling counter types:
    AC_ICON_PROFILE_PWR_COUNTER_POWER,
    AC_ICON_PROFILE_PWR_COUNTER_FREQUENCY,
    AC_ICON_PROFILE_PWR_COUNTER_CURRENT,
    AC_ICON_PROFILE_PWR_COUNTER_VOLTAGE,
    AC_ICON_PROFILE_PWR_COUNTER_DVFS,
    AC_ICON_PROFILE_PWR_COUNTER_PID,
    AC_ICON_PROFILE_PWR_COUNTER_TEMPRATURE,
    AC_ICON_PROFILE_PWR_COUNTER_COUNT,

    //////////////////////////////////////////////////////////////////////////
    // Thread profiling
    //////////////////////////////////////////////////////////////////////////
    // Thread profiling tree items:
    AC_ICON_PROFILE_THR_APPTREE_SESSION_SINGLE,
    AC_ICON_PROFILE_THR_APPTREE_SESSION_MULTI,
    AC_ICON_PROFILE_THR_APPTREE_TIMELINE,
    AC_ICON_PROFILE_THR_APPTREE_OVERVIEW,

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    // Analyzer
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    AC_ICON_BUILD_AND_ANALYZE_MODE,

    // Build & Analyze app tree:
    AC_ICON_ANALYZE_APPTREE_KERNELS,
    AC_ICON_ANALYZE_APPTREE_FOLDER_VK,
    AC_ICON_ANALYZE_APPTREE_FOLDER_CL,
    AC_ICON_ANALYZE_APPTREE_FOLDER_GL,
    AC_ICON_ANALYZE_APPTREE_FOLDER_DX,
    AC_ICON_ANALYZE_APPTREE_FOLDER,
    AC_ICON_ANALYZE_APPTREE_KERNEL,
    AC_ICON_ANALYZE_APPTREE_OVERVIEW,
    AC_ICON_ANALYZE_APPTREE_ANALYSIS,
    AC_ICON_ANALYZE_APPTREE_STATISTICS,
    AC_ICON_ANALYZE_APPTREE_IL,
    AC_ICON_ANALYZE_APPTREE_ISA,
    AC_ICON_ANALYZE_APPTREE_IL_ISA,
    AC_ICON_ANALYZE_APPTREE_ADDNEW,
    AC_ICON_ANALYZE_APPTREE_OPEN,
    AC_ICON_ANALYZE_APPTREE_SOURCE,
    AC_ICON_ANALYZE_APPTREE_VERTEX,
    AC_ICON_ANALYZE_APPTREE_TESSCONT_HULL,
    AC_ICON_ANALYZE_APPTREE_TESSEVAL_DOMAIN,
    AC_ICON_ANALYZE_APPTREE_GEOMETRY,
    AC_ICON_ANALYZE_APPTREE_FRAGMENT_PIXEL,
    AC_ICON_ANALYZE_APPTREE_KERNEL_COMPUTE,

    // Analyze Statistics view:
    AC_ICON_ANALYZE_STATISTICS_MEETS,
    AC_ICON_ANALYZE_STATISTICS_DEGRADE,
    AC_ICON_ANALYZE_STATISTICS_LIGHTBULB,

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    // Frame Analysis
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    AC_ICON_FRAME_ANALYSIS_MODE,
    AC_ICON_FRAME_ANALYSIS_APPTREE_SINGLE,
    AC_ICON_FRAME_ANALYSIS_APPTREE_MULTI,
    AC_ICON_FRAME_ANALYSIS_APP_TREE_DASHBOARD,
    AC_ICON_FRAME_ANALYSIS_APP_TREE_FRAME,
    AC_ICON_FRAME_ANALYSIS_APP_TREE_OVERVIEW,
    AC_ICON_FRAME_ANALYSIS_APP_TREE_TIMELINE,
    AC_ICON_FRAME_ANALYSIS_APP_TREE_PERFORMANCE_PROFILE,
    AC_ICON_FRAME_ANALYSIS_APP_TREE_IMAGE,
    AC_ICON_FRAME_ANALYSIS_OPEN_TIMELINE,

//++IOR: LPGPU2 Code Coverage Analysis
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    // LPGPU2 project Code Coverage (CCVG)
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    AC_ICON_CCVG_MODE,
    AC_ICON_CCVG_APPTREEROOTNODE_SETFILESGOOD,
    AC_ICON_CCVG_APPTREEROOTNODE_SETFILESBAD,
    AC_ICON_CCVG_APPTREECCVGITEM,
    AC_ICON_CCVG_APPTREECCVGITEM_SOURCE,
    AC_ICON_CCVG_APPTREECCVGITEM_RESULTS,
    AC_ICON_CCVG_APPTREECCVGITEM_DRIVERS,
    AC_ICON_CCVG_APPTREECCVGITEM_EXECUTEABLE,
    AC_ICON_CCVG_APPTREECCVGITEM_BAD,
    AC_ICON_CCVG_APPTREECCVGITEM_SOURCE_BAD,
    AC_ICON_CCVG_APPTREECCVGITEM_RESULTS_BAD,
    AC_ICON_CCVG_APPTREECCVGITEM_DRIVERS_BAD,
    AC_ICON_CCVG_APPTREECCVGITEM_EXECUTEABLE_BAD,
//--IOR: LPGPU2 Code Coverage Analysis

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    // Total number:
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    AC_NUMBER_OF_ICON_IDS
};

AC_API int acIconSizeToPixelSize(acIconSize iconSize);
AC_API acIconSize acGetRecommendedIconSize();
AC_API acIconSize acGetScaledIconSize(acIconSize x1Size);
AC_API const QImage& acGetIcon(acIconId iconId, acIconSize iconSize = acGetRecommendedIconSize());
AC_API bool acSetIconInPixmap(QPixmap& pixmap, acIconId iconId, acIconSize iconSize = acGetRecommendedIconSize());

#endif  // __ACICONS

