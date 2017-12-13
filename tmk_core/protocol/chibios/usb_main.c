/*
 * (c) 2015 flabberast <s3+flabbergast@sdfeu.org>
 *
 * Based on the following work:
 *  - Guillaume Duc's raw hid example (MIT License)
 *    https://github.com/guiduc/usb-hid-chibios-example
 *  - PJRC Teensy examples (MIT License)
 *    https://www.pjrc.com/teensy/usb_keyboard.html
 *  - hasu's TMK keyboard code (GPL v2 and some code Modified BSD)
 *    https://github.com/tmk/tmk_keyboard/
 *  - ChibiOS demo code (Apache 2.0 License)
 *    http://www.chibios.org
 *
 * Since some GPL'd code is used, this work is licensed under
 * GPL v2 or later.
 */

#include "ch.h"
#include "hal.h"

#include "usb_main.h"

#include "host.h"
#include "debug.h"
#include "suspend.h"
#ifdef SLEEP_LED_ENABLE
#include "sleep_led.h"
#include "led.h"
#endif
#include "wait.h"
#include "usb_descriptor.h"

#ifdef NKRO_ENABLE
  #include "keycode_config.h"

  extern keymap_config_t keymap_config;
#endif

/* ---------------------------------------------------------
 *       Global interface variables and declarations
 * ---------------------------------------------------------
 */

#ifndef usb_lld_connect_bus
  #define usb_lld_connect_bus(usbp)
#endif

#ifndef usb_lld_disconnect_bus
  #define usb_lld_disconnect_bus(usbp)
#endif

uint8_t keyboard_idle __attribute__((aligned(2))) = 0;
uint8_t keyboard_protocol __attribute__((aligned(2))) = 1;
uint16_t keyboard_led_stats __attribute__((aligned(2))) = 0;
volatile uint16_t keyboard_idle_count = 0;
static virtual_timer_t keyboard_idle_timer;
static void keyboard_idle_timer_cb(void *arg);

report_keyboard_t keyboard_report_sent = {{0}};
#ifdef MOUSE_ENABLE
report_mouse_t mouse_report_blank = {0};
#endif /* MOUSE_ENABLE */
#ifdef EXTRAKEY_ENABLE
uint8_t extra_report_blank[3] = {0};
#endif /* EXTRAKEY_ENABLE */

#ifdef CONSOLE_ENABLE
/* The emission buffers queue */
output_buffers_queue_t console_buf_queue;
static uint8_t console_queue_buffer[BQ_BUFFER_SIZE(CONSOLE_QUEUE_CAPACITY, CONSOLE_EPSIZE)];

static virtual_timer_t console_flush_timer;
void console_queue_onotify(io_buffers_queue_t *bqp);
static void console_flush_cb(void *arg);
#endif /* CONSOLE_ENABLE */

#ifdef RAW_ENABLE
#define RAW_QUEUE_CAPACITY 4
static input_buffers_queue_t raw_buf_queue;
static void raw_queue_inotify(io_buffers_queue_t *bqp);
static void raw_in_cb(USBDriver *usbp, usbep_t ep);
static void raw_out_cb(USBDriver *usbp, usbep_t ep);
static void configure_raw_interface(void);
static uint8_t raw_queue_buffer[BQ_BUFFER_SIZE(RAW_QUEUE_CAPACITY, RAW_EPSIZE)];
#endif

#ifdef MIDI_ENABLE
#define MIDI_QUEUE_CAPACITY 4
static uint8_t midi_queue_buffer_in[BQ_BUFFER_SIZE(MIDI_QUEUE_CAPACITY, MIDI_STREAM_EPSIZE)];
static uint8_t midi_queue_buffer_out[BQ_BUFFER_SIZE(MIDI_QUEUE_CAPACITY, MIDI_STREAM_EPSIZE)];
#endif

/* ---------------------------------------------------------
 *            Descriptors and USB driver objects
 * ---------------------------------------------------------
 */

/* HID specific constants */
#define HID_GET_REPORT 0x01
#define HID_GET_IDLE 0x02
#define HID_GET_PROTOCOL 0x03
#define HID_SET_REPORT 0x09
#define HID_SET_IDLE 0x0A
#define HID_SET_PROTOCOL 0x0B

/*
 * Handles the GET_DESCRIPTOR callback
 *
 * Returns the proper descriptor
 */
static const USBDescriptor *usb_get_descriptor_cb(USBDriver *usbp, uint8_t dtype, uint8_t dindex, uint16_t wIndex) {
  (void)usbp;
  static USBDescriptor desc;
  uint16_t wValue = ((uint16_t)dtype << 8) | dindex;
  desc.ud_string = NULL;
  desc.ud_size = get_usb_descriptor(wValue, wIndex, (const void** const)&desc.ud_string);
  if (desc.ud_string == NULL)
    return NULL;
  else
    return &desc;
}

/* keyboard endpoint state structure */
static USBInEndpointState kbd_ep_state;
/* keyboard endpoint initialization structure (IN) */
static const USBEndpointConfig kbd_ep_config = {
  USB_EP_MODE_TYPE_INTR,        /* Interrupt EP */
  NULL,                         /* SETUP packet notification callback */
  kbd_in_cb,                    /* IN notification callback */
  NULL,                         /* OUT notification callback */
  KEYBOARD_EPSIZE,              /* IN maximum packet size */
  0,                            /* OUT maximum packet size */
  &kbd_ep_state,                /* IN Endpoint state */
  NULL,                         /* OUT endpoint state */
  2,                            /* IN multiplier */
  NULL                          /* SETUP buffer (not a SETUP endpoint) */
};

