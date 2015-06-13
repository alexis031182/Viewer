extern "C" {
    #include <libavdevice/avdevice.h>
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libswscale/swscale.h>
}

#include <QtCore/QElapsedTimer>

#include <QtGui/QImage>

#include "acapturethread.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
ACaptureThread::ACaptureThread(QObject *parent) : QThread(parent) {}


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

    AVInputFormat *av_inp_fmt = NULL;
    if(!grp_name.isEmpty()) {
        av_inp_fmt = av_find_input_format(grp_name.toLatin1());
        if(!av_inp_fmt) {
            QMetaObject::invokeMethod(this, "failed", Qt::QueuedConnection);

            return;
        }
    }

    AVFormatContext *av_fmt_ctx = NULL;
    if(avformat_open_input(&av_fmt_ctx, dev_name.toLatin1()
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

        img = img.convertToFormat(QImage::Format_ARGB32_Premultiplied);

        QMetaObject::invokeMethod(this, "captured"
            , Qt::QueuedConnection, Q_ARG(QImage,img));

        const qint64 elapsed = stream_timer.restart();
        if(av_vid_strm->avg_frame_rate.num > elapsed)
            QThread::msleep(av_vid_strm->avg_frame_rate.num - elapsed);
    }

    av_frame_free(&av_vid_frm);
    av_frame_free(&av_cap_frm);
    sws_freeContext(av_sws_ctx);
    avcodec_close(av_vid_strm->codec);
    avformat_close_input(&av_fmt_ctx);

    QMetaObject::invokeMethod(this, "captured"
        , Qt::QueuedConnection, Q_ARG(QImage,QImage()));
}
