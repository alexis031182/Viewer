extern "C" {
    #include <libavdevice/avdevice.h>
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libswscale/swscale.h>
}

#include <QtCore/QElapsedTimer>
#include <QtCore/QFileInfo>
#include <QtCore/QTimer>

#include <QtGui/QImageReader>
#include <QtGui/QImage>

#include "acapturethread.h"
#include "afilterinterface.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
ACaptureThread::ACaptureThread(QObject *parent) : QThread(parent)
    , _filter(NULL), _vid_strm_cap_avg_fps(0), _vid_strm_prw_avg_fps(0) {}


// ========================================================================== //
// Get filter.
// ========================================================================== //
AFilterInterface *ACaptureThread::filter() {
    QMutexLocker locker(&_mutex); return _filter;
}


// ========================================================================== //
// Set filter.
// ========================================================================== //
void ACaptureThread::setFilter(AFilterInterface *filter) {
    QMutexLocker locker(&_mutex); _filter = filter;
}


// ========================================================================== //
// Unset filter.
// ========================================================================== //
void ACaptureThread::unsetFilter() {setFilter(NULL);}


// ========================================================================== //
// Get capture fps.
// ========================================================================== //
double ACaptureThread::captureFps() {
    QMutexLocker locker(&_mutex); return _vid_strm_cap_avg_fps;
}


// ========================================================================== //
// Get preview fps.
// ========================================================================== //
double ACaptureThread::previewFps() {
    QMutexLocker locker(&_mutex); return _vid_strm_prw_avg_fps;
}


// ========================================================================== //
// Get device name.
// ========================================================================== //
QString ACaptureThread::deviceName() {
    QMutexLocker locker(&_mutex); return _dev_name;
}


// ========================================================================== //
// Get device group name.
// ========================================================================== //
QString ACaptureThread::groupName() {
    QMutexLocker locker(&_mutex); return _grp_name;
}


// ========================================================================== //
// Set device name.
// ========================================================================== //
void ACaptureThread::setDeviceName(const QString &name) {
    QMutexLocker locker(&_mutex); _dev_name = name;
}


// ========================================================================== //
// Set device group name.
// ========================================================================== //
void ACaptureThread::setGroupName(const QString &name) {
    QMutexLocker locker(&_mutex); _grp_name = name;
}