#ifdef MOUSE_ENABLE
/* mouse endpoint state structure */
static USBInEndpointState mouse_ep_state;

/* mouse endpoint initialization structure (IN) */
static const USBEndpointConfig mouse_ep_config = {
  USB_EP_MODE_TYPE_INTR,        /* Interrupt EP */
  NULL,                         /* SETUP packet notification callback */
  mouse_in_cb,                  /* IN notification callback */
  NULL,                         /* OUT notification callback */
  MOUSE_EPSIZE,                 /* IN maximum packet size */
  0,                            /* OUT maximum packet size */
  &mouse_ep_state,              /* IN Endpoint state */
  NULL,                         /* OUT endpoint state */
  2,                            /* IN multiplier */
  NULL                          /* SETUP buffer (not a SETUP endpoint) */
};
#endif /* MOUSE_ENABLE */

#ifdef CONSOLE_ENABLE
/* console endpoint state structure */
static USBInEndpointState console_ep_state;

/* console endpoint initialization structure (IN) */
static const USBEndpointConfig console_ep_config = {
  USB_EP_MODE_TYPE_INTR,        /* Interrupt EP */
  NULL,                         /* SETUP packet notification callback */
  console_in_cb,                /* IN notification callback */
  NULL,                         /* OUT notification callback */
  CONSOLE_EPSIZE,               /* IN maximum packet size */
  0,                            /* OUT maximum packet size */
  &console_ep_state,            /* IN Endpoint state */
  NULL,                         /* OUT endpoint state */
  2,                            /* IN multiplier */
  NULL                          /* SETUP buffer (not a SETUP endpoint) */
};
#endif /* CONSOLE_ENABLE */

#ifdef EXTRAKEY_ENABLE
/* extrakey endpoint state structure */
static USBInEndpointState extra_ep_state;

/* extrakey endpoint initialization structure (IN) */
static const USBEndpointConfig extra_ep_config = {
  USB_EP_MODE_TYPE_INTR,        /* Interrupt EP */
  NULL,                         /* SETUP packet notification callback */
  extra_in_cb,                  /* IN notification callback */
  NULL,                         /* OUT notification callback */
  EXTRAKEY_EPSIZE,              /* IN maximum packet size */
  0,                            /* OUT maximum packet size */
  &extra_ep_state,              /* IN Endpoint state */
  NULL,                         /* OUT endpoint state */
  2,                            /* IN multiplier */
  NULL                          /* SETUP buffer (not a SETUP endpoint) */
};
#endif /* EXTRAKEY_ENABLE */

#ifdef NKRO_ENABLE
/* nkro endpoint state structure */
static USBInEndpointState nkro_ep_state;

/* nkro endpoint initialization structure (IN) */
static const USBEndpointConfig nkro_ep_config = {
  USB_EP_MODE_TYPE_INTR,        /* Interrupt EP */
  NULL,                         /* SETUP packet notification callback */
  nkro_in_cb,                   /* IN notification callback */
  NULL,                         /* OUT notification callback */
  NKRO_EPSIZE,                  /* IN maximum packet size */
  0,                            /* OUT maximum packet size */
  &nkro_ep_state,               /* IN Endpoint state */
  NULL,                         /* OUT endpoint state */
  2,                            /* IN multiplier */
  NULL                          /* SETUP buffer (not a SETUP endpoint) */
};
#endif /* NKRO_ENABLE */

#ifdef RAW_ENABLE
static USBInEndpointState raw_in_ep_state;
static USBOutEndpointState raw_out_ep_state;

static const USBEndpointConfig raw_in_ep_config = {
  USB_EP_MODE_TYPE_INTR,        /* Interrupt EP */
  NULL,                         /* SETUP packet notification callback */
  raw_in_cb,                    /* IN notification callback */
  NULL,                         /* OUT notification callback */
  RAW_EPSIZE,                   /* IN maximum packet size */
  0,                            /* OUT maximum packet size */
  &raw_in_ep_state,             /* IN Endpoint state */
  NULL,                         /* OUT endpoint state */
  2,                            /* IN multiplier */
  NULL                          /* SETUP buffer (not a SETUP endpoint) */
};

static const USBEndpointConfig raw_out_ep_config = {
  USB_EP_MODE_TYPE_INTR,        /* Interrupt EP */
  NULL,                         /* SETUP packet notification callback */
  NULL,                         /* IN notification callback */
  &raw_out_cb,                  /* OUT notification callback */
  0,                            /* IN maximum packet size */
  RAW_EPSIZE,                   /* OUT maximum packet size */
  NULL,                         /* IN Endpoint state */
  &raw_out_ep_state,             /* OUT endpoint state */
  2,                            /* IN multiplier */
  NULL                          /* SETUP buffer (not a SETUP endpoint) */
};
#endif

#ifdef MIDI_ENABLE
static USBInEndpointState midi_in_ep_state;
static USBOutEndpointState midi_out_ep_state;

static const USBEndpointConfig midi_in_ep_config = {
  USB_EP_MODE_TYPE_BULK,        /* Bulk EP */
  NULL,                         /* SETUP packet notification callback */
  sduDataTransmitted,           /* IN notification callback */
  NULL,                         /* OUT notification callback */
  MIDI_STREAM_EPSIZE,           /* IN maximum packet size */
  0,                            /* OUT maximum packet size */
  &midi_in_ep_state,             /* IN Endpoint state */
  NULL,                         /* OUT endpoint state */
  2,                            /* IN multiplier */
  NULL                          /* SETUP buffer (not a SETUP endpoint) */
};

