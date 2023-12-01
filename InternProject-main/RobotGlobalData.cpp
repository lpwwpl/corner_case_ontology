#include "RobotGlobalData.h"
#include <App/DocumentObject.h>
#include <App/Application.h>
#include <App/Document.h>

using namespace RobotGui;
namespace bp = boost::placeholders;

RobotCurrentInfoDll::RobotCurrentInfoDll()
{
    m_timer = new QTimer();
    m_timer->setInterval(500);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(slotTimeout()));
    m_timer->setSingleShot(false);
    m_timer->start();
    ////App::GetApplication().signalNewObject.connect(boost::bind(&RobotCurrentInfoDll::slotAddObject, this, bp::_1));
    //App::GetApplication().signalDeletedObject.connect(boost::bind(&RobotCurrentInfoDll::slotRemoveObject, this, bp::_1));
    //App::GetApplication().signalChangedObject.connect(boost::bind(&RobotCurrentInfoDll::slotChangeObject, this, bp::_1, bp::_2));
    //App::GetApplication().signalActivatedObject.connect(boost::bind(&RobotCurrentInfoDll::slotActivatedObject, this, bp::_1));
    //App::GetApplication().signalActiveDocument.connect(boost::bind(&RobotCurrentInfoDll::slotActivateDocument, this, bp::_1));
    //App::GetApplication().signalDeleteDocument.connect(boost::bind(&RobotCurrentInfoDll::slotDeleteDocument, this, bp::_1));
    //App::GetApplication().signalNewDocument.connect(boost::bind(&RobotCurrentInfoDll::slotNewDocument, this, bp::_1,bp::_2));
}

RobotCurrentInfoDll::~RobotCurrentInfoDll()
{

}
RobotCurrentInfoDll& RobotCurrentInfoDll::instance(void)
{
    static RobotCurrentInfoDll singleton;

    return singleton;
}

const Robot::RobotFolder* RobotCurrentInfoDll::GetLinkRobotFolder(const App::DocumentObject& Obj)
{
    const Robot::RobotFolder* ret = NULL;
    if (Obj.getTypeId() == Robot::RobotFolder::getClassTypeId())
    {
        ret = dynamic_cast<const Robot::RobotFolder*>(&Obj);
        return ret;
    }

    std::vector<App::DocumentObject*> parents = Obj.getInListRecursive();
    int count = parents.size();
    for (int i = 0; i < count; i++)
    {
        App::DocumentObject* elem = parents.at(i);

        if (elem->getTypeId() == Robot::RobotFolder::getClassTypeId())
        {
            ret = dynamic_cast<Robot::RobotFolder*>(elem);
            break;
        }
    }
    return ret;
}
App::DocumentObject* RobotCurrentInfoDll::GetRobotObject(const App::DocumentObject& Obj)
{
    App::DocumentObject* ret = NULL;
    const Robot::RobotFolder*  folder = GetLinkRobotFolder(Obj);
    std::vector<App::DocumentObject* > robots = folder->getObjectsOfType(Robot::RobotObject::getClassTypeId());
    if (robots.size() > 0)
        ret = robots.at(0);
    return ret;
}
App::DocumentObject* RobotCurrentInfoDll::GetLinkParent(const App::DocumentObject& Obj, Base::Type bype)
{
    App::DocumentObject * ret = NULL;
    std::vector<App::DocumentObject*> parents = Obj.getInListRecursive();
    int count = parents.size();
    for (int i = 0; i < count; i++)
    {
        App::DocumentObject* elem = parents.at(i);

        if (elem->getTypeId() == bype)
        {
            ret = elem;
            break;
        }
    }
    return ret;
}