// ========================================================================== //
// Run.
// ========================================================================== //
void ACaptureThread::run() {
    _mutex.lock();
    const QString grp_name = _grp_name;
    const QString dev_name = _dev_name;
    _mutex.unlock();

    if(grp_name.isEmpty() && dev_name.isEmpty()) {
        QMetaObject::invokeMethod(this, "failed", Qt::QueuedConnection);

        return;
    }

    if(!dev_name.isEmpty()) {
        const QString suffix = QFileInfo(dev_name).suffix();

        QListIterator<QByteArray> itr(QImageReader::supportedImageFormats());
        while(itr.hasNext()) {
            if(suffix == itr.next()) {
                QImage img(dev_name);
                if(img.isNull()) continue;

                img = img.convertToFormat(QImage::Format_RGB888);

                QTimer timer;
                timer.setSingleShot(false);
                timer.setInterval(250);

                connect(&timer, &QTimer::timeout, [this,img]() {
                    QImage cap_img
                        = img.convertToFormat(
                            QImage::Format_ARGB32_Premultiplied);

                    QMetaObject::invokeMethod(this, "captured"
                        , Qt::QueuedConnection, Q_ARG(QImage,cap_img));

                    QImage flt_img = img.copy();
                    filter(flt_img);
                });

                timer.start(); exec();

                QMetaObject::invokeMethod(this, "captured"
                    , Qt::QueuedConnection, Q_ARG(QImage,QImage()));
                QMetaObject::invokeMethod(this, "filtered"
                    , Qt::QueuedConnection, Q_ARG(QImage,QImage()));

                return;
            }
        }
    }

    AVInputFormat *av_inp_fmt = NULL;
    if(!grp_name.isEmpty()) {
        av_inp_fmt = av_find_input_format(grp_name.toUtf8());
        if(!av_inp_fmt) {
            QMetaObject::invokeMethod(this, "failed", Qt::QueuedConnection);

            return;
        }
    }

    AVFormatContext *av_fmt_ctx = NULL;
    if(avformat_open_input(&av_fmt_ctx, dev_name.toUtf8()
        , av_inp_fmt, NULL) < 0) {

        QMetaObject::invokeMethod(this, "failed", Qt::QueuedConnection);

        return;
    }

    if(avformat_find_stream_info(av_fmt_ctx, NULL) < 0) {
        avformat_close_input(&av_fmt_ctx);

        QMetaObject::invokeMethod(this, "failed", Qt::QueuedConnection);

        return;
    }

    int vid_strm_idx = -1;
    for(int i = 0, n = av_fmt_ctx->nb_streams; i < n; ++i) {
        if(av_fmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            vid_strm_idx = i; break;
        }
    }

    if(vid_strm_idx == -1) {
        avformat_close_input(&av_fmt_ctx);

        QMetaObject::invokeMethod(this, "failed", Qt::QueuedConnection);

        return;
    }

    AVStream *av_vid_strm = av_fmt_ctx->streams[vid_strm_idx];

    AVCodec *av_dec = avcodec_find_decoder(av_vid_strm->codec->codec_id);
    if(!av_dec) {
        avformat_close_input(&av_fmt_ctx);

        QMetaObject::invokeMethod(this, "failed", Qt::QueuedConnection);

        return;
    }

    if(avcodec_open2(av_vid_strm->codec, av_dec, NULL) < 0) {
        avformat_close_input(&av_fmt_ctx);

        QMetaObject::invokeMethod(this, "failed", Qt::QueuedConnection);

        return;
    }

    AVCodecContext *av_dec_ctx = av_vid_strm->codec;

    SwsContext *av_sws_ctx
        = sws_getCachedContext(NULL
            , av_dec_ctx->width, av_dec_ctx->height, av_dec_ctx->pix_fmt
            , av_dec_ctx->width, av_dec_ctx->height, AV_PIX_FMT_RGB24
            , SWS_FAST_BILINEAR, NULL, NULL, NULL);

    if(!av_sws_ctx) {
        sws_freeContext(av_sws_ctx);
        avcodec_close(av_vid_strm->codec);
        avformat_close_input(&av_fmt_ctx);

        QMetaObject::invokeMethod(this, "failed", Qt::QueuedConnection);

        return;
    }

    AVFrame *av_cap_frm = av_frame_alloc();
    av_cap_frm->format = AV_PIX_FMT_RGB24;
    av_cap_frm->width  = av_dec_ctx->width;
    av_cap_frm->height = av_dec_ctx->height;
    if(av_frame_get_buffer(av_cap_frm, 32) < 0) {
        av_frame_free(&av_cap_frm);
        sws_freeContext(av_sws_ctx);
        avcodec_close(av_vid_strm->codec);
        avformat_close_input(&av_fmt_ctx);

        QMetaObject::invokeMethod(this, "failed", Qt::QueuedConnection);

        return;
    }

    AVFrame *av_vid_frm = av_frame_alloc();

    const AVRational &av_cap_frm_rate = av_vid_strm->avg_frame_rate;
    setCaptureFps(double(av_cap_frm_rate.num) / double(av_cap_frm_rate.den));
    const int vid_strm_avg_dur = 1000 / int(_vid_strm_cap_avg_fps);

    QElapsedTimer stream_timer;
    stream_timer.start();

    AVPacket av_pkt;
    while(!isInterruptionRequested()) {
        if(av_read_frame(av_fmt_ctx, &av_pkt) < 0) break;

        if(av_pkt.stream_index != vid_strm_idx) {
            av_free_packet(&av_pkt); continue;
        }

        av_packet_rescale_ts(&av_pkt, av_vid_strm->time_base
            , av_dec_ctx->time_base);

        int pkt_rdy = -1;
        if(avcodec_decode_video2(av_dec_ctx, av_vid_frm
            , &pkt_rdy, &av_pkt) < 0) {

            av_free_packet(&av_pkt);

            QMetaObject::invokeMethod(this, "failed", Qt::QueuedConnection);

            break;
        }

        if(pkt_rdy == 0) {av_free_packet(&av_pkt); continue;}

        sws_scale(av_sws_ctx
            , av_vid_frm->data, av_vid_frm->linesize, 0, av_vid_frm->height
            , av_cap_frm->data, av_cap_frm->linesize);

        av_free_packet(&av_pkt);

        const int &w = av_cap_frm->width;
        const int &h = av_cap_frm->height;

        QImage img(w, h, QImage::Format_RGB888);
        for(int row = 0, bpl = w*3; row < av_cap_frm->height; ++row) {
            memcpy(img.scanLine(row)
                , av_cap_frm->data[0] + row*av_cap_frm->linesize[0], bpl);
        }

        QImage cap_img
            = img.convertToFormat(QImage::Format_ARGB32_Premultiplied);

        QMetaObject::invokeMethod(this, "captured"
            , Qt::QueuedConnection, Q_ARG(QImage,cap_img));

        filter(img);

        int cur_dur = stream_timer.elapsed();
        if(vid_strm_avg_dur > cur_dur)
            QThread::msleep(vid_strm_avg_dur - cur_dur);

        cur_dur = stream_timer.restart();
        setPreviewFps(1000. / double(std::max(1,cur_dur)));
    }

    av_frame_free(&av_vid_frm);
    av_frame_free(&av_cap_frm);
    sws_freeContext(av_sws_ctx);
    avcodec_close(av_vid_strm->codec);
    avformat_close_input(&av_fmt_ctx);

    setCaptureFps(0);

    QMetaObject::invokeMethod(this, "captured"
        , Qt::QueuedConnection, Q_ARG(QImage,QImage()));
    QMetaObject::invokeMethod(this, "filtered"
        , Qt::QueuedConnection, Q_ARG(QImage,QImage()));
}


