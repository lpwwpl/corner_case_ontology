#ifndef ROBOTGLOBALDATA_H
#define ROBOTGLOBALDATA_H


#include <Mod/Robot/App/RobotObject.h>
#include <Mod/Robot/App/TrajectoryObject.h>
#include <Mod/Robot/App/ToolDataObject.h>
#include <Mod/Robot/App/FrameOfObject.h>
#include <Mod/Robot/App/RobotFolder.h>
#include <QMap>
#include "PreCompiled.h"
#include <boost/signals2.hpp>
#include <App/Application.h>
#include <App/Document.h>
#include <QTimer>
#include <QMutex>
namespace RobotGui {

    struct RobotGuiExport SFolderInfo
    {
        QString m_folderLabel;
        QString m_curFrameName;
        QString m_curToolName;
        QString m_curTrajName;
        QMap<QString, QString> m_frames;
        QMap<QString, QString> m_tools;
        QMap<QString, QString> m_trajs;

        bool operator==(SFolderInfo& t)
        {
            bool ret = false;
            ret = (m_folderLabel == t.m_folderLabel
                && m_curFrameName == t.m_curFrameName
                && m_curToolName == t.m_curToolName
                && m_curTrajName == t.m_curTrajName
                && m_frames == t.m_frames
                && m_tools == t.m_tools
                && m_trajs == t.m_trajs
                );
        }
    };
    struct RobotGuiExport SDocInfo
    {
        //QString m_doc;
        QString m_curFolderName;
        QMap<QString, SFolderInfo> m_folders;
        bool operator==(SDocInfo& t)
        {
            bool ret = false;
            ret = (m_curFolderName == t.m_curFolderName
               && m_folders == t.m_folders
                );
        }
    };
    
    class RobotGuiExport SDocCache
    {
    public:


    };
    class RobotGuiExport RobotCurrentInfoDll:public QObject
    {
        Q_OBJECT
    public Q_SLOTS:
        void slotTimeout();

    public:
        //objectname,nameindoc ;,text()
        QMap<QString, SDocInfo> m_data;
        QString m_curDocName;     
        QTimer* m_timer;
        QMutex m_lock;

        QMap<QString,QString> GetFolders()
        {
            QMap<QString, QString> ret;
           
            auto& app = App::GetApplication();
            App::Document* d = app.getActiveDocument();   

            QString docName = QString::fromLatin1(d->getName());
            if (!m_data.contains(docName))
                return ret;
            QMap<QString, SFolderInfo> temp = m_data[m_curDocName].m_folders;
            for (QString key : m_data[m_curDocName].m_folders.keys())
            {
                App::DocumentObject* folder = d->getObject(key.toLocal8Bit().constData());
                QString txt = QString::fromLatin1(folder->Label.getValue());
                ret[key] = txt;
            }
          
            return ret;
        }

        QMap<QString, QString> GetFrames()
        {
            QMap<QString, QString> ret;

            auto& app = App::GetApplication();
            App::Document* d = app.getActiveDocument();
            QString docName = QString::fromLatin1(d->getName());
            if (!m_data.contains(docName))
                return ret;
            if (m_data[m_curDocName].m_curFolderName == QString::fromLatin1(""))return ret;

            QMap<QString, QString> temp = m_data[m_curDocName].m_folders[m_data[m_curDocName].m_curFolderName].m_frames;
            for (QString key : m_data[m_curDocName].m_folders[m_data[m_curDocName].m_curFolderName].m_frames.keys())
            {
                App::DocumentObject* frame = d->getObject(key.toLocal8Bit().constData());
                QString txt = QString::fromLatin1(frame->Label.getValue());
                ret[key] = txt;
            }

            return ret;
        }
        QMap<QString, QString> GetTools()
        {
            QMap<QString, QString> ret;

            auto& app = App::GetApplication();
            App::Document* d = app.getActiveDocument();
            QString docName = QString::fromLatin1(d->getName());
            if (!m_data.contains(docName))
                return ret;
            if (m_data[m_curDocName].m_curFolderName == QString::fromLatin1(""))return ret;
            for (QString key : m_data[m_curDocName].m_folders[m_data[m_curDocName].m_curFolderName].m_tools.keys())
            {
                App::DocumentObject* tool = d->getObject(key.toLocal8Bit().constData());
                QString txt = QString::fromLatin1(tool->Label.getValue());
                ret[key] = txt;
            }

            return ret;
        }
        QMap<QString, QString> GetTrajs()
        {
            QMap<QString, QString> ret;

            auto& app = App::GetApplication();
            App::Document* d = app.getActiveDocument();
            QString docName = QString::fromLatin1(d->getName());
            if (!m_data.contains(docName))
                return ret;
            if (m_data[m_curDocName].m_curFolderName == QString::fromLatin1(""))return ret;
            for (QString key : m_data[m_curDocName].m_folders[m_data[m_curDocName].m_curFolderName].m_trajs.keys())
            {
                App::DocumentObject* traj = d->getObject(key.toLocal8Bit().constData());
                QString txt = QString::fromLatin1(traj->Label.getValue());
                ret[key] = txt;
            }

            return ret;
        }