std::vector<App::DocumentObject*> RobotCurrentInfoDll::GetLinkChildren(const App::DocumentObject& Obj,Base::Type bype)
{
    std::vector<App::DocumentObject*> ret;
    std::vector<App::DocumentObject*> children = Obj.getOutListRecursive();
    int count = children.size();
    for (int i = 0; i < count; i++)
    {
        App::DocumentObject* elem = children.at(i);

        if (elem->getTypeId() == bype)
        {
            ret.push_back(elem);
        }
    }
    return ret;
}
App::DocumentObject* RobotCurrentInfoDll::GetLinkChild(const App::DocumentObject& Obj, Base::Type bype)
{
    App::DocumentObject* ret = NULL;
    std::vector<App::DocumentObject*> children = Obj.getOutListRecursive();
    int count = children.size();
    for (int i = 0; i < count; i++)
    {
        App::DocumentObject* elem = children.at(i);

        if (elem->getClassTypeId() == bype)
        {
            ret = elem;
            break;
        }
    }
    return ret;
}
App::DocumentObject* RobotCurrentInfoDll::GetLinkChild(const App::DocumentObject& Obj, QString name)
{
    App::DocumentObject* ret = NULL;
    std::vector<App::DocumentObject*> children = Obj.getOutListRecursive();
    int count = children.size();
    for (int i = 0; i < count; i++)
    {
        App::DocumentObject* elem = children.at(i);

        if (QString::fromLatin1(elem->getNameInDocument()) == name)
        {
            ret = elem;
            break;
        }
    }
    return ret;
}

void RobotCurrentInfoDll::slotRemoveObject(const App::DocumentObject& obj)
{
    Base::Type type = obj.getTypeId();
    QString name = QString::fromLatin1(obj.getNameInDocument());
    if (type == Robot::RobotFolder::getClassTypeId())
    {
        QMap<QString, SFolderInfo>& folders = m_data[m_curDocName].m_folders;
        folders.remove(name);

        if (m_data[m_curDocName].m_curFolderName == name && folders.size() > 0)
        {
            m_data[m_curDocName].m_curFolderName = folders.firstKey();
        }
        signaRefreshRobot();
        //m_cur.m_folders.removeAll(&obj);
        //signalRemoveRobot(obj);
    }
    else if (type == Robot::FrameOfObject::getClassTypeId())
    {
        SFolderInfo& folder = m_data[m_curDocName].m_folders[m_data[m_curDocName].m_curFolderName];
        QMap<QString, QString>& frames = folder.m_frames;
        frames.remove(name);

        if (folder.m_curFrameName == name && frames.size()>0)
        {
            folder.m_curFrameName = frames.firstKey();
        }
        signaRefreshFrame();
        //if (GetLinkParent(obj, Robot::RobotFolder::getClassTypeId()) == m_cur.m_folder)
        //{
        //    m_cur.m_frames.removeAll(&obj);
        //    signalRemoveFrame(obj);
        //}
    }
    else if (type == Robot::ToolDataObject::getClassTypeId())
    {
        SFolderInfo& folder = m_data[m_curDocName].m_folders[m_data[m_curDocName].m_curFolderName];
        QMap<QString, QString>& tools = folder.m_tools;
        tools.remove(name);

        if (folder.m_curToolName == name && tools.size() > 0)
        {
            folder.m_curToolName = tools.firstKey();
        }

        signaRefreshTool();
        //if (GetLinkParent(obj, Robot::RobotFolder::getClassTypeId()) == m_cur.m_folder)
        //{
        //    m_cur.m_tools.removeAll(&obj);
        //    signalRemoveTool(obj);
        //}
    }
    else if (type == Robot::TrajectoryObject::getClassTypeId())
    {
        SFolderInfo& folder = m_data[m_curDocName].m_folders[m_data[m_curDocName].m_curFolderName];
        QMap<QString, QString>& trajs = folder.m_trajs;
        trajs.remove(name);

        if (folder.m_curTrajName == name && trajs.size() > 0)
        {
            folder.m_curTrajName = trajs.firstKey();
        }

        signaRefreshTraj();
        //if (GetLinkParent(obj, Robot::RobotFolder::getClassTypeId()) == m_cur.m_folder)
        //{
        //    m_cur.m_trajs.removeAll(&obj);
        //    signalRemoveTraj(obj);
        //}
    }
}


