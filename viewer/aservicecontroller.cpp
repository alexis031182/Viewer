extern "C" {
    #include <libavdevice/avdevice.h>
    #include <libavformat/avformat.h>
}

#include <QtCore/QCoreApplication>
#include <QtCore/QGlobalStatic>
#include <QtCore/QPluginLoader>
#include <QtCore/QDirIterator>
#include <QtCore/QJsonObject>
#include <QtCore/QDebug>

#include <QtGui/QStandardItemModel>

#include "aservicecontroller.h"
#include "adevicecontroller.h"
#include "afilterinterface.h"

Q_GLOBAL_STATIC(AServiceController, _g_service_ctrl)

// ========================================================================== //
// Get instance.
// ========================================================================== //
AServiceController *AServiceController::instance() {return _g_service_ctrl;}


// ========================================================================== //
// Constructor.
// ========================================================================== //
AServiceController::AServiceController(QObject *parent)
    : QObject(parent), _devices_obj(new QObject(this)) {

    av_register_all();
    avdevice_register_all();
    avformat_network_init();

    connect(qApp, &QCoreApplication::aboutToQuit, [this]() {
        delete _devices_obj;
    });

    connect(_devices_obj, &QObject::destroyed, [this]() {
        QListIterator<ADeviceController*> itr(devices());
        while(itr.hasNext()) itr.next()->stop();
    });

    createVideoDeviceModel();
    createVideoFilterModel();
}


// ========================================================================== //
// Register device.
// ========================================================================== //
void AServiceController::registerDevice(ADeviceController *ctrl) {
    if(ctrl) {
        ctrl->setParent(_devices_obj);
        emit deviceRegistered(ctrl);
    }
}


// ========================================================================== //
// Get devices.
// ========================================================================== //
QList<ADeviceController*> AServiceController::devices() const {
    return _devices_obj->findChildren<ADeviceController*>();
}


// ========================================================================== //
// Get video device model.
// ========================================================================== //
QAbstractItemModel *AServiceController::videoDeviceModel() const {
    return _vid_dev_model;
}


// ========================================================================== //
// Get video filter model.
// ========================================================================== //
QAbstractItemModel *AServiceController::videoFilterModel() const {
    return _vid_flt_model;
}


// ========================================================================== //
// Update video device model.
// ========================================================================== //
void AServiceController::updateVideoDeviceModel() {createVideoDeviceModel();}


// ========================================================================== //
// Update video filter model.
// ========================================================================== //
void AServiceController::updateVideoFilterModel() {createVideoFilterModel();}


// ========================================================================== //
// Create video device model.
// ========================================================================== //
void AServiceController::createVideoDeviceModel() {
    if(_vid_dev_model) _vid_dev_model->clear();
    else _vid_dev_model = new QStandardItemModel(this);

    _vid_dev_model->setHorizontalHeaderLabels(QStringList()
        << AServiceController::tr("Name")
        << AServiceController::tr("Description"));

    AVInputFormat *av_inp_fmt = NULL;
    while((av_inp_fmt = av_input_video_device_next(av_inp_fmt))) {
        QStandardItem *grp_name_item
            = new QStandardItem(QLatin1String(av_inp_fmt->name));

        QStandardItem *grp_desc_item
            = new QStandardItem(QLatin1String(av_inp_fmt->long_name));

        AVDeviceInfoList *av_dev_nfo_lst = NULL;
        if(avdevice_list_input_sources(av_inp_fmt, NULL, NULL
            , &av_dev_nfo_lst) >= 0) {

            for(int i = 0; i < av_dev_nfo_lst->nb_devices; ++i) {
                AVDeviceInfo *av_dev_nfo = av_dev_nfo_lst->devices[i];

                QStandardItem *dev_name_item
                    = new QStandardItem(QLatin1String(av_dev_nfo
                        ->device_name));

                QStandardItem *dev_desc_item
                    = new QStandardItem(QLatin1String(av_dev_nfo
                        ->device_description));

                grp_name_item->appendRow(QList<QStandardItem*>()
                    << dev_name_item << dev_desc_item);
            }
        }

        _vid_dev_model->appendRow(QList<QStandardItem*>()
            << grp_name_item << grp_desc_item);
    }
}


// ========================================================================== //
// Create video filter model.
// ========================================================================== //
void AServiceController::createVideoFilterModel() {
    if(_vid_flt_model) _vid_flt_model->clear();
    else _vid_flt_model = new QStandardItemModel(this);

    _vid_flt_model->setHorizontalHeaderLabels(QStringList()
        << AServiceController::tr("Name")
        << AServiceController::tr("File"));

    const QString fpath
        = qApp->applicationDirPath() + QLatin1String("/filters");

    if(QDir().exists(fpath)) {
        QDirIterator itr(fpath, QStringList()
            , QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);

        while(itr.hasNext()) {
            const QString fname = itr.next();
            QPluginLoader loader(fname);

            QJsonObject obj = loader.metaData();
            if(obj.isEmpty()) continue;

            if(obj.contains(QStringLiteral("IID"))
                && obj.value(QStringLiteral("IID")).toString()
                    == QLatin1String(AFilterInterface_iid)) {

                QJsonObject metadata
                    = (obj.contains(QStringLiteral("MetaData")))
                        ? obj.value(QStringLiteral("MetaData")).toObject()
                        : QJsonObject();

                if(metadata.isEmpty()) continue;

                const QString category
                    = metadata.value(QStringLiteral("category")).toString();
                const QString group
                    = metadata.value(QStringLiteral("group")).toString();
                const QString name
                    = metadata.value(QStringLiteral("name")).toString();

                if(category == QLatin1String("Video")
                    && !(group.isEmpty() && name.isEmpty())) {

                    AFilterInterface *filter
                        = qobject_cast<AFilterInterface*>(loader.instance());

                    if(!filter) continue;

                    QString dsp_name = filter->displayGroup();
                    if(dsp_name.isEmpty()) dsp_name = group;

                    const QModelIndexList indexes
                        = _vid_flt_model->match(_vid_flt_model->index(0,0)
                            , Qt::UserRole+1, group, 1, Qt::MatchFixedString);

                    QStandardItem *group_item = NULL;
                    if(indexes.isEmpty()) {
                        group_item = new QStandardItem(dsp_name);
                        group_item->setData(group, Qt::UserRole+1);

                        _vid_flt_model->appendRow(QList<QStandardItem*>()
                            << group_item);

                    } else {
                        group_item
                            = _vid_flt_model->itemFromIndex(indexes.first());

                        QStringList names
                            = group_item->text().split(QStringLiteral(" / ")
                                , QString::SkipEmptyParts);

                        if(!names.contains(dsp_name)) {
                            names.append(dsp_name);
                            group_item->setText(names.join(
                                QStringLiteral(" / ")));
                        }
                    }

                    dsp_name = filter->displayName();
                    if(dsp_name.isEmpty()) dsp_name = group;

                    QStandardItem *name_item = new QStandardItem(dsp_name);
                    name_item->setData(name, Qt::UserRole+1);

                    QStandardItem *file_item = new QStandardItem(fname);

                    group_item->appendRow(QList<QStandardItem*>()
                        << name_item << file_item);

                    loader.unload();
                }
            }
        }
    }
}
