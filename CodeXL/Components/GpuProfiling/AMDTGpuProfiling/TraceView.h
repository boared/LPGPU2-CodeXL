//=====================================================================
// Copyright (c) 2012 Advanced Micro Devices, Inc. All rights reserved.
//
/// \author GPU Developer Tools
/// \file $File: //devtools/main/CodeXL/Components/GpuProfiling/AMDTGpuProfiling/TraceView.h $
/// \version $Revision: #51 $
/// \brief  This file contains TraceView class
//
//=====================================================================
// $Id: //devtools/main/CodeXL/Components/GpuProfiling/AMDTGpuProfiling/TraceView.h#51 $
// Last checkin:   $DateTime: 2016/04/18 06:02:03 $
// Last edited by: $Author: salgrana $
// Change list:    $Change: 569613 $
//=====================================================================

#ifndef _TRACEVIEW_H_
#define _TRACEVIEW_H_

#include <qtIgnoreCompilerWarnings.h>


#include <QString>
#include <QMap>
#include <QList>
#include <QSplitter>
#include <QTabWidget>
#include <QAbstractTableModel>

// Backend header files
#include "IParserListener.h"
#include "IParserProgressMonitor.h"
#include "CLAPIInfo.h"
#include "CLAtpFile.h"
#include "SYCLAPIInfo.h"
#include "SYCLAtpFile.h"
#include "StackTraceAtpFile.h"
#include "PerfMarkerAtpFile.h"
#include "HSAAtpFile.h"
#include "AnalyzerHTMLUtils.h"

// AMDTSharedProfiling:
#include <AMDTSharedProfiling/inc/SharedSessionWindow.h>
#include <AMDTSharedProfiling/inc/SharedSessionViewTabWidget.h>

// Local:
#include <AMDTGpuProfiling/Session.h>
#include <AMDTGpuProfiling/SummaryView.h>
#include <AMDTGpuProfiling/FindToolBarView.h>
#include <AMDTGpuProfiling/TraceTable.h>
#include <AMDTGpuProfiling/ProjectSettings.h>
#include <AMDTGpuProfiling/gpBaseSessionView.h>

// forward declaration
class acTimeline;
class acTimelineBranch;
class acTimelineItem;


//forward declarations
class KernelOccupancyWindow;
class SymbolInfo;
class SessionViewTabWidget;

#ifdef SHOW_KERNEL_LAUNCH_AND_COMPLETION_LATENCY
    class CLAPITimelineItem;
#endif

/// UI for Application Trace GPUSessionTreeItemData view
class TraceView : public gpBaseSessionView, public IParserListener<CLAPIInfo>, public IParserListener<HSAAPIInfo>, public IParserListener<SYCLAPIInfo>, public IParserListener<SymbolFileEntry>, IParserListener<PerfMarkerEntry>, IParserProgressMonitor
{
    Q_OBJECT

public:

    /// Initializes a new instance of the TraceView class.
    /// \param parent the parent widget
    TraceView(QWidget* parent);

    /// Destructor
    ~TraceView();

    /// Builds the layout for the window
    void BuildWindowLayout();

    /// Display the session file. This function should be implemented for session views with multiple children
    /// \param sessionFilePath the file path for the requested session
    /// \param sessionInnerPage the item type describing the inner view to open, or AF_TREE_ITEM_ITEM_NONE when the root is supposed to open
    /// \param [out] errorMessage when the display fails, errorMessage should contain a message for the user
    virtual bool DisplaySession(const osFilePath& sessionFilePath, afTreeItemType sessionInnerPage, QString& errorMessage);

    /// Queries the window to see if it's current session is the specified session
    /// \param pSession the session to check for
    /// \return true if the specified session is the view's current session, false otherwise
    bool IsCurrentSession(GPUSessionTreeItemData* pSession) const { return pSession == m_pCurrentSession; };

    /// Sets the current parsed ATP file amount of API functions:
    /// \param threadId the thread id for which the api calls number is set
    /// \param apiNum the number of api calls expected in the parsed atp file
    void SetAPINum(osThreadId threadId, unsigned int apiNum);
    
//++CF:LPGPU2
    /// Sets the current parsed ATP depth of API functions for nested events.
    /// \param threadId the thread id for which the api depth is set
    /// \param apiNum the maximum depth of calls reached by the given thread
    void SetAPIDepth(osThreadId threadId, std::map<int, unsigned int>) override;
    