void RobotCurrentInfoDll::slotActivatedObject(const App::DocumentObject& Obj)
{
    auto& app = App::GetApplication();
    App::Document* d = app.getActiveDocument();
    QString curDocName = QString::fromLatin1(d->getName());
    if (!m_data.contains(curDocName))
    {
        SDocInfo sinfo;
        m_data[curDocName] = sinfo;
        m_curDocName = curDocName;
    }

    Base::Type type = Obj.getTypeId();
    QString curName = QString::fromLatin1(Obj.getNameInDocument());
    QString curTxt = QString::fromLatin1(Obj.Label.getValue());

    if (type == Robot::RobotFolder::getClassTypeId())
    {
        m_data[m_curDocName].m_curFolderName = curName;

        if (!m_data[m_curDocName].m_folders.contains(curName))
        {
            SFolderInfo sinfo;
            sinfo.m_folderLabel = curTxt;
            m_data[m_curDocName].m_folders[curName] = sinfo;
        }
       

        signaRefreshRobot();
        //m_cur.m_folder = &Obj;

        //signalActivateRobot(Obj);
        //signalActivatedRobot(Obj);

        ////for
        //m_cur.m_frames.clear();
        //m_cur.m_tools.clear();
        //m_cur.m_trajs.clear();
        signaRefreshFrame();
        signaRefreshTool();
        signaRefreshTraj();
        //if (m_cur.m_frame)
        //{
        //    signalActivateFrame(*m_cur.m_frame);
        //    signalActivatedFrame(*m_cur.m_frame);
        //}

        //if(m_cur.m_tool)
        //{
        //    signalActivateTool(*m_cur.m_tool);
        //    signalActivatedTool(*m_cur.m_tool);
        //}
    
        //if (m_cur.m_traj)
        //{
        //    signalActivateTraj(*m_cur.m_traj);
        //    signalActivatedTraj(*m_cur.m_traj);
        //}

    }
    else if (type == Robot::FrameOfObject::getClassTypeId())
    {
        if (m_data[m_curDocName].m_curFolderName == QString::fromLatin1(""))
            return;
        if (!m_data[m_curDocName].m_folders[m_data[m_curDocName].m_curFolderName].m_frames.contains(curName))
        {   
            return;
            //m_data[m_curDocName].m_folders[m_data[m_curDocName].m_curFolderName].m_frames[curName] = curTxt;
        }
        m_data[m_curDocName].m_folders[m_data[m_curDocName].m_curFolderName].m_curFrameName = curName;
        signaRefreshFrame();
        //signalActivateFrame(curName);
        //signalActivatedFrame(curName);
       /* m_cur.m_frame = &Obj;
        signalActivateFrame(Obj);
        signalActivatedFrame(Obj);*/
    }
    else if (type == Robot::ToolDataObject::getClassTypeId())
    {
        if (m_data[m_curDocName].m_curFolderName == QString::fromLatin1(""))
            return;
        if (!m_data[m_curDocName].m_folders[m_data[m_curDocName].m_curFolderName].m_tools.contains(curName))
        {
            return;
            //m_data[m_curDocName].m_folders[m_data[m_curDocName].m_curFolderName].m_tools[curName] = curTxt;
        }
        m_data[m_curDocName].m_folders[m_data[m_curDocName].m_curFolderName].m_curToolName = curName;
        signaRefreshTool();
        //signalActivateTool(curName);
        //signalActivatedTool(curName);
       /* m_cur.m_tool = &Obj;
        signalActivateTool(Obj);
        signalActivatedTool(Obj);*/
    }
    else if (type == Robot::TrajectoryObject::getClassTypeId())
    {
        if (m_data[m_curDocName].m_curFolderName == QString::fromLatin1(""))
            return;
        if (!m_data[m_curDocName].m_folders[m_data[m_curDocName].m_curFolderName].m_trajs.contains(curName))
        {
            return;
            //m_data[m_curDocName].m_folders[m_data[m_curDocName].m_curFolderName].m_trajs[curName] = curTxt;
        }
        m_data[m_curDocName].m_folders[m_data[m_curDocName].m_curFolderName].m_curTrajName = curName;
        signaRefreshTraj();
        //m_cur.m_traj = &Obj;
        //signalActivateTraj(Obj);
        //signalActivatedTraj(Obj);
    }
}

