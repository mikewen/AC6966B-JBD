
#ifndef _LE_USER_H_
#define _LE_USER_H_

#include "typedef.h"
#include "btstack/btstack_typedef.h"

#if defined __cplusplus
extern "C" {
#endif



#define BT_NAME_LEN_MAX		      29
#define ADV_RSP_PACKET_MAX        31

// hci con handles (12 bit): 0x0000..0x0fff
#define HCI_CON_HANDLE_INVALID 0xffff

#define BTSTACK_EVENT_STATE                                0x60
#define L2CAP_EVENT_CONNECTION_PARAMETER_UPDATE_RESPONSE   0x77


#define SM_EVENT_JUST_WORKS_REQUEST                        0xD0
#define SM_EVENT_PASSKEY_DISPLAY_NUMBER                    0xD2



#define GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NONE          0
#define GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION  1
#define GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_INDICATION    2


#define GATT_EVENT_NOTIFICATION                                  0xA7
#define GATT_EVENT_INDICATION                                    0xA8
#define GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT             0xA5
#define GATT_EVENT_LONG_CHARACTERISTIC_VALUE_QUERY_RESULT        0xA6
// #define GATT_EVENT_SERVICE_QUERY_RESULT                          0xA1
// #define GATT_EVENT_CHARACTERISTIC_QUERY_RESULT                   0xA2
// #define GATT_EVENT_QUERY_COMPLETE                                0xA0
#define GAP_EVENT_ADVERTISING_REPORT                             0xE2

// Authentication requirement flags
#define SM_AUTHREQ_NO_BONDING           0x00
#define SM_AUTHREQ_BONDING              0x01
#define SM_AUTHREQ_MITM_PROTECTION      0x04
#define SM_AUTHREQ_SECURE_CONNECTION    0x08
#define SM_AUTHREQ_KEYPRESS             0x10


#define L2CAP_EVENT_CONNECTION_PARAMETER_UPDATE_RESPONSE   0x77


#define  BT_OP_SUCCESS                             0x00
#define  BT_ERR_ADVERTISING_TIMEOUT                0x3C


//--------------------------------------------

    struct ble_server_operation_t {
        int(*adv_enable)(void *priv, u32 enable);
        int(*disconnect)(void *priv);
        int(*get_buffer_vaild)(void *priv);
        int(*send_data)(void *priv, void *buf, u16 len);
        int(*regist_wakeup_send)(void *priv, void *cbk);
        int(*regist_recieve_cbk)(void *priv, void *cbk);
        int(*regist_state_cbk)(void *priv, void *cbk);
        int(*latency_enable)(void *priv, u32 enable);
    };

    void ble_get_server_operation_table(struct ble_server_operation_t **interface_pt);


    struct ble_client_operation_t {
        int(*scan_enable)(void *priv, u32 enable);
        int(*disconnect)(void *priv);
        int(*get_buffer_vaild)(void *priv);
        int(*write_data)(void *priv, void *buf, u16 len);
        int(*read_do)(void *priv);
        int(*regist_wakeup_send)(void *priv, void *cbk);
        int(*regist_recieve_cbk)(void *priv, void *cbk);
        int(*regist_state_cbk)(void *priv, void *cbk);
        int (*init_config)(void *priv, void *cfg);
        int (*opt_comm_send)(u16 handle, u8 *data, u16 len, u8 att_op_type);
    };

    struct ble_client_operation_t *ble_get_client_operation_table(void);

    typedef struct {
        //base info
        uint8_t   type;                 ///< See <btstack/hci_cmds.h> SM_...
        uint8_t   size;
        hci_con_handle_t con_handle;
        uint8_t   addr_type;
        uint8_t   address[6];
        //extend info
        uint8_t   data[4];
    } sm_just_event_t;

    static inline uint32_t ble_min(uint32_t a, uint32_t b)
    {
        return a < b ? a : b;
    }

//---------------------------------------------------------------------------------------------------


//----------------------------------------
//----------------------------------------



    extern int get_ble_btstack_state(void);
    extern int get_indicate_state(void);


    extern u8 get_ble_local_name(u8 *name_buf);
    extern u8 get_ble_local_name_len();




    extern void hci_event_callback_set(void(*cbk_ph)(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size));
    extern void ll_hci_connection_updata(u8 *data);

#endif