    /// Sets the current parsed ATP depth of API functions for nested events.
    /// \param queueId the queue id for which the api lane depth is set
    /// \param mapCatDepth map of event categories to their max depth for given queue.
    void SetAPIQueueDepth(unsigned int queueId, std::map<int, unsigned int>) override;
//--CF:LPGPU2

    /// IParserListener implementation for CL api trace/timeline data in .atp file.
    /// This method is called once for each CL api in the .atp file
    /// \param pAPIInfo the current CLAPIInfo item from the parser
    /// \param[out] stopParsing flag indicating if parsing should stop after this item
    void OnParse(CLAPIInfo* pAPIInfo, bool& stopParsing);

    /// IParserListener implementation for CL api trace/timeline data in .atp file.
    /// This method is called once for each CL api in the .atp file
    /// \param pAPIInfo the current CLAPIInfo item from the parser
    /// \param[out] stopParsing flag indicating if parsing should stop after this item
    void OnParse(SYCLAPIInfo* pAPIInfo, bool& stopParsing);

    /// IParserListener implementation for HSA api trace/timeline data in .atp file.
    /// This method is called once for each HSA api in the .atp file
    /// \param pAPIInfo the current CLAPIInfo item from the parser
    /// \param[out] stopParsing flag indicating if parsing should stop after this item
    void OnParse(HSAAPIInfo* pAPIInfo, bool& stopParsing);

    /// IParserListener implementation for symbol data in .atp file
    /// This method is called once for each entry in the symbol section of the .atp file
    /// \param pSymFileEntry the current SymbolFileEntry item from the parser
    /// \param[out] stopParsing flag indicating if parsing should stop after this item
    void OnParse(SymbolFileEntry* pSymFileEntry, bool& stopParsing);

    /// IParserListener implementation for perf marker data in .atp file
    /// This method is called once for each entry in the perf marker section of the .atp file
    /// \param pPerfMarkerEntry the current PerfMarkerEntry item from the parser
    /// \param[out] stopParsing flag indicating if parsing should stop after this item
    void OnParse(PerfMarkerEntry* pPerfMarkerEntry, bool& stopParsing);

    /// IParserProgressMonitor implementation for reporting progress when loading trace data
    /// \param strProgressMessage the progress message to display for this progress event
    /// \param uiCurItem the index of the current item being parsed
    /// \param uiTotalItems the total number of items to be parsed
    void OnParserProgress(const std::string& strProgressMessage, unsigned int uiCurItem, unsigned int uiTotalItems);

    /// Display the requested summary type
    /// \param summaryType - The requested summary to display on the summary page tab
    void DisplaySummaryPageType(int selectedIndex);

    /// Update the session folder after rename
    /// \param oldSessionFileName the session original file path
    /// \param newSessionDirectory the session original file path after rename
    virtual void UpdateRenamedSession(const osFilePath& oldSessionFileName, const osFilePath& newSessionFileName);


    virtual void onUpdateEdit_Copy(bool& isEnabled) {isEnabled = true;}
    virtual void onUpdateEdit_SelectAll(bool& isEnabled) {isEnabled = true;};

    bool TimelinePropertiesAreSet() const {return m_areTimelinePropertiesSet;}
public slots:

    /// Copies any selected text to the clipboard:
    virtual void OnEditCopy();

    /// Select all:
    virtual void OnEditSelectAll();


private slots:
    /// handler for a mouse clicked in the trace table
    /// \param modelIndex the model index of the cell that was clicked
    void TraceTableMouseClickedHandler(const QModelIndex& modelIndex);

    /// handler for a mouse clicked in the trace table
    /// \param modelIndex the model index of the cell that was clicked
    void TraceTableMouseDoubleClickedHandler(const QModelIndex& modelIndex);

    /// handler for a mouse enters a cell
    /// \param modelIndex the model index of the cell that was entered
    void TraceTableMouseEnteredHandler(const QModelIndex& modelIndex);