static const USBEndpointConfig midi_out_ep_config = {
  USB_EP_MODE_TYPE_BULK,        /* Bulk EP */
  NULL,                         /* SETUP packet notification callback */
  NULL,                         /* IN notification callback */
  sduDataReceived,              /* OUT notification callback */
  0,                            /* IN maximum packet size */
  MIDI_STREAM_EPSIZE,           /* OUT maximum packet size */
  NULL,                         /* IN Endpoint state */
  &midi_out_ep_state,           /* OUT endpoint state */
  2,                            /* IN multiplier */
  NULL                          /* SETUP buffer (not a SETUP endpoint) */
};

SerialUSBDriver midi_driver;
const SerialUSBConfig midi_driver_conf = {
  &USB_DRIVER,
  MIDI_STREAM_IN_EPNUM,
  MIDI_STREAM_OUT_EPNUM,
  0,
};
#endif

#ifdef VIRTSER_ENABLE
SerialUSBDriver SDU1;

static USBOutEndpointState virtser_out_ep_state;
static USBInEndpointState virtser_in_ep_state;
static USBInEndpointState virtser_int_ep_state;

static const USBEndpointConfig virtser_in_ep_config = {
  USB_EP_MODE_TYPE_BULK,
  NULL,
  sduDataTransmitted,
  NULL,
  CDC_EPSIZE,
  0,
  &virtser_in_ep_state,
  NULL,
  2,
  NULL
};

static const USBEndpointConfig virtser_out_ep_config = {
  USB_EP_MODE_TYPE_BULK,
  NULL,
  NULL,
  sduDataReceived,
  0,
  CDC_EPSIZE,
  NULL,
  &virtser_out_ep_state,
  2,
  NULL
};

static const USBEndpointConfig virtser_int_ep_config = {
  USB_EP_MODE_TYPE_INTR,
  NULL,
  sduInterruptTransmitted,
  NULL,
  CDC_NOTIFICATION_EPSIZE,
  0,
  &virtser_int_ep_state,
  NULL,
  1,
  NULL
};

const SerialUSBConfig serusbcfg = {
  &USB_DRIVER,
  CDC_IN_EPNUM,
  CDC_OUT_EPNUM,
  CDC_NOTIFICATION_EPNUM
};

#endif

/* ---------------------------------------------------------
 *                  USB driver functions
 * ---------------------------------------------------------
 */

/* Handles the USB driver global events
 * TODO: maybe disable some things when connection is lost? */
static void usb_event_cb(USBDriver *usbp, usbevent_t event) {
  switch(event) {
  case USB_EVENT_ADDRESS:
    return;

  case USB_EVENT_CONFIGURED:
    osalSysLockFromISR();
    /* Enable the endpoints specified into the configuration. */
    usbInitEndpointI(usbp, KEYBOARD_IN_EPNUM, &kbd_ep_config);
#ifdef MOUSE_ENABLE
    usbInitEndpointI(usbp, MOUSE_IN_EPNUM, &mouse_ep_config);
#endif /* MOUSE_ENABLE */
#ifdef CONSOLE_ENABLE
    usbInitEndpointI(usbp, CONSOLE_IN_EPNUM, &console_ep_config);
    /* don't need to start the flush timer, it starts from console_in_cb automatically */
#endif /* CONSOLE_ENABLE */
#ifdef EXTRAKEY_ENABLE
    usbInitEndpointI(usbp, EXTRAKEY_IN_EPNUM, &extra_ep_config);
#endif /* EXTRAKEY_ENABLE */
#ifdef NKRO_ENABLE
    usbInitEndpointI(usbp, NKRO_IN_EPNUM, &nkro_ep_config);
#endif /* NKRO_ENABLE */
#ifdef RAW_ENABLE
    usbInitEndpointI(usbp, RAW_IN_EPNUM, &raw_in_ep_config);
    usbInitEndpointI(usbp, RAW_OUT_EPNUM, &raw_out_ep_config);
    configure_raw_interface();
#endif
#ifdef MIDI_ENABLE
    usbInitEndpointI(usbp, MIDI_STREAM_IN_EPNUM, &midi_in_ep_config);
    usbInitEndpointI(usbp, MIDI_STREAM_OUT_EPNUM, &midi_out_ep_config);
    sduConfigureHookI(&midi_driver);
#endif
#ifdef VIRTSER_ENABLE
    /* Enables the endpoints specified into the configuration.
       Note, this callback is invoked from an ISR so I-Class functions
       must be used.*/
    usbInitEndpointI(usbp, CDC_IN_EPNUM, &virtser_in_ep_config);
    usbInitEndpointI(usbp, CDC_OUT_EPNUM, &virtser_out_ep_config);
    usbInitEndpointI(usbp, CDC_NOTIFICATION_EPNUM, &virtser_int_ep_config);

    /* Resetting the state of the CDC subsystem.*/
    sduConfigureHookI(&SDU1);
#endif
    osalSysUnlockFromISR();
    return;
  case USB_EVENT_SUSPEND:
    //TODO: from ISR! print("[S]");
#ifdef VIRTSER_ENABLE
    sduDisconnectI(&SDU1);
#endif
#ifdef MIDI_ENABLE
    sduDisconnectI(&midi_driver);
#endif
#ifdef SLEEP_LED_ENABLE
    sleep_led_enable();
#endif /* SLEEP_LED_ENABLE */
    /* Falls into.*/
  case USB_EVENT_UNCONFIGURED:
    /* Falls into.*/
  case USB_EVENT_RESET:
    return;

  case USB_EVENT_WAKEUP:
    //TODO: from ISR! print("[W]");
    suspend_wakeup_init();
#ifdef SLEEP_LED_ENABLE
    sleep_led_disable();
    // NOTE: converters may not accept this
    led_set(host_keyboard_leds());
#endif /* SLEEP_LED_ENABLE */
    return;

  case USB_EVENT_STALLED:
    return;
  }
}