// ========================================================================== //
// Filter.
// ========================================================================== //
void ACaptureThread::filter(QImage &img) {
    QMutexLocker locker(&_mutex);
    if(!_filter) return;

    cv::Mat src_mat(img.height(), img.width(), CV_8UC3, img.bits()
        , img.bytesPerLine());

    cv::Mat mat = src_mat.clone();
    _filter->run(mat);

    img = QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
    img = img.convertToFormat(QImage::Format_ARGB32_Premultiplied);

    QMetaObject::invokeMethod(this, "filtered"
        , Qt::QueuedConnection, Q_ARG(QImage,img));
}


// ========================================================================== //
// Set capture fps.
// ========================================================================== //
void ACaptureThread::setCaptureFps(double fps) {
    _mutex.lock();
    const bool stilled = (_vid_strm_cap_avg_fps == fps) ? true : false;
    _vid_strm_cap_avg_fps = fps;
    _mutex.unlock();

    if(!stilled) {
        QMetaObject::invokeMethod(this, "captureFpsChanged"
            , Qt::QueuedConnection, Q_ARG(double,fps));
    }
}


// ========================================================================== //
// Set preview fps.
// ========================================================================== //
void ACaptureThread::setPreviewFps(double fps) {
    _mutex.lock();
    const bool stilled = (_vid_strm_prw_avg_fps == fps) ? true : false;
    _vid_strm_prw_avg_fps = fps;
    _mutex.unlock();

    if(!stilled) {
        QMetaObject::invokeMethod(this, "previewFpsChanged"
            , Qt::QueuedConnection, Q_ARG(double,fps));
    }
}