    /// handler for when a timeline item is clicked
    /// \param item the timeline item clicked
    void TimelineItemClickedHandler(acTimelineItem* item);

    /// handler for when a timeline branch is clicked
    /// \param branch the timeline branch clicked
    void TimelineBranchClickedHandler(acTimelineBranch* branch);

    /// handler for when a link on the Summary pages is clicked
    /// \param strPagename the title of the summary page containing the link clicked on (the title shown in the combo box)
    /// \param threadID the thread id of the item to show
    /// \param callIndex the call index of the item to show
    /// \param viewType flag indicating whether the link should be shown in the timeline or the trace view
    void SummaryPageLinkClickedHandler(const QString& strPageName, unsigned int threadID, unsigned int callIndex, AnalyzerHTMLViewType viewType);

    /// handler for when context menu is shown for trace table
    /// \param pt the point at which context menu is requested
    void TraceTableContextMenuHandler(const QPoint& pt);

    /// handler for when the Goto source menu item is selected
    void OnGotoSource();

    /// handler for when the zoom item in timeline is selected
    void OnZoomItemInTimeline();

    /// handler for when the collapse all action is selected
    void OnCollapseAll();

    /// handler for when the expand all action is selected
    void OnExpandAll();

    /// Handler for export to CSV context menu:
    void OnExportToCSV();

    /// Application tree selection signal:
    void OnApplicationTreeSelection() {m_areTimelinePropertiesSet = false;};

private:
    /// Typedefs:
    using MapCatIDToDepth_t = std::map<int, unsigned int>;
    
    /// struct that holds the queue, data transfer and kernel execution branches
    struct OCLQueueBranchInfo
    {
        acTimelineBranch* m_pQueueBranch;          ///< the queue branch
        acTimelineBranch* m_pKernelBranch;         ///< the kernel execution branch
        acTimelineBranch* m_pMemoryBranch;         ///< the data transfer branch
        acTimelineBranch* m_pFillOperationBranch;  ///< the fill operations branch
        OCLQueueBranchInfo() : m_pQueueBranch(nullptr), m_pKernelBranch(nullptr), m_pMemoryBranch(nullptr), m_pFillOperationBranch(nullptr)
        {

        }
    };

	/// struct that holds the queue, data transfer and kernel execution branches
	struct SYCLQueueBranchInfo
	{
		acTimelineBranch* m_pQueueBranch;          ///< the queue branch
		acTimelineBranch* m_pKernelBranch;         ///< the kernel execution branch
		acTimelineBranch* m_pMemoryBranch;         ///< the data transfer branch
		acTimelineBranch* m_pCommandGroupBranch;  ///< the command group branch
    acTimelineBranch* m_pCommandGroupDetailBranch;   /// the command group detail event branch
		SYCLQueueBranchInfo() : m_pQueueBranch(nullptr), m_pKernelBranch(nullptr), m_pMemoryBranch(nullptr), m_pCommandGroupBranch(nullptr), m_pCommandGroupDetailBranch(nullptr)
		{

		}
	};

    /// Gets the host timeline branch for the give thread and API
    /// \param threadId the thread id whose host timeline branch is needed
    /// \param strAPIName the API whose host timeline branch is needed
    /// \return the timeline branch for the given thread and API
    acTimelineBranch* GetHostBranchForAPI(osThreadId threadId, const QString& strAPIName);

    /// Loads the Trace GPUSessionTreeItemData from the specified .atp file
    /// \param the .atp file to load into the session view
    /// \return true if successful, false otherwise
    bool LoadSessionUsingBackendParser(const osFilePath& sessionFile);

    /// Handle the specified CLAPIInfo instance supplied by the .atp file parser. Adds an item to the timeline and api trace list
    /// \param pApiInfo the CLAPIInfo instance to add to the trace/timeline
    void HandleCLAPIInfo(CLAPIInfo* pApiInfo);

    /// Handle the specified SYCLAPIInfo instance supplied by the .atp file parser. Adds an item to the timeline and api trace list
    /// \param pApiInfo the SYCLAPIInfo instance to add to the trace/timeline
    void HandleSYCLAPIInfo(SYCLAPIInfo* pApiInfo);