/* Function used locally in os/hal/src/usb.c for getting descriptors
 * need it here for HID descriptor */
static uint16_t get_hword(uint8_t *p) {
  uint16_t hw;

  hw = (uint16_t)*p++;
  hw |= (uint16_t)*p << 8U;
  return hw;
}

/*
 * Appendix G: HID Request Support Requirements
 *
 * The following table enumerates the requests that need to be supported by various types of HID class devices.
 * Device type     GetReport   SetReport   GetIdle     SetIdle     GetProtocol SetProtocol
 * ------------------------------------------------------------------------------------------
 * Boot Mouse      Required    Optional    Optional    Optional    Required    Required
 * Non-Boot Mouse  Required    Optional    Optional    Optional    Optional    Optional
 * Boot Keyboard   Required    Optional    Required    Required    Required    Required
 * Non-Boot Keybrd Required    Optional    Required    Required    Optional    Optional
 * Other Device    Required    Optional    Optional    Optional    Optional    Optional
 */

/* Callback for SETUP request on the endpoint 0 (control) */
static bool usb_request_hook_cb(USBDriver *usbp) {
  const USBDescriptor *dp;

  /* usbp->setup fields:
   *  0:   bmRequestType (bitmask)
   *  1:   bRequest
   *  2,3: (LSB,MSB) wValue
   *  4,5: (LSB,MSB) wIndex
   *  6,7: (LSB,MSB) wLength (number of bytes to transfer if there is a data phase) */

  /* Handle HID class specific requests */
  if(((usbp->setup[0] & USB_RTYPE_TYPE_MASK) == USB_RTYPE_TYPE_CLASS) &&
     ((usbp->setup[0] & USB_RTYPE_RECIPIENT_MASK) == USB_RTYPE_RECIPIENT_INTERFACE)) {
    switch(usbp->setup[0] & USB_RTYPE_DIR_MASK) {
    case USB_RTYPE_DIR_DEV2HOST:
      switch(usbp->setup[1]) {   /* bRequest */
      case HID_GET_REPORT:
        switch(usbp->setup[4]) {     /* LSB(wIndex) (check MSB==0?) */
        case KEYBOARD_INTERFACE:
#ifdef NKRO_ENABLE
        case NKRO_INTERFACE:
#endif /* NKRO_ENABLE */
          usbSetupTransfer(usbp, (uint8_t *)&keyboard_report_sent, sizeof(keyboard_report_sent), NULL);
          return TRUE;
          break;

#ifdef MOUSE_ENABLE
        case MOUSE_INTERFACE:
          usbSetupTransfer(usbp, (uint8_t *)&mouse_report_blank, sizeof(mouse_report_blank), NULL);
          return TRUE;
          break;
#endif /* MOUSE_ENABLE */

#ifdef CONSOLE_ENABLE
        case CONSOLE_INTERFACE:
          usbSetupTransfer(usbp, console_queue_buffer, CONSOLE_EPSIZE, NULL);
          return TRUE;
          break;
#endif /* CONSOLE_ENABLE */

#ifdef EXTRAKEY_ENABLE
        case EXTRAKEY_INTERFACE:
          if(usbp->setup[3] == 1) { /* MSB(wValue) [Report Type] == 1 [Input Report] */
            switch(usbp->setup[2]) { /* LSB(wValue) [Report ID] */
              case REPORT_ID_SYSTEM:
                extra_report_blank[0] = REPORT_ID_SYSTEM;
                usbSetupTransfer(usbp, (uint8_t *)extra_report_blank, sizeof(extra_report_blank), NULL);
                return TRUE;
                break;
              case REPORT_ID_CONSUMER:
                extra_report_blank[0] = REPORT_ID_CONSUMER;
                usbSetupTransfer(usbp, (uint8_t *)extra_report_blank, sizeof(extra_report_blank), NULL);
                return TRUE;
                break;
              default:
                return FALSE;
            }
          } else {
            return FALSE;
          }
          break;
#endif /* EXTRAKEY_ENABLE */

        default:
          usbSetupTransfer(usbp, NULL, 0, NULL);
          return TRUE;
          break;
        }
        break;

      case HID_GET_PROTOCOL:
        if((usbp->setup[4] == KEYBOARD_INTERFACE) && (usbp->setup[5] == 0)) {   /* wIndex */
          usbSetupTransfer(usbp, &keyboard_protocol, 1, NULL);
          return TRUE;
        }
        break;

      case HID_GET_IDLE:
        usbSetupTransfer(usbp, &keyboard_idle, 1, NULL);
        return TRUE;
        break;
      }
      break;

    case USB_RTYPE_DIR_HOST2DEV:
      switch(usbp->setup[1]) {   /* bRequest */
      case HID_SET_REPORT:
        switch(usbp->setup[4]) {       /* LSB(wIndex) (check MSB==0 and wLength==1?) */
        case KEYBOARD_INTERFACE:
#ifdef NKRO_ENABLE
        case NKRO_INTERFACE:
#endif  /* NKRO_ENABLE */
        /* keyboard_led_stats = <read byte from next OUT report>
         * keyboard_led_stats needs be word (or dword), otherwise we get an exception on F0 */
          usbSetupTransfer(usbp, (uint8_t *)&keyboard_led_stats, 1, NULL);
          return TRUE;
          break;
        }
        break;

      case HID_SET_PROTOCOL:
        if((usbp->setup[4] == KEYBOARD_INTERFACE) && (usbp->setup[5] == 0)) {   /* wIndex */
          keyboard_protocol = ((usbp->setup[2]) != 0x00);   /* LSB(wValue) */
#ifdef NKRO_ENABLE
          keymap_config.nkro = !!keyboard_protocol;
          if(!keymap_config.nkro && keyboard_idle) {
#else /* NKRO_ENABLE */
          if(keyboard_idle) {
#endif /* NKRO_ENABLE */
          /* arm the idle timer if boot protocol & idle */
            osalSysLockFromISR();
            chVTSetI(&keyboard_idle_timer, 4*MS2ST(keyboard_idle), keyboard_idle_timer_cb, (void *)usbp);
            osalSysUnlockFromISR();
          }
        }
        usbSetupTransfer(usbp, NULL, 0, NULL);
        return TRUE;
        break;

      case HID_SET_IDLE:
        keyboard_idle = usbp->setup[3];     /* MSB(wValue) */
        /* arm the timer */
#ifdef NKRO_ENABLE
        if(!keymap_config.nkro && keyboard_idle) {
#else /* NKRO_ENABLE */
        if(keyboard_idle) {
#endif /* NKRO_ENABLE */
          osalSysLockFromISR();
          chVTSetI(&keyboard_idle_timer, 4*MS2ST(keyboard_idle), keyboard_idle_timer_cb, (void *)usbp);
          osalSysUnlockFromISR();
        }
        usbSetupTransfer(usbp, NULL, 0, NULL);
        return TRUE;
        break;
      }
      break;
    }
  }

  /* Handle the Get_Descriptor Request for HID class (not handled by the default hook) */
  if((usbp->setup[0] == 0x81) && (usbp->setup[1] == USB_REQ_GET_DESCRIPTOR)) {
    dp = usbp->config->get_descriptor_cb(usbp, usbp->setup[3], usbp->setup[2], get_hword(&usbp->setup[4]));
    if(dp == NULL)
      return FALSE;
    usbSetupTransfer(usbp, (uint8_t *)dp->ud_string, dp->ud_size, NULL);
    return TRUE;
  }
  #ifdef VIRTSER_ENABLE
    return sduRequestsHook(usbp);
  #endif

  return FALSE;
}

/* Start-of-frame callback */
static void usb_sof_cb(USBDriver *usbp) {
  kbd_sof_cb(usbp);
#if defined(VIRTSER_ENABLE) || defined(MIDI_ENABLE)
  osalSysLockFromISR();
  #ifdef VIRTSER_ENABLE
  sduSOFHookI(&SDU1);
  #endif
  #ifdef MIDI_ENABLE
  sduSOFHookI(&midi_driver);
  #endif
  osalSysUnlockFromISR();
#endif
}


/* USB driver configuration */
static const USBConfig usbcfg = {
  usb_event_cb,                 /* USB events callback */
  usb_get_descriptor_cb,        /* Device GET_DESCRIPTOR request callback */
  usb_request_hook_cb,          /* Requests hook callback */
  usb_sof_cb                    /* Start Of Frame callback */
};

/*
 * Initialize the USB driver
 */
void init_usb_driver(USBDriver *usbp) {
#ifdef VIRTSER_ENABLE
  sduObjectInit(&SDU1);
  sduStart(&SDU1, &serusbcfg);
#endif

#ifdef MIDI_ENABLE
  sduObjectInit(&midi_driver);
  bqnotify_t notify = midi_driver.ibqueue.notify;
  ibqObjectInit(&midi_driver.ibqueue, midi_queue_buffer_in, MIDI_STREAM_EPSIZE, MIDI_QUEUE_CAPACITY, notify, &midi_driver);
  notify = midi_driver.obqueue.notify;
  obqObjectInit(&midi_driver.obqueue, midi_queue_buffer_out, MIDI_STREAM_EPSIZE, MIDI_QUEUE_CAPACITY, notify, &midi_driver);
  sduStart(&midi_driver, &midi_driver_conf);
#endif

#ifdef CONSOLE_ENABLE
  obqObjectInit(&console_buf_queue, console_queue_buffer, CONSOLE_EPSIZE, CONSOLE_QUEUE_CAPACITY, console_queue_onotify, (void*)usbp);
  chVTObjectInit(&console_flush_timer);
#endif

  /*
   * Activates the USB driver and then the USB bus pull-up on D+.
   * Note, a delay is inserted in order to not have to disconnect the cable
   * after a reset.
   */
  usbDisconnectBus(usbp);
  wait_ms(1500);
  usbStart(usbp, &usbcfg);
  usbConnectBus(usbp);

  chVTObjectInit(&keyboard_idle_timer);
#ifdef RAW_ENABLE
  ibqObjectInit(&raw_buf_queue, raw_queue_buffer, RAW_EPSIZE, RAW_QUEUE_CAPACITY, raw_queue_inotify, (void*)usbp);
#endif
}

/* ---------------------------------------------------------
 *                  Keyboard functions
 * ---------------------------------------------------------
 */

/* keyboard IN callback hander (a kbd report has made it IN) */
void kbd_in_cb(USBDriver *usbp, usbep_t ep) {
  /* STUB */
  (void)usbp;
  (void)ep;
}

#ifdef NKRO_ENABLE
/* nkro IN callback hander (a nkro report has made it IN) */
void nkro_in_cb(USBDriver *usbp, usbep_t ep) {
  /* STUB */
  (void)usbp;
  (void)ep;
}
#endif /* NKRO_ENABLE */

/* start-of-frame handler
 * TODO: i guess it would be better to re-implement using timers,
 *  so that this is not going to have to be checked every 1ms */
void kbd_sof_cb(USBDriver *usbp) {
  (void)usbp;
}

/* Idle requests timer code
 * callback (called from ISR, unlocked state) */
static void keyboard_idle_timer_cb(void *arg) {
  USBDriver *usbp = (USBDriver *)arg;

  osalSysLockFromISR();

  /* check that the states of things are as they're supposed to */
  if(usbGetDriverStateI(usbp) != USB_ACTIVE) {
    /* do not rearm the timer, should be enabled on IDLE request */
    osalSysUnlockFromISR();
    return;
  }

#ifdef NKRO_ENABLE
  if(!keymap_config.nkro && keyboard_idle) {
#else /* NKRO_ENABLE */
  if(keyboard_idle) {
#endif /* NKRO_ENABLE */
    /* TODO: are we sure we want the KBD_ENDPOINT? */
    if(!usbGetTransmitStatusI(usbp, KEYBOARD_IN_EPNUM)) {
      usbStartTransmitI(usbp, KEYBOARD_IN_EPNUM, (uint8_t *)&keyboard_report_sent, KEYBOARD_EPSIZE);
    }
    /* rearm the timer */
    chVTSetI(&keyboard_idle_timer, 4*MS2ST(keyboard_idle), keyboard_idle_timer_cb, (void *)usbp);
  }

  /* do not rearm the timer if the condition above fails
   * it should be enabled again on either IDLE or SET_PROTOCOL requests */
  osalSysUnlockFromISR();
}

/* LED status */
uint8_t keyboard_leds(void) {
  return (uint8_t)(keyboard_led_stats & 0xFF);
}

/* prepare and start sending a report IN
 * not callable from ISR or locked state */
void send_keyboard(report_keyboard_t *report) {
  osalSysLock();
  if(usbGetDriverStateI(&USB_DRIVER) != USB_ACTIVE) {
    osalSysUnlock();
    return;
  }
  osalSysUnlock();

#ifdef NKRO_ENABLE
  if(keymap_config.nkro) {  /* NKRO protocol */
    /* need to wait until the previous packet has made it through */
    /* can rewrite this using the synchronous API, then would wait
     * until *after* the packet has been transmitted. I think
     * this is more efficient */
    /* busy wait, should be short and not very common */
    osalSysLock();
    if(usbGetTransmitStatusI(&USB_DRIVER, NKRO_IN_EPNUM)) {
      /* Need to either suspend, or loop and call unlock/lock during
       * every iteration - otherwise the system will remain locked,
       * no interrupts served, so USB not going through as well.
       * Note: for suspend, need USB_USE_WAIT == TRUE in halconf.h */
      osalThreadSuspendS(&(&USB_DRIVER)->epc[NKRO_IN_EPNUM]->in_state->thread);
    }
    usbStartTransmitI(&USB_DRIVER, NKRO_IN_EPNUM, (uint8_t *)report, sizeof(report_keyboard_t));
    osalSysUnlock();
  } else
#endif /* NKRO_ENABLE */
  { /* boot protocol */
    /* need to wait until the previous packet has made it through */
    /* busy wait, should be short and not very common */
    osalSysLock();
    if(usbGetTransmitStatusI(&USB_DRIVER, KEYBOARD_IN_EPNUM)) {
      /* Need to either suspend, or loop and call unlock/lock during
       * every iteration - otherwise the system will remain locked,
       * no interrupts served, so USB not going through as well.
       * Note: for suspend, need USB_USE_WAIT == TRUE in halconf.h */
      osalThreadSuspendS(&(&USB_DRIVER)->epc[KEYBOARD_IN_EPNUM]->in_state->thread);
    }
    usbStartTransmitI(&USB_DRIVER, KEYBOARD_IN_EPNUM, (uint8_t *)report, KEYBOARD_EPSIZE);
    osalSysUnlock();
  }
  keyboard_report_sent = *report;
}

/* ---------------------------------------------------------
 *                     Mouse functions
 * ---------------------------------------------------------
 */

#ifdef MOUSE_ENABLE

/* mouse IN callback hander (a mouse report has made it IN) */
void mouse_in_cb(USBDriver *usbp, usbep_t ep) {
  (void)usbp;
  (void)ep;
}

void send_mouse(report_mouse_t *report) {
  osalSysLock();
  if(usbGetDriverStateI(&USB_DRIVER) != USB_ACTIVE) {
    osalSysUnlock();
    return;
  }
  osalSysUnlock();

  /* TODO: LUFA manually waits for the endpoint to become ready
   * for about 10ms for mouse, kbd, system; 1ms for nkro
   * is this really needed?
   */

  osalSysLock();
  usbStartTransmitI(&USB_DRIVER, MOUSE_IN_EPNUM, (uint8_t *)report, sizeof(report_mouse_t));
  osalSysUnlock();
}

#else /* MOUSE_ENABLE */
void send_mouse(report_mouse_t *report) {
  (void)report;
}
#endif /* MOUSE_ENABLE */

/* ---------------------------------------------------------
 *                   Extrakey functions
 * ---------------------------------------------------------
 */

#ifdef EXTRAKEY_ENABLE

/* extrakey IN callback hander */
void extra_in_cb(USBDriver *usbp, usbep_t ep) {
  /* STUB */
  (void)usbp;
  (void)ep;
}

static void send_extra_report(uint8_t report_id, uint16_t data) {
  osalSysLock();
  if(usbGetDriverStateI(&USB_DRIVER) != USB_ACTIVE) {
    osalSysUnlock();
    return;
  }

  report_extra_t report = {
    .report_id = report_id,
    .usage = data
  };

  usbStartTransmitI(&USB_DRIVER, EXTRAKEY_IN_EPNUM, (uint8_t *)&report, sizeof(report_extra_t));
  osalSysUnlock();
}

void send_system(uint16_t data) {
  send_extra_report(REPORT_ID_SYSTEM, data);
}

void send_consumer(uint16_t data) {
  send_extra_report(REPORT_ID_CONSUMER, data);
}

#else /* EXTRAKEY_ENABLE */
void send_system(uint16_t data) {
  (void)data;
}
void send_consumer(uint16_t data) {
  (void)data;
}
#endif /* EXTRAKEY_ENABLE */

/* ---------------------------------------------------------
 *                   Console functions
 * ---------------------------------------------------------
 */

#ifdef CONSOLE_ENABLE

/* console IN callback hander */
void console_in_cb(USBDriver *usbp, usbep_t ep) {
  (void)ep; /* should have ep == CONSOLE_IN_EPNUM, so use that to save time/space */
  uint8_t *buf;
  size_t n;

  osalSysLockFromISR();

  /* rearm the timer */
  chVTSetI(&console_flush_timer, MS2ST(CONSOLE_FLUSH_MS), console_flush_cb, (void *)usbp);

  /* Freeing the buffer just transmitted, if it was not a zero size packet.*/
  if (usbp->epc[CONSOLE_IN_EPNUM]->in_state->txsize > 0U) {
    obqReleaseEmptyBufferI(&console_buf_queue);
  }

  /* Checking if there is a buffer ready for transmission.*/
  buf = obqGetFullBufferI(&console_buf_queue, &n);

  if (buf != NULL) {
    /* The endpoint cannot be busy, we are in the context of the callback,
       so it is safe to transmit without a check.*/
    /* Should have n == CONSOLE_EPSIZE; check it? */
    usbStartTransmitI(usbp, CONSOLE_IN_EPNUM, buf, CONSOLE_EPSIZE);
  } else {
    /* Nothing to transmit.*/
  }

  osalSysUnlockFromISR();
}

/* Callback when data is inserted into the output queue
 * Called from a locked state */
void console_queue_onotify(io_buffers_queue_t *bqp) {
  size_t n;
  USBDriver *usbp = bqGetLinkX(bqp);

  if(usbGetDriverStateI(usbp) != USB_ACTIVE)
    return;

  /* Checking if there is already a transaction ongoing on the endpoint.*/
  if (!usbGetTransmitStatusI(usbp, CONSOLE_IN_EPNUM)) {
    /* Trying to get a full buffer.*/
    uint8_t *buf = obqGetFullBufferI(&console_buf_queue, &n);
    if (buf != NULL) {
      /* Buffer found, starting a new transaction.*/
      /* Should have n == CONSOLE_EPSIZE; check this? */
      usbStartTransmitI(usbp, CONSOLE_IN_EPNUM, buf, CONSOLE_EPSIZE);
    }
  }
}

/* Flush timer code
 * callback (called from ISR, unlocked state) */
static void console_flush_cb(void *arg) {
  USBDriver *usbp = (USBDriver *)arg;
  osalSysLockFromISR();

  /* check that the states of things are as they're supposed to */
  if(usbGetDriverStateI(usbp) != USB_ACTIVE) {
    /* rearm the timer */
    chVTSetI(&console_flush_timer, MS2ST(CONSOLE_FLUSH_MS), console_flush_cb, (void *)usbp);
    osalSysUnlockFromISR();
    return;
  }

  /* If there is already a transaction ongoing then another one cannot be
     started.*/
  if (usbGetTransmitStatusI(usbp, CONSOLE_IN_EPNUM)) {
    /* rearm the timer */
    chVTSetI(&console_flush_timer, MS2ST(CONSOLE_FLUSH_MS), console_flush_cb, (void *)usbp);
    osalSysUnlockFromISR();
    return;
  }

  /* Checking if there only a buffer partially filled, if so then it is
     enforced in the queue and transmitted.*/
  if(obqTryFlushI(&console_buf_queue)) {
    size_t n,i;
    uint8_t *buf = obqGetFullBufferI(&console_buf_queue, &n);

    osalDbgAssert(buf != NULL, "queue is empty");

    /* zero the rest of the buffer (buf should point to allocated space) */
    for(i=n; i<CONSOLE_EPSIZE; i++)
      buf[i]=0;
    usbStartTransmitI(usbp, CONSOLE_IN_EPNUM, buf, CONSOLE_EPSIZE);
  }

  /* rearm the timer */
  chVTSetI(&console_flush_timer, MS2ST(CONSOLE_FLUSH_MS), console_flush_cb, (void *)usbp);
  osalSysUnlockFromISR();
}


int8_t sendchar(uint8_t c) {
  osalSysLock();
  if(usbGetDriverStateI(&USB_DRIVER) != USB_ACTIVE) {
    osalSysUnlock();
    return 0;
  }
  osalSysUnlock();
  /* Timeout after 100us if the queue is full.
   * Increase this timeout if too much stuff is getting
   * dropped (i.e. the buffer is getting full too fast
   * for USB/HIDRAW to dequeue). Another possibility
   * for fixing this kind of thing is to increase
   * CONSOLE_QUEUE_CAPACITY. */
  return(obqPutTimeout(&console_buf_queue, c, US2ST(100)));
}

#else /* CONSOLE_ENABLE */
int8_t sendchar(uint8_t c) {
  (void)c;
  return 0;
}
#endif /* CONSOLE_ENABLE */

void sendchar_pf(void *p, char c) {
  (void)p;
  sendchar((uint8_t)c);
}

#ifdef RAW_ENABLE
void raw_hid_send( uint8_t *data, uint8_t length ) {
	// TODO: implement variable size packet
	if ( length != RAW_EPSIZE )
	{
		return;

	}
  osalSysLock();
  if(usbGetDriverStateI(&USB_DRIVER) != USB_ACTIVE) {
    osalSysUnlock();
    return;
  }
  if(usbGetTransmitStatusI(&USB_DRIVER, RAW_IN_EPNUM)) {
    /* Need to either suspend, or loop and call unlock/lock during
    * every iteration - otherwise the system will remain locked,
    * no interrupts served, so USB not going through as well.
    * Note: for suspend, need USB_USE_WAIT == TRUE in halconf.h */
    osalThreadSuspendS(&(&USB_DRIVER)->epc[RAW_IN_EPNUM]->in_state->thread);
  }

  usbStartTransmitI(&USB_DRIVER, RAW_IN_EPNUM, data, length);
  osalSysUnlock();
}

__attribute__ ((weak))
void raw_hid_receive( uint8_t *data, uint8_t length ) {
	// Users should #include "raw_hid.h" in their own code
	// and implement this function there. Leave this as weak linkage
	// so users can opt to not handle data coming in.
}

void raw_hid_task(void) {
  uint8_t buffer[RAW_EPSIZE];
  size_t size = 0;
  do {
    size = ibqReadTimeout(&raw_buf_queue, buffer, RAW_EPSIZE, TIME_IMMEDIATE);
    if (size == RAW_EPSIZE) {
      raw_hid_receive(buffer, size);
    }
  } while(size>0);
}

static void raw_in_cb(USBDriver* usbp, usbep_t ep) {
}

static void start_receive_raw(void) {
  return;
  /* If the USB driver is not in the appropriate state then transactions
     must not be started.*/
  if (usbGetDriverStateI(&USB_DRIVER) != USB_ACTIVE) {
    return;
  }

  /* Checking if there is already a transaction ongoing on the endpoint.*/
  if (!usbGetReceiveStatusI(&USB_DRIVER, RAW_IN_EPNUM)) {
    /* Trying to get a free buffer.*/
    uint8_t *buf = ibqGetEmptyBufferI(&raw_buf_queue);
    if (buf != NULL) {
      /* Buffer found, starting a new transaction.*/
      usbStartReceiveI(&USB_DRIVER, RAW_IN_EPNUM, buf, RAW_EPSIZE);
    }
  }
}

static void raw_out_cb(USBDriver* usbp, usbep_t ep) {
  osalSysLockFromISR();

  /* Posting the filled buffer in the queue.*/
  ibqPostFullBufferI(&raw_buf_queue, usbGetReceiveTransactionSizeX(usbp, ep));
  start_receive_raw();

  osalSysUnlockFromISR();
}

static void raw_queue_inotify(io_buffers_queue_t *bqp) {
  start_receive_raw();
}

static void configure_raw_interface(void) {
  ibqResetI(&raw_buf_queue);
  start_receive_raw();
}
#endif

#ifdef MIDI_ENABLE

void send_midi_packet(MIDI_EventPacket_t* event) {
  chnWrite(&midi_driver, (uint8_t*)event, sizeof(MIDI_EventPacket_t));
}

bool recv_midi_packet(MIDI_EventPacket_t* const event) {
  size_t size = chnReadTimeout(&midi_driver, (uint8_t*)event, sizeof(MIDI_EventPacket_t), TIME_IMMEDIATE);
  return size == sizeof(MIDI_EventPacket_t);
}

#endif

#ifdef VIRTSER_ENABLE

void virtser_send(const uint8_t byte) {
  chnWrite(&SDU1, &byte, 1);
}

__attribute__ ((weak))
void virtser_recv(uint8_t c)
{
  // Ignore by default
}

void virtser_task(void) {
  uint8_t numBytesReceived = 0;
  uint8_t buffer[16];
  do {
    numBytesReceived = chnReadTimeout(&SDU1, buffer, sizeof(buffer), TIME_IMMEDIATE);
    for (int i=0;i<numBytesReceived;i++) {
      virtser_recv(buffer[i]);
    }
  } while (numBytesReceived > 0);
}

#endif