        static RobotCurrentInfoDll& instance(void);
        RobotCurrentInfoDll();
        ~RobotCurrentInfoDll();
        App::DocumentObject*  GetRobotObject(const App::DocumentObject& Obj);
        const Robot::RobotFolder* GetLinkRobotFolder(const App::DocumentObject& Obj);
        std::vector<App::DocumentObject*> GetLinkChildren(const App::DocumentObject& Obj, Base::Type bype);
        App::DocumentObject* GetLinkChild(const App::DocumentObject& Obj, QString name);
        App::DocumentObject* GetLinkChild(const App::DocumentObject& Obj,  Base::Type bype);
        App::DocumentObject* GetLinkParent(const App::DocumentObject& Obj, Base::Type bype);
        void slotRemoveObject(const App::DocumentObject& obj);
        void slotChangeObject(const App::DocumentObject& obj, const App::Property& propertyIn);
        void slotActivatedObject(const App::DocumentObject& Obj);
        void slotActivateDocument(const App::Document&);
        void slotDeleteDocument(const App::Document&);
        void slotAddObject(const App::DocumentObject& obj);
        void slotNewDocument(const App::Document&,bool);


        void SetCurFolder(QString folderName)
        {
            if(folderName == QString::fromLatin1(""))return;
            m_lock.lock();
            m_data[m_curDocName].m_curFolderName = folderName;     
            m_lock.unlock();
            signaRefreshRobot();
            signaRefreshFrame();
            signaRefreshTool();
            signaRefreshTraj();
        }

        void SetCurFrame(QString name)
        {
            SDocInfo& docInfo = m_data[m_curDocName];
            QString curFolder = docInfo.m_curFolderName;
            m_lock.lock();
            docInfo.m_folders[curFolder].m_curFrameName = name;
            m_lock.unlock();
            signaRefreshFrame();
        }

        void SetCurTool(QString name)
        {
            SDocInfo& docInfo = m_data[m_curDocName];
            QString curFolder = docInfo.m_curFolderName;
            m_lock.lock();
            docInfo.m_folders[curFolder].m_curToolName = name;
            m_lock.unlock();
            signaRefreshTool();
        }

        void SetCurTraj(QString name)
        {
            SDocInfo& docInfo = m_data[m_curDocName];
            QString curFolder = docInfo.m_curFolderName;
            m_lock.lock();
            docInfo.m_folders[curFolder].m_curTrajName = name;
            m_lock.unlock();
            signaRefreshTraj();
        }

        boost::signals2::signal<void(const App::Document&)> removeUnActiveDocumentHighlight;

        //update combo
        boost::signals2::signal<void()> signaRefreshRobot;
        boost::signals2::signal<void()> signaRefreshFrame;
        boost::signals2::signal<void()> signaRefreshTool;
        boost::signals2::signal<void()> signaRefreshTraj;

        //////update combo
        boost::signals2::signal<void()> signaEmptyRobot;
        boost::signals2::signal<void()> signaEmptyFrame;
        boost::signals2::signal<void()> signaEmptyTool;
        boost::signals2::signal<void()> signaEmptyTraj;

        ////////update combo
        boost::signals2::signal<void(QString)> signalRemoveRobot; 
        boost::signals2::signal<void(QString)> signalRemoveFrame;
        boost::signals2::signal<void(QString)> signalRemoveTool;
        boost::signals2::signal<void(QString)> signalRemoveTraj;
        ////////update combo
        boost::signals2::signal<void(QString)> signalAddRobot;
        boost::signals2::signal<void(QString)> signalAddFrame;
        boost::signals2::signal<void(QString)> signalAddTool;
        boost::signals2::signal<void(QString)> signalAddTraj;


        //////update tree
        boost::signals2::signal<void(QString)> signalActivateRobot;
        boost::signals2::signal<void(QString)> signalActivateFrame;
        boost::signals2::signal<void(QString)> signalActivateTool;
        boost::signals2::signal<void(QString)> signalActivateTraj;

        ////////update combo
        boost::signals2::signal<void(QString)> signalActivatedRobot;
        boost::signals2::signal<void(QString)> signalActivatedFrame;
        boost::signals2::signal<void(QString)> signalActivatedTool;
        boost::signals2::signal<void(QString)> signalActivatedTraj;
    };    
}
#endif 