    /// Initializes or creates branch info for the requested queue:
    /// \param contextId the id of the context for which the branch is created
    /// \param queueId the id for the queue for which the branch is created
    /// \strContextHandle the string for the context
    /// \deviceNameStr the device name
    /// \return the branch info for the queue
    OCLQueueBranchInfo* GetBranchInfo(unsigned int contextId, unsigned int queueId, const QString& strContextHandle, const QString& deviceNameStr, const QString& strQueueHandle);


	/// Initializes or creates branch info for the requested queue:
	/// \param queueId the id for the queue for which the branch is created
	SYCLQueueBranchInfo* GetSYCLBranchInfo(uint64_t queueId);

    /// Handle the specified HSAAPIInfo instance supplied by the .atp file parser. Adds an item to the timeline and api trace list
    /// \param pApiInfo the HSAAPIInfo instance to add to the trace/timeline
    void HandleHSAAPIInfo(HSAAPIInfo* pApiInfo);

    /// Handle the specified SymbolFileEntry instance supplied by the .atp file parser. Stores the symbol info for use when navigating to source
    /// \param pSymFileEntry the SymbolFileEntry instance begin added
    void HandleSymFileEntry(SymbolFileEntry* pSymFileEntry);

    /// Handle the specified PerfMarkerEntry instance supplied by the .atp file parser. Adds the perf markers to the timeline
    /// \param pPerfMarkerEntry the CLPerfMarkerEntry instance begin added
    void HandlePerfMarkerEntry(PerfMarkerEntry* pPerfMarkerEntry);

    /// After parsing is done, populate the view with the data collected by the parser
    void DoneParsingATPFile();

    /// Loads the summary pages for the specified session
    void LoadSummary(afTreeItemType summaryType);

    /// Adds a Trace Table tab with the specified model and tab caption
    /// \param model the model for the trace table
    /// \param threadId the thread id for this trace table
    void AddTraceTable(TraceTableModel* model, unsigned int threadId);

    /// Clears the timeline and trace table
    void Clear();

    /// Helper to get the symbol info for the API specified by threadId and callIndex
    /// \param threadId the thread Id of the API
    /// \param callIndex the call index of the API
    /// \return the symbol info for the API specified by threadId and callIndex or NULL if not found
    SymbolInfo* GetSymbolInfo(int threadId, int callIndex);

    /// Helper function indicating whether or not symbol information is available for the current session
    /// \return true if symbol information is available for the current session, false otherwise
    bool SessionHasSymbolInformation();

    /// Get or create a perf marker sub branch
    /// \param name Name of the branch
    /// \param pParent parent branch
    /// \return sub branch
    acTimelineBranch* GetPerfMarkerSubBranchHelper(const QString& name, acTimelineBranch* pParent);

    /// Gets the tab index of the tab with the specified text
    /// \param strTabText the text of the tab whose index is needed
    /// \return the index of the tab, or -1 if the tab is not found
    int GetTabIndex(const QString& strTabText);

    /// Display a timeline item in the properties view
    /// \param pItem the item to display
    void DisplayItemInPropertiesView(acTimelineItem* pItem);

    /// Checks whether the parser should be stopped due to too many api calls
    /// \return true if the parser should stop, false otherwise
    bool CheckStopParsing();

    TraceSession*                            m_pCurrentSession;        ///< the current session
    QSplitter*                               m_pMainSplitter;           ///< the splitter for the main view
    acTimeline*                              m_pTimeline;               ///< Timeline control
    QTabWidget*                              m_pTraceTabView;           ///< Tab widget for table and summary view
    SummaryView*                             m_pSummaryView;            ///< SummaryView control

    //    FindToolBarView*                         m_findToolBar;            ///< Find toolbar
    QMenu*                                   m_pTraceTableContextMenu;  ///< Context menu for trace table
    QAction*                                 m_pGotoSourceAction;       ///< Goto Source menu item action
    QAction*                                 m_pZoomInTimelineAction;   ///< Zoom item in timeline
    QAction*                                 m_pExpandAllAction;        ///< Expand all items in table
    QAction*                                 m_pCollapseAllAction;      ///< Collapse all items in table
    QAction*                                 m_pCopyAction;             ///< Copy action
    QAction*                                 m_pSelectAllAction;        ///< Select all action
    QAction*                                 m_pExportToCSVAction;      ///< Export to csv action
    QHBoxLayout*                             m_pMainLayout;             ///< Main layout