void RobotCurrentInfoDll::slotActivateDocument(const App::Document& d)
{


    QString docName = d.getName();
    m_curDocName = docName;
    if (!m_data.contains(m_curDocName))
    {
        SDocInfo sinfo;
        m_data[docName] = sinfo;
    }
    //if (m_cur.isValid())
    //{
    //    removeUnActiveDocumentHighlight(*(m_cur.m_doc));
    //}
    //QString docName = d.getName();
    //m_cur = m_data[docName];// .m_docName = docName;

    ////
    signaRefreshRobot();
    signaRefreshFrame();
    signaRefreshTool();
    signaRefreshTraj();
}
void RobotCurrentInfoDll::slotDeleteDocument(const App::Document& d)
{
    QString docName = d.getName();
    m_data.remove(docName);
    signaEmptyRobot();
    signaEmptyFrame();
    signaEmptyTool();
    signaEmptyTraj();
}

void RobotCurrentInfoDll::slotChangeObject(const App::DocumentObject& obj, const App::Property& propertyIn)
{
    std::string name("Empty Name");
    if (propertyIn.hasName())
        name = propertyIn.getName();
    assert(!name.empty());
    if (std::string("Label") == name)
    {
        App::DocumentObject* parentFolder = RobotGui::RobotCurrentInfoDll::instance().GetLinkParent(obj, Robot::RobotFolder::getClassTypeId());
        if (!parentFolder)return;
        QString folderName = QString::fromLatin1(parentFolder->getNameInDocument());

        const App::PropertyString& property = dynamic_cast<const App::PropertyString&>(propertyIn);
        QString newLabel = property.getValue();

        Base::Type type = obj.getTypeId();
        QString key = QString::fromLatin1(obj.getNameInDocument());
        if (type == Robot::ToolDataObject::getClassTypeId())
        {           
            m_data[m_curDocName].m_folders[folderName].m_tools[key] = newLabel;
            signaEmptyTool();
        }
        else if (type == Robot::FrameOfObject::getClassTypeId())
        {
            m_data[m_curDocName].m_folders[folderName].m_frames[key] = newLabel;
            signaRefreshFrame();
        }
        else if (type == Robot::RobotFolder::getClassTypeId())
        {
            m_data[m_curDocName].m_folders[folderName].m_folderLabel = newLabel;
            signaRefreshRobot();
        }
        else if (type == Robot::TrajectoryObject::getClassTypeId())
        {
            m_data[m_curDocName].m_folders[folderName].m_trajs[key] = newLabel;
            signaRefreshTraj();
        }
    }
    //    std::string name("Empty Name");
    //    if (propertyIn.hasName())
    //        name = propertyIn.getName();
    //    assert(!name.empty());
    //    if (std::string("Label") == name)
    //    {
    //        const App::PropertyString& property = dynamic_cast<const App::PropertyString&>(propertyIn);
    //        QList<QAction*> robots = _group->actions();

    //        //QStringList enable_wbs;
    //        for (QList<QAction*>::Iterator it = robots.begin(); it != robots.end(); ++it)
    //        {
    //            QString objName = (*it)->objectName();
    //            QString docName = QString::fromLatin1(obj.getNameInDocument());
    //            if (objName == docName)
    //            {
    //                QString w2 = QString::fromLatin1(property.getValue());

    //                QString wb = objName;
    //                QPixmap px = QPixmap(QLatin1String(":icons/Robot_CreateTarget.svg"));
    //                //QString tip = Application::Instance->workbenchToolTip(wb);
    //                (*it)->setObjectName(QString());
    //                (*it)->setIcon(QIcon());
    //                (*it)->setText(QString());
    //                (*it)->setToolTip(QString());
    //                (*it)->setStatusTip(QString());
    //                (*it)->setVisible(false); // do this at last
    //                //propertyIn.getValue();
    //                (*it)->setObjectName(wb);
    //                (*it)->setIcon(px);
    //                (*it)->setText(w2);
    //                (*it)->setToolTip(QString());
    //                (*it)->setStatusTip(tr("Select the '%1'").arg(wb));
    //                (*it)->setVisible(true); // do this at last
    //                (*it)->setCheckable(true);
    //                break;
    //            }
    //        }
    //    }
    //}
}
void RobotCurrentInfoDll::slotNewDocument(const App::Document& d,bool bFlag)
{
    QString docName = d.getName();
    SDocInfo sinfo;
    m_data[docName] = sinfo;    
    m_curDocName = docName;
    signaRefreshRobot();
    signaRefreshFrame();
    signaRefreshTool();
    signaRefreshTraj();
}

