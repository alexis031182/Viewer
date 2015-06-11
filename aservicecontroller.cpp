extern "C" {
    #include <libavdevice/avdevice.h>
    #include <libavformat/avformat.h>
}

#include <QtCore/QGlobalStatic>

#include <QtGui/QStandardItemModel>

#include "aservicecontroller.h"

Q_GLOBAL_STATIC(AServiceController, _g_service_ctrl)

// ========================================================================== //
// Get instance.
// ========================================================================== //
AServiceController *AServiceController::instance() {return _g_service_ctrl;}


// ========================================================================== //
// Constructor.
// ========================================================================== //
AServiceController::AServiceController(QObject *parent) : QObject(parent) {
    av_register_all();
    avdevice_register_all();
    avformat_network_init();

    createVideoDeviceModel();
}


// ========================================================================== //
// Get video device model.
// ========================================================================== //
QAbstractItemModel *AServiceController::videoDeviceModel() const {
    return _vid_dev_model;
}


// ========================================================================== //
// Update video device model.
// ========================================================================== //
void AServiceController::updateVideoDeviceModel() {createVideoDeviceModel();}


// ========================================================================== //
// Create video device model.
// ========================================================================== //
void AServiceController::createVideoDeviceModel() {
    if(_vid_dev_model) _vid_dev_model->clear();
    else _vid_dev_model = new QStandardItemModel(this);

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
                    << dev_desc_item << dev_name_item);
            }
        }

        _vid_dev_model->appendRow(QList<QStandardItem*>()
            << grp_name_item << grp_desc_item);
    }
}