    SymbolInfo*                              m_pSymbolInfo;             ///< symbol info cached when context menu is shown
    CLSummarizer*                            m_pSummarizer;             ///< OpenCL Trace Summarizer

    // members used by new parser code
    QMap<osThreadId, acTimelineBranch*>      m_hostBranchMap;           ///< map from thread id to the host timeline branch for that thread
    QMap<osThreadId, TraceTableModel*>       m_modelMap;                ///< a map from host thread label to model map
    QMap<uint, QList<SymbolInfo*> >          m_symbolTableMap;          ///< stack trace table map. From thread ID to symbol list
    acTimelineBranch*                        m_pOpenCLBranch;            ///< the main OpenCL branch in the timeline
    acTimelineBranch*                        m_pSYCLBranch;               ///< the main HSA branch in the timeline
    acTimelineBranch*                        m_pHSABranch;               ///< the main HSA branch in the timeline
    QMap<unsigned int, acTimelineBranch*>    m_oclCtxMap;               ///< map from OCL context id to the branch for that context
    QMap<unsigned int, OCLQueueBranchInfo*>  m_oclQueueMap;             ///< map from OCL queue id to the QueueBranchInfo for that queue
    QMap<uint64_t, SYCLQueueBranchInfo*>     m_syclQueueToQBIMap;       /// < map from SYCL queue id to SYCLQueueBranchInfo for that queue
    QMap<QString, acTimelineBranch*>         m_hsaQueueMap;             ///< map from HSA queue handle string to the branch for that queue
    QMap<osThreadId, int>                    m_oclThreadOccIndexMap;    ///< map from thread id to the current occupancy index for that thread (for OCL)
    QMap<osThreadId, int>                    m_hsaThreadOccIndexMap;    ///< map from thread id to the current occupancy index for that thread (for HSA)
    QStack<unsigned long long>               m_timestampStack;          ///< stack of timestamps - used when handling perf markers
    QStack<QString>                          m_titleStack;              ///< stack of branch titles - used when handling perf markers
    QStack<acTimelineBranch*>                m_branchStack;             ///< stack of branches - used when handling perf markers
    bool                                     m_perfMarkersAdded;        ///< flag indicating whether or not perf markers have been added from the .atp file
    QString                                  m_strOccupancyKernelName;  ///< name of the kernel for which the occupancy is showing
    QMap<osThreadId, MapCatIDToDepth_t>      m_syclThreadDepthMap;      ///< map from thread id to the maximum api nesting depth for that thread (SYCL)
    QMap<osThreadId, MapCatIDToDepth_t>      m_syclQueueDepthMap;      ///< map from queue id to the maximum api nesting depths for that queue (SYCL)
    QMap<osThreadId, MapCatIDToDepth_t>      m_syclThreadCumulativeDepthMap;      ///< map from thread id to the maximum api nesting depth for that thread (SYCL)


#ifdef SHOW_KERNEL_LAUNCH_AND_COMPLETION_LATENCY
    CLAPITimelineItem*                       m_lastDeviceItem;          ///< reference to the most recent item on the device
    int                                      m_lastDeviceItemIdx;       ///< sequence id of the most recent item on the device
#endif

    bool m_areTimelinePropertiesSet;

    unsigned int m_parseCallsCounter;                                   /// <Is used to count the amount of parse calls, so that we will update the UI only every 1000 calls

    bool m_alreadyDisplayedAPILimitMessage;                             /// <Flag indicating whether or not we've already displayed the api limit message for this session
    bool m_shouldStopParsing;                                           /// <Flag indicating whether or not we should stop parsing. (We parse until we exceed a pre-defined limit, and then ask the user if we should continue):

    APIToTrace                              m_api;                      /// < API type for the currently loaded session
    bool m_isProgressRangeSet;                                          /// < true iff the progress range from the backend parser is already set
    std::string                             m_currentProgressMessage;   /// Store the current message presneted in the progress bar and dialog
};

#endif // _TRACEVIEW_H_
