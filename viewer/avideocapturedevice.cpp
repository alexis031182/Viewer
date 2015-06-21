extern "C" {
    #include <libavdevice/avdevice.h>
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libswscale/swscale.h>
}

#include <QtCore/QTimerEvent>

#include <QtConcurrent/QtConcurrent>

#include <QtGui/QImage>

#include "avideocapturedevice.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
AVideoCaptureDevice::AVideoCaptureDevice(QObject *parent)
    : ACaptureDevice(parent), _timer_id(0)
    , _av_fmt_ctx(NULL), _av_sws_ctx(NULL)
    , _av_cap_frm(NULL), _av_vid_frm(NULL)
    , _vid_strm_idx(-1) {}


// ========================================================================== //
// Get is running.
// ========================================================================== //
bool AVideoCaptureDevice::isRunning() const {
    return (_timer_id == 0) ? false : true;
}


// ========================================================================== //
// Get device name.
// ========================================================================== //
QString AVideoCaptureDevice::deviceName() const {return _dev_name;}


// ========================================================================== //
// Set device name.
// ========================================================================== //
void AVideoCaptureDevice::setDeviceName(const QString &name) {_dev_name = name;}


// ========================================================================== //
// Get device group name.
// ========================================================================== //
QString AVideoCaptureDevice::groupName() const {return _grp_name;}


// ========================================================================== //
// Set device group name.
// ========================================================================== //
void AVideoCaptureDevice::setGroupName(const QString &name) {_grp_name = name;}


// ========================================================================== //
// Start.
// ========================================================================== //
void AVideoCaptureDevice::start() {
    if(isRunning()) return;

    emit starting();

    if(_grp_name.isEmpty() && _dev_name.isEmpty()) {
        emit failed(); return;
    }

    AVInputFormat *av_inp_fmt = NULL;
    if(!_grp_name.isEmpty()) {
        av_inp_fmt = av_find_input_format(_grp_name.toUtf8());
        if(!av_inp_fmt) {emit failed(); return;}
    }

    if(avformat_open_input(&_av_fmt_ctx, _dev_name.toUtf8()
        , av_inp_fmt, NULL) < 0) {clean(); emit failed(); return;}

    if(avformat_find_stream_info(_av_fmt_ctx, NULL) < 0) {
        clean(); emit failed(); return;
    }

    for(int i = 0, n = _av_fmt_ctx->nb_streams; i < n; ++i) {
        if(_av_fmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            _vid_strm_idx = i; break;
        }
    }

    if(_vid_strm_idx == -1) {clean(); emit failed(); return;}

    AVStream *av_vid_strm = _av_fmt_ctx->streams[_vid_strm_idx];

    AVCodec *av_dec = avcodec_find_decoder(av_vid_strm->codec->codec_id);
    if(!av_dec) {clean(); emit failed(); return;}

    if(avcodec_open2(av_vid_strm->codec, av_dec, NULL) < 0) {
        clean(); emit failed(); return;
    }

    AVCodecContext *av_dec_ctx = av_vid_strm->codec;

    _av_sws_ctx
        = sws_getCachedContext(NULL
            , av_dec_ctx->width, av_dec_ctx->height, av_dec_ctx->pix_fmt
            , av_dec_ctx->width, av_dec_ctx->height, AV_PIX_FMT_RGB24
            , SWS_FAST_BILINEAR, NULL, NULL, NULL);

    if(!_av_sws_ctx) {clean(); emit failed(); return;}

    _av_cap_frm = av_frame_alloc();
    _av_cap_frm->format = AV_PIX_FMT_RGB24;
    _av_cap_frm->width  = av_dec_ctx->width;
    _av_cap_frm->height = av_dec_ctx->height;
    if(av_frame_get_buffer(_av_cap_frm, 32) < 0) {
        clean(); emit failed(); return;
    }

    _av_vid_frm = av_frame_alloc();

    const AVRational &av_cap_frm_rate = av_vid_strm->avg_frame_rate;

    const double frm_rate = av_cap_frm_rate.num / av_cap_frm_rate.den;
    emit captureFpsChanged(frm_rate);

    int timer_id = startTimer(1000 / int(frm_rate), Qt::PreciseTimer);
    if(timer_id == 0) {clean(); emit failed(); return;}

    _timer_id = timer_id;

    _semaphore.release();

    emit started();
}


// ========================================================================== //
// Stop.
// ========================================================================== //
void AVideoCaptureDevice::stop() {
    if(!isRunning()) return;

    emit stopping();

    _semaphore.acquire();

    killTimer(_timer_id); _timer_id = 0;

    clean();

    emit stopped();
}


// ========================================================================== //
// Timer event.
// ========================================================================== //
void AVideoCaptureDevice::timerEvent(QTimerEvent *event) {
    if(event->timerId() == _timer_id) {
        if(_semaphore.tryAcquire())
            QtConcurrent::run(this, &AVideoCaptureDevice::threadRun);
    }
}


// ========================================================================== //
// Clean.
// ========================================================================== //
void AVideoCaptureDevice::clean() {
    if(_av_vid_frm) {av_frame_free(&_av_vid_frm); _av_vid_frm = NULL;}
    if(_av_cap_frm) {av_frame_free(&_av_cap_frm); _av_cap_frm = NULL;}

    if(_av_sws_ctx) {sws_freeContext(_av_sws_ctx); _av_sws_ctx = NULL;}

    if(_av_fmt_ctx) {
        if(_vid_strm_idx > -1) {
            AVStream *av_vid_strm = _av_fmt_ctx->streams[_vid_strm_idx];
            if(av_vid_strm && av_vid_strm->codec)
                avcodec_close(av_vid_strm->codec);
        }

        avformat_close_input(&_av_fmt_ctx); _av_fmt_ctx = NULL;
    }

    _vid_strm_idx = -1;
}


// ========================================================================== //
// Thread run.
// ========================================================================== //
void AVideoCaptureDevice::threadRun() {
    AVPacket av_pkt;

    forever {
        if(av_read_frame(_av_fmt_ctx, &av_pkt) < 0) break;

        if(av_pkt.stream_index != _vid_strm_idx) {
            av_free_packet(&av_pkt); continue;
        }

        AVStream *av_vid_strm = _av_fmt_ctx->streams[_vid_strm_idx];
        av_packet_rescale_ts(&av_pkt, av_vid_strm->time_base
            , av_vid_strm->codec->time_base);

        AVCodecContext *av_dec_ctx = av_vid_strm->codec;

        int pkt_rdy = -1;
        if(avcodec_decode_video2(av_dec_ctx, _av_vid_frm, &pkt_rdy, &av_pkt) < 0
            || pkt_rdy == 0) {av_free_packet(&av_pkt); continue;}

        sws_scale(_av_sws_ctx
            , _av_vid_frm->data, _av_vid_frm->linesize, 0, _av_vid_frm->height
            , _av_cap_frm->data, _av_cap_frm->linesize);

        av_free_packet(&av_pkt);

        const int &w = _av_cap_frm->width;
        const int &h = _av_cap_frm->height;

        QImage img(w, h, QImage::Format_RGB888);
        for(int row = 0, bpl = w*3; row < h; ++row) {
            memcpy(img.scanLine(row)
                , _av_cap_frm->data[0] + row*_av_cap_frm->linesize[0], bpl);
        }

        QMetaObject::invokeMethod(this, "completed"
            , Qt::QueuedConnection, Q_ARG(QImage,img));

        break;
    }

    _semaphore.release();
}