void  RobotCurrentInfoDll::slotAddObject(const App::DocumentObject& obj)
{
    auto& app = App::GetApplication();
    App::Document* d = app.getActiveDocument();    
    QString curDocName = QString::fromLatin1(d->getName());
    Base::Type type = obj.getTypeId();

    if (!m_data.contains(curDocName))
    {
        SDocInfo sinfo;
        m_data[curDocName] = sinfo;
        m_curDocName = curDocName;
    }

    //QString name = QString::fromLatin1(obj.getNameInDocument());
    if (type == Robot::RobotFolder::getClassTypeId())
    {
        QString curFolderName = QString::fromLatin1(obj.getNameInDocument());
        QString curFolderLabel = QString::fromLatin1(obj.Label.getValue());

        SFolderInfo sinfo;
        sinfo.m_folderLabel = curFolderLabel;
        m_data[m_curDocName].m_folders[curFolderName] = sinfo;
        //m_curFolderName = curFolderName;

        slotActivatedObject(obj);  

        //signaRefreshRobot();
        //signalAddRobot(curFolderName);
    }
    else if (type == Robot::FrameOfObject::getClassTypeId())
    {
        if (m_data[m_curDocName].m_curFolderName == QString::fromLatin1(""))
            return;
        QString curFrameName = QString::fromLatin1(obj.getNameInDocument());
        QString curFrameLabel = QString::fromLatin1(obj.Label.getValue());
        App::DocumentObject* folder = GetLinkParent(obj, Robot::RobotFolder::getClassTypeId());

        /// /////////////////////////////////////////////////////////无法确定哪个folder增加frame
        m_data[m_curDocName].m_folders[m_data[m_curDocName].m_curFolderName].m_frames[curFrameName] = curFrameLabel;
        //m_curFrameName = curFrameName;
        slotActivatedObject(obj);

        //signaRefreshFrame();
        //signalAddFrame(curFrameName);
    }
    else if (type == Robot::ToolDataObject::getClassTypeId())
    {
        if (m_data[m_curDocName].m_curFolderName == QString::fromLatin1(""))
            return;
        QString curToolName = QString::fromLatin1(obj.getNameInDocument());
        QString curToolLabel = QString::fromLatin1(obj.Label.getValue());
        /// /////////////////////////////////////////////////////////无法确定哪个folder增加frame
        m_data[m_curDocName].m_folders[m_data[m_curDocName].m_curFolderName].m_tools[curToolName] = curToolLabel;
        //m_curToolName = curToolName;
        slotActivatedObject(obj);

        //signaRefreshTool();
        //signalAddTool(curToolName);
    }
    else if (type == Robot::TrajectoryObject::getClassTypeId())
    {
        if (m_data[m_curDocName].m_curFolderName == QString::fromLatin1(""))
            return;
        QString curTrajName = QString::fromLatin1(obj.getNameInDocument());
        QString curTrajLabel = QString::fromLatin1(obj.Label.getValue());
        /// /////////////////////////////////////////////////////////无法确定哪个folder增加frame
        m_data[m_curDocName].m_folders[m_data[m_curDocName].m_curFolderName].m_trajs[curTrajName] = curTrajLabel;
        //m_curTrajName = curTrajName;
        slotActivatedObject(obj);

        //signaRefreshTraj();
        //signalAddTraj(curTrajName);
    }
}
#include "moc_RobotGlobalData.cpp"