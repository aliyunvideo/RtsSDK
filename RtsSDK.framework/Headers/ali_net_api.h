/* Copyright (c) 2021 Alibaba Group Holding Limited
 *
 * main api file for ali net sdk
 */

#if !defined(__ALI_NET_API_H__)
#define __ALI_NET_API_H__

/*
* The media abstraction and management of ali net sdk is shown below.
*  connection (usr cfg)         - - - - net_conn_handle_t handle;
*  |----stream (url)            - - - - net_stream_t      streamId;
*       |----track (msid)       - - - - 
*            |----av (ssrc)     - - - - 
*/

#include <cstdarg>
#include "rts_messages.h"

#if defined(__cplusplus)
extern "C" {
#endif

// structure to store subscribed stream info
// use ioctl(..., "get_stream_info", ...) to fetch
struct rts_worker_demux_info
{
    // audio part
    int audio_flag; // 1 - following audio info is valid; 0 - invalid
    int audio_channels; // 1
    int audio_sample_rate; // 48000

    // video part
    int video_flag; // 1 - following video info is valid; 0 - invalid
    int video_codec;      // 1 - h264  2 - hevc
    int video_width;
    int video_height;
    int video_profile;
    int video_level;

    unsigned char spspps[10 * 1024]; // large enough
    int spspps_len; // actual bytes used in spspps
};

struct pusher_delay{
    long long cap_time_ms;  //current utc time when capture the video frame
    long long enc_time_ms;  //current utc time when the videoframe send to encoder
};

struct player_delay{
    long long decoder_time_ms;  //current utc time when the video frame send to decoder
    long long render_time_ms;   //current utc time when the video frame send to render
    unsigned long long pts;     //the video frame's pts
};

struct rts_worker_demux_info2
{
    unsigned int uid; // 0: unspecified

    // audio part
    int audio_flag; // 1 - following audio info is valid; 0 - invalid
    int audio_channels; // 1
    int audio_sample_rate; // 48000

    // video part
    int video_flag; // 1 - following video info is valid; 0 - invalid
    int video_codec;      // 1 - h264
    int video_width;
    int video_height;
    int video_profile;
    int video_level;

    unsigned char spspps[10 * 1024]; // large enough
    int spspps_len; // actual bytes used in spspps
};

// structure to store subscribed stream info
// use ioctl(..., "get_pub_info", ...) to fetch
// not implemented yet
struct rts_worker_mux_info {
    // audio part
    int audio_flag; // 1 - following audio info is valid; 0 - invalid

    // video part
    int video_flag; // 1 - following video info is valid; 0 - invalid
};

// https://datatracker.ietf.org/doc/html/draft-ietf-mmusic-msid-16#section-2
#define NET_MSID_MAX_LENGTH 129
struct rts_frame {
    void *buf;              // where frame data is stored
    int size;               // size of frame data in bytes
    int is_audio;           // 1 for audio frame, 0 for video frame
    unsigned long long pts; // presentation time stamp, in ms
    unsigned long long dts; // decoding time stamp, in ms
    int flag;               // for video frame (is_audio == 0)
                            //     bit 0: key frame;
                            //     bit 1: corruption
                            //     bit 2: sps
                            //     bit 3: sps change
    int duration;           // in ms

    // use this function to free rts_frame object
    void (*free_ptr)(struct rts_frame *);

    unsigned int uid; // reserved. which user this stream attached
    struct pusher_delay delay;

    // msid-value = msid-id [ SP msid-appdata ]
    char msid[NET_MSID_MAX_LENGTH + 1];
};

enum NET_API_LOG_LEVEL {
    NET_API_LOG_ERROR = 0,
    NET_API_LOG_WARN  = 1,
    NET_API_LOG_INFO  = 2,
    NET_API_LOG_DEBUG = 3
};
/*
* 网络连接中的全局配置, 适用于网络库连接中的所有码流
* 比如，日志等级，日志打印接口，等等
*/
#define HELP_ID_PREFIX_LENGTH 64
struct net_conn_cfg_t{
    int (*__cb_log)(void *opaque, int level, const char *fmt, va_list args) = nullptr;
    
    // first parameter to __cb_log
    void *__log__cb_opaque = nullptr; 

    // only for internal logger
    bool __internal_log_to_console = false;

    // only for internal logger
    bool __internal_log_to_file = false;

    // only for internal logger
    bool __internal_log_to_server = false;

    // only for internal logger
    NET_API_LOG_LEVEL __internal_log_level = NET_API_LOG_INFO;

    int (* __cb_message)(void *opaque,
                        int type,
                        void *data, // temp data, do not cache it for later use!
                        long long data_size) = nullptr;

    // first parameter to __cb_message
    void *__msg_cb_opaque = nullptr; 

    // aacd stuff
    void *__cb_aacd_create = nullptr;
    void *__cb_aacd_decode = nullptr;
    void *__cb_aacd_close  = nullptr;

    // if null, set to be "emptyappid"
    // c-string length should not greater than HELP_ID_PREFIX_LENGTH
    char __helpidprefix[HELP_ID_PREFIX_LENGTH + 8] = {0};

    /* mcdn, global settings, begin */
    bool enable_high_interleave_delay = true;
    struct _remote_config{
        // [100, 1000], 
        // if @buffering_time = -1, sdk sets @buffering_time to be a default value
        long long buffering_time = -1;
        // if @buffering_time is not -1, sdk sets @enable_dymamic_video_jitter_buffer to be false
        bool enable_dymamic_video_jitter_buffer{true};
    }remoteConfig;
    struct _local_config{
        // [codec, profile] is as follows. otherwise, reset to default
        // [1, 1]: aac/aac-lc
        // [1, 4]: aac-he
        // [1, 28]: aac-he2
        // [2, 0]: opus
        // [3, 0]: g722
        int pub_audio_codec{1};
        int pub_audio_profile{1}; 

        // only in {44100, 48000, 16000, 8000, 32000, 22050, 24000}, otherwise, reset to default
        // for HE AAC, sample rate set by half (22050 or 24000)
        int pub_audio_sample_rate{48000};
        int pub_audio_rtp_clock_rate{48000};
        
        // only in {1, 2}, otherwise, reset to default
        int pub_audio_channels{1};
        
        // use 48000 as clock rate or use audio sample rate as clock rate
        bool pub_audio_timebase_follow_samplerate{false};

        // only in {1: h264, 2: h265/hevc}, otherwise, reset to default
        int  pub_video_codec{1}; 
        int pub_enable_padding{1};
    }localConfig;
    /* mcdn, global settings, end */
};

/*
* msid列表
*/
struct net_msid_t {
    int cnt = 0;
    char **info = nullptr; // 调用者进行释放
};

/*
* 在网络连接中，某一个码流的配置信息
*/
struct net_stream_cfg_t{
    int scenario; 
    const char *url = nullptr;
    struct net_msid_t *msid = nullptr;
};


/*
* 网络连接实例的句柄
*/
typedef void* net_conn_handle_t;

/*
* 管理连接中的多路码流
*/ 
typedef int net_stream_t;

/*
* 功能: 创建网络库连接实例
* 返回值: 
*    1, 空指针, 创建失败;
*    2, 其它, 创建成功
*/
net_conn_handle_t ali_net_create_connection(const net_conn_cfg_t *connCfg);

/*
* 功能: 发布码流, 
* 返回值：内部管理的 streamId
*    1, 负数id, 创建失败;
*    2, 其它id, 发布成功 
*/
net_stream_t ali_net_publish_open(net_conn_handle_t handle, net_stream_cfg_t *streamCfg);

/*
* 功能: 更新发流
* 返回值: 
*    1, -1, 更新失败;
*    2, 0,  更新成功. 
*/
int ali_net_publish_update(net_conn_handle_t handle, net_stream_t streamId, net_stream_cfg_t *streamCfg);

/*
* 功能: 取消发流
* 返回值:
*    1, -1, 取消失败;
*    2, 0,  取消成功. 
*/ 
int ali_net_publish_close(net_conn_handle_t handle, net_stream_t streamId);

/*
* 功能: 发起订阅
* 返回值： 内部管理的 streamId
*    1, 负数id, 创建失败;
*    2, 其它id, 发布成功 
*/
net_stream_t ali_net_subscribe_open(net_conn_handle_t handle, net_stream_cfg_t *streamCfg);

/*
* 功能: 更新订阅
* 返回值: 
*    1, -1, 更新失败;
*    2, 0,  更新成功. 
*/
int ali_net_subscribe_update(net_conn_handle_t handle, net_stream_t streamId, net_stream_cfg_t *streamCfg);

/*
* 功能: 取消订阅
* 返回值:
*    1, -1, 取消失败;
*    2, 0,  取消成功. 
*/ 
int ali_net_subscribe_close(net_conn_handle_t handle, net_stream_t streamId);

/* read one frame
*  caller need free the returned frame
*  return value: 
*       (1) 1 for one frame read into '*frame'; 
*       (2) 0 for try later; 
*       (3) other negative value for fatal error
*/
int ali_net_read(net_conn_handle_t handle, net_stream_t streamId, struct rts_frame **frame);

/* write one frame. callee free the frame
*  return value: 
*        (1) 1 for ok; 
*        (2) 0 for try later;
*        (3) other negative value for fatal error
*/
int ali_net_write(net_conn_handle_t handle, net_stream_t streamId, struct rts_frame **frame);

/*
* 功能: 销毁网络连接
* 返回值:
*    1, -1, 销毁失败;
*    2, 0,  销毁成功. 
*/ 
int ali_net_destroy_connection(net_conn_handle_t handle);

/*
* get the @uid of the specified stream by @url
* negative return value for error
*/
long long ali_net_get_uid_with_url(net_conn_handle_t handle, const char *url, unsigned int *uid);

/*
* get the stream information of the specified @uid
* if @uid = 0, then get the information of the newly added stream
* negative return value for error
*/
long long ali_net_get_stream_info(net_conn_handle_t handle, net_stream_t streamId, unsigned int uid, struct rts_worker_demux_info *si);

/*
* get the information of the specified pubnished stream by @streamId
* negative return value for error
*/
long long ali_net_get_pub_info(net_conn_handle_t handle, net_stream_t streamId, struct rts_worker_mux_info *si);

enum ALI_NET_OPER_TYPE {
    /* add get oper */ 
    ALI_NET_GET_VIDEO_CACHE_SIZE, // @value is digital

    /* add set oper*/ 
    ALI_NET_SET_VIDEO_BITRATE, // @value is digital
    ALI_NET_SET_BUFFER_DURATION, // @value is digital
    ALI_NET_SET_INFOCB_INVOKE_INTERVAL, // @value is digital

    ALI_NET_SET_AES_KEY, // @value is c-string
};
/*
* get playback parameters of specified type @operType
* query result is saved in @value (digital or c-string)
* negative return value for error
*/
long long ali_net_get_param(net_conn_handle_t handle, net_stream_t streamId, ALI_NET_OPER_TYPE operType, long long *value);

/*
* set playback parameters @value(digital or c-string pointer) of specified type @operType
* negative return value for error
*/
long long ali_net_set_param(net_conn_handle_t handle, net_stream_t streamId, ALI_NET_OPER_TYPE operType, long long value);

/*
* reload the ali net connection
* negative return value for error
*/
long long ali_net_reload(net_conn_handle_t handle);


/*
* 功能: 操作网络连接实例, 提供技术预览或特别定制功能, 操作选项默认不公开.  
* 返回值:
*    1, 负数, 操作失败;
*    2, 其它, 销毁成功. 
*/ 
long long ali_net_ioctl(net_conn_handle_t handle, net_stream_t streamId, const char *cmd, void *arg);


#if defined(__cplusplus)
}
#endif

#endif
