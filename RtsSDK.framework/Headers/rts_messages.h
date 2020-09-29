#if !defined(__ALI_RTS_MESSAGES_H__)
#define __ALI_RTS_MESSAGES_H__

#if defined(__cplusplus)
extern "C" {
#endif

/* event callback
 *
 * Set callback to rtssdk: call preconfig("MessageCallback", ...) and
 * preconfig("MessageCbParam", ...)
 * rtssdk will invoke your callback when an event happens (async mode). and
 * your callback need handle the event ASAP otherwise following event
 * messages will be blocked
 *
 * callback prototype
 * int (*event_callback)(void *opaque,
 *                    int type,
 *                    void *data,  // a string: "when=<time_in_ms>,where=<place>,who=<id>,desc=<extra text>"
 *                                 // if type == E_PROFILING_REPORT, desc=\"key1:val1,key2:val2,...\"
 *                                 // ATTENTION: temp data, do not cache it for later use
 *                    long long data_size // bytes of 'data'
 *                    );
 *
 * type:
 *       105           state report event (called periodically)
 *                     'data' points to a rtssdk_profiling_data object
 *       1100          opening an url
 *       1101          received first audio packet
 *       1102          received first video packet
 *       1103          output first video frame
 *       1104          output first audio frame
 *       20000~30000   error event
 */

/* help support IDs
 * app need record these IDs. provide them to aliyun when ask for
 * issue support
 */
#define E_HELP_SUPPORT_ID_LOAD       100 // issue for loading
#define E_HELP_SUPPORT_ID_CONNECT    102 // issue for connect operation
#define E_HELP_SUPPORT_ID_PUBLISH    103 // issue for publish operation
#define E_HELP_SUPPORT_ID_SUBSCRIBE  104 // issue for subscribe operation

/* profiling data. sent every 4 seconds by default
 * data format: key1:value1,key2:value2,key3:value3...
 */
#define E_PROFILING_REPORT  105


#define EVENT_ERROR_BASE 20000
// errors happening during opening stage
#define E_DNS_FAIL          (EVENT_ERROR_BASE + 1 )  // could not resolve host name
#define E_AUTH_FAIL         (EVENT_ERROR_BASE + 2 )  // bad auth code
#define E_CONN_OK           (EVENT_ERROR_BASE + 9 )  // fail to connect to sfu
#define E_CONN_FAIL         (EVENT_ERROR_BASE + 10)  // fail to connect to sfu
#define E_SUB_TIMEOUT       (EVENT_ERROR_BASE + 12)  // timeout for subscribe response
#define E_SUB_NO_STREAM     (EVENT_ERROR_BASE + 13)  // stream not exist
#define E_SUB_NO_AUDIO      (EVENT_ERROR_BASE + 14)  // audio track not found
#define E_SUB_NO_VIDEO      (EVENT_ERROR_BASE + 15)  // video track not found
#define E_SUB_UNKNOWN_ERROR (EVENT_ERROR_BASE + 20)  // other unknown error

// errors happening during running stage
#define E_CONGESTION_BEGIN  (EVENT_ERROR_BASE + 50)  // lost rate too high
#define E_CONGESTION_END    (EVENT_ERROR_BASE + 51)  // lost rate decrease to normal level
#define E_STREAM_BROKEN     (EVENT_ERROR_BASE + 52)  // no any audio and video packets
#define E_STREAM_RECOVERED  (EVENT_ERROR_BASE + 53)  // audio or video packets recovered
#define E_STREAM_EOF        (EVENT_ERROR_BASE + 54)  // EOF received from sfu. App need stop playback
#define E_CONNECT_LOST      (EVENT_ERROR_BASE + 55)  // require reconnection
#define E_STREAM_RESTARTED  (EVENT_ERROR_BASE + 56)  // stream restart detected
#define E_DOWNGRADE_RTMP    (EVENT_ERROR_BASE + 57)  // need downgrade to rtmp

#if defined(__cplusplus)
}
#endif

#endif  // __ALI_RTS_MESSAGES_H__
